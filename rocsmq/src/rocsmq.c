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



TCPsocket rocsmq_init(char const *name, p_rocsmq_serveradata server, Uint32 filter, Uint32 mask) {
	TCPsocket sock;
	IPaddress ip;
	
	/* create client information */
	t_rocsmq_message message;
	strncpy(message.sender,name,ROCS_CLIENTNAMESIZE);

		p_rocsmq_clientdata cdata = (p_rocsmq_clientdata) message.tail;
		cdata->filter = filter;
		cdata->mask=mask;
		strncpy(cdata->name,name,ROCS_CLIENTNAMESIZE);



	/*
	 * verbindung aufbauen
	 */
	/* initialize SDL_net */
	if(SDLNet_Init()==-1) return 0;

	if(SDLNet_ResolveHost(&ip,server->ip,server->port)==-1)
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

	printf("sending login info: %s\n", message.sender);
	/*
	 * send client data
	 */
	if (!rocsmq_send(sock,&message, 0)) {
		return 0;
	}

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
			printf("SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
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
				printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
		}
	return result;
}


json_object * rocsmq_get_message_json(p_rocsmq_message mesg) {
	return json_tokener_parse(mesg->tail);
}

int rocsmq_set_message_json(p_rocsmq_message mesg, json_object *object) {
	strncpy (mesg->tail, json_object_to_json_string(object), ROCS_MESSAGESIZE);
}


#endif
