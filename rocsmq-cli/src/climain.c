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
#include <getopt.h>

#include "cli_config.h"
#include "cli_functions.h"

#define CLIENTNAME "cli"	
#define CLIENTVERSION "0.1.0"

int sock;

t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.port = 8389,
	.rundaemon = 0,
	.loglevel = INFO,
	.logfile = "",
	.clientname = CLIENTNAME,
	.filter = "*",
	};


t_rocsmq_message message = {
	.sender = CLIENTNAME,
	.id = "",
	.tail = "",
};

t_cliconfig cliconfig = {
	.mode = MODE_INTERACTIVE,
};

/**
 * send the message
 */
void send_message ();
  
/**
 * get the parameters from command line
 * @param argc
 * @param argv
 * @return zero or error code
 */ 
int get_parameters(int argc, char **argv);

/**
 * thread reader
 */ 
void * read_thread(void *data);

/**
 * stop program
 */ 
int func_quit(char *ptr) {
	rocsmq_thread_set_running(ROCSMQ_THREAD_STOP);	
}

/**
 * stop program
 */ 
int func_send(char *ptr) {
	char *pos_message = ptr;
	char *pos_tail;
	// search for message id
	while(*pos_message == ' ' || *pos_message == '\t' && *pos_message != '\n' ) pos_message++;
	pos_tail = pos_message;
	
	// search for end of message id
	while(*pos_tail != ' ' && *pos_tail != '\t' && *pos_message != '\n') pos_tail++;
	pos_tail = '\0';
	pos_tail++;
	
	// search for first char in tail
	while(*pos_tail == ' ' || *pos_tail == '\t' && *pos_message != '\n') pos_tail++;
	
	// set message id
	strncpy(message.id, pos_message, strlen(pos_message));
	
	// set message tail
	strncpy(message.tail, pos_tail, strlen(pos_tail));
	
	// send message
	send_message();
}

/**
 * main function.. no comment..
 */ 
int main(int argc, char **argv) {
	pthread_t thread, readprocess;
	void * threadret;
	char input[1024];

	// parse command line parameters 
	get_parameters(argc, argv);
	
	// always logging to console
	// set loglevel	
	log_setlevel(baseconfig.loglevel);
	log_message(DEBUG, "loglevel = %d\n", baseconfig.loglevel);	
	log_message(INFO, "clientname: %s", baseconfig.clientname);
	log_message(INFO, "clientversion: %d", CLIENTVERSION);


	// initializing rocs
	sock = rocsmq_init(&baseconfig);
	if (!sock) {
		log_message(ERROR,"could not connect to Server: %s\n\t ===dieing now.===", rocsmq_error());
		exit(1);
	}


	// start network listener
	thread = rocsmq_start_thread(sock);

	// for interactive mode
	if (cliconfig.mode == MODE_INTERACTIVE) {
		log_message(INFO, "Entering interactive mode.");

		int res = pthread_create(&readprocess, NULL, read_thread, NULL);
		if (0 != res) {
			exit(1);
		}

		// as long as program has to run..
		while (rocsmq_thread_is_running()) {
			// read input
			if (!fgets(input, 1024, stdin)) {
				log_message(ERROR, "could not read from console.");
			}

			execute("quit",input,func_quit);
			execute("send",input,func_send);
			
		}
		
	} else { // and this is for writing
		send_message();
	}

	// wait for read thread.
	pthread_join(readprocess, &threadret);
	
	rocsmq_destroy_thread(thread);
	rocsmq_exit(sock);
	return 0;
}

/**
 * sends the message
 */ 
void send_message () {
	if (message.id != 0) {
			log_message( INFO, "sending message\n ->id:\t%d\n ->tail:\t'%s'\n",message.id, message.tail);
			if (! rocsmq_send(sock,&message,0)) {
				log_message(ERROR,"could not send: %s\n",rocsmq_error());
			}
	}
}


/**
 * get the parameters from command line
 * @param argc
 * @param argv
 * @return zero or error code
 */ 
int get_parameters(int argc, char **argv) {
	int opt;
	// get command line options
	while ((opt = getopt(argc, argv, "s:p:n:i:m:f:SI")) != -1) {
		switch(opt) {
			case 's': /* server ip */
				strcpy(baseconfig.serverip, optarg);
				break;
			case 'p': /* server port */
				baseconfig.port = atoi(optarg);
				break;				
			case 'n': /* client name */
				strcpy(baseconfig.clientname, optarg);
				break;
			case 'i': /* message id for single transmission */
				strncpy(message.id,optarg, ROCS_IDSIZE);
				break;
			case 'm': /* message tail for single transmission */
				strncpy(message.tail,optarg, 1000);
				break;
			case 'f': /* parse a config file */
				parseconfig(optarg, &baseconfig,0, 0,0);
				break;	
			case 'S': /* mode single transmission */
				cliconfig.mode = MODE_SINGLETRANSMISSION;
				break;
			case 'I': /* mode single transmission */
				cliconfig.mode = MODE_INTERACTIVE;
				break;
			default:
				printf("usage:\n %s [-options]\n", argv[0]);
				printf(" -s - server ip, localhost is default\n");
				printf(" -p - server port, 0x20C5 (rocs) is default\n");
				printf(" -n - name of the client\n");
				printf(" -i - identifier of the message\n");
				printf(" -m - body of the message\n");
				printf(" -f - configuration file \n");
				printf(" -S - single mode exit after sending\n");
				printf(" -I - interactive mode (default)\n");
				break;
		}
		
	}
	
}

/**
 * read thread
 */
void *read_thread(void *data) {
	while (rocsmq_thread_is_running()) {
		while(rocsmq_has_messages()) {
			rocsmq_get_message(&message);
			log_message( INFO, "incoming message\n ->id:\t%d\n ->tail:\t'%s'\n",message.id, message.tail);
		}
		rocsmq_delayms(100);
	}
	
	return 0;
}
