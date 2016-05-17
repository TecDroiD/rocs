#include "clientsockets.h"

p_linkedlist clientinfos;

/**
 * compare function to find socket in clientsockets 
 */
int cmp_clientsocket_match_name (p_linkedlist a, p_linkedlist b) {
	p_clientsocket ca = a->data;
	p_clientsocket cb = b->data;
	
	return strcmp(ca->name, cb->name);
}

/**
 * compare function to find socket in clientsockets 
 */
int cmp_clientsocket_match_socket (p_linkedlist a, p_linkedlist b) {
	p_clientsocket ca = a->data;
	p_clientsocket cb = b->data;

	return cb->socket != ca->socket;
} 

int add_clientsocket(p_rocsmq_clientdata clientdata, TCPsocket socket) {
		// fill clientsocket data
	t_clientsocket cs;
		strcpy(cs.name, clientdata->name);
		cs.socket = socket; 
		
	// add clientsocket data to clientlist	
	p_linkedlist client = ll_create(&cs, sizeof(p_clientsocket));
	clientinfos = ll_add(clientinfos,client,LL_SORT, cmp_clientsocket_match_name);
	if(!clientinfos) {
		return 1;
	}

	return 0;
}


p_clientsocket get_next_clientsocket(p_clientsocket sock) {

	static p_linkedlist old = 0;
	
	if (sock == 0) {
		old = clientinfos;
	} else if (old->data == sock) {
		old = old->next;
	} else {
		p_linkedlist l = ll_create(sock, sizeof(t_clientsocket));
		old = ll_find(clientinfos, l, cmp_clientsocket_match_socket);
		ll_destroy(l);
		old = old->next;
	}
	return old->data;
}
