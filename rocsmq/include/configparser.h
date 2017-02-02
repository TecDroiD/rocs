/*
 * configparser.h
 *
 *  Created on: 07.10.2015
 *      Author: tecdroid
 */
#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <SDL/SDL.h>
#include <json-c/json.h>
#include <b64/cencode.h>
#include <b64/cdecode.h>

#include <log.h>

#define KEY_SERVERIP	"server"
#define KEY_PORT		"port"
#define KEY_FILTER		"filter"
#define KEY_MASK		"mask"
#define KEY_DAEMON		"daemonize"
#define KEY_LOGLEVEL	"loglevel"
#define KEY_LOGFILE		"logfile"
#define KEY_CLIENTNAME  "clientname"

/**
 * base configuration for a rocsmq product
 * 
 * you may want to initialize with
 * t_rocsmq_baseconfig baseconfig = {
 *		.serverip = "127.0.0.1"
 *		.port = ROCSMQ_PORT,
 * 		.filter = 0x00,
 * 		.mask = 0x00,
 *		.rundaemon = 0,
 *		.loglevel = INFO,
 *		.logfile = "",
 *		.clientname = PROGNAME,
 * };
 */

typedef struct s_rocsmq_baseconfig {
	char serverip[15];
	int  port;
	char  filter[256];
	int  rundaemon;
	int  loglevel;
	int  logtofile; /** deprecated */
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
/**
 * get an integer by name from json config
 */ 
int get_boolval		(json_object *json, char *key, int *val);
/**
 * get a double value from json config 
 */ 
int get_doubleval		(json_object *json, char *key, double *val); 
/**
 * get a string value from json config
 */ 
int get_stringval	(json_object *json, char *key, char *val, int maxlen);
/**
 * encode binary data in base64
 */ 
int b64encode(char * in, char *out, int size);
/**
 * decode base64 to binary data
 */ 
int b64decode(char * in, char *out, int size);

int parseconfig(char const *filename, p_rocsmq_baseconfig baseconfig, char *custom_key, custom_config_t customs, void *p_datastruct);

#ifdef __cplusplus
}
#endif

#endif
