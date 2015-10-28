/*
 * rocsmq.h
 *
 *  Created on: 29.09.2015
 *      Author: tecdroid
 */

#ifndef ROCSMQ_H_
#define ROCSMQ_H_

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <json-c/json.h>

#include "linkedlist.h"
#include "configparser.h"

#define ROCSMQ_PORT		0x20C5
#define ROCSMQ_POLL		0x01
#define ROCSMQ_IP		"127.0.0.1"

#define ROCS_MESSAGESIZE 	1000
#define ROCS_CLIENTNAMESIZE 20

/**
 * data type for the standard message
 */
typedef struct s_rocsmq_message {
	Uint32 id;
	char sender[ROCS_CLIENTNAMESIZE];
	char tail[ROCS_MESSAGESIZE];
} t_rocsmq_message, *p_rocsmq_message;


/*
 *
 */
typedef struct s_rocsmq_clientdata {
	char   name[ROCS_CLIENTNAMESIZE];
	Uint32 filter;
	Uint32 mask;
} t_rocsmq_clientdata, *p_rocsmq_clientdata;


TCPsocket rocsmq_init(p_rocsmq_baseconfig server) ;
int rocsmq_exit	(TCPsocket sock);
int rocsmq_recv (TCPsocket sock, p_rocsmq_message mesg, int flags);
int rocsmq_send (TCPsocket sock, p_rocsmq_message mesg, int flags);
char *rocsmq_error();

json_object * rocsmq_get_message_json(p_rocsmq_message mesg);
int rocsmq_set_message_json(p_rocsmq_message mesg, json_object *object);

#endif /* ROCSMQ_H_ */
