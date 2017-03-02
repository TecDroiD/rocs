#include "client_config.h"

int custom_config (json_object *json, void * p_datastruct) {
	json_object *scripts,*script;
	int i;
	p_clientconfig config = (p_clientconfig)p_datastruct;
	get_stringval(json,CONFIG_KEY_VOICE, config->voice, 255);
		printf( "voice configuration: %s", config->voice );
	get_intval(json,CONFIG_KEY_PITCH, &config->pitch);
		printf("pitch configuration: %d", config->pitch );
	get_intval(json,CONFIG_KEY_VOLUME, &config->volume);
		printf("volume configuration: %d", config->volume );
	get_intval(json,CONFIG_KEY_SPEED, &config->speed);
		printf("speed configuration: %d", config->speed );
	get_intval(json,CONFIG_KEY_GAP, &config->gap);
		printf("wordgap configuration: %d", config->gap );
}
