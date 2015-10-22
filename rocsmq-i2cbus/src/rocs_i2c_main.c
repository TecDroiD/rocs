/*
 * client_main.c
 *
 *  Created on: 06.10.2015
 *      Author: tecdroid
 */

#include <daemonizer.h>
#include <getopt.h>
#include <linux/i2c-dev.h>
#include <log.h>
#include <rocsmq.h>
#include <rocsmqthread.h>
#include <messages.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_timer.h>

#include <json-c/json.h>

#include "i2cbus.h"
#include "pca9685.h"

#define PROGNAME "rocsmq-i2c"
#define PROG_FILTER 0x0000
#define PROG_MASK	0x0000

#define MOTORS_PER_DEVICE	16

int port 		= ROCSMQ_PORT;
int rundaemon 	= 0;
int loglevel 	= INFO;
int logtofile 	= 0;
char logfile[255] = "log/"PROGNAME".log";
int baudrate	= 125;
char device[255] = "/dev/i2c-0\0";

t_pca_device pca_devices[] = {
	 {.addr=0xa1}
	,{.addr=0xa2}
};



/**
 * signal handler function for signals to handle ;-p
 */
void client_signal_handler(int sig) {
//	simple_signal_handler(sig);

	if (sig == SIGTERM) {
		log_message(INFO, "quitting..\n");
		rocsmq_thread_set_running(0);
	}
}

/**
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
	printf(" -f [file] : Set i2c bus device (std: /dev/i2c-0)\n");
	printf(" -c [file] : Configuration file\n");
	//printf(" -b [baud] : Set baudrate in kbaud\n");
}

/**
 * get functional options
 */
int getoptions (int argc, char **argv) {
	int opt;
	while((opt = getopt(argc, argv, "DIEWSl:dp:c:"))!= -1) {
		switch(opt) {
		case 'D': loglevel = DEBUG; break; /* Debug level DEBUG */
		case 'I': loglevel = INFO; break; /* Debug level INFO */
		case 'E': loglevel = ERROR; break; /* Debug level ERROR */
		case 'W': loglevel = WARNING; break; /* Debug level WARNING */
		case 'l': strncpy(logfile,optarg,255); logtofile=1; break; /* log to file [filename] */
		case 'd': rundaemon = 1; break;
		case 'p': port = atoi(optarg); break;
		case 'c': strncpy(device, optarg, 255); break;
		default:
			print_usage();
			return 1;
		}
	}

	/* daemonize if neccessary */
	if(rundaemon) {
		if(0 != daemonize("~", client_signal_handler))
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

/**
 * parse json object
 */ 
int set_motor_per_json(json_object * json) {
	int id = 0;
	int pos = 0;
	int arraylen;
	int i;
	
	enum json_type type;
	json_object * data;
		
	json_object_object_foreach(json,key,val) {
	
		if (strncmp(key, "motor",5) == 0) {
			// if item is a motor
			type = json_object_get_type(val);
			if (type == json_type_array) {
				// read motor and value data
				arraylen = json_object_array_length(val);
				for (i = 0; i < arraylen; i++) { 
					// get motor id
					json_object_object_get_ex(val, "id", &data);
					id = json_object_get_int(data);
					// get motor position
					json_object_object_get_ex(val, "pos", &data);
					pos = json_object_get_int(data);
								
					//set motor position
					log_message(DEBUG, "Setting motor %d to %d ppm.", id, pos);
					pca_port(&(pca_devices[id%MOTORS_PER_DEVICE]),id/MOTORS_PER_DEVICE,pos);
				}
			}  else {
				log_message(ERROR, "Message Syntax Error.");
				return -1;
			}
		} else {
			log_message(ERROR, "Wrong Message Error.");
		}
	}
	return 0;
}

/**
 * read sensor data 
 */ 
void aquire_sensor_data(json_object *json) {
	
	
}

/*
 * handle rocsmq message
 */
int handle_message(p_rocsmq_message message) {
	// parse message into json object
	json_object *json = rocsmq_get_message_json(message);
	
	// switch message type
	switch(message->id & MESSAGE_MASK_SUB) {
	case MESSAGE_ID_MOTORPOS:
		// 	to motor array
		// send data to i2c bus
		set_motor_per_json(json);
		break;
	case MESSAGE_ID_AQUIRE:	
		aquire_sensor_data(json);
		break;
	default:
		break;
	}

	// delete json object
	json_object_put(json);
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

	/* initialize i2c */
	if(0 > i2cbus_init(device))
		exit(1);
		

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

		}

			/*
			 * todo: handle i²c message - not yet neccessary
			 */
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



