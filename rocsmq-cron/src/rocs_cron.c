#include "rocs_cron.h"

p_linkedlist cronlist = 0;

t_cronjob searchjob = {
	.timestamp = 0,
	.period = 0,
	.repetitions = 0,
	.message = "",
};
t_linkedlist cronitem = {
	.last = 0,
	.next = 0,
	.data = &searchjob,
};

t_rocsmq_message cronmessage = {
	.id = 0,
	.sender = CRONJOB_MESSAGE_SENDER,
	.tail = "",
};


int jobsort (p_linkedlist a, p_linkedlist b) {
	p_cronjob ca = (p_cronjob) a->data;
	p_cronjob cb = (p_cronjob) b->data;
	
	return (cb->timestamp - ca->timestamp);
}

int jobequal (p_linkedlist a, p_linkedlist b) {
	p_cronjob ca = (p_cronjob) a->data;
	p_cronjob cb = (p_cronjob) b->data;
	return (ca->timestamp == cb->timestamp) && (0 == strcmp(ca->message, cb->message));
	
	return 0;
}

/**
 * TODO: make it safe
 */ 
int parse_cronjob(json_object * json, p_cronjob job) {
	json_object *message;
	
	get_intval(json, CRONJOB_TAG_TIMESTAMP, &job->timestamp);
	get_intval(json, CRONJOB_TAG_PERIOD, &job->period);
	get_intval(json, CRONJOB_TAG_COUNT, &job->repetitions);

	get_stringval(json, CRONJOB_TAG_MESSAGE, job->message, 512);

	get_objval(json, CRONJOB_TAG_DATA, &message);
	strcpy (job->data, json_object_to_json_string(message));
	
}

int add_cronjob(p_cronjob job) {
	log_message(ERROR,"Adding job for timestamp %d, cronlist %d", job->timestamp, cronlist);
	p_linkedlist item = ll_create(job, sizeof(t_cronjob));
	log_message(DEBUG, ".");
	cronlist = ll_add(cronlist,item, LL_BACK, jobsort);
	log_message(DEBUG, "..");
}

int del_cronjob(p_cronjob job){
	p_linkedlist item = ll_create(job, sizeof(t_cronjob));
	cronlist = ll_remove(cronlist,item);
	ll_destroy(item);
}

int32_t tick (TCPsocket sock, uint32_t add) {
	p_linkedlist item, next;
	p_cronjob job = (p_cronjob)cronitem.data;

	uint32_t timestamp = job->timestamp + add;
	job->timestamp = timestamp;
	
	item = cronlist;

	log_message(DEBUG,"Tick : %d, cronlist: %d", timestamp, cronlist);
	/*
	 * handle cron call
	 */ 
	while(item) {
		next = item->next;

		job = (p_cronjob)item->data;
		if (! job) {
			return -1;
		}
		
		if (job->timestamp == timestamp) {
			log_message(DEBUG, "Found a job!");
					
			/*
			 * set message data
			 */
			strncpy(cronmessage.id, job->message, ROCS_IDSIZE);
			strcpy(cronmessage.tail, job->data);  
			rocsmq_send(sock, &cronmessage, 0);
			
			log_message(DEBUG, "Data sent!");
			/* 
			 * re-add or delete item
			 * if repetititons == -1, repeat infinitel
			 */ 
			if (job->repetitions != 0) {
				if (job->repetitions != REPEAT_INFINITE)
					job->repetitions--;	
				job->timestamp += job->period;
				//ll_sort(cronlist, jobsort);
		
			} else {
				log_message(DEBUG, "removing item");
				cronlist = ll_remove(cronlist, item);
				log_message(DEBUG, "removed, destroying");
				ll_destroy(item);
			}
		
		}
		/*
		 * check next item
		 */ 
		item = next; 
	}
}
