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
	return SDLNet_GetError();
}



TCPsocket rocsmq_init(p_rocsmq_baseconfig server) {
	TCPsocket sock;
	IPaddress ip;
	
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
	/* initialize SDL_net */
	if(SDLNet_Init()==-1) return 0;

	if(SDLNet_ResolveHost(&ip,server->serverip,server->port)==-1)
	{
		SDLNet_Quit();
		return 0;
	}

	/* open the server socket */
	sock=SDLNet_TCP_Open(&ip);
	if(!sock)
	{
		SDLNet_Quit();
		return 0;
	}

	log_message(DEBUG,"sending login info: %s\n", message.sender);
	log_message(DEBUG," -- %d, %s \n", sock, message.tail);
	/*
	 * send client data
	 */
	if (!rocsmq_send(sock,&message, 0)) {
		return 0;
	}

	log_message(DEBUG, "successfully logged in");
	return sock;
}
/**
 * exit rocsmq system
 */
int rocsmq_exit	 (TCPsocket sock) {
	/*
	 * todo: verbindung abbauen
	 */
	SDLNet_TCP_Close(sock);
	SDLNet_Quit();
	return 0;
}

/*
 * warten auf daten und transferieren in message
 */
int rocsmq_recv (TCPsocket sock,p_rocsmq_message mesg, int flags) {
	Uint32 result;

	/* receive the length of the string message */
	result = SDLNet_TCP_Recv(sock, mesg, sizeof(t_rocsmq_message));
	if (result < sizeof(t_rocsmq_message)) {
		if (SDLNet_GetError() && strlen(SDLNet_GetError())) /* sometimes blank! */
			log_message(ERROR, "SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
	}

	return result;

}

/**
 * senden von daten
 */
int rocsmq_send (TCPsocket sock,p_rocsmq_message mesg, int flags) {
	Uint32 result;
	result=SDLNet_TCP_Send(sock,mesg,sizeof(t_rocsmq_message));
		if(result<sizeof(t_rocsmq_message)) {
			if(SDLNet_GetError() && strlen(SDLNet_GetError())) /* sometimes blank! */
				log_message(DEBUG, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
		}
	
	return result;
}


json_object * rocsmq_get_message_json(p_rocsmq_message mesg) {
	return json_tokener_parse(mesg->tail);
}

int rocsmq_set_message_json(p_rocsmq_message mesg, json_object *object) {
	strncpy (mesg->tail, json_object_to_json_string(object), ROCS_MESSAGESIZE);
}


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

#endif
