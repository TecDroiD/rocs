#ifndef TCLCLIENT_H
#define TCLCLIENT_H

#include <tcl/tcl.h>

#include <rocsmq.h>
#include <configparser.h>

#define CONFIG_KEY_FILTER	 "filter"
#define CONFIG_KEY_SCRIPTDIR "scriptdir"
#define CONFIG_KEY_SCRIPTS 	 "scripts"
#define CONFIG_KEY_MESSAGE_ID "message"
#define CONFIG_KEY_SCRIPTFILE "scriptfile"


typedef struct s_script {
		int filter;
		char filename[255];
		char *script;
} t_script, *p_script;

typedef struct s_tclclient_config {
	char scriptdir[255];
	int cntscripts;
	p_script scripts;
	
} t_tclclient_config, *p_tclclient_config;

typedef struct s_tcl {
	Tcl_Interp 		*interpreter;
	Tcl_HashTable 	*data_exchange;
	char 			json_string[ROCS_MESSAGESIZE];
} t_tcl, *p_tcl;

int custom_config (json_object *json, void * p_datastruct);
void clear_custom_config(p_tclclient_config config);
int load_scriptfile(p_script script );
	
#endif
