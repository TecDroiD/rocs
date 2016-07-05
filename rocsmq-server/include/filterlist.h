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
	// list of pp_clientsocket
	p_linkedlist clients;
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
 * find a filter in filter list by comparison
 */
p_filter find_filter(char *id);

/**
 * find a filter in filter list by expression
 */
p_filter match_filter(char *id);  
/**
 * add a socket to a filter
 */   
int filter_add_client(p_filter filter, pp_clientsocket client);
/**
 * add a socket to a filter
 */   
int filter_remove_client(p_filter filter, p_clientsocket client);
/**
 * add a socket to a filter
 */   
int filter_clear_socketlist(p_filter filter);
 
/** 
 * compare function to find filter object by regular expression id
 */ 
int cmp_filter_match_id(p_linkedlist a, p_linkedlist b);
/** 
 * compare function to find filter object by comparing id
 */ 
int cmp_filter_cmp_id(p_linkedlist a, p_linkedlist b);

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


void list_clients(char *dest);
#endif
