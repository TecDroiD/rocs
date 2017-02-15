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


#include "client_config.h"
#include "recognizer.h"

#define CLIENTNAME "sphinx"	
#define CONFIGFILE "rocsmq-"CLIENTNAME".config"

int sock;

volatile int record;

t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.port = 8389,
	.rundaemon = 0,
	.loglevel = INFO,
	.logfile = "",
	.clientname = "speech recognizer",
	.filter = "sphinx*",
};

t_clientconfig clientconfig = {
	.device = "",
	.samplingrate = 0,
	.sendto = "speech.speak",
	.keywordlist = "",
	.hmmfile = "",
	.lmfile = "",
	.dictfile = "",
	.delay_after = 1,
};

void client_signal_handler(int sig);
int handle_message(p_rocsmq_message message);

int main(int argc, char **argv) {
	pthread_t thread;

	int opt;
	t_rocsmq_message message;
	int x_it = 0;
	
	record = TRUE;
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

	if(sphinx_init(&clientconfig)) {
		log_message(ERROR, "could not initialize sphinx");
		exit(0);
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
		
		if(record) {
			
			char * text = (char *)recognize();
			log_message(DEBUG, "recognized '%s'", text);
			strcpy(message.id,clientconfig.sendto);
			sprintf(message.tail,"{\"text\":\"%s\",\"source\":\"%s\",}", text, CLIENTNAME);
			rocsmq_send(sock,&message, 0);
			
			log_message(DEBUG, "waiting %dms",clientconfig.delay_after);
			rocsmq_delayms(clientconfig.delay_after);
			
		}
		
		/*
		 * wait 1ms
		 */
		rocsmq_delayms(1);
	}

	/*
	 * cleanup
	 */
	sphinx_shutdown();
	 
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

	if(0 == strcmp(message->id,ORDER_START)) {
		log_message(DEBUG, "starting record");
		record = TRUE;
	} else if(0 == strcmp(message->id,ORDER_STOP)) {
		log_message(DEBUG, "stopping record");
		record = FALSE;
	}
 
	return 0;
}
