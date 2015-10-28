#include "customconfig.h"

int i2cbus_custom_config (json_object *json, void * p_datastruct) {
	p_clientconfig config = (p_clientconfig) p_datastruct;
	
	get_stringval(json, CONFIG_KEY_DEVICE, config->devicefile, 255 );
	get_intval(json, CONFIG_KEY_BAUDRATE, &(config->kbaud));
	
}
