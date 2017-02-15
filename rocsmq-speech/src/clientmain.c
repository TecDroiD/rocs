/*
 * testmain.c
 *
 *  Created on: 29.09.2015
 *      Author: tecdroid
 */

//#include <linkedlist.h>
#include <daemonizer.h>
#include <rocsmq.h>
#include <rocsmqthread.h>
#include <configparser.h> 
#include <log.h> 

#include <signal.h>	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
//#include <unistd.h>

#include <espeak/speak_lib.h>

#include "client_config.h"

#define CLIENTNAME "console"	
#define CONFIGFILE "conf/rocsmq-testclient.config"

#define ORDER_SPEAK	"speech.speak"
#define ORDER_CANCEL "speech.cancel"

#define TAG_TEXT	"text"
int sock;

t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.port = 8389,
	.rundaemon = 0,
	.loglevel = INFO,
	.logfile = "",
	.clientname = "speech output",
	.filter = "speech*",
};

t_clientconfig clientconfig = {
	.voice = "default",
	.speed = 120,
	.volume = 50,
	.pitch = 50,
};

void client_signal_handler(int sig);
int handle_message(p_rocsmq_message message);

int main(int argc, char **argv) {

	pthread_t thread;

	int opt;
	t_rocsmq_message message;
	int x_it = 0;
	
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
	
	
	sock = rocsmq_init(&baseconfig);
	if (!sock) {
		log_message(ERROR, "could not connect to Server: %s\n", rocsmq_error());
		exit(1);
	}

	/*
	 * initialize logging system
	 */
	openlog(CLIENTNAME, baseconfig.logfile);
	log_setlevel(baseconfig.loglevel);

	log_message(DEBUG, CLIENTNAME " starting..");

	init_espeak(&clientconfig);

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
	// parse message into json object
	json_object *json = rocsmq_get_message_json(message);
	char text[256];

	if (0 == strcmp(message->id, ORDER_SPEAK)) {

		get_stringval(json, TAG_TEXT, text, 256);
		if (strlen(text)) {
			log_message(DEBUG, "speaking message '%s'",text);
			speak(text);
		}
	
	// react on order cancel	
	} else if (0 == strcmp(message->id, ORDER_CANCEL)) {
		log_message(DEBUG, "canceling speech");
		shutup();
	}
	return 0;
}
