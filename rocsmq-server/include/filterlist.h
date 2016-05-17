#ifndef FILTERLIST_H
#define FILTERLIST_H

#include <SDL/SDL.h>
#include <SDL/SDL_endian.h>
#include <SDL/SDL_error.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_stdinc.h>

#include <fnmatch.h>

#include <rocsmq.h>
#include <linkedlist.h>
#include <log.h>


#include "clientsockets.h"

/*
 * filters for the clients 
 */ 
typedef struct {
	char filter[ROCS_IDSIZE];
	p_linkedlist sockets;
} t_filter, * p_filter;

/**
 * add a filter to filter list
 */ 
p_filter add_filter(char *id);

/**
 * remove a filter from filter list
 */
int remove_filter(char *id);

/**
 * clear filter list
 */
int clear_filterlist();
    
/**
 * find a filter in filter list 
 */
p_filter find_filter(char *id);
  
/**
 * add a socket to a filter
 */   
int filter_add_socket(p_filter filter, TCPsocket socket);
/**
 * add a socket to a filter
 */   
int filter_remove_socket(p_filter filter, TCPsocket socket);
/**
 * add a socket to a filter
 */   
int filter_clear_socketlist(p_filter filter);
 
/** 
 * compare function to find filter object by id
 */ 
int cmp_filter_match_id(p_linkedlist a, p_linkedlist b);

/**
 * compare function to find socket in filter
 */ 
int cmp_socket_match(p_linkedlist a, p_linkedlist b);

/**
 * add a client
 */ 
int add_client(p_rocsmq_clientdata clientdata, TCPsocket socket );

/**
 * remove a client by socket
 */
int remove_client_by_sock(TCPsocket socket);  
/**
 * send message to all objects in filter
 */ 
int send_message_to_filter(p_filter, p_rocsmq_message);

/**
 * send message to all clients
 */ 
int send_all(p_rocsmq_message message);
#endif
