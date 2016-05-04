#ifndef CRON_CLIENTCONFIG_H
#define CRON_CLIENTCONFIG_H

#include <rocsmq.h>
#include <configparser.h>

#include "rocs_cron.h"

#define TAG_DELAY	"delay"
#define TAG_TICK	"tick"
#define TAG_JOBS	"jobs"

typedef struct s_cronconfig {
	int32_t delay;
	int32_t tick;
} t_cronconfig, * p_cronconfig;

int custom_config (json_object *json, void * p_datastruct);
void clear_custom_config(void * p_datastruct);
	
#endif
