/*
 * client_main.c
 *
 *  Created on: 06.10.2015
 *      Author: tecdroid
 * 
 * message 
 * {
 * 	"type":"read|write",
 *  "slave":10,
 *  "addr":"120",
 *  "length":2,
 *  "data": "xwwer",
 * }
 */

#include <daemonizer.h>
#include <getopt.h>
#include <linux/i2c-dev.h>
#include <log.h>
#include <rocsmq.h>
#include <rocsmqthread.h>
#include <messages.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_timer.h>

#include <json-c/json.h>

#include "i2cbus.h"
#include "pca9685.h"
#include "customconfig.h"


#define PROGNAME "rocsmq-i2cbus"
#define CONFIGFILE "conf/"PROGNAME".config"

#define PROG_FILTER 0x0000
#define PROG_MASK	0x0000

#define MOTORS_PER_DEVICE	16

TCPsocket sock;


t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.filter 	= MESSAGE_ID_INFRASTRUCTURE,
	.mask 		= MESSAGE_MASK_MAIN,
	.port = 8389,
	.rundaemon = 0,
	.loglevel = INFO,
	.logfile = "",
	.clientname = "i2cbus",
};

t_clientconfig clientconfig = {
	.kbaud	= 125,
	.devicefile = "/dev/i2c-0\0",
};

/**
 * signal handler function for signals to handle ;-p
 */
void client_signal_handler(int sig) {
//	simple_signal_handler(sig);

	if (sig == SIGTERM) {
		log_message(INFO, "quitting..\n");
		rocsmq_thread_set_running(0);
	}
}

/**
 * print program usage
 */
void print_usage (void) {
	printf("rocsmq client - usage\n" );
	printf(" rocsmq_client [options]\n" );
	printf(" options\n --------\n" );
	printf(" -D : Set output level to DEBUG\n" );
	printf(" -I : Set output level to INFO\n" );
	printf(" -W : Set output level to WARNING\n" );
	printf(" -E : Set output level to ERROR\n" );
	printf(" -S : Output log to console(standard)\n" );
	printf(" -d : Run as daemon\n" );
	printf(" -l [file] : Output log to file [file]\n" );
	printf(" -p [port] : Set listener port\n" );
	printf(" -f [file] : Set i2c bus device (std: /dev/i2c-0)\n");
	printf(" -c [file] : Configuration file\n");
	//printf(" -b [baud] : Set baudrate in kbaud\n");
}

/**
 * get functional options
 */
int getoptions (int argc, char **argv) {
	int opt;
	while((opt = getopt(argc, argv, "DIEWSl:dp:c:"))!= -1) {
		switch(opt) {
		case 'D': baseconfig.loglevel = DEBUG; break; /* Debug level DEBUG */
		case 'I': baseconfig.loglevel = INFO; break; /* Debug level INFO */
		case 'E': baseconfig.loglevel = ERROR; break; /* Debug level ERROR */
		case 'W': baseconfig.loglevel = WARNING; break; /* Debug level WARNING */
		case 'l': strncpy(baseconfig.logfile,optarg,255); break; /* log to file [filename] */
		case 'd': baseconfig.rundaemon = 1; break;
		case 'p': baseconfig.port = atoi(optarg); break;
		case 'c': strncpy(clientconfig.devicefile, optarg, 255); break;
		default:
			print_usage();
			return 1;
		}
	}

	/* daemonize if neccessary */
	if(baseconfig.rundaemon) {
		if(0 != daemonize("~", client_signal_handler))
			return 1;
	}

	/*
	 * initialize logging system
	 */
	openlog(PROGNAME, baseconfig.logfile);
	log_setlevel(baseconfig.loglevel);

	log_message(DEBUG, PROGNAME " starting..");

	return 0;
}

/*
 * handle rocsmq message
 */
int handle_message(p_rocsmq_message message) {
	// parse message into json object
	json_object *json = rocsmq_get_message_json(message);

	int write;
	int32_t d;
	char buf[255];
	char answer[1000];
	
	t_i2cmesg i2c = {
		.slave=0,
		.addr=0,
		.data="\0",
		.length=0
	};
	
	get_intval(json, JSON_KEY_TYPE, &write);
	get_intval(json, JSON_KEY_SLAVE, &d);
	i2c.slave = d;
	get_intval(json, JSON_KEY_ADDR, &d);
	i2c.addr = d;
	get_intval(json, JSON_KEY_LENGTH, &d);
	i2c.length = d;
	get_stringval(json, JSON_KEY_DATA, buf, I2C_MAXLEN);

	i2cbus_setslave(i2c.slave);
	if(write) {
		// write data to i2c
		b64decode(buf,i2c.data,I2C_MAXLEN);	
		i2cbus_write(&i2c.addr, 1);
		i2cbus_write(i2c.data,strlen(i2c.data));
	} else {
		// read data from i2c
		i2cbus_write(&i2c.addr, 1);
		i2cbus_read(i2c.data, i2c.length);
		b64encode(i2c.data,buf,255);
		
		// prepare message and send 
		message->id = MESSAGE_ID_SENSOR | MESSAGE_CLIENT_I2C;
		sprintf(answer,"{\""JSON_KEY_SLAVE"\":%d,\""JSON_KEY_ADDR"\":%d,\""JSON_KEY_LENGTH"\":\"%s\",}"
				,i2c.slave, i2c.addr, i2c.length,buf);
		rocsmq_send(sock,message,0);
	}
	// delete json object
	json_object_put(json);
	return 0;
}

/**
 * main function
 */
int main(int argc, char **argv) {
	SDL_Thread *thread;
	t_rocsmq_message message;

	/* initialize sdl */
	SDL_Init(0);
	

	/* get options */
	parseconfig(CONFIGFILE, &baseconfig, i2cbus_custom_config, &clientconfig);
	if(0 != getoptions(argc, argv))
		exit(1);

	/* initialize i2c */
	if(0 > i2cbus_init(clientconfig.devicefile))
		exit(1);
		

	sock = rocsmq_init(&baseconfig);
	if(! sock) {
		log_message(ERROR, "could not connect to server %s", rocsmq_error());
		exit(1);
	}
	thread = rocsmq_start_thread(sock);
	if (!thread) {
		log_message(ERROR, "could not start listener thread %s", rocsmq_error());
	}

	/**
	 * process main loop
	 */
	while (rocsmq_thread_is_running()) {

		/* as long as messages are there */
		while (rocsmq_has_messages()) {

			/* handle next message */
			rocsmq_get_message(&message);
			log_message(INFO, "incoming:%s\n",message.tail);
			handle_message(&message);

		}

			/*
			 * todo: handle i²c message - not yet neccessary
			 */
		/*
		 * wait 100ms
		 */
		SDL_Delay(1);
	}

	/*
	 * cleanup
	 */
	rocsmq_destroy_thread(thread);
	rocsmq_error(sock);

	if (baseconfig.logtofile)
		closelog();

	SDL_Quit();

	return 0;
}



