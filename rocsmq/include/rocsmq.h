/*
 * rocsmq.h
 *
 *  Created on: 29.09.2015
 *      Author: tecdroid
 */

#ifndef ROCSMQ_H_
#define ROCSMQ_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/time.h> // for FDSET
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>

#include <unistd.h>
#include <json-c/json.h>

#include "messages.h"
#include "linkedlist.h"
#include "configparser.h"

#define ROCSMQ_PORT		0x20C5
#define ROCSMQ_POLL		0x01
#define ROCSMQ_IP		"127.0.0.1"

#define ROCS_IDSIZE		256
#define ROCS_MESSAGESIZE 	1000
#define ROCS_CLIENTNAMESIZE 20

#define EMPTY_MESSAGE		0xffffffff

#define MESSAGE_SEPARATOR ","

/**
 * increase abstraction level for socket connector
 */ 
typedef int rocsmq_socket;

/**
 * increase abstraction level for function result
 */
typedef int rocsmq_result;
   
/**
 * data type for the standard message
 */
typedef struct s_rocsmq_message {
	char id[ROCS_IDSIZE];
	char sender[ROCS_CLIENTNAMESIZE];
	char tail[ROCS_MESSAGESIZE];
} t_rocsmq_message, *p_rocsmq_message;


/**
 * client data, contains name and filter set for message routing 
 */
typedef struct s_rocsmq_clientdata {
	char   name[ROCS_CLIENTNAMESIZE];
	char filter[ROCS_IDSIZE];
} t_rocsmq_clientdata, *p_rocsmq_clientdata;


/**
 * initialize rocsmq connection to server
 * @param server base configuration with server connection data
 * @return socket to server
 */  
rocsmq_socket rocsmq_init(p_rocsmq_baseconfig server);
/**
 * disconnect from server
 * @param sock socket to server 
 * @return 0 or error code
 */ 
rocsmq_result rocsmq_exit	(rocsmq_socket sock);
/**
 * receive data
 * @param sock socket to com partner
 * @param mesg pointer to pre allocated message data
 * @param flags currently unused
 * @return number of bytes read
 */ 
rocsmq_result rocsmq_recv (rocsmq_socket sock, p_rocsmq_message mesg, int flags);
/**
 * send data
 * @param sock socket to com partner
 * @param mesg pointer to pre allocated message data
 * @param flags currently unused
 * @return number of bytes written
 */ 
rocsmq_result rocsmq_send (rocsmq_socket sock, p_rocsmq_message mesg, int flags);
/**
 * print last rocsmq error
 * @return error message text
 */ 
char *rocsmq_error();

/**
 * convert message text to json object
 * @param mesg pointer to message data
 * @return pointer to json object
 */ 
json_object * rocsmq_get_message_json(p_rocsmq_message mesg);

/**
 * convert json object to message text
 * @param pointer to pre allocated message data
 * @param object pointer to json object
 * @return 0 or error code
 */ 
rocsmq_result rocsmq_set_message_json(p_rocsmq_message mesg, json_object *object);

/**
 * wildchar match
 * @param subject text to search in
 * @param pattern pattern to search
 * @return 1 if pattern matches or 0 if not
 */ 
rocsmq_result rocsmq_message_match(char *subject, char *pattern);

/**
 * react on all system messages
 * @param messageid id of message to react on
 * @return 0 or error code
 */ 
//rocsmq_result rocsmq_check_system_message(char *messageid);

/**
 * delay for some ms
 * @param ms time to delay
 */ 
void rocsmq_delayms(long ms);

#ifdef __cplusplus
}
#endif

#endif /* ROCSMQ_H_ */
