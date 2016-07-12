#include "client_config.h"

int custom_config (json_object *json, void * p_datastruct) {
	json_object *scripts,*script;
	int i;
	p_clientconfig config = (p_clientconfig)p_datastruct;
	
	get_stringval(json,KEY_DEVICE, config->device, 255);
	get_stringval(json,KEY_KEYWORDLIST, config->keywordlist, 255);
	get_stringval(json,KEY_SENDTO, config->sendto, ROCS_IDSIZE);
	get_stringval(json,KEY_HMMFILE, config->hmmfile, 255);
	get_stringval(json,KEY_LMFILE, config->lmfile, 255);
	get_stringval(json,KEY_DICTFILE, config->dictfile, 255);
		
	get_intval(json,KEY_SAMPLINRATE, &config->samplingrate);
	get_intval(json,KEY_DELAY_AFTER, &config->delay_after);
}
