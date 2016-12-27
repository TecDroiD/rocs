#ifndef __CUSTOMCONFIG_H
#define __CUSTOMCONFIG_H

#include <rocsmq.h>
#include <linkedlist.h>

#include <json-c/json.h>

#define CONFIG_KEY_DEVICE "device"
#define CONFIG_KEY_BAUDRATE "baudrate"
#define CONFIG_KEY_MESSAGEMAP "messages"

#define CONFIG_KEY_CAN	"canid"
#define CONFIG_KEY_ROCS	"message"
#define CONFIG_KEY_PERMANENT "permanent"


typedef struct s_clientconfig {
	int kbaud;
	char devicefile[255];
	
	p_linkedlist messagemap;
	
} t_clientconfig, *p_clientconfig;


int canbus_custom_config (json_object *json, void * p_datastruct);


/**
 * a message map entry item
 */ 
typedef struct s_messagemap {
	int can_id;
	char message[ROCS_IDSIZE];
	int permanent;
	
} t_messagemap, *p_messagemap;

/**
 * add an entry to the message map
 */ 
int add_message(p_clientconfig config, int can_id, char *message, int permanent);
int add_messagemap(p_clientconfig config, p_messagemap map);
p_messagemap get_message(p_clientconfig config, int can_id, char *message);
int del_message(p_clientconfig config, p_messagemap message);


#endif
