#ifndef CLIENTSOCKETS_H
#define CLIENTSOCKETS_H

#include <SDL/SDL.h>
#include <SDL/SDL_endian.h>
#include <SDL/SDL_error.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_stdinc.h>

#include <rocsmq.h>
#include <linkedlist.h>
#include <log.h>

typedef struct {
	char name[ROCS_CLIENTNAMESIZE];
	TCPsocket socket;
} t_clientsocket, * p_clientsocket;

/**
 * add a client socket to socket list
 */ 
int add_clientsocket(p_rocsmq_clientdata clientdata, TCPsocket sock);

/**
 * compare function to find socket in clientsockets 
 */
int cmp_clientsocket_match_name (p_linkedlist a, p_linkedlist b);

/**
 * compare function to find socket in clientsockets 
 */
int cmp_clientsocket_match_socket (p_linkedlist a, p_linkedlist b);

p_clientsocket get_next_clientsocket(p_clientsocket sock);

#endif
