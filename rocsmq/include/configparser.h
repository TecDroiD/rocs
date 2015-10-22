/*
 * configparser.h
 *
 *  Created on: 07.10.2015
 *      Author: tecdroid
 */
#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#include <SDL/SDL.h>
#include <json-c/json.h>

#include <log.h>

#define KEY_SERVERIP	"server"
#define KEY_PORT		"port"
#define KEY_DAEMON		"daemonize"
#define KEY_LOGLEVEL	"loglevel"
#define KEY_LOGFILE		"logfile"
#define KEY_CLIENTNAME  "clientname"


typedef struct s_rocsmq_baseconfig {
	char serverip[15];
	int  port;
	int  rundaemon;
	int  loglevel;
	int  logtofile;
	char logfile[255];
	char clientname[20];
	
} t_rocsmq_baseconfig, *p_rocsmq_baseconfig;

/**
 * data type for customized program parameters
 */ 
typedef int (*custom_config_t) (json_object *json, void * p_datastruct);

/**
 * get an object by name from json config
 * doesn't check anything but the existing key
 */ 
int get_objval		(json_object *json, char *key, json_object **val);
/**
 * get an integer by name from json config
 */
  
int get_intval		(json_object *json, char *key, int *val);
int get_doubleval		(json_object *json, char *key, double *val); 
int get_stringval	(json_object *json, char *key, char *val, int maxlen);

int parseconfig(char const *filename, p_rocsmq_baseconfig baseconfig, custom_config_t customs, void *p_datastruct);

#endif
