#ifndef __CUSTOMCONFIG_H
#define __CUSTOMCONFIG_H

#include <json-c/json.h>

#define CONFIG_KEY_DEVICE "device"
#define CONFIG_KEY_BAUDRATE "baudrate"


typedef struct s_clientconfig {
	int kbaud;
	char devicefile[255];
} t_clientconfig, *p_clientconfig;


int i2cbus_custom_config (json_object *json, void * p_datastruct);
#endif
