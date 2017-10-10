#include "customconfig.h"

int custom_config (json_object *json, void * p_datastruct) {

	log_message(DEBUG, "Loading custom config.");
	p_clientconfig config = (p_clientconfig) p_datastruct;
	
	get_intval(json, CONFIG_KEY_QUEUENUM, &(config->queue));

}
