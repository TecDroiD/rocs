#ifndef __CUSTOMCONFIG_H
#define __CUSTOMCONFIG_H

#include "configparser.h"

#include <json-c/json.h>
#include <log.h>

#define CONFIG_KEY_QUEUENUM "key"

typedef struct s_clientconfig {
	int queue;
} t_clientconfig, *p_clientconfig;


int custom_config (json_object *json, void * p_datastruct);
#endif
