#include "client_config.h"
int custom_config (json_object *json, void * p_datastruct) {
	p_cronconfig config = p_datastruct;
	t_cronjob cronjob;
	
	json_object *jobs = 0;
	json_object *job;
	int length = 0;
	int i;
	
	// parse tick and delay
	get_intval(json, TAG_DELAY, &config->delay);
	get_intval(json, TAG_TICK, &config->tick);
	
	log_message(ERROR, "Delay: %d, Ticks: %d",config->delay, config->tick);

	// parse cronjobs if there..
	get_objval(json, TAG_JOBS, &jobs);
	if (jobs != 0) {
		length = json_object_array_length(jobs);
		log_message(ERROR, "Having %d number of jobs", length);
		for(i = 0; i < length; i++) {
			job = json_object_array_get_idx(jobs,i);
			log_message(ERROR, "Parsing job %d", i);
			parse_cronjob(job, &cronjob);
			add_cronjob(&cronjob);
			log_message(ERROR, "Job added");
		}
	}


}

void clear_custom_config(void * p_datastruct) {
	p_cronconfig config = p_datastruct;
	free(config);	
}
