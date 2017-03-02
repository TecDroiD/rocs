#include "rocs_cron.h"

p_linkedlist cronlist = 0;

// only the timestamp is used
t_cronjob searchjob = {
	.timestamp = 1,
	.period = 1,
	.repetitions = 1,
	.message = "",
};

// a complete cronitem, just to search.
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
	
	//int ts = ca->timestamp - cb->timestamp;	
	//return (0 == ts % ca->period) && (0 == strcmp(ca->message, cb->message));
	return (0 == strcmp(ca->message, cb->message));
}

/**
 * TODO: make it safe
 */ 
int parse_cronjob(json_object * json, p_cronjob job) {
	json_object *message;
	
	get_intval(json, CRONJOB_TAG_PERIOD, &job->period);
	get_intval(json, CRONJOB_TAG_TIMESTAMP, &job->timestamp);
	get_intval(json, CRONJOB_TAG_COUNT, &job->repetitions);

	get_stringval(json, CRONJOB_TAG_MESSAGE, job->message, 512);

	get_objval(json, CRONJOB_TAG_DATA, &message);
	strcpy (job->data, json_object_to_json_string(message));
	
}

int add_cronjob(p_cronjob job) {
	job->timestamp += get_timestamp() + 1;
	
	log_message(INFO,"Adding job for timestamp %d, cronlist %d", job->timestamp, cronlist);
	p_linkedlist item = ll_create(job, sizeof(t_cronjob));
	log_message(DEBUG, ".");
	
	cronlist = ll_add(cronlist,item, LL_BACK, 0);
	log_message(DEBUG, "..");
}

/**
 * remove all jobs matching the name..
 */ 
int del_cronjob(p_cronjob job){
	p_linkedlist item = ll_create(job, sizeof(t_cronjob));
	p_linkedlist remove	= ll_find(cronlist, item, jobequal);
	while(remove != 0) {
		cronlist = ll_remove(cronlist,remove);
		ll_destroy(remove);
		remove	= ll_find(cronlist, item, jobequal);
	} 

	ll_destroy(item);
}

int32_t tick (int sock, uint32_t add) {
	
	p_linkedlist item, next;
	p_cronjob job;
	
	// increment timestamp
	searchjob.timestamp += add;
	uint32_t timestamp = searchjob.timestamp;
	

	log_message(DEBUG,"Tick : %d, cronlist: %d", timestamp, cronlist);
	/*
	 * handle cron call
	 */ 
	 
	item = cronlist;
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
			
			log_message(DEBUG, "message '%s' sent!", cronmessage.id);

			// put new timestamp 
			job->timestamp += job->period;

			// repeat until all repititions done
			if (job->repetitions > 0) {
				job->repetitions --;
		
			// after the last repitition, remove job
			} 
			
			if (job->repetitions == 0){
				log_message(DEBUG, "removing item");
				cronlist = ll_remove(cronlist,item);
				ll_destroy(item);
			}
		
		}
		/*
		 * check next item
		 */ 
		item = next; 
	}
}

int get_timestamp() {
	return searchjob.timestamp;
}
