/*
 * client_main.c
 *
 *  Created on: 06.10.2015
 *      Author: tecdroid
 * 
 * message 
 * { "pins" : [
 * 	   {
 *        "name": "%s",
 *        "value": %d,
 *     },
 *  ]
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

#include "tty.h"
#include "customconfig.h"

#define CLIENTNAME "tty"
#define PROGNAME "rocsmq-tty"
#define CONFIGFILE "conf/"PROGNAME".config"

#define ORDER_WRITE	"tty.write"

#define MESSAGE_KEY_VALUE	"value"

#define MESSAGE_RESPONSE "sensor.tty"
#define MESSAGE_HEAD "{ \"input\" : \"%s\", }"
TCPsocket sock;

int32_t lastread = 0;

t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.filter 	= MESSAGE_CLIENT_I2C,
	.port = 8389,
	.rundaemon = 0,
	.loglevel = DEBUG,
	.logfile = "",
	.clientname = CLIENTNAME,
};

t_clientconfig clientconfig = {
	.device = "/dev/ttyS1",
	.baudrate= 9600,
	.bitrate = 8,
	.parity = 1,
	.stopbits = 1,
};

void client_signal_handler(int sig);
int handle_message(p_rocsmq_message message);

/**
 * main function
 */
int main(int argc, char **argv) {
	SDL_Thread *thread;
	t_rocsmq_message message;
	char recvdata[255];
	char b64data[255];
	
	/* initialize sdl */
	SDL_Init(0);
	

	// parse configuration
	if (argc <= 1) {
		parseconfig(CONFIGFILE, &baseconfig, CLIENTNAME, custom_config, &clientconfig);
	} else if (argc == 2) {
		parseconfig(argv[1], &baseconfig, CLIENTNAME, custom_config, &clientconfig);
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

	/* initialize tty */
	memset(recvdata, 0, 255);
	if(0 > tty_init(&clientconfig)) {
		log_message(ERROR, "could not init tty ");		
		exit(1);
	}

	/* initialize rocs */
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
		
		/* if data received, send message */
		if (tty_hasdata()) {
			strcpy(message.id, MESSAGE_RESPONSE);
			tty_read(recvdata,255);
			b64encode(recvdata, b64data, 255);
			sprintf(message.tail, MESSAGE_HEAD,b64data);
			rocsmq_send(sock, &message, 0);
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

	tty_close();
	
	if (baseconfig.logtofile)
		closelog();

	SDL_Quit();

	return 0;
}



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




/*
 * handle rocsmq message
 */
int handle_message(p_rocsmq_message message) {
	char b64data[255];
	char senddata[255];
	memset(b64data, 0, 255);
	
	// parse message into json object
	json_object *json = rocsmq_get_message_json(message);
	get_stringval(json, MESSAGE_KEY_VALUE, b64data, 255);
	b64decode(b64data, senddata, 255);
	
	// if there's something to write
	if (0 == strcmp(message->id, ORDER_WRITE) ) {
		tty_write(senddata, strlen(senddata));
	}
	
	return 0;
}



