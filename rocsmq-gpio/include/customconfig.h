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

#define CONFIG_KEY_PINCONFIG 	"pinconfig"
#define CONFIG_KEY_NUMBER 		"number"
#define CONFIG_KEY_DIRECTION 	"direction"
#define CONFIG_KEY_MAPNAME 		"mapname"

typedef struct s_pin {
	char number[8];
	int8_t direction;
	char mapname[20];
} t_pin, *p_pin;

typedef struct s_clientconfig {
	int8_t num_pins;
	p_pin pins;
} t_clientconfig, *p_clientconfig;


int gpio_custom_config (json_object *json, void * p_datastruct);

int release_config( void * p_datastruct);
#endif
