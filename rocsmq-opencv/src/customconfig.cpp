#include "customconfig.h"

int custom_config (json_object *json, void * p_datastruct) {
	
	log_message(DEBUG, "Loading custom config.");
	p_clientconfig config = (p_clientconfig) p_datastruct;

	// get device name
	get_stringval(json, CONFIG_KEY_DEVICE, config->device, 255);
	log_message(DEBUG, "Device file %s ", config->device);

} 
