/*
 * server.c
 *
 *  Created on: 29.09.2015
 *      Author: tecdroid
 */

#include <getopt.h>
#include <log.h>
#include <configparser.h>
#include <messages.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_endian.h>
#include <SDL/SDL_error.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_stdinc.h>

#include <rocsmq.h>
#include <daemonizer.h>
//#include <unistd.h>

#define CONFIGFILE "conf/rocsmq-server.config"

t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.port = ROCSMQ_PORT,
	.rundaemon = 0,
	.loglevel = INFO,
	.logfile = "",
	.clientname = "rocsmq-server",
	};

typedef struct {
	TCPsocket sock;
	t_rocsmq_clientdata info;
} t_client, *p_client;

p_client clients = NULL;
int num_clients = 0;
TCPsocket server;


/**
 * create a socket set that has the server socket and all the client sockets
 */
SDLNet_SocketSet create_sockset();

/**
 * match message filter
 */
int filtermatch(p_rocsmq_message message, p_rocsmq_clientdata client);
/**
 * send message to all clients which match the filter
 */
void send_all(p_rocsmq_message message);

/**
 * receive filter info from client
 */
Uint32 read_clientdata(TCPsocket sock, p_rocsmq_clientdata client);

/**
 * add a client into our array of clients
 */
t_client *add_client(TCPsocket sock, p_rocsmq_clientdata info);

/**
 * remove a client from our array of clients
 */
void remove_client(int i);

/**
 * print program usage
 */
void print_usage(void);

/**
 * signal handler for the server 
 */ 
void server_signal_handler(int sig);

/**
 * handle messages for infrastructure
 */
int handle_message(p_rocsmq_message message);
  
/**
 * quit the system
 */   
void quit (void);  

/**
 * server main routine
 */
int main(int argc, char **argv) {
	IPaddress ip;
	TCPsocket sock;
	SDLNet_SocketSet set;
	t_rocsmq_message message;
	const char *host = NULL;
	Uint32 ipaddr;

	// parse options
	switch (argc) {
		case 1: parseconfig(CONFIGFILE, &baseconfig, 0 ,0);
			break;
		case 2: parseconfig(argv[1], &baseconfig, 0 ,0);
			break;
		default:
			printf("Usage: %s [configfile]\n", argv[0]);
			exit(EXIT_FAILURE);
	}

	/* daemonize if neccessary */
	if(baseconfig.rundaemon) {
		log_message(DEBUG, "daemonizing...\n");
		if(strlen(baseconfig.logfile) == 0) {
			log_message(DEBUG, "  -> creating std-logfilename"); 
		  sprintf(baseconfig.logfile, "/tmp/log.out");
		}
		
		if (0 != daemonize("~", server_signal_handler)) {
			exit(0);
		}
	}
	
	log_message(DEBUG, "	-> done...");

	/*
	 * initialize logging system
	 */
	openlog(baseconfig.clientname, baseconfig.logfile);
	log_setlevel(baseconfig.loglevel);
	log_message(DEBUG,  "%s starting server ..", baseconfig.clientname);

	/* initialize SDL */
	if (SDL_Init(0) == -1) {
		log_message(ERROR,"SDL_Init: %s\n", SDL_GetError());
		exit(1);
	}

	/* initialize SDL_net */
	if (SDLNet_Init() == -1) {
		log_message(ERROR,"SDLNet_Init: %s\n", SDLNet_GetError());
		SDL_Quit();
		exit(2);
	}

	/* Resolve the argument into an IPaddress type */
	if (SDLNet_ResolveHost(&ip, NULL, baseconfig.port) == -1) {
		log_message(ERROR,"SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		SDLNet_Quit();
		SDL_Quit();
		exit(3);
	}

	/* perform a byte endianess correction for the next printf */
	ipaddr = SDL_SwapBE32(ip.host);

	/* output the IP address nicely */
	log_message(DEBUG,"IP Address : %d.%d.%d.%d\n", ipaddr >> 24, (ipaddr >> 16) & 0xff,
			(ipaddr >> 8) & 0xff, ipaddr & 0xff);

	/* resolve the hostname for the IPaddress */
	host = SDLNet_ResolveIP(&ip);

	/* open the server socket */
	server = SDLNet_TCP_Open(&ip);
	if (!server) {
		log_message(ERROR,"SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		SDLNet_Quit();
		SDL_Quit();
		exit(4);
	}

	log_message(INFO,"Server started. waiting for clients");
	while (1) {
		int numready, i;

		set = create_sockset();
		numready = SDLNet_CheckSockets(set, (Uint32) -1);
		if (numready == -1) {
			log_message(ERROR,"SDLNet_CheckSockets: %s\n", SDLNet_GetError());
			break;
		}
		if (!numready)
			continue;
		if (SDLNet_SocketReady(server)) {
			numready--;
			/*printf("Connection...\n"); */
			sock = SDLNet_TCP_Accept(server);
			if (sock) {
				t_rocsmq_clientdata clientinfo;

				/*printf("Accepted...\n"); */
				if ( 0 != read_clientdata(sock, &clientinfo)) {
					log_message(DEBUG,"welcoming client %s",clientinfo.name);
					log_message(DEBUG," filter : %d, filtermask : %d", clientinfo.filter, clientinfo.mask);
					add_client(sock, &clientinfo);
				} else {
					log_message(ERROR,"could not connect client: %s",rocsmq_error());
					SDLNet_TCP_Close(sock);
				}
			}
		}
		/**
		 * receive message and send it to all
		 */
		log_message(DEBUG,"clients: %d, numready: %d", num_clients,numready);
		for (i = 0; numready && i < num_clients; i++) {
			if (SDLNet_SocketReady(clients[i].sock)) {
				// receive message
				if (rocsmq_recv(clients[i].sock, (p_rocsmq_message) & message,
						ROCSMQ_POLL)) {
					numready--;
					log_message(DEBUG,"received message\nid is %d\nsender is %s\ntail is %s\n",
								message.id, message.sender,message.tail);

					// send message to all
					send_all( & message);
					
					// handle message if it's for the infrastructure		
					if(message.id & MESSAGE_ID_SYSTEM) {
						handle_message(&message);
					}
				} else {
					log_message(DEBUG,"removing client %s\n", clients[i].info.name);
					remove_client(i);
				}
			}
		}
	}

	quit();
}

/**
 * add a client into our array of clients
 */
t_client *add_client(TCPsocket sock, p_rocsmq_clientdata info) {
	clients = (p_client) realloc(clients, (num_clients + 1) * sizeof(t_client));
	memcpy((void *) &clients[num_clients].info, info,
			sizeof(t_rocsmq_clientdata));
	clients[num_clients].sock = sock;
	num_clients++;
	/* server side info */
	//printf("--> %s\n",name);
	return (&clients[num_clients - 1]);
}


/**
 * remove a client from our array of clients
 */
void remove_client(int i) {
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
 * create a socket set that has the server socket and all the client sockets
 */
SDLNet_SocketSet create_sockset() {
	static SDLNet_SocketSet set = NULL;
	int i;

	if (set)
		SDLNet_FreeSocketSet(set);
	set = SDLNet_AllocSocketSet(num_clients + 1);
	if (!set) {
		printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		exit(1); /*most of the time this is a major error, but do what you want. */
	}
	SDLNet_TCP_AddSocket(set, server);
	for (i = 0; i < num_clients; i++)
		SDLNet_TCP_AddSocket(set, clients[i].sock);
	return (set);
}

/**
 * match message filter
 */
int filtermatch(p_rocsmq_message message, p_rocsmq_clientdata client) {
	return (message->id & client->mask)==(client->filter & client->mask);
}
/**
 *  send a buffer to all clients
 */
void send_all(p_rocsmq_message message) {
	int cindex;
	if (!message || !num_clients) return;
	cindex = 0;
	log_message(DEBUG, "Sending message '%s'", message->tail);
	while (cindex < num_clients) {
		/* send to all clients that macht the filter */
		/* with error checking */
		if (message->id & MESSAGE_ID_SYSTEM || filtermatch(message, &clients[cindex].info)) {
			log_message(DEBUG, "  - to client %s", clients[cindex].info.name);
			if (rocsmq_send(clients[cindex].sock, message, 0)) {
				cindex++;
			} else {
				remove_client(cindex);
			}
		}
	}
}

/**
 * receive filter info from client
 */
Uint32 read_clientdata(TCPsocket sock, p_rocsmq_clientdata client) {
	Uint32 result;
	t_rocsmq_message message;
	result = rocsmq_recv(sock, &message,0);
	log_message(DEBUG, "-->sender: %s\n",message.sender);

	if(result == sizeof(t_rocsmq_message)) {
		memcpy(client, message.tail , sizeof(t_rocsmq_clientdata));
	}

	return result;
}

/*
 * print program usage
 */
void print_usage (void) {
	printf("Usage\n" );
	printf("%s [options]\n",baseconfig.clientname  );
	printf(" options\n --------\n" );
	printf(" -D : Set output level to DEBUG\n" );
	printf(" -I : Set output level to INFO\n" );
	printf(" -W : Set output level to WARNING\n" );
	printf(" -E : Set output level to ERROR\n" );
	printf(" -S : Output log to console(standard)\n" );
	printf(" -d : Run as daemon\n" );
	printf(" -l [file] : Output log to file [file]\n" );
	printf(" -p [port]: Set listener port\n" );
}

/**
 * signal handler function for signals to handle ;-p
 */
void server_signal_handler(int sig) {
	simple_signal_handler(sig);
	
	if (sig == SIGTERM) {
		log_message(INFO, "catched TERM signal. quitting..\n");
		closelog();
		/* shutdown SDL_net */
		SDLNet_Quit();
		/* shutdown SDL */
		SDL_Quit();
		exit (0);
	}
}


/**
 * handle rocsmq message
 */
int handle_message(p_rocsmq_message message) {
	
	return 0;
}


void quit(void) {
		log_message(INFO, "cleaning up server");
	closelog();
	/* shutdown SDL_net */
	SDLNet_Quit();
	/* shutdown SDL */
	SDL_Quit();
	exit (0);
}
