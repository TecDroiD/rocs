#include "customconfig.h"

int i2cbus_custom_config (json_object *json, void * p_datastruct) {

	log_message(DEBUG, "Loading custom config.");
	p_clientconfig config = (p_clientconfig) p_datastruct;
	p_initscript init;
	
	json_object *initlist = 0;
	json_object *slave;
	json_object *script = 0;
	json_object *sbyte;
	int cnt, scnt;
	int d;

	get_stringval(json, CONFIG_KEY_DEVICE, config->devicefile, 255 );
	get_intval(json, CONFIG_KEY_BAUDRATE, &(config->kbaud));

/*		
	// get init scripts
	get_objval(json, CONFIG_KEY_SLAVES, &initlist);
	if (json_type_array == json_object_get_type(initlist)) {
		config->num_initscripts = json_object_array_length(initlist);
		// if there are init scripts 
		if (config->num_initscripts) {			
			// initialize script dataset
			log_message(DEBUG, "Getting %d init scripts");
			config->initscripts = (p_initscript) malloc(sizeof(t_initscript) * config->num_initscripts);
			memset(config->initscripts, 0,  sizeof(t_initscript) * config->num_initscripts);
			
			//for each init script
			for (cnt = 0; cnt < config->num_initscripts; cnt++) {
				// read init script meta data
				slave = json_object_array_get_idx(initlist,cnt);
				init = &(config->initscripts[cnt]);
				// hole slave id
				get_intval(slave, CONFIG_KEY_ADDR, &d);
				init->slave = (uint8_t) d;

				// hole skripts
				get_objval(slave, CONFIG_KEY_INIT, &script);
				
				if (json_type_array == json_object_get_type(script)) {
					for(scnt = 0; scnt < json_object_array_length(script)) {
						sbyte = json_obect_array_get_idx(script,scnt);
						init->initfile[scnt] = json_object_get_numb
				get_stringval(slave,CONFIG_KEY_INIT, init->initfile, 255);
			}
		}
	}
*/
}
