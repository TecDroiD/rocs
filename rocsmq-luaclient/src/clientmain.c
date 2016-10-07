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
#include <getopt.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_timer.h>
#include <lua5.1/lua.h>
#include <lua5.1/lualib.h>
#include <lua5.1/lauxlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cdb.h>

//#include <unistd.h>

#include "client_config.h"

#define CLIENTNAME "lua"
#define CONFIGFILE "conf/rocsmq-luaclient.config"
#define SCRIPTDIR "script/"

TCPsocket sock;


t_rocsmq_baseconfig baseconfig = {
	.serverip = "127.0.0.1",
	.filter 	= CLIENTNAME,
	.port = 8389,
	.rundaemon = 0,
	.loglevel = DEBUG,
	.logfile = "",
	.clientname = CLIENTNAME,
};

t_luaclient_config clientconfig = {
	.cntscripts = 0,
	.scripts = 0,
	.libpath = "lib/",
	.dbpath = "lua.db",
};

t_lua lua = {
	.interpreter = 0,
	.json_string = "",
};

/**
 * initialize the lua interpreter
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
 * lua command for sending messages to rocsmq 
 */
int lua_send_message(lua_State *interpreter);

/**
 * lua command for logging messages to rocsmq 
 */
int lua_log_message(lua_State *interpreter);

/**
 * lua command for encoding data into base 64 
 */ 
int lua_b64_encode(lua_State *interpreter);

/**
 * lua command for decoding base 64 into whatever.. 
 */ 
int lua_b64_decode(lua_State *interpreter);  

/**
 * lua command for persisting data
 */
int lua_persist(lua_State *interpreter);  

/**
 * lua command for retrieving persisted data
 */ 
int lua_retrieve(lua_State *interpreter);  
 
/**
 * main function
 */ 
int main(int argc, char **argv) {
	SDL_Init(0);
	SDL_Thread *thread;
	int i;
	char buffer[32];

	char clientname[32];
	
	// initialize whatever neccesssary
	strcpy(clientname, CLIENTNAME);
	
	int c;
	// get config name if neccessary
	while ((c = getopt(argc, argv, "n:")) != -1) {
		switch(c) {
			case 'n':
				strcpy(clientname, optarg);
				break;
			default:
				break;
		}
	}

	// parse configuration
	// load file or not..
	if (optind >= argc) {
		parseconfig(CONFIGFILE, &baseconfig, clientname, custom_config, &clientconfig);
	} else {
		parseconfig(argv[optind], &baseconfig, clientname, custom_config, &clientconfig);
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
	memset  (message.tail, 0, ROCS_MESSAGESIZE);
	memset (message.id, 0, ROCS_IDSIZE);

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
			log_message( DEBUG, "incoming:%s\n",message.tail);
			handle_message(&message);
		}

			SDL_Delay(100);
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
	rocsmq_check_system_message(message->id);
	 
	// copy json content from message

	//json = rocsmq_get_message_json(message);
	
	log_message(DEBUG, "message-id %s", message->id);
	log_message(DEBUG, "  -> message-tail %s", message->tail);
	log_message(DEBUG, "have scripts %d", clientconfig.cntscripts);
		
	
	for(i = 0; i < clientconfig.cntscripts; i++) {
		if (0 ==  strncmp(clientconfig.scripts[i].filter, message->id, ROCS_IDSIZE)) {
			log_message(DEBUG, "calling script %s", clientconfig.scripts[i].filename);
			
			// load script file
			if (luaL_loadfile(lua.interpreter,clientconfig.scripts[i].filename)) {
				log_message(ERROR, "Error loading script: %s\n", lua_tostring(lua.interpreter, -1));
			}

			// call programm
			if (lua_pcall(lua.interpreter, 0, 0, 0)) {
				printf( "Error running script: %s\n", lua_tostring(lua.interpreter, -1));
			}

			// prepare variable stack
			lua_getglobal(lua.interpreter, "run");
			lua_pushstring(lua.interpreter,message->id);
			lua_pushstring(lua.interpreter,message->tail);
			
			// call programm
			if (lua_pcall(lua.interpreter, 2, 0, 0)) {
				printf( "Error running script: %s\n", lua_tostring(lua.interpreter, -1));
			}
		}
	}
	 
}
/**
 * 
 */ 
int init_interpreter() {
	// initialize interpreter
	lua.interpreter = lua_open();
	if (!lua.interpreter) {
		log_message(ERROR, "Could not instanciate lua interpreter");
		return -1;
	}
	// initialize lua interpreter
	luaL_openlibs(lua.interpreter);
	
	lua_pushcfunction(lua.interpreter, lua_send_message);
	lua_setglobal(lua.interpreter, "send_message");
	
	lua_pushcfunction(lua.interpreter, lua_log_message);
	lua_setglobal(lua.interpreter, "log_message");

	lua_pushcfunction(lua.interpreter, lua_b64_decode);
	lua_setglobal(lua.interpreter, "b64_decode");

	lua_pushcfunction(lua.interpreter, lua_b64_encode);
	lua_setglobal(lua.interpreter, "b64_encode");

	lua_pushcfunction(lua.interpreter, lua_persist);
	lua_setglobal(lua.interpreter, "persist");
	
	lua_pushcfunction(lua.interpreter, lua_retrieve);
	lua_setglobal(lua.interpreter, "retrieve");

	return 0;	
}

/**
 * de-initialize interpreter
 */
void uninit_interpreter() {
	lua_close(lua.interpreter);
}

/**
 * lua procedure for encoding to b64
 */
int lua_b64_encode(lua_State *interpreter) {
	char *data = luaL_checkstring(interpreter, 1);
	char retval[1000]; 
	b64encode(data, retval, 1000);

	lua_pushstring(interpreter,retval);
	return lua_OK;
}  

/**
 * lua procedure for decoding b64
 */
int lua_b64_decode(lua_State *interpreter) {
	char *data = luaL_checkstring(interpreter, 1);
	char retval[1000]; 
	b64decode(data, retval, 1000);

	lua_pushstring(interpreter,retval);
	return lua_OK;
}  

/**
 * lua procedure for sending messages
 */
int lua_send_message(lua_State *interpreter) {
	t_rocsmq_message message;
	memset(message.id, '\0', ROCS_IDSIZE);
	memset(message.tail, '\0', ROCS_MESSAGESIZE);
	memset(message.sender, '\0', ROCS_CLIENTNAMESIZE);
	
	strncpy(message.id, luaL_checkstring(interpreter, 1), ROCS_IDSIZE);
	strncpy (message.sender, baseconfig.clientname, 20);
	char * tail = luaL_checkstring(interpreter, 2);
	strncpy (message.tail,tail,ROCS_MESSAGESIZE);
	
	log_message(DEBUG, "sending message '%s' with tail '%s' from '%s'", message.id, message.tail,message.sender);
	rocsmq_send(sock,&message,0);
	return lua_OK;
}  

/**
 * lua procedure for sending messages
 */
int lua_log_message(lua_State *interpreter) {
	char *level = luaL_checkstring(interpreter,1);
	char *message = luaL_checkstring(interpreter,2);
	 
	log_message(log_getlevel(level),message);
	 
	return lua_OK;
}  


/**
 * lua command for persisting data
 */
int lua_persist(lua_State *interpreter) {
	struct cdb_make cdbm;
	
	char *key = luaL_checkstring(interpreter,1);
	char *val = luaL_checkstring(interpreter,2);
	int keylen = strlen(key);
	int vallen = strlen(val);
	
	// init db file
	int fp = open(clientconfig.dbpath, O_RDWR|O_CREAT);
	cdb_make_start(&cdbm, fp); 
	
	// add or replace data
	if (cdb_make_exists(&cdbm, key, keylen)) {
		cdb_make_put( &cdbm, key, keylen, val, vallen, CDB_PUT_REPLACE);
	} else {
		cdb_make_put( &cdbm, key, keylen, val, vallen, CDB_PUT_ADD);		
	}
	
	// deinit db file
	cdb_make_finish(&cdbm);
	close(fp);
	return lua_OK;
}

/**
 * lua command for retrieving persisted data
 */ 
int lua_retrieve(lua_State *interpreter) {
	struct cdb cdb;
	char *key = luaL_checkstring(interpreter,1);
	char *val;
	int vlen, vpos;
	
	// init db file
	int fp = open(clientconfig.dbpath, O_RDWR|O_CREAT);
	if (fp == 0) {
		return lua_ERROR;
	}
	
	cdb_init(&cdb, fp); 
	
	// find entry
	if(cdb_find(&cdb,key,strlen(key))) {
		// and retrieve it
		vpos = cdb_datapos(&cdb);
		vlen = cdb_datalen(&cdb);
		val = malloc(vlen);
		cdb_read(&cdb,val,vlen,vpos);
		lua_pushstring(interpreter,val);
		free(val);
	}
	
	// deinit db file
	close(fp);
	
	return lua_OK;	
}
