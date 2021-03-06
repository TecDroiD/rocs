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
//#include <unistd.h>

#include <sys/msg.h>

#define CLIENTNAME "console"	
#define CONFIGFILE "conf/rocsmq-testclient.config"

int sock;
int queueid = 0;

t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.port = 8389,
	.rundaemon = 0,
	.loglevel = INFO,
	.logfile = "",
	.clientname = "queue",
	};



int main(int argc, char **argv) {
	pthread thread;

	int opt;
	t_rocsmq_message message;
	int x_it = 0;
	
	// parse config file 
	parseconfig(CONFIGFILE, &baseconfig, 0, 0 ,0);
	
	
	// open log
	log_message(DEBUG, "logging to file.. '%s'\n", baseconfig.logfile);
	openlog((char const *)baseconfig.clientname, (char const*)baseconfig.logfile);
	log_message(DEBUG, "loglevel = %d\n", baseconfig.loglevel);
		
	// set loglevel	
	log_setlevel(baseconfig.loglevel);
	
	log_message(DEBUG, "clientname: %s", baseconfig.clientname);
	strncpy (message.sender, baseconfig.clientname, 20);
	memset  (message.tail, 0, 1000);
	strncpy (message.id, "", ROCS_IDSIZE);

	// get command line options
	while ((opt = getopt(argc, argv, "n:i:m:f:x")) != -1) {
		switch(opt) {
			case 'n':
				strcpy(baseconfig.clientname, optarg);
				break;
			case 'i':
				strncpy(message.id,optarg, ROCS_IDSIZE);
				break;
			case 'm': 
				strncpy(message.tail,optarg, 1000);
				break;
			case 'x':
				x_it = 1;
				break;
			case 'f':
				parseconfig(optarg, &baseconfig,0, 0,0);	
			default:
				printf("usage:\n %s [-n name] [-i message-id] [-m message] [-x]\n", argv[0]);
				printf(" -n - name of the client\n");
				printf(" -i - identifier of the message\n");
				printf(" -m - body of the message\n");
				printf(" -x - exit after sending, goes into listening mode else.\n");
		}
		
	}


	sock = rocsmq_init(&baseconfig);
	if (!sock) {
		printf("could not connect to Server: %s\n", rocsmq_error());
		exit(1);
	}


	// start network listener
	thread = rocsmq_start_thread(sock);

	if (message.id != 0) {
			log_message( INFO, "sending message\n ->id:\t%d\n ->tail:\t'%s'\n",message.id, message.tail);
			if (! rocsmq_send(sock,&message,0)) {
				log_message(ERROR,"could not send: %s\n",rocsmq_error());
			}
	}

	if (! x_it) 
	while (rocsmq_thread_is_running()) {
		while(rocsmq_has_messages()) {

			rocsmq_get_message(&message);
			log_message( INFO, "incoming message\n ->id:\t%d\n ->tail:\t'%s'\n",message.id, message.tail);
			if (strcmp("quit",message.tail) == 0) {
				rocsmq_thread_set_running(0);
				log_message(INFO,"quitting..\n");
			}

		}
		rocsmq_delayms(100);
		printf(".");
	}

	rocsmq_destroy_thread(thread);
	rocsmq_exit(sock);
	return 0;
}
