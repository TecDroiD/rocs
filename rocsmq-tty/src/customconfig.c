#include "customconfig.h"

int custom_config (json_object *json, void * p_datastruct) {
	
	log_message(DEBUG, "Loading custom config.");
	p_clientconfig config = (p_clientconfig) p_datastruct;

	// get device name
	get_stringval(json, CONFIG_KEY_DEVICE, config->device, 255);
	log_message(DEBUG, "Device file %s ", config->device);
	
	// get baudrate
	get_intval(json, CONFIG_KEY_BAUDRATE, &config->baudrate);
	log_message(DEBUG, "Baud rate %d", config->baudrate);
	 
	// get bitrate
	get_intval(json, CONFIG_KEY_BITRATE, &config->bitrate);
	log_message(DEBUG, "Bit rate %d", config->bitrate);

	// get stopbits
	get_intval(json, CONFIG_KEY_STOPBITS, &config->stopbits);
	log_message(DEBUG, "Stop bits %d", config->stopbits);


} 
