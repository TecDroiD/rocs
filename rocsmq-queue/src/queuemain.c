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
#include <log.h>
#include <rocsmq.h>
#include <rocsmqthread.h>
#include <messages.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <json-c/json.h>

#include "customconfig.h"

#define CLIENTNAME "queue"
#define PROGNAME "rocsmq-queue"
#define CONFIGFILE "conf/"PROGNAME".config"

#define TYPE_QUEUE2ROCS	1
#define TYPE_ROCS2QUEUE 2

#define DIVIDER	"##"
#define QUEUEMESSAGE "%s"DIVIDER"%s"



t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.filter 	= "*",
	.port = 8389,
	.rundaemon = 0,
	.loglevel = DEBUG,
	.logfile = "",
	.clientname = CLIENTNAME,
};

t_clientconfig clientconfig = {
	.queue	= 8389,
};

typedef struct s_queuemessage {
	long type;
	char name[ROCS_IDSIZE];
	char message[ROCS_MESSAGESIZE];
	
} t_queuemessage;

// id der messagequeue
int messagequeue;
// socketverbindung
int sock;

void init_queue() {
	messagequeue = msgget(clientconfig.queue, 0666 | IPC_CREAT);
	if (messagequeue == -1) {
		log_message(ERROR, "IPC get error: %s", strerror(errno));
		exit(2);
	}

}


	
/**
 * signal handler function for signals to handle ;-p
 */
void client_signal_handler(int sig) {
//	simple_signal_handler(sig);
	switch (sig) {
		case SIGTERM:
		case SIGKILL:
			log_message(INFO, "quitting..\n");
			rocsmq_thread_set_running(0);
			break;
		default:
			break;
	}
}


/**
 * main function
 */
int main(int argc, char **argv) {
	pthread_t thread;
	t_rocsmq_message message;
	t_queuemessage queue;
	
	// initialize values
	
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

	sock = rocsmq_init(&baseconfig);
	if(! sock) {
		log_message(ERROR, "could not connect to server %s", rocsmq_error());
		exit(1);
	}
	thread = rocsmq_start_thread(sock);
	if (!thread) {
		log_message(ERROR, "could not start listener thread %s", rocsmq_error());
	}
	  
	init_queue();
	if(messagequeue == -1) {
		log_message(ERROR, "could not initialize message queue.");
		exit(1);
	}
	/**
	 * process main loop
	 */
	while (rocsmq_thread_is_running()) {

		/* as long as messages are there */
		while (rocsmq_has_messages()) {

			/* handle next message */
			rocsmq_get_message(&message);
			log_message(INFO, "incoming:%s -- %s\n",message.id, message.tail);

			memset(&queue,0, sizeof(t_queuemessage));
			queue.type = TYPE_ROCS2QUEUE;
			strcpy(queue.name, message.id);
			strcpy(queue.message, message.tail);
			
			int bytes =  msgsnd(messagequeue,&queue,sizeof(t_queuemessage), IPC_NOWAIT);
			if(0 < bytes) {
				log_message(ERROR,"could not send data %s", strerror(errno));
			} else {
				log_message(DEBUG,"%d bytes sent via mq", bytes);
			}
		}	
		
		/* read message queue */
		while(0 < msgrcv(messagequeue,&queue, sizeof(t_queuemessage),TYPE_QUEUE2ROCS,IPC_NOWAIT|MSG_NOERROR)) {

			log_message(INFO, "from queue :%s\n",queue.name);

			memset(&message, 0, sizeof(t_rocsmq_message));
			strcpy(message.sender,PROGNAME);
			strcpy(message.id,queue.name);
			strcpy(message.tail, queue.message);
			rocsmq_send(sock, &message, 0);
		}
		
		/*
		 * wait 1ms
		 */
		rocsmq_delayms(1);
	}

	/*
	 * cleanup
	 */
	// schließe queue
	msgctl(messagequeue,IPC_RMID, 0);

	rocsmq_destroy_thread(thread);
	rocsmq_error(sock);

	if (baseconfig.logtofile)
		closelog();

	return 0;
}



