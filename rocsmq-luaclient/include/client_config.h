#ifndef luaCLIENT_H
#define luaCLIENT_H

#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>

#include <rocsmq.h>
#include <configparser.h>

#define CONFIG_KEY_FILTER	 "filter"
#define CONFIG_KEY_SCRIPTDIR "scriptdir"
#define CONFIG_KEY_SCRIPTS 	 "scripts"
#define CONFIG_KEY_MESSAGE_ID "message"
#define CONFIG_KEY_SCRIPTFILE "scriptfile"

#define lua_OK	1
#define lua_ERROR 0

typedef struct s_script {
		int filter;
		char filename[255];
		char *script;
} t_script, *p_script;

typedef struct s_luaclient_config {
	int cntscripts;
	p_script scripts;
	
} t_luaclient_config, *p_luaclient_config;

typedef struct s_lua {
	lua_State 		*interpreter;
	char 			json_string[ROCS_MESSAGESIZE];
} t_lua, *p_lua;

int custom_config (json_object *json, void * p_datastruct);
void clear_custom_config(p_luaclient_config config);
int load_scriptfile(p_script script );
	
#endif
