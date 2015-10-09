/*
 * client_main.c
 * 
 * this is a standard rocsmq client frame
 * it connects to rocs via standard port, 
 * is able to be daemonized
 * configurable via commandline options
 * uses polling thread for messages which are queued for the 
 * main loop to be handled there
 * 
 *  Created on: 06.10.2015
 *      Author: tecdroid
 */

#include <daemonizer.h>
#include <getopt.h>
#include <log.h>
#include <rocsmq.h>
#include <rocsmqthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_thread.h>

#define PROGNAME "rocsmq-client-example"
#define PROG_FILTER 0x0000
#define PROG_MASK	0x0000

int port 		= ROCSMQ_PORT;
int rundaemon 	= 0;
int loglevel 	= INFO;
int logtofile 	= 0;
char logfile[255] = "log/"PROGNAME".log";
int baudrate	= 125;
char candev[255] = "/dev/can0\0";

/**
 * signal handler function for signals to handle ;-p
 */
void client_signal_handler(int sig) {
	simple_signal_handler(sig);

	if (sig == SIGTERM) {
		log_message(INFO, "quitting..\n");
		rocsmq_thread_set_running(0);
	}
}

/*
 * print program usage
 */
void print_usage (void) {
	printf("rocsmq client - usage\n" );
	printf(" rocsmq_client [options]\n" );
	printf(" options\n --------\n" );
	printf(" -D : Set output level to DEBUG\n" );
	printf(" -I : Set output level to INFO\n" );
	printf(" -W : Set output level to WARNING\n" );
	printf(" -E : Set output level to ERROR\n" );
	printf(" -S : Output log to console(standard)\n" );
	printf(" -d : Run as daemon\n" );
	printf(" -l [file] : Output log to file [file]\n" );
	printf(" -p [port] : Set listener port\n" );
}

/**
 * get functional options
 */
int getoptions (int argc, char **argv) {
	int opt;
	while((opt = getopt(argc, argv, "DIEWSl:dp:"))!= -1) {
		switch(opt) {
		case 'D': loglevel = DEBUG; break; /* Debug level DEBUG */
		case 'I': loglevel = INFO; break; /* Debug level INFO */
		case 'E': loglevel = ERROR; break; /* Debug level ERROR */
		case 'W': loglevel = WARNING; break; /* Debug level WARNING */
		case 'l': strncpy(logfile,optarg,255); logtofile=1; break; /* log to file [filename] */
		case 'd': rundaemon = 1; break;
		case 'p': port = atoi(optarg); break;
		default:
			print_usage();
			return 1;
		}
	}

	/* daemonize if neccessary */
	if(rundaemon) {
		if (0 != daemonize("~", client_signal_handler))
			return 1;
	}

	/*
	 * initialize logging system
	 */
	if (logtofile || rundaemon) {
		openlog(PROGNAME, logfile);
	} else {
		log_init(PROGNAME,stdout);
	}
	log_setlevel(loglevel);

	log_message(DEBUG, PROGNAME " starting..");

	return 0;
}

/*
 * todo: handle rocsmq message
 */
int handle_message(p_rocsmq_message message) {
	switch(message->id) {
	default:
		break;
	}

	return 0;
}

/**
 * main function
 */
int main(int argc, char **argv) {
	TCPsocket sock;
	SDL_Thread *thread;
	t_rocsmq_message message;

	/* initialize sdl */
	SDL_Init(0);

	/* get options */
	if(0 != getoptions(argc, argv))
		exit(1);

	/* do initialization stuff */
	sock = rocsmq_init(PROGNAME,PROG_FILTER,PROG_MASK);
	if(! sock) {
		log_message(ERROR, "could not connect to server %s", rocsmq_error());
		exit(1);
	}
	thread = rocsmq_start_thread(sock);
	if (!thread) {
		log_message(ERROR, "could not start listener thread %s", rocsmq_error());
	}

	/**
	 * process main loop
	 */
	while (rocsmq_thread_is_running()) {

		/* as long as messages are there */
		while (rocsmq_has_messages()) {

			/* handle next message */
			rocsmq_get_message(&message);
			log_message(INFO, "incoming:%s\n",message.tail);
			handle_message(&message);
			/*
			 * do your stuff
			 */

		}

		/*
		 * wait 100ms
		 */
		SDL_Delay(100);
	}

	/*
	 * cleanup
	 */
	rocsmq_destroy_thread(thread);
	rocsmq_error(sock);

	if (logtofile)
		closelog();

	SDL_Quit();

	return 0;
}



