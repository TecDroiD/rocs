#include "customconfig.h"

int gpio_custom_config (json_object *json, void * p_datastruct) {
	json_object *ios, *value;
	int size = 0;
	int i;
	log_message(DEBUG, "Loading cussssstom config.");
	p_clientconfig config = (p_clientconfig) p_datastruct;

	// read inputs
	
	get_objval(json, CONFIG_KEY_INPUTS, &ios);
	size = json_object_array_length(ios);
	log_message(DEBUG, "Reading %d inputs.", size);
	config->inputs = malloc(sizeof(int8_t) * size);
	for(i = 0; i < size; i++) {
		value = json_object_array_get_idx(ios, i);
		get_intval(value, CONFIG_KEY_INPUT, &(config->inputs[i]));
	}

	// read outputs
	get_objval(json, CONFIG_KEY_OUTPUTS, &ios);
	size = json_object_array_length(ios);
	log_message(DEBUG, "Reading %d outputs.", size);
	config->outputs = malloc(sizeof(int8_t) * size);
	for(i = 0; i < size; i++) {
		value = json_object_array_get_idx(ios, i);
		get_intval(value, CONFIG_KEY_OUTPUT, &(config->outputs[i]));
	}
} 

int release_config( void * p_datastruct) {
	p_clientconfig config = (p_clientconfig) p_datastruct;

	free (config->inputs);
	free (config->outputs);
}
