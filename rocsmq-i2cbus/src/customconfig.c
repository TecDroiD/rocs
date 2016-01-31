#include "customconfig.h"

int i2cbus_custom_config (json_object *json, void * p_datastruct) {

	log_message(DEBUG, "Loading custom config.");
	p_clientconfig config = (p_clientconfig) p_datastruct;
	p_initscript init;
	
	json_object *initlist = 0;
	json_object *slave;
	int cnt;
	int d;

	get_stringval(json, CONFIG_KEY_DEVICE, config->devicefile, 255 );
	get_intval(json, CONFIG_KEY_BAUDRATE, &(config->kbaud));

	// get init scripts
	get_objval(json, CONFIG_KEY_SLAVES, &initlist);
	if (json_type_array == json_object_get_type(initlist)) {
		config->num_initscripts = json_object_array_length(initlist);
		
		// for each init script
		if (config->num_initscripts) {
			log_message(DEBUG, "Getting %d init scripts");
			config->initscripts = (p_initscript) malloc(sizeof(t_initscript) * config->num_initscripts);
			// read init script meta data
			for (cnt = 0; cnt < config->num_initscripts; cnt++) {
				slave = json_object_array_get_idx(initlist,cnt);
				init = &(config->initscripts[cnt]);
				
				get_intval(slave, CONFIG_KEY_ADDR, &d);
				init->slave = (uint8_t) d;
				get_stringval(slave,CONFIG_KEY_INIT, init->initfile, 255);
			}
		}
	}
}
