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
#include <log.h> 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_timer.h>
//#include <unistd.h>

#define CONFIGFILE "conf/rocsmq-testclient.config"

TCPsocket sock;

t_rocsmq_serverdata server = {
	.ip = ROCSMQ_IP,
	.port = ROCSMQ_PORT
};

t_rocsmq_baseconfig baseconfig = {
	};


int main(int argc, char **argv) {
	SDL_Init(0);
	SDL_Thread *thread;
	int i;
	char buffer[32];
	
	// parse config file 
	parseconfig(CONFIGFILE, &baseconfig, 0 ,0);
	
	// open log
	if (strlen(baseconfig.logfile) > 0) {
		printf("logging to file.. '%s'\n", baseconfig.logfile);
		openlog((char const *)baseconfig.clientname, (char const*)baseconfig.logfile);
	} else {
		printf("logging to stdout\n");
		log_init((char const *)baseconfig.clientname, stdout);
	}
	printf("loglevel = %d\n", baseconfig.loglevel);
		
	// set loglevel	
	log_setlevel(baseconfig.loglevel);
	
	log_message(DEBUG, "clientname: %s", baseconfig.clientname);
	
	t_rocsmq_message message;
	strncpy (message.sender, baseconfig.clientname, 20);
	memset  (message.tail, 0, 1000);
	message.id = 0;



	sock = rocsmq_init(baseconfig.clientname,&server, 0,0);
	if (!sock) {
		SDL_Quit();
		printf("could not connect to Server: %s\n", rocsmq_error());
		exit(1);
	}


	// start network listener
	thread = rocsmq_start_thread(sock);

	while (rocsmq_thread_is_running()) {
		if (argc > 1) {
			printf("> ");
			fgets(buffer,32,stdin);
			printf("sending: %s\n",buffer);
			strncpy(message.tail,buffer,32);
			if (! rocsmq_send(sock,&message,0)) {
				printf("could not send: %s\n",rocsmq_error());
			}
		}


		while(rocsmq_has_messages()) {

			rocsmq_get_message(&message);
			printf("incoming:%s\n",message.tail);
			if (strcmp("quit",message.tail) == 0) {
				rocsmq_thread_set_running(0);
				printf("quitting..\n");
			}

		}

		if(argc < 1) {
			SDL_Delay(5000);
		}
		for (i = 0; i < 10; i++) {
			sprintf(message.tail, "testmessage %d",i);
			rocsmq_send(sock,&message, 0);
		}
	}

	rocsmq_destroy_thread(thread);
	rocsmq_exit(sock);
	SDL_Quit();
	return 0;
}
