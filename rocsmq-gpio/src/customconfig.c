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

	// for mapping direction
	char dir[8];

	// read pin data
	for(i = 0; i < size; i++) {
		pinconfig = json_object_array_get_idx(pinlist, i);
		pin = &config->pins[i];
		
		pin->countdown = 0;
		get_stringval(pinconfig, CONFIG_KEY_NUMBER,pin->number, 8);
		
		get_stringval(pinconfig, CONFIG_KEY_DIRECTION,dir, 8);
		if (0 == strcmp(dir, GPIO_DIRECTION_IN)) {
			log_message(DEBUG, "Input");
			pin->direction = 0;
		} else if (0 == strcmp(dir, GPIO_DIRECTION_OUT)) {
			log_message(DEBUG, "Output");
			pin->direction = 1;
		} 
		
		get_stringval(pinconfig, CONFIG_KEY_MAPNAME, pin->mapname,20);
		get_intval(pinconfig, CONFIG_KEY_INHIBIT, pin->inhibition, 0);
		
		log_message(INFO, " - Having Pin %d with direction %d mapped to '%s'", 
			pin->number, pin->direction, pin->mapname);
	}	
} 

int release_config( void * p_datastruct) {
	p_clientconfig config = (p_clientconfig) p_datastruct;

	free (config->pins);
}
