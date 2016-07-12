#ifndef CLIENTCONFIG_H
#define CLIENTCONFIG_H

#include <rocsmq.h>
#include <configparser.h>

#define KEY_DEVICE	"device"
#define KEY_SAMPLINRATE	"samplingrate"
#define KEY_KEYWORDLIST "keywordlist"
#define KEY_SENDTO "sendmessage"
#define KEY_HMMFILE "hmmfile"
#define KEY_LMFILE	"lmfile"
#define KEY_DICTFILE "dictfile"
#define KEY_DELAY_AFTER "delay_after"

#define ORDER_START	"sphinx.start"
#define ORDER_STOP  "sphinx.stop"

typedef struct s_clientconfig {
	char  device[255];
	int  samplingrate;
	char  sendto[ROCS_IDSIZE];
	char keywordlist[255];
	char hmmfile[255];
	char lmfile[255];
	char dictfile[255];
	int delay_after;
} t_clientconfig, *p_clientconfig;


int custom_config (json_object *json, void * p_datastruct);
	
#endif
