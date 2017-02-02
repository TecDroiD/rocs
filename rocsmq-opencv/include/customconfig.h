#ifndef __CUSTOMCONFIG_H
#define __CUSTOMCONFIG_H

/**
 * pin configuration
 * 
 * "pinconfig" : [
 *   {
 *     "number" : 22,
 *     "direction" : 0,
 *     "mapname" : "motor-1-a"
 *   },
 *   ...
 *  ]
 */

#include <json-c/json.h>

#include <daemonizer.h>
#include <log.h>
#include <rocsmq.h>
#include <rocsmqthread.h>
#include <configparser.h>

#define CONFIG_KEY_DEVICE		"device"

typedef struct s_clientconfig {
	char device[255];
} t_clientconfig, *p_clientconfig;

int custom_config (json_object *json, void * p_datastruct);

#endif
