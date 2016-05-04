#ifndef ROCS_CRON_H
#define ROCS_CRON_H

#include <rocsmq.h>
#include <log.h>
#include <linkedlist.h>

#define CRONJOB_MESSAGE_ID	MESSAGE_ID_INFRASTRUCTURE | MESSAGE_CLIENT_CRON

#define CRONJOB_MESSAGE_ADD	CRONJOB_MESSAGE_ID | 0x0001
#define CRONJOB_MESSAGE_DEL	CRONJOB_MESSAGE_ID | 0x0000

#define CRONJOB_MESSAGE_SENDER	"cron"

#define CRONJOB_TAG_TIMESTAMP	"timestamp"
#define CRONJOB_TAG_PERIOD		"period"
#define CRONJOB_TAG_COUNT		"count"
#define CRONJOB_TAG_MESSAGE		"message"
#define CRONJOB_TAG_DATA		"data"

#define REPEAT_INFINITE			-1

typedef struct s_cronjob {
	uint32_t timestamp;
	uint32_t period;
	int32_t repetitions;
	uint32_t message;
	char data[512];
} t_cronjob, *p_cronjob;


int parse_cronjob(json_object * json, p_cronjob job);

int add_cronjob(p_cronjob job);
int del_cronjob(p_cronjob job);

int32_t tick (TCPsocket sock, uint32_t add);

#endif
