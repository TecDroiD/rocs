#include "client_config.h"
int load_scriptfile(p_script script ) {
	FILE *fp;
	int size = 0;
	if(script->script == 0) {
		
		log_message(DEBUG, "loading script %s", script->filename);
		
		// get filesize
		fp = fopen(script->filename, "r");
		if (!fp) {
			return -1;
		}
		
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		
		script->script = malloc(size);
		
		fread(script->script, size, 1, fp);
		fclose(fp);
	}
	
	return 0;
		
}

int custom_config (json_object *json, void * p_datastruct) {
	json_object *scripts,*script;
	int i;
	p_luaclient_config config = (p_luaclient_config)p_datastruct;
		
	// get size of scripts
	get_objval(json, CONFIG_KEY_SCRIPTS, &scripts);
	config->cntscripts = json_object_array_length(scripts);

	log_message(DEBUG, "got %d scripts", config->cntscripts);

	// allocate data for scripts
	config->scripts = malloc(config->cntscripts * sizeof(t_script));
	memset(config->scripts,0,config->cntscripts * sizeof(t_script));	

	// create scripts
	for (i = 0; i < config->cntscripts; i++) {
		log_message(DEBUG, "script %d: %s", i, config->scripts[i]);
		p_script ps = &(config->scripts[i]);
		script = json_object_array_get_idx(scripts, i);
		get_intval(script,CONFIG_KEY_MESSAGE_ID, &(ps->filter));
		get_stringval(script,CONFIG_KEY_SCRIPTFILE, ps->filename,255);

		printf("script %d: %d, %s\n", i, config->scripts[i].filter, config->scripts[i].filename);
		
	}
		
	
}

void clear_custom_config(p_luaclient_config config) {
	int i;
	for (i = 0; i < config->cntscripts; i++) {
		free(config->scripts[i].script);
	}
	free (config->scripts);
	config->scripts = 0;
}
