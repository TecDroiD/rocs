/*
 * client_main.c
 *
 *  Created on: 06.10.2015
 *      Author: tecdroid
 * 
 * message 
 * {
 * 	"read":1|0,
 *  "slave":10,
 *  "addr":120,
 *  "length":2,
 *  "data": "b64",
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
#include "customconfig.h"

#define CLIENTNAME "i2c"
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
	.loglevel = DEBUG,
	.logfile = "",
	.clientname = "i2cbus",
};

t_clientconfig clientconfig = {
	.kbaud	= 125,
	.devicefile = "/dev/i2c-0\0",
	.num_initscripts = 0,
	.initscripts = 0,
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
 * get functional options
 */
int getoptions (int argc, char **argv) {

	return 0;
}

/*
 * handle rocsmq message
 */
int handle_message(p_rocsmq_message message) {
	// parse message into json object
	json_object *json = rocsmq_get_message_json(message);

	int read;
	int32_t d;
	char buf[255];
	char answer[1000];
	
	t_i2cmesg i2c = {
		.slave=0,
		.addr=0,
		.data="\0",
		.length=0
	};
	
	get_intval(json, JSON_KEY_READ, &read);
	get_intval(json, JSON_KEY_SLAVE, &d);
	i2c.slave = d;
	get_intval(json, JSON_KEY_ADDR, &d);
	i2c.addr = d;
	get_intval(json, JSON_KEY_LENGTH, &d);
	i2c.length = d;
	get_stringval(json, JSON_KEY_DATA, buf, I2C_MAXLEN);

	i2cbus_setslave(i2c.slave);
	if(read) {
		// write data to i2c
		b64decode(buf,i2c.data,I2C_MAXLEN);	
		i2cbus_write(i2c.addr, i2c.data,strlen(i2c.data));
	} else {
		// read data from i2c
		i2cbus_read(i2c.addr, i2c.data, i2c.length);
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
 * initialize devices 
 */
void init_devices() {
	int cnt, addr, val;
	char buf[2];
	
	char line[255];
	
	p_initscript init;
	FILE *fp;
	
	// for each init entry
	for (cnt = 0; cnt < clientconfig.num_initscripts; cnt++) {
		init = &clientconfig.initscripts[cnt];
		i2cbus_setslave(init->slave);
	
		log_message(DEBUG, "reading init script file '%s' for slave %x", init->initfile, init->slave);
		// open file
		fp = fopen(init->initfile, "r");
		if (! fp) {
			log_message(ERROR,"Could not open init file '%s'", init->initfile);
			return;
		}
		
		// read until eof
		while(! feof(fp)) {
			fgets (line, 255,fp);
			log_message(DEBUG, "reading line '%s'", line);
			if (line[0] != '#') {
				sscanf(line, "%d %d", &addr, &val);
				buf[0] = addr;
				buf[1] = val;
				i2cbus_write(buf[0],&buf[1],1);
			} else {
				SDL_Delay(2);
			}
		}
		
		// close file
		fclose(fp);
	}	
}
/**
 * main function
 */
int main(int argc, char **argv) {
	SDL_Thread *thread;
	t_rocsmq_message message;
	
	/* initialize sdl */
	SDL_Init(0);
	

	// parse configuration
	if (argc <= 1) {
		parseconfig(CONFIGFILE, &baseconfig, CLIENTNAME, i2cbus_custom_config, &clientconfig);
	} else if (argc == 2) {
		parseconfig(argv[1], &baseconfig, CLIENTNAME, i2cbus_custom_config, &clientconfig);
	} else {
		printf("Usage: %s [configfile]\n", argv[0]);
		exit(EXIT_FAILURE);
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

	/* initialize i2c */
	if(0 > i2cbus_init(clientconfig.devicefile)) {
		log_message(ERROR, "could not init i2c device %s", clientconfig.devicefile);		
		exit(1);
	}

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
	 * initialize devices
	 */ 
	init_devices();
	  
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
		 * wait 1ms
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



