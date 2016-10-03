#ifndef __CUSTOMCONFIG_H
#define __CUSTOMCONFIG_H

#include <json-c/json.h>
#include <log.h>

#define CONFIG_KEY_INPUTS "inputs"
#define CONFIG_KEY_OUTPUTS "outputs"
#define CONFIG_KEY_INPUT "input"
#define CONFIG_KEY_OUTPUT "output"

typedef struct s_clientconfig {
	int8_t num_inputs;
	int8_t num_outputs;
	
	int8_t *inputs;
	int8_t *outputs;
} t_clientconfig, *p_clientconfig;


int gpio_custom_config (json_object *json, void * p_datastruct);

int release_config( void * p_datastruct);
#endif
