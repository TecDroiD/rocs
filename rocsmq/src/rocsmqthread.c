/*
 * rocsmqthread.c
 *
 *  Created on: 05.10.2015
 *      Author: tecdroid
 */

#include "rocsmqthread.h"

t_processdata pdata = {
		1, 0, 0, 0
};


/**
 * add a message to the rocsmq
 */
void rocsmq_add(p_rocsmq_message message) {
	pthread_mutex_lock(&pdata.mutex);

	p_linkedlist listitem;
	listitem = ll_create(message, sizeof(t_rocsmq_message));
	pdata.queue = ll_add(pdata.queue,listitem,LL_BACK, 0);

	pthread_mutex_unlock(&pdata.mutex);
}

/**
 * rocsmq threaed function
 */
void * rocsmq_thread (void *data) {
	t_rocsmq_message message;

	while (pdata.running) {
		if (rocsmq_recv(pdata.sock,&message,0)) {
			rocsmq_add((p_rocsmq_message) &message);
		}
		rocsmq_delayms(50);
	}
	return 0;
}

/**
 * get next message from message queue
 */
void rocsmq_get_message (p_rocsmq_message message) {
	pthread_mutex_lock(&pdata.mutex);

	p_linkedlist listitem;
	pdata.queue = ll_pop(pdata.queue,&listitem,LL_FRONT,0);
	memcpy(message,listitem->data, sizeof(t_rocsmq_message));
	ll_destroy(listitem);

	pthread_mutex_unlock(&pdata.mutex);
}

/**
 * start the rocsmq thread
 */
pthread_t rocsmq_start_thread(int socket) {
	pdata.sock = socket;

	// create mutex
	int res = pthread_mutex_init(&pdata.mutex, NULL);
	if (0 != res) {
	  fprintf(stderr, "Couldn't create mutex: %d", res);
	  return 0;
	}

	// create thread
	pthread_t thread;
	res = pthread_create(&thread, NULL, rocsmq_thread,NULL);
	if (0 != res) {
		fprintf(stderr, "Couldn't create thread: %d", res);
		return 0;
	}
	
	// activate thread
	rocsmq_thread_set_running(1);
	
	// return thread
	return thread;
}

/**
 * destroy the rocsmq thread
 */
void rocsmq_destroy_thread(pthread_t thread) {
	void *thread_result = 0;
	rocsmq_thread_set_running(0);
	pthread_join(thread, &thread_result);
	pthread_mutex_destroy(&pdata.mutex);
}


/**
 * has queue more messages?
 */
int rocsmq_has_messages(){
	int x;
	pthread_mutex_lock(&pdata.mutex);
	x = (pdata.queue != 0);
	pthread_mutex_unlock(&pdata.mutex);
	return x;
}

/**
 * @param run
 */
void rocsmq_thread_set_running(int run) {
	pthread_mutex_lock(&pdata.mutex);

	pdata.running = run;

	pthread_mutex_unlock(&pdata.mutex);
}

/**
 * @return running
 */
int rocsmq_thread_is_running() {
	int run;
	pthread_mutex_lock(&pdata.mutex);
	run = pdata.running;
	pthread_mutex_unlock(&pdata.mutex);

	return run;
}
