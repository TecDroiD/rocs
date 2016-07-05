/*
 * testmain.c
 *
 *  Created on: 29.09.2015
 *      Author: tecdroid
 */

//#include <linkedlist.h>
#include <rocsmq.h>
#include <rocsmqthread.h>
#include <configparser.h>
#include <messages.h> 
#include <log.h> 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_timer.h>

//#include <unistd.h>

#include "client_config.h"
#include "rocs_cron.h"

#define CLIENTNAME MESSAGE_CLIENT_CRON
#define CLIENTFILTER MESSAGE_FILTER_CRON

#define CONFIGFILE "conf/rocsmq-cron.config"

TCPsocket sock;


t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.filter 	= CLIENTFILTER,
	.port = 8389,
	.rundaemon = 0,
	.loglevel = DEBUG,
	.logfile = "",
	.clientname = CLIENTNAME,
};

t_cronconfig clientconfig = {
	.delay = 100,
	.tick = 1,
};


void handle_message(p_rocsmq_message message);

/**
 * main function
 */ 
int main(int argc, char **argv) {
	SDL_Init(0);
	SDL_Thread *thread;
	int i;
	char buffer[32];
	
	// parse configuration
	if (argc <= 1) {
		parseconfig(CONFIGFILE, &baseconfig, CLIENTNAME, custom_config, &clientconfig);
	} else if (argc == 2) {
		parseconfig(argv[1], &baseconfig, CLIENTNAME, custom_config, &clientconfig);
	} else {
		printf("Usage: %s [configfile]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	// open log
	printf("logging to file.. '%s'\n", baseconfig.logfile);
	openlog((char const *)baseconfig.clientname, (char const*)baseconfig.logfile);
	printf("loglevel = %d\n", baseconfig.loglevel);
		
	// set loglevel	
	log_setlevel(baseconfig.loglevel);
	
	log_message(DEBUG, "clientname: %s", baseconfig.clientname);
	log_message(DEBUG, "Delay: %d, Ticks: %d",clientconfig.delay, clientconfig.tick);

	t_rocsmq_message message;
	strncpy (message.sender, baseconfig.clientname, 20);
	memset  (message.tail, 0, 1000);
	strncpy (message.id, "\0", 256);

	sock = rocsmq_init(&baseconfig);
	if (!sock) {
		SDL_Quit();
		log_message(ERROR,"could not connect to Server: %s\n", rocsmq_error());
		exit(1);
	}

	// start network listener
	thread = rocsmq_start_thread(sock);

	while (rocsmq_thread_is_running()) {

		tick(sock, clientconfig.tick);
		
		while(rocsmq_has_messages()) {
 			rocsmq_get_message(&message);
			log_message( DEBUG, "incoming:%s\n",message.tail);
			handle_message(&message);
		}

			SDL_Delay(clientconfig.delay);
	}

	/*
	 * shut system down
	 */ 
	clear_custom_config(&clientconfig); 
	log_message( INFO, "Process shutdown.");
	rocsmq_destroy_thread(thread);
	rocsmq_exit(sock);
	SDL_Quit();
	
	return 0;
}

/**
 * work on message...
 */ 
void handle_message(p_rocsmq_message message) {
	json_object * json;
	t_cronjob cronjob;
	char val[32];
	int i;
	char *result;

	log_message(DEBUG, "message-id '%s'", message->id);

	char mesgid[ROCS_IDSIZE];
	strncpy(mesgid, message->id, ROCS_IDSIZE);
	
	
	// react on system messages
	if(rocsmq_check_system_message(message->id)){
		log_message(INFO, "system message handled.");
		return;
	}
	

	// copy json content from message
	log_message(DEBUG, "  -> message-tail (%s)", message->tail);
	json = rocsmq_get_message_json(message);
	parse_cronjob(json, &cronjob);
	
	log_message(DEBUG,"checking order '%s'", CRONJOB_MESSAGE_ADD);
	
	if (0 == strcmp(mesgid, CRONJOB_MESSAGE_ADD)) {
		log_message(INFO, "adding cronjob %s", message->tail);
		add_cronjob(&cronjob);
	} else if (0 == strcmp(mesgid, CRONJOB_MESSAGE_DEL)) {
		log_message(INFO, "adding cronjob", message->tail);
		del_cronjob(&cronjob);
	}

	return;
}
