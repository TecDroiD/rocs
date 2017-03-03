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
#include <log.h>
#include <rocsmq.h>
#include <rocsmqthread.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <json-c/json.h>

#include "customconfig.h"

static const char* CLIENTNAME = "vision";
#define PROGNAME "rocsmq-opencv"
#define CONFIGFILE "conf/"PROGNAME".config"


#define MESSAGE_RESPONSE "sensor.vision"

int sock;

int32_t lastread = 0;

t_rocsmq_baseconfig baseconfig;
/*
 = {
	serverip : "127.0.0.1",
	filter 	: MESSAGE_CLIENT_I2C,
	port : 8389,
	rundaemon : 0,
	loglevel : DEBUG,
	logfile : "",
	clientname : CLIENTNAME,
};
*/
t_clientconfig clientconfig;
/*
  = {
	device   : "/dev/ttyUSB0",
};
*/

void init_configs(void);
void client_signal_handler(int sig);
int handle_message(p_rocsmq_message message);

/**
 * main function
 */
int main(int argc, char **argv) {
	pthread_t thread;
	t_rocsmq_message message;
	

	// parse configuration
	if (argc <= 1) {
		parseconfig(CONFIGFILE, &baseconfig, (char *)CLIENTNAME, custom_config, &clientconfig);
	} else if (argc == 2) {
		parseconfig(argv[1], &baseconfig, (char *)CLIENTNAME, custom_config, &clientconfig);
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

	log_message(INFO, PROGNAME " starting..");

	/* initialize opencv */

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
		
		/*
		 * wait 1ms
		 */
		rocsmq_delayms(1);
	}

	/*
	 * cleanup
	 */
	rocsmq_destroy_thread(thread);
	rocsmq_exit(sock);

	if (baseconfig.logtofile)
		closelog();


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
	log_message(DEBUG, "handling message %s", message->id);
	return 0;
}



/**
 * initialize configuration
 */ 
void init_configs(void) {
/*
 * base config
 */ 	
	
	strcpy(baseconfig.serverip, "127.0.0.1");
	strcpy(baseconfig.filter, "vision*");
	strcpy(baseconfig.clientname, (char *)CLIENTNAME);
	baseconfig.port = 8389;
	baseconfig.rundaemon = 0;
	baseconfig.loglevel= DEBUG;
	
/*
 * client config
 */
	strcpy(clientconfig.device, "/dev/ttyUSB0");
}
