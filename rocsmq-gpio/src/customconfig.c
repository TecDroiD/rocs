#include "customconfig.h"

int gpio_custom_config (json_object *json, void * p_datastruct) {
	json_object *pinlist = 0, *pinconfig = 0, *value = 0;
	p_pin pin = 0;
	
	int size = 0;
	int i;
	log_message(DEBUG, "Loading custom config.");
	p_clientconfig config = (p_clientconfig) p_datastruct;


    // find pinlist
    get_objval (json, CONFIG_KEY_PINCONFIG, & pinlist);
    if (!pinlist) { /* maybe check for json_type_array */
		log_message(ERROR, "could not find pinlist");
		return -1;
	}
	
	// get number of list items and create list memory
	size = json_object_array_length(pinlist); 
	log_message(DEBUG, "Reading %d Pins.", size);
	config->num_pins = size;
	config->pins = malloc(sizeof(t_pin) * size);
	if (! config->pins) {
		log_message(ERROR, "could not allocate memory for pinlist.");
		return -2;
	}
	
	// read pin data
	for(i = 0; i < size; i++) {
		pinconfig = json_object_array_get_idx(pinlist, i);
		pin = &config->pins[i];
		
		get_intval(pinconfig, CONFIG_KEY_NUMBER, &(pin->number));
		get_intval(pinconfig, CONFIG_KEY_DIRECTION, &(pin->direction));
		get_stringval(pinconfig, CONFIG_KEY_MAPNAME, pin->mapname);
		
		log_message(DEBUG, " - Having Pin %d with direction %d mapped to '%s'", 
			pin->number, pin->direction, pin->mapname);
	}	
} 

int release_config( void * p_datastruct) {
	p_clientconfig config = (p_clientconfig) p_datastruct;

	free (config->pins);
}
