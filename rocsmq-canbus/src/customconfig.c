#include "customconfig.h"

int canbus_custom_config (json_object *json, void * p_datastruct) {
	p_clientconfig config = (p_clientconfig) p_datastruct;
	json_object *messagemap, *messageitem;
	
	get_stringval(json, CONFIG_KEY_DEVICE, config->devicefile, 255 );
	get_intval(json, CONFIG_KEY_BAUDRATE, &(config->kbaud));
	
	// get messagemap
	get_objval(json, CONFIG_KEY_MESSAGEMAP, &messagemap);
	// iterate all items 
	int entries = json_object_array_length(messagemap);
	int i;
	for (i = 0; i < entries; i++) {
		messageitem = json_object_array_get_idx(messagemap,i);
		
		t_messagemap map; 
		memset(&map, 0, sizeof(t_messagemap));
		
		if (! parse_mapentry(messageitem, &map)) {
			printf( "could not parse map entry json_object\n");
			return -1; 
		}
		
		add_messagemap(config, &map);
	}
}

/**
 * parse a message map entry json text 
 */ 
int parse_mapentry(json_object *json, p_messagemap map) {
	get_stringval (json, CONFIG_KEY_MESSAGEMAP, map->message, ROCS_IDSIZE);
	get_intval    (json, CONFIG_KEY_CAN,&(map->can_id));
	get_boolval	  (json, CONFIG_KEY_PERMANENT, &(map->permanent));
} 


/**
 * add a message to message list
 */ 
int add_message(p_clientconfig config, int can_id, char *message, int permanent) {
	t_messagemap map;
	map.can_id = can_id;
	strcpy(map.message, message);
	map.permanent = permanent;
	
	return add_messagemap(config, &map);
	
}

int add_messagemap(p_clientconfig config, p_messagemap map) {

	p_linkedlist item = ll_create(&map, sizeof(t_messagemap));
	
	config->map = ll_add(config->map, item, LL_BACK);
	
	return 0;
}

/**
 * compare function to find an item in list
 */ 
int find_message(p_linkedlist listitem, p_linkedlist searchitem) {
	p_messagemap listelem = (p_messagemap)listitem->data;
	p_messagemap searchelem = (p_messagemap) searchitem->data;
	
	// search for can id if neccessary
	if (searchelem->can_id != 0) {
		return listelem->can_id - searchelem->can_id;
	}
	
	// or search for message id string
	if (searchelem->message != 0) {
		return strncmp(listelem->message, searchelem->message, ROCS_IDSIZE);
	}
	
	// otherwise never find..
	return 1;
}

/**
 * get message from message map
 */ 
int get_message(p_clientconfig config, int can_id, char *message, p_messagemap result) {
	// create search item
	t_messagemap map;
	map.can_id = can_id;
	strcpy(map.message, message);
	map.permanent = false;
	p_linkedlist find = ll_create(&map,sizeof(t_messagemap));
	
	// find item and set it to result
	p_linkedlist item = ll_find(config->map, find, find_message);
	if (item) {
		memcpy(item->data, result, sizeof(t_messagemap));
		
		// delete item if not permanent
		if (result->permanent == 0) {
			ll_remove(config->map, item);
			ll_destroy(item);
		}
		
		// return that something has been found
		return 1;
	}
	// nothing found
	return 0;
}

/**
 * delete message from message map
 */ 
int del_message(p_clientconfig config, p_linkedlist item) {
	config->map = ll_remove(config->map, item);
	ll_destroy(item);
}
