#ifndef __CUSTOMCONFIG_H
#define __CUSTOMCONFIG_H

#include "configparser.h"

#include <json-c/json.h>
#include <log.h>

#define CONFIG_KEY_DEVICE "device"
#define CONFIG_KEY_BAUDRATE "baudrate"
#define CONFIG_KEY_INIT	  "init"
#define CONFIG_KEY_SLAVES  "slaves"
#define CONFIG_KEY_ADDR	  "addr"

typedef struct s_initscript {
	uint8_t slave;
	char initfile[255];
} t_initscript, *p_initscript;

typedef struct s_clientconfig {
	int kbaud;
	char devicefile[255];
	int num_initscripts;
	p_initscript initscripts;
} t_clientconfig, *p_clientconfig;


int i2cbus_custom_config (json_object *json, void * p_datastruct);
#endif
