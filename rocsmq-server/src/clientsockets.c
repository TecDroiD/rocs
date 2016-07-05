#include "clientsockets.h"

p_linkedlist clientinfos = 0;
int cnt_clients = 0;

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

/**
 * compare function to find socket in clientsockets 
 */
int cmp_clientsocket_match_addr (p_linkedlist a, p_linkedlist b) {
	pp_clientsocket ca = a->data;
	pp_clientsocket cb = b->data;

	return *ca == *cb;
}

p_clientsocket add_clientsocket(p_rocsmq_clientdata clientdata, TCPsocket socket) {
		// fill clientsocket data
	t_clientsocket cs;
		memset(&cs, 0, sizeof(t_clientsocket));
		strcpy(cs.name, clientdata->name);
		cs.socket = socket; 

	log_message(DEBUG, "adding %s to client list", cs.name);
		
	// add clientsocket data to clientlist	
	p_linkedlist client = ll_create(&cs, sizeof(t_clientsocket));
		
	if (!client) {
		log_message(ERROR, "Could not create client element.");
	}
	
	clientinfos = ll_add(clientinfos,client,LL_BACK, 0);

	p_clientsocket pcs = client->data;
	log_message(DEBUG, "\t->%s added.",pcs->name);

	if(!clientinfos) {
		log_message(ERROR, "Clientinfo list is empty.. dunno why");
		return 0;
	}

	cnt_clients++;
	return client->data;
}

int remove_clientsocket(TCPsocket sock) {
	
	t_clientsocket cs;
		cs.socket = sock; 

	p_linkedlist search = ll_create(&cs, sizeof(t_clientsocket));
	p_linkedlist element = ll_find(clientinfos, search, cmp_clientsocket_match_socket);
	ll_destroy (search);
	
	if (element) {
		clientinfos = ll_remove(clientinfos, element);
		ll_destroy(element);
		cnt_clients--;
	}
}

p_clientsocket get_next_clientsocket(p_clientsocket sock) {

	static p_linkedlist old = 0;
	log_message(DEBUG, "  --> socket = %d, old = %d", sock, old);
	if (sock == 0) {
		log_message(DEBUG, "   --> new list request");
		old = clientinfos;
	} else if (old->data == sock) {
		log_message(DEBUG, "   --> getting next item..");
		old = old->next;
	} else {
		log_message(DEBUG, "   --> searching for item");
		p_linkedlist l = ll_create(sock, sizeof(t_clientsocket));
		old = ll_find(clientinfos, l, cmp_clientsocket_match_socket);
		ll_destroy(l);
		old = old->next;
	}
	
//	log_message(DEBUG, "new element has addr %d", old);
	if (old == 0) return 0;
//	log_message(DEBUG, "   --> with data addr %d", old->data);
	
	return old->data;
}


int num_clients() {
	return cnt_clients;
}
