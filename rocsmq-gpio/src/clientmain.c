/*
 * client_main.c
 *
 *  Created on: 06.10.2015
 *      Author: tecdroid
 * 
 * message 
 * { "pins" : [
 * 	   {
 *        "name": "%s",
 *        "value": %d,
 *     },
 *  ]
 * }
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

#include <json-c/json.h>

#include "gpio.h"
#include "customconfig.h"

#define CLIENTNAME "gpio"
#define PROGNAME "rocsmq-gpio"
#define CONFIGFILE "conf/"PROGNAME".config"

#define ORDER_READ	"gpio.read"
#define ORDER_WRITE	"gpio.set"

#define MESSAGE_KEY_PINS	"pins"
#define MESSAGE_KEY_NAME	"name"
#define MESSAGE_KEY_VALUES	"value"

#define MESSAGE_RESPONSE "sensor.gpio"
#define MESSAGE_PINVAL "%s{\"name\":\"%s\",\"value\":%d,},"
#define MESSAGE_HEAD "{ \"pins\" : [ %s ], }"
int sock;

t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.filter 	= MESSAGE_CLIENT_I2C,
	.port = 8389,
	.rundaemon = 0,
	.loglevel = DEBUG,
	.logfile = "",
	.clientname = CLIENTNAME,
};

t_clientconfig clientconfig = {
	.num_pins = 0,
	.pins = 0,
};


int match_pin(p_pin pin, char *name);
void client_signal_handler(int sig);
int handle_message(p_rocsmq_message message);

/**
 * main function
 */
int main(int argc, char **argv) {
	pthread_t thread;
	t_rocsmq_message message;
		

	// parse configuration
	if (argc <= 1) {
		parseconfig(CONFIGFILE, &baseconfig, CLIENTNAME, gpio_custom_config, &clientconfig);
	} else if (argc == 2) {
		parseconfig(argv[1], &baseconfig, CLIENTNAME, gpio_custom_config, &clientconfig);
	} else {
		printf("Usage: %s [configfile]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* daemonize if neccessary */
	if(baseconfig.rundaemon) {
		if(0 != daemonize("~", client_signal_handler))
			return 1;
	}

	/*
	 * initialize logging system
	 */
	openlog(PROGNAME, baseconfig.logfile);
	log_setlevel(baseconfig.loglevel);

	log_message(DEBUG, PROGNAME " starting..");

	/* initialize gpio */
	if(0 > gpio_init(&clientconfig)) {
		log_message(ERROR, "could not init gpio ");		
		exit(1);
	}

	sock = rocsmq_init(&baseconfig);
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
		 * wait 1ms
		 */
		rocsmq_delayms(1);
	}

	/*
	 * cleanup
	 */
	rocsmq_destroy_thread(thread);
	rocsmq_exit(sock);

	release_config(&clientconfig);

	if (baseconfig.logtofile)
		closelog();

	return 0;
}



/**
 * match pin by name
 */
  
int match_pin(p_pin pin, char *name) {
	return (rocsmq_message_match(pin->mapname, name));
}


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




/*
 * handle rocsmq message
 */
int handle_message(p_rocsmq_message message) {
	// parse message into json object
	json_object *json = rocsmq_get_message_json(message);
	json_object *pinlist = 0;
	json_object *pindesc = 0;
	
	p_pin pin;
	
	char name[32];
	int value;
	int read;
	
	int size;
	int i,a;
	
	char tail[ROCS_MESSAGESIZE];
	tail[0] = '\0';
	
	t_rocsmq_message response = {
		.id = MESSAGE_RESPONSE,
		.sender = CLIENTNAME,
	};


	get_objval(json, MESSAGE_KEY_PINS, &pinlist);
	if (! pinlist) {
		log_message(ERROR, "json string has no pins.");
		return -1;
	}
	
	size = json_object_array_length(pinlist);
	
	int changed = 0;
	
	log_message(DEBUG, "reading pin list %s", json_object_to_json_string(pinlist));
	log_message(DEBUG, "order is %s", message->id);

	if (0 == strcmp(message->id, ORDER_READ)) {
		// iterate through array
		for (i = 0; i < size; i++) {
			pindesc = json_object_array_get_idx(pinlist, i);
			get_stringval(pindesc, MESSAGE_KEY_NAME, name, 20);
			get_intval(pindesc, MESSAGE_KEY_VALUES, &value);

	log_message(DEBUG, "Pinnane is %s, expected Val is %d", name, value);

			// look for changed or questioned pins and read them
			for (a = 0; a < clientconfig.num_pins; a++) {
				pin = &(clientconfig.pins[a]);
				if(match_pin(pin, name)) {
					// check inhibition rate
					// this inhibits measurement for some cycles
					if (pin->countdown-- == 0) {
						pin->countdown = pin->inhibition;

						log_message(DEBUG, "Checking Pin %s, number %d", pin->mapname, pin->number);
						
						read = gpio_read(pin->number);
						if ((value == -1) || (value == read)) {
							log_message(DEBUG, "  --> Pin %s has expected value %d.", pin->mapname, value);
							sprintf(tail, MESSAGE_PINVAL, tail, pin->mapname, read); 
							changed = 1;
						}
					} else {
						log_message(DEBUG, "Inhibiting Pin %s", pin->mapname);
					}
					
				}
			}
			// send data if neccessary
			if(changed) {
				sprintf(response.tail, MESSAGE_HEAD, tail);
				rocsmq_send(sock, &response, 0);
			}
		}
	} else 	if (0 == strcmp(message->id, ORDER_WRITE)) {
		// iterate through array
		for (i = 0; i < size; i++) {
			pindesc = json_object_array_get_idx(pinlist, i);
			get_stringval(pindesc, MESSAGE_KEY_NAME, name, 20);
			get_intval(pindesc, MESSAGE_KEY_VALUES, &value);
			
			log_message(DEBUG, "Pinnane is %s, desired Val is %d", name, value);
			// set pins where neccessary
			for (a = 0; a < clientconfig.num_pins; a++) {
				pin = &(clientconfig.pins[a]);
				
				log_message(DEBUG, " -> Checking Pin %s", pin->mapname);

				if(match_pin(pin, name)) {
					log_message(DEBUG, " ---> Setting value %d", value);
					gpio_write(pin->number, value);
				}
			}
		}
	} 

	return 0;
}
