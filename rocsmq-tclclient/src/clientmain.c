/*
 * testmain.c
 *
 *  Created on: 29.09.2015
 *      Author: tecdroid
 */

//#include <linkedlist.h>
#include <rocsmq.h>
#include <rocsmqthread.h>
#include <configparser.h>
#include <messages.h> 
#include <log.h> 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_timer.h>
#include <tcl/tcl.h>

//#include <unistd.h>

#include "client_config.h"

#define CONFIGFILE "conf/rocsmq-tclclient.config"
#define SCRIPTDIR "script/"

TCPsocket sock;


t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.filter 	= MESSAGE_ID_INFRASTRUCTURE,
	.mask 		= MESSAGE_MASK_MAIN,
	.port = 8389,
	.rundaemon = 0,
	.loglevel = INFO,
	.logfile = "",
	.clientname = "tclclient",
};

t_tclclient_config clientconfig = {
	.scriptdir	= SCRIPTDIR,
};

t_tcl tcl = {
	.interpreter = 0,
	.json_string = "",
	.data_exchange = 0,
};

/**
 * initialize the tcl interpreter
 */ 
int init_interpreter();

/**
 * cleanup interpreter
 */
void uninit_interpreter();  
/**
 * handle the rocs message
 */ 
void handle_message(p_rocsmq_message message);
/**
 * tcl command for sending messages to rocsmq 
 */
int tcl_send_message(ClientData cdata, Tcl_Interp *interpreter, int argc, const char *argv[]);

/**
 * tcl command for logging messages to rocsmq 
 */
int tcl_log_message(ClientData cdata, Tcl_Interp *interpreter, int argc, const char *argv[]);

/**
 * tcl command for encoding data into base 64 
 */ 
int tcl_b64_encode(ClientData cdata, Tcl_Interp *interpreter, int argc, const char *argv[]);

/**
 * tcl command for decoding base 64 into whatever.. 
 */ 
int tcl_b64_decode(ClientData cdata, Tcl_Interp *interpreter, int argc, const char *argv[]);  

/**
 * main function
 */ 
int main(int argc, char **argv) {
	SDL_Init(0);
	SDL_Thread *thread;
	int i;
	char buffer[32];
	
	// parse configuration
	if (argc <= 1) {
		parseconfig(CONFIGFILE, &baseconfig, custom_config, &clientconfig);
	} else if (argc == 2) {
		parseconfig(argv[1], &baseconfig, custom_config, &clientconfig);
	} else {
		printf("Usage: %s [configfile]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	// open log
	printf("logging to file.. '%s'\n", baseconfig.logfile);
	openlog((char const *)baseconfig.clientname, (char const*)baseconfig.logfile);
	printf("loglevel = %d\n", baseconfig.loglevel);
		
	// set loglevel	
	log_setlevel(baseconfig.loglevel);
	
	log_message(DEBUG, "clientname: %s", baseconfig.clientname);
	
	t_rocsmq_message message;
	strncpy (message.sender, baseconfig.clientname, 20);
	memset  (message.tail, 0, 1000);
	message.id = 0;

	sock = rocsmq_init(&baseconfig);
	if (!sock) {
		SDL_Quit();
		log_message(ERROR,"could not connect to Server: %s\n", rocsmq_error());
		exit(1);
	}

	if (init_interpreter()) {
		log_message(ERROR, "could not start interpreter");
		exit(1);
	};

	// start network listener
	thread = rocsmq_start_thread(sock);

	while (rocsmq_thread_is_running()) {

		while(rocsmq_has_messages()) {

			rocsmq_get_message(&message);
			//log_message( DEBUG, "incoming:%s\n",message.tail);
			handle_message(&message);
		}

		if(argc < 1) {
			SDL_Delay(100);
		}
	}

	/*
	 * shut system down
	 */ 
	uninit_interpreter(); 
	clear_custom_config(&clientconfig); 
	log_message( INFO, "Process shutdown.");
	rocsmq_destroy_thread(thread);
	rocsmq_exit(sock);
	SDL_Quit();
	
	return 0;
}

/**
 * work on message...
 */ 
void handle_message(p_rocsmq_message message) {
	json_object * json;
	char val[32];
	int i;
	char *result;
	
	// react on system messages
	if (message->id & MESSAGE_ID_SYSTEM) {
		switch (message->id & ~MESSAGE_MASK_SUB) {		
			 
		case MESSAGE_ID_SHUTDOWN: /* shutdown message, stop system */
			rocsmq_thread_set_running(0);
			break;
		}
		
		return;
	} 
	// copy json content from message

	json = rocsmq_get_message_json(message);
	
	result = (char*)Tcl_SetVar(tcl.interpreter,"json",message->tail, 0);
	if (result == 0) {
		log_message(ERROR, "Error setting value %s: %s",
			message->tail,Tcl_GetStringResult(tcl.interpreter));	
	}
	log_message(DEBUG, "message-id %d", message->id);
	log_message(DEBUG, "  -> message-tail %s", result);
	log_message(DEBUG, "have scripts %d", clientconfig.cntscripts);
		
	
	for(i = 0; i < clientconfig.cntscripts; i++) {
		if (clientconfig.scripts[i].filter == message->id) {
			log_message(DEBUG, "calling script %s", clientconfig.scripts[i].filename);
			if (TCL_OK != Tcl_EvalFile(tcl.interpreter, clientconfig.scripts[i].filename)) {
				log_message(ERROR, "Error calling script %s: %s",
					clientconfig.scripts[i].filename,Tcl_GetStringResult(tcl.interpreter));
			}
		}
	}
}
/**
 * 
 */ 
int init_interpreter() {
	// initialize interpreter
	tcl.interpreter = Tcl_CreateInterp();
	if (!tcl.interpreter) {
		log_message(ERROR, "Could not instanciate TCL interpreter");
		return -1;
	}
	// initialize tcl interpreter
	Tcl_Init(tcl.interpreter);
	
	// add commands to interpreter
	if (0 == Tcl_CreateCommand(tcl.interpreter,"send_message",tcl_send_message,NULL,NULL)) {
		log_message(ERROR, "Could not create command send_message");
		return -1;
	}
	if (0 == Tcl_CreateCommand(tcl.interpreter,"log_message",tcl_log_message,NULL,NULL)) {
		log_message(ERROR, "Could not create command log_message");
		return -1;
	}
	if (0 == Tcl_CreateCommand(tcl.interpreter,"b64_encode",tcl_b64_encode,NULL,NULL)) {
		log_message(ERROR, "Could not create command b64_encode");
		return -1;
	}
	if (0 == Tcl_CreateCommand(tcl.interpreter,"b64_decode",tcl_b64_decode,NULL,NULL)) {
		log_message(ERROR, "Could not create command b64_decode");
		return -1;
	}
	
	
	// link json to interpreter
	if (TCL_OK != Tcl_LinkVar(tcl.interpreter,"json", tcl.json_string,TCL_LINK_STRING)) {
		log_message(ERROR, "Could not link json string to interpreter.");
		return -1;
	}
		
	
	// initialize data exchange table
	tcl.data_exchange = (Tcl_HashTable *)Tcl_NewObj();
	Tcl_InitHashTable(tcl.data_exchange, TCL_STRING_KEYS);
	if (!tcl.data_exchange) {
		log_message(ERROR, "Could not create hash for data interchange.");
		return -1;
	}
	
	// link to interpreter
	if (0 == Tcl_SetVar2Ex(tcl.interpreter, "data", 0, (Tcl_Obj *) tcl.data_exchange, 0)) {
		log_message(ERROR, "Could not link data hash to interpreter.");
		return -1;
	}
	
	return 0;	
}

/**
 * de-initialize interpreter
 */
void uninit_interpreter() {
	Tcl_UnsetVar(tcl.interpreter, "json", 0);
	Tcl_UnsetVar(tcl.interpreter, "data", 0);
	Tcl_DeleteHashTable(tcl.data_exchange);
	Tcl_DecrRefCount(tcl.data_exchange);  
	Tcl_DecrRefCount(tcl.json_string);  
	Tcl_DeleteInterp(tcl.interpreter);
}

/**
 * tcl procedure for encoding to b64
 */
int tcl_b64_encode(ClientData cdata, Tcl_Interp *interpreter, int argc, const char *argv[]) {
	char *data = (char*) argv[1];
	char retval[1000]; 
	b64encode(data, retval, 1000);
	Tcl_SetResult(interpreter, retval, TCL_VOLATILE);
	return TCL_OK;
}  

/**
 * tcl procedure for decoding b64
 */
int tcl_b64_decode(ClientData cdata, Tcl_Interp *interpreter, int argc, const char *argv[]) {
	char *data = (char *)argv[1];
	char retval[1000]; 
	b64decode(data, retval, 1000);
	Tcl_SetResult(interpreter, retval, TCL_VOLATILE);
	return TCL_OK;
}  

/**
 * tcl procedure for sending messages
 */
int tcl_send_message(ClientData cdata, Tcl_Interp *interpreter, int argc, const char *argv[]) {
	t_rocsmq_message message;
	if(argc != 3) {
		log_message(ERROR, "Parameter count wrong in log_message. 2 wanted, %d given.", argc);
		return TCL_ERROR;
	}
	
	message.id = atoi(argv[1]);
	strncpy (message.tail,argv[2],ROCS_MESSAGESIZE);
	rocsmq_send(sock,&message,0);
	return TCL_OK;
}  

/**
 * tcl procedure for sending messages
 */
int tcl_log_message(ClientData cdata, Tcl_Interp *interpreter, int argc, const char *argv[]) {
	int i;
	if(argc != 3) {
		log_message(ERROR, "Parameter count wrong in log_message. 2 wanted, %d given.", argc);
		for (i = 0; i < argc; i++) {
			log_message(ERROR, " [%d] : '%s'", i, argv[i]);
		}
		return TCL_ERROR;
	}
	
	log_message(log_getlevel((char *)argv[1]),(char *)argv[2]);
	 
	return TCL_OK;
}  
