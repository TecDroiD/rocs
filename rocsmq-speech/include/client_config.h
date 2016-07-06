#ifndef TCLCLIENT_H
#define TCLCLIENT_H

#include <tcl/tcl.h>

#include <rocsmq.h>
#include <configparser.h>

#define CONFIG_KEY_VOICE	 "voice"
#define CONFIG_KEY_SPEED	 "speed"
#define CONFIG_KEY_VOLUME	 "volume"
#define CONFIG_KEY_PITCH	 "pitch"


typedef struct s_clientconfig {
	char voice[32];
	int speed;
	int volume;
	int pitch;
} t_clientconfig, *p_clientconfig;


int custom_config (json_object *json, void * p_datastruct);
	
#endif
