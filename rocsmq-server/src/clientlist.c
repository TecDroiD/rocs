#include "clientlist.h"

int num_clients = 0;
p_client clients = NULL;

/**
 * add a client into our array of clients
 */
t_client *add_client(TCPsocket sock, p_rocsmq_clientdata info) {
	clients = (p_client) realloc(clients, (num_clients + 1) * sizeof(t_client));
	memcpy((void *) &clients[num_clients].info, info,
			sizeof(t_rocsmq_clientdata));
	clients[num_clients].sock = sock;
	num_clients++;

	return (&clients[num_clients - 1]);
}


/**
 * remove a client from our array of clients
 */
void remove_client_idx(int i) {
	char *name = clients[i].info.name;

	if (i < 0 && i >= num_clients)
		return;

	/* close the old socket, even if it's dead... */
	SDLNet_TCP_Close(clients[i].sock);

	num_clients--;
	if (num_clients > i)
		memmove(&clients[i], &clients[i + 1],
				(num_clients - i) * sizeof(t_client));
	clients = (t_client*) realloc(clients, num_clients * sizeof(t_client));
	/* server side info */
	/* inform all clients, excluding the old one, of the disconnected user */
}

/**
 * remove a client by it's reference
 */ 
void remove_client(p_client client) {
	
	int index = find_client_idx(client);
	if(index >= 0) {
		remove_client_idx(index);
	}
}


/**
 * get number of clients
 */ 
int count_clients() {
	return num_clients;
}

/**
 * get client array
 */
p_client get_clients() {
	return clients;
}

/**
 * return a single client
 */ 
p_client get_client_idx(int number) {
	if (number >= num_clients) {
		return 0;
	}
	
	return &clients[number];
}

/**
 * return the index of a client.. or -1;
 */ 
int find_client_idx(p_client client) {
	int i;
	for (i = 0; i < count_clients(); i++) {
		if (get_client_idx(i) == client) {
			return i;
		}
	}
	return -1;
}


/**
 * match the filters of a client
 */ 
int filter_match(p_client client, char *message) {
	
	return 0;
}
/**
 * find a client 
 */ 
p_client next_client_by_message(p_client client, char *message) {
	
	static int oldclient = 0;
	p_client check = get_client_idx(oldclient);
	
	// if some unknown client is given, find it.
	if (client != check) {
		oldclient = find_client_idx(client);
	}
	
	// check next client
	oldclient++;
	if ( oldclient < count_clients()) {
		// if it matches the message, return it 
		check = get_client_idx(oldclient);
		if (filter_match(check, message)) {
			return check;
		} 
		
		// else check next client
		return next_client_by_message(check, message);
	}
	return 0;
}
