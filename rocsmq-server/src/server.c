/*
 * server.c
 *
 *  Created on: 29.09.2015
 *      Author: tecdroid
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/time.h> // for FDSET
#include <netinet/in.h>
#include <arpa/inet.h>

#include <rocsmq.h>
#include <daemonizer.h>
#include <log.h>
#include <configparser.h>
#include <messages.h>


#include "clientlist.h"

#define CONFIGFILE "conf/rocsmq-server.config"

t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.port = ROCSMQ_PORT,
	.rundaemon = 0,
	.loglevel = INFO,
	.logfile = "",
	.clientname = "rocsmq-server",
	};

  

int server_sock; /*socket */

/**
 * receive filter info from client
 */
int read_clientdata(int sock, p_rocsmq_clientdata client);

/**
 * send message to all clients
 */ 
void send_all(p_rocsmq_message message);

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
	int client_sock;
	struct sockaddr_in server, client;
	
	t_rocsmq_message message;
	const char *host = NULL;

	// client set for activity monitoring
	fd_set set, readset;
	

	// parse options
	switch (argc) {
		case 1: parseconfig(CONFIGFILE, &baseconfig, 0, 0 ,0);
			break;
		case 2: parseconfig(argv[1], &baseconfig, 0, 0 ,0);
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
	
	/* 
	 * handle signals
	 */
	t_sighandler sig_handler = server_signal_handler;
	 
		/* signal handler */
	signal(SIGCHLD, sig_handler); /* ignore child */
	signal(SIGTSTP, sig_handler); /* ignore tty signals */
	signal(SIGTTOU, sig_handler);
	signal(SIGTTIN, sig_handler);
	signal(SIGHUP, sig_handler); /* catch hangup signal */
	signal(SIGTERM, sig_handler); /* catch kill signal */
	signal(SIGKILL, sig_handler); /* catch kill signal */
  
	log_message(DEBUG, "	-> done...");

	/*
	 * initialize logging system
	 */
	openlog(baseconfig.clientname, baseconfig.logfile);
	log_setlevel(baseconfig.loglevel);
	log_message(DEBUG,  "%s starting server ..", baseconfig.clientname);

	/* create socket */
	server_sock = socket(AF_INET, SOCK_STREAM,0);
	if (-1 == server_sock) {
		log_message(ERROR, "Could not create socket");
		exit(1);
	}
	
	/* create server */
	server.sin_family = AF_INET;
	inet_aton(baseconfig.serverip, &server.sin_addr);
	server.sin_port=htons(baseconfig.port);
	
	/* bind socket to address and port */
	if ( 0 > bind (server_sock, (struct sockaddr *) &server, sizeof(server))) {
		log_message(ERROR, "Bind host address failed :", rocsmq_error());
		exit(2);
	}
	
	/* output the IP address nicely */
	log_message(DEBUG,"IP Address : %s, port: %d", baseconfig.serverip, baseconfig.port);


	/* listen on port */
	if ( 0 > listen (server_sock, 5)) {
		log_message(ERROR, "listen failed :", rocsmq_error());
		exit(2);
	}

	
	/* 
	 * add master to set
	 */ 
	FD_ZERO(&set);
	FD_SET(server_sock, &set);
	 
	/**
	 * infinite main loop
	 */ 
	log_message(INFO,"Server started. waiting for clients");
	while (1) {
		/* check for data from sockets */
		int i;


		/* copy working set */
		memcpy(&readset, &set, sizeof(set)); 
		//readset = set;
		
		// wait for readable sockets
		int numready = select( 500, &readset, NULL, NULL, NULL );
		if (numready == -1) {
			log_message(ERROR,"select (): %s\n", rocsmq_error());
			break;
		}
		
		//if server socket is readable
		if (FD_ISSET(server_sock, &readset)) {
			// accept new client
			int c =  sizeof(struct sockaddr_in);
			client_sock = accept(server_sock, (struct sockaddr *) &client, (socklen_t *) &c );
			if (client_sock) {
				// add client socket
				FD_SET(client_sock, &set);
				
				t_rocsmq_clientdata clientinfo;
				/*printf("Accepted...\n"); */
				if ( 0 != read_clientdata(client_sock, &clientinfo)) {
					log_message(DEBUG,"welcoming client %s",clientinfo.name);
					log_message(DEBUG," filter : %s", clientinfo.filter);
					add_client(client_sock, &clientinfo);
					log_message(DEBUG," client created, socket: %d", client_sock);
				} else {
					log_message(ERROR,"could not connect client: %s",rocsmq_error());
					close(client_sock);
				}
			}					
		}

		// check clients for setting them
		for (i = 0; i < count_clients(); i++) {		
			p_client client = get_client_idx(i);
			if (FD_ISSET(client->sock, &readset)) {
			log_message(DEBUG, "client %s isset : %d", client->name, FD_ISSET(client->sock, &readset));
				
				// receive message
				if (rocsmq_recv(client->sock, (p_rocsmq_message) & message,
						ROCSMQ_POLL)) {
					log_message(DEBUG,"received message");
					log_message(DEBUG," id is '%s', sender is '%s'",
								message.id, message.sender);
					log_message(DEBUG,"	tail is %s",message.tail);

					// send message to all
					send_all( & message);
					
					// handle message if it's for the infrastructure		
					if(0 == strcmp(message.id, MESSAGE_ID_SYSTEM)) {
						handle_message(&message);
					}
				} else {
					// client disconnected, delete
					log_message(DEBUG,"removing client %s\n", client->name);
					FD_CLR(client->sock, &set);
					remove_client(client);
				}

			}
			rocsmq_delayms(1);
		}
	}
	quit();
}

/**
 * receive filter info from client
 */
int read_clientdata(int sock, p_rocsmq_clientdata client) {
	int result;
	t_rocsmq_message message;
	result = rocsmq_recv(sock, &message,0);
	log_message(DEBUG, "-->sender: %s\n",message.sender);

	if(result == sizeof(t_rocsmq_message)) {
		memcpy(client, message.tail , sizeof(t_rocsmq_clientdata));
	}

	return result;
}

/**
 *  send a buffer to all clients
 */
void send_all(p_rocsmq_message message) {
	
	p_client client = 0;
	
	log_message(DEBUG, "Sending message '%s', possibly to %d clients.", message->tail, count_clients());
	while((client = next_client_by_message(client,message->id)) != 0) {

		log_message(DEBUG, "  - to client %s",  client->name);
		if (rocsmq_send(client->sock, message, 0)) {
		} else {
			remove_client(client);
		}		
	}
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
		log_message(INFO, "catched TERM signal.");
		quit();
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
	rocsmq_exit(server_sock);
	exit (0);
}
