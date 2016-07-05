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
		filter.clients = 0;
	log_message(DEBUG, "Searching for filter '%s'", id);
	
	p_linkedlist element = ll_create(&filter, sizeof(t_filter));
	
	if(element == 0) return 0;
	
	p_linkedlist search = ll_find(filters, element, cmp_filter_cmp_id);
	ll_destroy(element);
	
	return search;
}

/**
 * find a filter in filter list 
 */
p_linkedlist match_filter_elem(char *id){ 
	t_filter filter;
		strcpy(filter.filter, id);
		filter.clients = 0;
	log_message(DEBUG, "Matching pattern '%s'", id);
	
	p_linkedlist element = ll_create(&filter, sizeof(t_filter));
	if(element == 0) return 0;
	
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
		filter.clients = 0;
		
	p_linkedlist element = ll_create(&filter, sizeof(t_filter));
	filters = ll_add(filters,element, LL_SORT, cmp_filter_match_id);
	
	log_message(DEBUG, "Filter %s has client 0 addr %d", filter.filter, filter.clients);
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
	if(search != 0)
		return search->data;
	return 0;
}

/** 
 * find a filter by it's id
 */ 
p_filter match_filter(char *id){ 
	p_linkedlist search = match_filter_elem(id);
	p_filter f;
	if(search != 0) {
		f = search->data;
		return f;
	}
	log_message(DEBUG, "no matching element found");
	return 0;
}
  
/**
 * add a socket to a filter
 */   
int filter_add_client(p_filter filter, pp_clientsocket client){ 
	
	log_message(DEBUG, "adding client %s to filter %s", (*client)->name, filter->filter);
	p_linkedlist element = ll_create(client, sizeof(p_clientsocket));
	
	filter->clients = ll_add(filter->clients, element, LL_BACK, 0);
	return 0;
}
/**
 * add a socket to a filter
 */   
int filter_remove_socket(p_filter filter,pp_clientsocket client){ 
	p_linkedlist element = ll_create(client, sizeof(p_clientsocket));
	p_linkedlist search = ll_find(filter->clients, element, cmp_clientsocket_match_addr);
	ll_destroy(element);
	
	if (search != 0) {
		filter->clients = ll_remove(filter->clients, search);
		ll_destroy(search);
		return 0;
	} 
	return 1;	
}

/**
 * add a socket to a filter
 */   
int filter_clear_socketlist(p_filter filter){ 
	ll_destroy_list(filter->clients);
	filter->clients = 0;
}
 

/** 
 * compare function to find filter object by id
 */ 
int cmp_filter_match_id(p_linkedlist a, p_linkedlist b){ 
	p_filter fa = a->data;
	p_filter fb = b->data;
	char subject[ROCS_IDSIZE];
	char pattern[ROCS_IDSIZE];
	strcpy(subject, fa->filter);
	strcpy(pattern, fa->filter);
	
		log_message(DEBUG, "test '%s' ~ '%s'", fa->filter, fb->filter);
	
	return (1 == rocsmq_message_match(subject, pattern));
}

/** 
 * compare function to find filter object by id
 */ 
int cmp_filter_cmp_id(p_linkedlist a, p_linkedlist b){ 
	p_filter fa = a->data;
	p_filter fb = b->data;
//		log_message(DEBUG, "test '%s' == '%s'", fa->filter, fb->filter);

	return (strcmp(fa->filter, fb->filter));
}



/**
 * send message to all objects in filter
 */ 
int send_message_to_filter(p_filter filter, p_rocsmq_message message){ 
	p_linkedlist socket = filter->clients;
	p_linkedlist next;
	pp_clientsocket pc;	
	p_clientsocket client;
	
	
	while(socket != 0) {
		pc = socket->data;
		client = *pc;
		log_message(DEBUG, "    --> client name %s", client->name);

		if (rocsmq_send(client->socket, message, 0)) {
			log_message(DEBUG, "    --> sent.");
			
		} else {
			ll_remove(filter->clients, socket);
			remove_clientsocket(client->socket);
		}

		socket = socket->next;
	}
	
	return 0;
}

/**
 * add a client
 */ 
int add_client(p_rocsmq_clientdata clientdata, TCPsocket socket ) {
	char * tok;
	p_filter filter;
	p_clientsocket clientsocket = 0; 
	
	clientsocket = add_clientsocket(clientdata, socket);
	if(!clientsocket) {
		log_message(ERROR, "could not add clientsocket for client %s", clientsocket->name);
		return -1;
	}
	log_message(DEBUG, "	-> %s added", clientsocket->name);
	// for each filter in clientinfo
	
	tok = strtok(clientdata->filter, ",");
	

	while (tok != 0) {

		// find or create matching filter
		filter = find_filter(tok);
		if (! filter) {
			log_message(DEBUG, "%s has no filter yet. Adding..", tok);
			filter = add_filter(tok);
		}
		

		// add socket to filter
		if (filter_add_client(filter,&clientsocket)) {
			log_message(ERROR, "Could not add socket");

			return -2;
		}
		
		tok = strtok(0, ",");

	}
	
	return 0;
}

/**
 * remove a client by socket
 */
int remove_client(p_clientsocket client) {
	
	return 0;
}


/**
 * send message by filterlist
 */ 
int send_all(p_rocsmq_message message) {
	log_message(DEBUG, "sending message (id : %s) ", message->id);
	p_filter filter = match_filter(message->id);
	
	if( filter ) {
		log_message (DEBUG, "  --> to filter %s", filter->filter);
		send_message_to_filter(filter, message);
		return 0;
	}
	
	return -1;
}

void list_filterclients(p_linkedlist clients) {
	p_linkedlist elem = clients;
	pp_clientsocket c;
	printf("[");
	while(elem != 0) {
		c = elem->data;
		printf("[%s]",(*c)->name);
		elem = elem->next;
	}
	printf("]");
}
/**
 * show clients
 */
void list_clients(char *dest) {
	p_linkedlist elem = filters;

	p_filter filter;
	printf("[[");
	while (elem != 0) {
		filter = elem->data;
		printf( "\t\"%s\":",filter->filter);
		list_filterclients(filter->clients);
		printf( ";\n");
		elem = elem->next;
	}
	printf( "]]\n",dest);
	
}  
