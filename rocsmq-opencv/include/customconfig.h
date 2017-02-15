#ifndef __CUSTOMCONFIG_H
#define __CUSTOMCONFIG_H

/**
 * color-range
 * 
 * "color-range" : [
 *   {
 *     "message" : "sensor.vision.red",
 *     "min-hue" : 0,
 *     "min-sat" : 64,
 *     "min-val" : 128,
 *     "max-hue" : 32,
 *     "max-sat" : 255,
 *     "max-val" : 255,
 * 	   "hotspots" : 3,
 *   },
 *   ...
 *  ]
 * 
 *   hotspots - maximum of matches to send
 */

#include <json-c/json.h>

#include <daemonizer.h>
#include <log.h>
#include <rocsmq.h>
#include <rocsmqthread.h>
#include <configparser.h>

#define CONFIG_KEY_DEVICE		(char*)"device"
#define CONFIG_KEY_ONCOLOR		(char*)"color-range"
#define CONFIG_KEY_SENDMESSAGE	(char*)"message"
#define CONFIG_KEY_MIN_HUE		(char*)"min-hue"
#define CONFIG_KEY_MIN_SAT		(char*)"min-sat"
#define CONFIG_KEY_MIN_VAL		(char*)"min-val"
#define CONFIG_KEY_MAX_HUE		(char*)"max-hue"
#define CONFIG_KEY_MAX_SAT		(char*)"max-sat"
#define CONFIG_KEY_MAX_VAL		(char*)"max-val"
#define CONFIG_KEY_HOTSPOTS		(char*)"hotspots" 

#define HUE	0
#define SAT	1
#define VAL 2

typedef struct s_colorrange {
	char message[255];
	int8_t minvals[3];
	int8_t maxvals[3];
	int8_t hotspots;
} t_colorrange, *p_colorrange;

typedef struct s_clientconfig {
	char device[255];
	int num_cr;
	p_colorrange cr;
	
} t_clientconfig, *p_clientconfig;

int custom_config (json_object *json, void * p_datastruct);

#endif
