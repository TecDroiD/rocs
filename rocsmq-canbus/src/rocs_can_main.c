/*
 * client_main.c
 *
 *  Created on: 06.10.2015
 *      Author: tecdroid
 */

#include <daemonizer.h>
#include <getopt.h>
#include <log.h>
#include <rocsmq.h>
#include <rocsmqthread.h>
#include <configparser.h>
#include <messages.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_thread.h>

#include "canbus.h"
#include "customconfig.h"

#define PROGNAME "rocsmq-canbus"
#define CONFIGFILE "conf/"PROGNAME".conf"
#define PROG_FILTER (MESSAGE_ID_ACTOR | MESSAGE_CLIENT_CAN)
#define PROG_MASK	(MESSAGE_MASK_MAIN | MESSAGE_MASK_CLIENT)

t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.port = ROCSMQ_PORT,
	.filter = 0,
	.mask = 0,
	.rundaemon = 0,
	.loglevel = INFO,
	.logfile = "",
	.clientname = PROGNAME,
};
	
t_clientconfig customconfig = {
	.kbaud = 125,
	.devicefile = "/dev/can0\0",
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

/*
 * print program usage
 */
void print_usage (void) {
	printf("rocsmq client - usage\n" );
	printf(" rocsmq_client [options]\n" );
	printf(" options\n --------\n" );
	printf(" -D : Set output level to DEBUG\n" );
	printf(" -I : Set output level to INFO\n" );
	printf(" -W : Set output level to WARNING\n" );
	printf(" -E : Set output level to ERROR\n" );
	printf(" -S : Output log to console(standard)\n" );
	printf(" -d : Run as daemon\n" );
	printf(" -l [file] : Output log to file [file]\n" );
	printf(" -p [port] : Set listener port\n" );
	printf(" -c [file] : Set can bus device (std: /dev/can0)\n");
	printf(" -b [baud] : Set baudrate in kbaud\n");
}

/**
 * get functional options
 */
int getoptions (int argc, char **argv) {
	int opt;
	while((opt = getopt(argc, argv, "DIEWSl:dp:c:b:"))!= -1) {
		switch(opt) {
		case 'D': baseconfig.loglevel = DEBUG; break; /* Debug level DEBUG */
		case 'I': baseconfig.loglevel = INFO; break; /* Debug level INFO */
		case 'E': baseconfig.loglevel = ERROR; break; /* Debug level ERROR */
		case 'W': baseconfig.loglevel = WARNING; break; /* Debug level WARNING */
		case 'l': strncpy(baseconfig.logfile,optarg,255); break; /* log to file [filename] */
		case 'd': baseconfig.rundaemon = 1; break;
		case 'p': baseconfig.port = atoi(optarg); break;
		case 'b': customconfig.kbaud = atoi(optarg); break;
		case 'c': strncpy(customconfig.devicefile, optarg, 255); break;
		default:
			print_usage();
			return -1;
		}
	}

	return 0;
}

#define CAN_MESSAGE_ID 	"messageid"
#define CAN_MESSAGE 	"message64"
/*
 * handle rocsmq message
 */
int handle_message(p_rocsmq_message message) {
	char data[1000];
	char decoded[250];
	int id;
	canmsg_t can;
	json_object *json = rocsmq_get_message_json(message);
	get_intval(json, CAN_MESSAGE_ID, &id);
	can.id = id;
	get_stringval(json, CAN_MESSAGE, data, 250);
	b64decode(data,decoded,250);
	strncpy(can.data,decoded, CAN_MSG_LENGTH);
	can_send(&can);

	return 0;
}

void create_rocs_message(canmsg_t * can, p_rocsmq_message message) {
	json_object *json;
	char b64data[250];

	message->id = MESSAGE_ID_SENSOR | MESSAGE_CLIENT_CAN;
	b64encode(can->data,b64data,250);
	memset(message->tail,'\0',ROCS_MESSAGESIZE);
	sprintf(message->tail, "{\""CAN_MESSAGE_ID"\":%d,\""CAN_MESSAGE"\":\"%s\"}",can->id,b64data);
	log_message(DEBUG,"sending received can message %s", message->tail);
}

/**
 * main function
 */
int main(int argc, char **argv) {
	TCPsocket sock;
	SDL_Thread *thread;
	t_rocsmq_message message;
	canmsg_t canmessage;
	
	
	int recv;

	/* initialize sdl */
	SDL_Init(0);
	
	/* parse configuration file */
	parseconfig(CONFIGFILE, &baseconfig, canbus_custom_config, &customconfig);

	/* get options */
	if(getoptions(argc, argv))
		exit(1);

	/* daemonize if neccessary */
	if(baseconfig.rundaemon) {
		if (0 != daemonize("~", client_signal_handler))
			return -1;
	}

	/*
	 * initialize logging system
	 */
	openlog(PROGNAME, baseconfig.logfile);
	log_setlevel(baseconfig.loglevel);

	log_message(DEBUG, PROGNAME " starting..");

	/* do initialization stuff */
	can_init(customconfig.devicefile, customconfig.kbaud);

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
			log_message(DEBUG, "incoming:%s\n",message.tail);
			handle_message(&message);

		}

		recv = can_recv(&canmessage);
		if (recv) {
			create_rocs_message(&canmessage, &message);
			rocsmq_send(sock,&message,0);
		}
		/*
		 * wait 100ms
		 */
		SDL_Delay(100);
	}

	/*
	 * cleanup
	 */
	can_uninit();
	rocsmq_destroy_thread(thread);
	rocsmq_error(sock);

	closelog();

	SDL_Quit();

	return 0;
}



