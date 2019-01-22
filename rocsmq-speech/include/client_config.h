#ifndef SPEECHCLIENT_H
#define SPEECHCLIENT_H

#include <rocsmq.h>
#include <configparser.h>

#define CONFIG_KEY_VOICE	 "voice"
#define CONFIG_KEY_SPEED	 "speed"
#define CONFIG_KEY_VOLUME	 "volume"
#define CONFIG_KEY_PITCH	 "pitch"
#define CONFIG_KEY_GAP		 "gap"


typedef struct s_clientconfig {
	char voice[32];
	int speed;
	int volume;
	int pitch;
	int gap;
} t_clientconfig, *p_clientconfig;


int custom_config (json_object *json, void * p_datastruct);
	
#endif
