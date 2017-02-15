/*
 * rocsmq.c
 *
 *  Created on: 29.09.2015
 *      Author: tecdroid
 */

#include "rocsmq.h"

#ifndef tcputil_h
#define tcputil_h 1

char *rocsmq_error() {
	return strerror(errno);
}



int rocsmq_init(p_rocsmq_baseconfig server) {
	int sock;
	struct sockaddr_in servaddr;
	
	/* create client information */
	t_rocsmq_message message;
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
	inet_aton(server->serverip, &servaddr.sin_addr.s_addr);
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
int rocsmq_exit	 (int sock) {
	/*
	 * todo: verbindung abbauen
	 */
	close(sock);
	return 0;
}

/*
 * warten auf daten und transferieren in message
 */
int rocsmq_recv (int sock,p_rocsmq_message mesg, int flags) {
	int result;

	/* receive the length of the string message */
	result = read(sock, mesg, sizeof(t_rocsmq_message));
	if (result < sizeof(t_rocsmq_message)) {
		log_message(ERROR, "Could not read socket");
	}

	return result;

}

/**
 * senden von daten
 */
int rocsmq_send (int sock,p_rocsmq_message mesg, int flags) {
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

int rocsmq_set_message_json(p_rocsmq_message mesg, json_object *object) {
	strncpy (mesg->tail, json_object_to_json_string(object), ROCS_MESSAGESIZE);
}

/**
 * returns true if message matches pattern 
 */ 
int rocsmq_message_match(char *subject, char *pattern) {
	char *s = subject;
	char *p = pattern;

    for (p = pattern; *p != '\0'; p++) {
//		log_message(DEBUG, "pattern : %c", *p);
		if (!p) {
//			log_message(ERROR, "NO P");
			return 0;
		}
        if ((*p) == '*') {
//			log_message(DEBUG,"gg");
            if (*(p + 1) == '\0') {
//				log_message(DEBUG, "done");
                return 1;
            }
            for (s = subject; *s != '\0' ; s++) {
                if (rocsmq_message_match(s, p + 1)) {
//					log_message(DEBUG, "matched");
                    return 1;
                }
            }
            return 0;

       } else {
//			log_message(DEBUG, "subject : %c", *s);
            if (*s == '\0') {
//				log_message(DEBUG, "unexpected end");
                return 0;
            }
            
//			log_message(DEBUG, "compare");
            if (*p != *s) {
//				log_message(DEBUG, "nomatch");
                return 0;				
			}
            s++;
        }
//				log_message(DEBUG, "nomatch");
    }

    /* End of pattern reached.  If this also the end of the subject, then
       match succeeds. */
    return (*s == '\0');
} 

int rocsmq_check_system_message(char *messageid) {
	
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

/**
 * sleep for n milliseconds 
 */ 
int rocsmq_delayms(long ms) {
	usleep (ms * 1000);
}
#endif
