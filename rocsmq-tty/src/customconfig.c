#include "customconfig.h"

int custom_config (json_object *json, void * p_datastruct) {
	
	log_message(DEBUG, "Loading custom config.");
	p_clientconfig config = (p_clientconfig) p_datastruct;

	// get device name
	get_stringval(json, CONFIG_KEY_DEVICE, config->device, 255);
	
	// get baudrate
	get_intval(json, CONFIG_KEY_BAUDRATE, &config->baudrate);
	 
	// get bitrate
	get_intval(json, CONFIG_KEY_BITRATE, &config->bitrate);

	// get stopbits
	get_intval(json, CONFIG_KEY_STOPBITS, &config->stopbits);

} 
