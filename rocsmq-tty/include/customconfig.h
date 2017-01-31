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
#include <log.h>

#define CONFIG_KEY_DEVICE		"device"
#define CONFIG_KEY_BAUDRATE		"baudrate"
#define CONFIG_KEY_BITRATE		"bitrate"
#define CONFIG_KEY_STOPBITS		"stopbits"
#define CONFIG_KEY_PARITY		"parity"

typedef struct s_clientconfig {
	char device[255];
	unsigned baudrate;
	unsigned bitrate;
	unsigned stopbits;
	unsigned parity;
	
} t_clientconfig, *p_clientconfig;

int custom_config (json_object *json, void * p_datastruct);

#endif
