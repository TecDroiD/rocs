#include "customconfig.h"

int add_color_range(json_object * json, p_clientconfig clientconfig) {
	int size = json_object_array_length(json);
	log_message(DEBUG, "having %d items.", size);
	int i;
	int load;
	
	// allocate memory for color ranges
	clientconfig->cr = new t_colorrange[size];
	
	if (!clientconfig->cr) {
		log_message(ERROR, "could not allocate memory for color range data");
		return -1;
	}

	memset(clientconfig->cr, 0, sizeof(t_colorrange) * size);
	
	// read color range data
	for (i = 0; i < size; i++) {
		json_object *node = json_object_array_get_idx(json, i);
		p_colorrange r = & clientconfig->cr[i];
		
		get_stringval(node, CONFIG_KEY_SENDMESSAGE, r->message, 255);
	
		get_intval(node, CONFIG_KEY_MIN_HUE, & load);
		r->minvals[HUE] = (int8_t)load;
		get_intval(node, CONFIG_KEY_MIN_SAT, & load);
		r->minvals[SAT] = (int8_t)load;
		get_intval(node, CONFIG_KEY_MIN_VAL, & load);
		r->minvals[VAL] = (int8_t)load;
		get_intval(node, CONFIG_KEY_MAX_HUE, & load);
		r->maxvals[HUE] = (int8_t)load;
		get_intval(node, CONFIG_KEY_MAX_SAT, & load);
		r->maxvals[SAT] = (int8_t)load;
		get_intval(node, CONFIG_KEY_MAX_VAL, & load);
		r->maxvals[VAL] = (int8_t)load;
		get_intval(node, CONFIG_KEY_HOTSPOTS, & load);
		r->hotspots = (int8_t)load;
		
	}
	
	return 0;
}

int custom_config (json_object *json, void * p_datastruct) {
	json_object * color_range = 0;
	
	log_message(DEBUG, "Loading custom config.");
	p_clientconfig config = (p_clientconfig) p_datastruct;

	// get device name
	get_stringval(json, CONFIG_KEY_DEVICE, config->device, 255);
	log_message(INFO, "OpenCV device file %s ", config->device);
	
	// get color range items
	get_objval(json, CONFIG_KEY_ONCOLOR, & color_range);
	if (color_range) {
		log_message(INFO, "Reading color range elements");
		if (add_color_range(color_range, config)) {
			return -1;
		}
	}
} 
