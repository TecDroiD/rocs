#ifndef luaCLIENT_H
#define luaCLIENT_H

#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>

#include <rocsmq.h>
#include <configparser.h>

#define CONFIG_KEY_LIBPATH   "librarypath"
#define CONFIG_KEY_DBFILE	 "dbfile"
#define CONFIG_KEY_FILTER	 "filter"
#define CONFIG_KEY_SCRIPTDIR "scriptdir"
#define CONFIG_KEY_SCRIPTS 	 "scripts"
#define CONFIG_KEY_MESSAGE_ID "message"
#define CONFIG_KEY_SCRIPTFILE "scriptfile"

#define lua_OK	1
#define lua_ERROR 0

/**
 * the description of a single lua script
 */ 
typedef struct s_script {
		char filter[ROCS_IDSIZE];
		char filename[255];
		char *script;
} t_script, *p_script;

/**
 * client configuration
 */ 
typedef struct s_luaclient_config {
	// count scripts in list
	int cntscripts;
	// script array
	p_script scripts;
	// path to lua library
	char libpath[255];
	// path to script key-val store
	char dbpath[255];
	
} t_luaclient_config, *p_luaclient_config;

/**
 * the lua interpreter state
 */ 
typedef struct s_lua {
	lua_State 		*interpreter;
	char 			json_string[ROCS_MESSAGESIZE];
} t_lua, *p_lua;

/**
 * load the custom config
 */ 
int custom_config (json_object *json, void * p_datastruct);
/**
 * cleanup custom config
 */ 
void clear_custom_config(p_luaclient_config config);
/**
 * load a sctipt file into memory
 */ 
int load_scriptfile(p_script script );
	
#endif
