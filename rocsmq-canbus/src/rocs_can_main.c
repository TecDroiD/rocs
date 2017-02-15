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

#define CLIENTNAME "can"
#define PROGNAME "rocsmq-canbus"
#define CONFIGFILE "conf/"PROGNAME".conf"
#define PROG_FILTER (MESSAGE_ID_ACTOR | MESSAGE_CLIENT_CAN)
#define PROG_MASK	(MESSAGE_MASK_MAIN | MESSAGE_MASK_CLIENT)

t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.port = ROCSMQ_PORT,
	.filter = MESSAGE_CLIENT_CAN,
	.rundaemon = 0,
	.loglevel = INFO,
	.logfile = "",
	.clientname = CLIENTNAME,
};
	
t_clientconfig custom_config = {
	.kbaud = 125,
	.devicefile = "/dev/can0\0",
	.messagemap = 0,
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

#define ORDER_SEND		"can.send"
#define ORDER_ADDMAP	"can.addmap"
#define ORDER_DELMAP	"can.delmap"
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

	if(0 == strcmp(message->id, ORDER_SEND)) {
		// get can bus message id
		get_intval(json, CAN_MESSAGE_ID, &id);
		can.id = id;
		// get can message data
		get_stringval(json, CAN_MESSAGE, data, 250);
		// decode data
		b64decode(data,decoded,250);
		strncpy(can.data,decoded, CAN_MSG_LENGTH);
		// send data via can bus
		can_send(&can);

		return 0;
	} else if (0 == strcmp(message->id, ORDER_ADDMAP)) {
		// create map entry item
		t_messagemap map;
		memset(&map, 0, sizeof(t_messagemap));
		// parse map entry
		parse_mapentry(json, &map);
		add_messagemap(&custom_config,&map);
		
	} else if (0 == strcmp(message->id, ORDER_DELMAP)) {
		// create map entry item
		t_messagemap map;
		memset(&map, 0, sizeof(t_messagemap));
		// parse map entry
		parse_mapentry(json, &map);
		del_messagemap(&custom_config,&map);
		
	}
	return 1;
}

void create_rocs_message(canmsg_t * can, p_rocsmq_message message) {
	json_object *json;
	char b64data[250];
	// get message id from map or generate id
	t_messagemap map;
	if (get_message(&custom_config, can->id, 0, &map, 0)) {
		strncpy (message->id, map.message, ROCS_IDSIZE);
	} else {
		strncpy (message->id, CREATE_CLIENTORDER(MESSAGE_ID_SENSOR, MESSAGE_CLIENT_CAN), ROCS_IDSIZE);
	}
	
	// encode binary data for 
	b64encode(can->data,b64data,250);
	memset(message->tail,'\0',ROCS_MESSAGESIZE);
	sprintf(message->tail, "{\""CAN_MESSAGE_ID"\":%d,\""CAN_MESSAGE"\":\"%s\"}",can->id,b64data);
	log_message(DEBUG,"sending received can message %s, %s", message->id, message->tail);
}

/**
 * main function
 */
int main(int argc, char **argv) {
	int sock;
	pthread_t thread;
	t_rocsmq_message message;
	canmsg_t canmessage;
	
	
	int recv;

	/* parse configuration file */
//	parseconfig(CONFIGFILE, &baseconfig, CLIENTNAME, canbus_custom_config, &customconfig);
	// parse configuration
	if (argc <= 1) {
		parseconfig(CONFIGFILE, &baseconfig, CLIENTNAME, canbus_custom_config, &custom_config);
	} else if (argc == 2) {
		parseconfig(argv[1], &baseconfig, CLIENTNAME, canbus_custom_config, &custom_config);
	} else {
		printf("Usage: %s [configfile]\n", argv[0]);
		exit(EXIT_FAILURE);
	}


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
	can_init(custom_config.devicefile, custom_config.kbaud);

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

		// if can bus has something to say
		recv = can_recv(&canmessage);
		if (recv) {
			// send a new rocs message
			create_rocs_message(&canmessage, &message);
			rocsmq_send(sock,&message,0);
		}
		/*
		 * wait 100ms
		 */
		rocsmq_delayms(10);
	}

	/*
	 * cleanup
	 */
	can_uninit();
	rocsmq_destroy_thread(thread);
	rocsmq_exit(sock);

	closelog();

	return 0;
}
