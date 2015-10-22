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
	SDL_LockMutex(pdata.mutex);

	p_linkedlist listitem;
	listitem = ll_create(message, sizeof(t_rocsmq_message));
	pdata.queue = ll_add(pdata.queue,listitem,LL_BACK, 0);

	SDL_UnlockMutex(pdata.mutex);
}

/**
 * rocsmq threaed function
 */
int rocsmq_thread (void *data) {
	t_rocsmq_message message;

	while (pdata.running) {
		if (rocsmq_recv(pdata.sock,&message,0)) {
			rocsmq_add((p_rocsmq_message) &message);
		}
	}
	return 0;
}

/**
 * get next message from message queue
 */
void rocsmq_get_message (p_rocsmq_message message) {
	SDL_LockMutex(pdata.mutex);

	p_linkedlist listitem;
	pdata.queue = ll_pop(pdata.queue,&listitem,LL_FRONT,0);
	memcpy(message,listitem->data, sizeof(t_rocsmq_message));
	ll_destroy(listitem);

	SDL_UnlockMutex(pdata.mutex);
}

/**
 * start the rocsmq thread
 */
SDL_Thread * rocsmq_start_thread(TCPsocket socket) {
	pdata.sock = socket;

	pdata.mutex = SDL_CreateMutex();
	if (!pdata.mutex) {
	  fprintf(stderr, "Couldn't create mutex\n");
	  return 0;
	}

	return SDL_CreateThread(rocsmq_thread,0);
}

/**
 * destroy the rocsmq thread
 */
void rocsmq_destroy_thread(SDL_Thread* thread) {
	rocsmq_thread_set_running(0);
	SDL_KillThread(thread);
	SDL_DestroyMutex(pdata.mutex);
}


/**
 * has queue more messages?
 */
int rocsmq_has_messages(){
	int x;
	SDL_LockMutex(pdata.mutex);
	x = (pdata.queue != 0);
	SDL_UnlockMutex(pdata.mutex);
	return x;
}

/**
 * @param run
 */
void rocsmq_thread_set_running(int run) {
	SDL_LockMutex(pdata.mutex);

	pdata.running = run;

	SDL_UnlockMutex(pdata.mutex);
}

/**
 * @return running
 */
int rocsmq_thread_is_running() {
	int run;
	SDL_LockMutex(pdata.mutex);
	run = pdata.running;
	SDL_UnlockMutex(pdata.mutex);

	return run;
}
