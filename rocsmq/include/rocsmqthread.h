/*
 * rocsmqthread.h
 *
 *  Created on: 05.10.2015
 *      Author: tecdroid
 */

#ifndef ROCSMQTHREAD_H_
#define ROCSMQTHREAD_H_

#include <SDL/SDL_mutex.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_thread.h>

#include "linkedlist.h"
#include "rocsmq.h"

/**
 * data for a process
 */
typedef struct s_processdata{
	int running;
	volatile p_linkedlist queue;
	TCPsocket sock;
	SDL_mutex *mutex;
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
SDL_Thread * rocsmq_start_thread(TCPsocket socket);

/**
 * destroy the rocsmq thread
 */
void rocsmq_destroy_thread(SDL_Thread* thread);

/**
 * @param run
 */
void rocsmq_thread_set_running(int run);

/**
 * is thread still running?
 */
int rocsmq_thread_is_running();

#endif /* ROCSMQTHREAD_H_ */
