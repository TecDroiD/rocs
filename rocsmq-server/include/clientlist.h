#ifndef CLIENTLIST_H
#define CLIENTLIST_H

#include <string.h>

#include <SDL/SDL.h>
#include <SDL/SDL_endian.h>
#include <SDL/SDL_error.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_stdinc.h>

#include <linkedlist.h>
#include <log.h>

#include <rocsmq.h>


typedef struct {
	/**
	 * here, you connect with the client
	 */ 
	TCPsocket sock;
	/**
	 * here is all client data you need
	 */ 
	//t_rocsmq_clientdata info;
	/**
	 *  the clients name
	 */ 
	char name[ROCS_CLIENTNAMESIZE];
	/**
	 * here is the list of filters..
	 */ 
	p_linkedlist filters;
} t_client, *p_client;



/**
 * add a client into our array of clients
 */
t_client *add_client(TCPsocket sock, p_rocsmq_clientdata info);

/**
 * remove a client from our array of clients
 */
void remove_client_idx(int i);

/**
 * remove a client by it's reference
 */ 
void remove_client(p_client client);

/**
 * get number of clients
 */ 
int count_clients();

/**
 * get client array
 */
p_client get_clients();  

/**
 * return a specific client
 */ 
p_client get_client_idx(int number);

/**
 * find a client 
 */ 
p_client next_client_by_message(p_client client, char *message);



#endif
