/*
 * rocsmqthread.h
 *
 *  Created on: 05.10.2015
 *      Author: tecdroid
 */

#ifndef ROCSMQTHREAD_H_
#define ROCSMQTHREAD_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <pthread.h>

#include "linkedlist.h"
#include "rocsmq.h"

#define ROCSMQ_THREAD_RUN	1
#define ROCSMQ_THREAD_STOP	0

/**
 * data for a process
 */
typedef struct s_processdata{
	int running;
	volatile p_linkedlist queue;
	int sock;
	pthread_mutex_t mutex;
} t_processdata, *p_processdata;

/**
 * has queue more messages?
 */
int rocsmq_has_messages();

/**
 * get next message from message queue
 */
void rocsmq_get_message(p_rocsmq_message message);

/**
 * start the rocsmq thread
 */
pthread_t rocsmq_start_thread(int socket);

/**
 * destroy the rocsmq thread
 */
void rocsmq_destroy_thread(pthread_t thread);

/**
 * @param run
 */
void rocsmq_thread_set_running(int run);

/**
 * is thread still running?
 */
int rocsmq_thread_is_running();

#ifdef __cplusplus
}
#endif

#endif /* ROCSMQTHREAD_H_ */
