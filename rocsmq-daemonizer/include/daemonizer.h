/*
 * daemonizer.h
 *
 *  Created on: 27.08.2015
 *      Author: tecdroid
 */

#ifndef DAEMONIZER_H_
#define DAEMONIZER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <log.h>

#define LOCK_FILE ".pid"

typedef void (*t_sighandler)(int);

void simple_signal_handler(int sig);

int daemonize(const char *runningdir, t_sighandler sig_handler);

#ifdef __cplusplus
}
#endif

#endif /* DAEMONIZER_H_ */
