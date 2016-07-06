#include "client_config.h"

int custom_config (json_object *json, void * p_datastruct) {
	json_object *scripts,*script;
	int i;
	p_clientconfig config = (p_clientconfig)p_datastruct;
	
	get_stringval(json,CONFIG_KEY_VOICE, config->voice, 255);
		
	get_intval(json,CONFIG_KEY_PITCH, &config->pitch);
	get_intval(json,CONFIG_KEY_VOLUME, &config->volume);
	get_intval(json,CONFIG_KEY_PITCH, &config->speed);
}
