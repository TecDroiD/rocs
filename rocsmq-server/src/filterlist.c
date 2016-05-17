#include "filterlist.h"

/*
 * list of filters
 */
p_linkedlist filters;

/**
 * find a filter in filter list 
 */
p_linkedlist find_filter_elem(char *id){ 
	t_filter filter;
		strcpy(filter.filter, id);
		filter.sockets = 0;
		
	p_linkedlist element = ll_create(&filter, sizeof(t_filter));
	p_linkedlist search = ll_find(filters, element, cmp_filter_match_id);
	ll_destroy(element);
	
	return search;
}

/**
 * add a filter to filter list
 */ 
p_filter add_filter(char *id){ 
	t_filter filter;
		strcpy(filter.filter, id);
		filter.sockets = 0;
		
	p_linkedlist element = ll_create(&filter, sizeof(t_filter));
	filters = ll_add(filters,element, LL_SORT, cmp_filter_match_id);
	
	return element->data;
}

/**
 * remove a filter from filter list
 */
int remove_filter(char *id){ 
	p_linkedlist search = find_filter_elem(id);
	
	if(search != 0) {
		filters = ll_remove(filters, search);
		filter_clear_socketlist(search->data);
		ll_destroy(search);
		
		return 0;
	}
	
	return 1;
	
}

/**
 * clear filter list
 */
int clear_filterlist(){ 
	p_linkedlist old;
	while(filters != 0) {
		old = filters;
		filter_clear_socketlist(old->data);
		filters = ll_remove(filters, old);
		ll_destroy(old);
	}
}
    
/** 
 * find a filter by it's id
 */ 
p_filter find_filter(char *id){ 
	p_linkedlist search = find_filter_elem(id);
	return search->data;
}
  
/**
 * add a socket to a filter
 */   
int filter_add_socket(p_filter filter, TCPsocket socket){ 
	p_linkedlist element = ll_create(socket, sizeof(TCPsocket));
	filter->sockets = ll_add(filter->sockets, element, LL_BACK, 0);
	
	return 0;
}
/**
 * add a socket to a filter
 */   
int filter_remove_socket(p_filter filter, TCPsocket socket){ 
	p_linkedlist element = ll_create(socket, sizeof(TCPsocket));
	p_linkedlist search = ll_find(filter->sockets, element, cmp_socket_match);
	ll_destroy(element);
	
	if (search != 0) {
		filter->sockets = ll_remove(filter->sockets, search);
		ll_destroy(search);
		return 0;
	} 
	return 1;	
}

/**
 * add a socket to a filter
 */   
int filter_clear_socketlist(p_filter filter){ 
	ll_destroy_list(filter->sockets);
	filter->sockets = 0;
}
 

/** 
 * compare function to find filter object by id
 */ 
int cmp_filter_match_id(p_linkedlist a, p_linkedlist b){ 
	p_filter fa = a->data;
	p_filter fb = b->data;
	
	return (1 == rocsmq_message_match(fa->filter, fb->filter));
}

/**
 * compare function to find socket in filter
 */ 
int cmp_socket_match(p_linkedlist a, p_linkedlist b){ 
	TCPsocket * sa = a->data;
	TCPsocket * sb = b->data;
	
	return sb - sa;
}


int remove_socket() {
	
}

/**
 * send message to all objects in filter
 */ 
int send_message_to_filter(p_filter filter, p_rocsmq_message message){ 
	p_linkedlist socket = filter->sockets;
	p_linkedlist next;	
//	TCPsocket *sock;
	
	while(socket != 0) {
		next = socket;
//			sock = socket->data;
			if (rocsmq_send(socket->data, message, 0)) {
			} else {
				remove_client_by_sock(socket->data);
			}
		
		socket = next;
	}
	
	return 0;
}

/**
 * add a client
 */ 
int add_client(p_rocsmq_clientdata clientdata, TCPsocket socket ) {
	char * tok;
	p_filter filter;

	if(add_clientsocket(clientdata, socket)) {
		return -1;
	}
	
	// for each filter in clientinfo
	tok = strtok(clientdata->filter, ",");
	while (tok != 0) {
		// find or create matching filter
		filter = find_filter(tok);
		if (! filter) {
			filter = add_filter(tok);
		}
		
		// add socket to filter
		if (filter_add_socket(filter,socket)) {
			return -2;
		}
	}
	
	return 0;
}

/**
 * remove a client by socket
 */
int remove_client_by_sock(TCPsocket socket) {
	
	return 0;
}


/**
 * send message by filterlist
 */ 
int send_all(p_rocsmq_message message) {
	p_filter filter = find_filter(message->id);
	if( filter ) {
		send_message_to_filter(filter, message);
		return 0;
	}
	
	return -1;
}
