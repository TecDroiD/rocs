/*
 * rocsmq.c
 *
 *  Created on: 29.09.2015
 *      Author: tecdroid
 */

#include "rocsmq.h"

#ifndef tcputil_h
#define tcputil_h 1

/*
 *
 */  
char *rocsmq_error() {
	return strerror(errno);
}


/*
 * 
 */ 
rocsmq_socket rocsmq_init(p_rocsmq_baseconfig server) {
	rocsmq_socket sock;
	struct sockaddr_in servaddr;
	
	/* create client information */
	t_rocsmq_message message;
	memset(&message, 0, sizeof(t_rocsmq_message));
	message.id[0] = '\0';
	strncpy(message.sender,server->clientname,ROCS_CLIENTNAMESIZE);

		p_rocsmq_clientdata cdata = (p_rocsmq_clientdata) message.tail;
		strncpy(cdata->filter,server->filter, ROCS_IDSIZE);
		strncpy(cdata->name,server->clientname,ROCS_CLIENTNAMESIZE);

	/*
	 * verbindung aufbauen
	 */

	/* create socket socket */
	sock=socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sock) {
		log_message(ERROR, "Could not create socket");
		return 0;
	}

	/* create server connection */
	servaddr.sin_family = AF_INET;
	inet_aton(server->serverip, &servaddr.sin_addr);
	servaddr.sin_port=htons(server->port);

	/* connect server */
	if (0 > connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr))) {
		log_message(ERROR, "could not connect to server.");
		return 0; 
	}
	
	/*
	 * send client data
	 */
	log_message(DEBUG,"sending login info: %s\n", message.sender);
	log_message(DEBUG," -- %d, %s \n", sock, message.tail);
	if (!rocsmq_send(sock,&message, 0)) {
		close(sock);
		return 0;
	}

	log_message(DEBUG, "successfully logged in");
	return sock;
}
/**
 * exit rocsmq system
 */
rocsmq_result rocsmq_exit	 (rocsmq_socket sock) {
	/*
	 * todo: verbindung abbauen
	 */
//	int iMode = 0; 
//	int result = ioctlsocket(sock, FIONBIO, &iMode); 
	
	close(sock);
	return 0;
}

/*
 * warten auf daten und transferieren in message
 */
rocsmq_result rocsmq_recv (rocsmq_socket sock,p_rocsmq_message mesg, int flags) {
	int result;
	log_message(DEBUG, "reading Message from socket %d", sock);
	/* cleaning up is never bad */
	memset(mesg, 0, sizeof(t_rocsmq_message));
	
	/* receive the length of the string message */
	result = read(sock, mesg, sizeof(t_rocsmq_message));

	if (result < 0 ) {
		log_message(ERROR, "Could not read socket: ", rocsmq_error());
	}

	log_message(DEBUG, "Having %d bytes", result);
	return result;

}

/**
 * senden von daten
 */
rocsmq_result rocsmq_send (rocsmq_socket sock,p_rocsmq_message mesg, int flags) {
	int result;
	result=write(sock,mesg,sizeof(t_rocsmq_message));
	if(result<sizeof(t_rocsmq_message)) {
		log_message(ERROR, "Could not write socket");
	}
	
	return result;
}


json_object * rocsmq_get_message_json(p_rocsmq_message mesg) {
	enum json_tokener_error error;
	json_object * obj = json_tokener_parse_verbose(mesg->tail, &error);
	int i;
	if (error != json_tokener_success) {
		log_message(ERROR,"Json Parse: %s", json_tokener_error_desc(error));
		return 0;
	}
	return obj; 
}

rocsmq_result rocsmq_set_message_json(p_rocsmq_message mesg, json_object *object) {
	strncpy (mesg->tail, json_object_to_json_string(object), ROCS_MESSAGESIZE);
}

/**
 * returns true if message matches pattern 
 */ 
rocsmq_result rocsmq_message_match(char *subject, char *pattern) {
	char *s = subject;
	char *p = pattern;

    for (p = pattern; *p != '\0'; p++) {
		if (!p) {
			return 0;
		}
        if ((*p) == '*') {
            if (*(p + 1) == '\0') {
                return 1;
            }
            for (s = subject; *s != '\0' ; s++) {
                if (rocsmq_message_match(s, p + 1)) {
                    return 1;
                }
            }
            return 0;

       } else {
            if (*s == '\0') {
                return 0;
            }
            
            if (*p != *s) {
                return 0;				
			}
            s++;
        }
    }

    /* End of pattern reached.  If this also the end of the subject, then
       match succeeds. */
    return (*s == '\0');
} 

/*
 * react on system message
rocsmq_result rocsmq_check_system_message(char *messageid) {
	
	char id[ROCS_IDSIZE];
	strcpy(id, messageid);

//	log_message(DEBUG, "( %s == %s )",id, MESSAGE_FILTER_SYSTEM);
	if (rocsmq_message_match(messageid, MESSAGE_FILTER_SYSTEM)) {
		if(0 == strcmp(messageid, CREATE_CLIENTORDER(MESSAGE_ID_SYSTEM, MESSAGE_ID_SHUTDOWN))) {		
			rocsmq_thread_set_running(0);
		}
		
		return 1;
	} 
	return 0;
}
 */ 

/**
 * sleep for n milliseconds 
 */ 
void rocsmq_delayms(long ms) {
	usleep (ms * 1000);
}
#endif
