/*
 * daemonizer.c
 *
 *  Created on: 27.08.2015
 *      Author: tecdroid
 */

#include "daemonizer.h"

/**
 * signal handler function for signals to handle ;-p
 */
void simple_signal_handler(int sig) {
	switch (sig) {
	case SIGHUP:
		log_message(INFO, "hangup signal catched");
		break;
	case SIGTERM:
		log_message(INFO, "terminate signal catched");
		break;
	default: /* ignore the rest */
		break;
	}
}

int daemonize(const char *runningdir, t_sighandler sig_handler_func) {
	int i, lfp;
	char str[10];

	t_sighandler sig_handler = sig_handler_func;
	if (!sig_handler) {
		sig_handler= simple_signal_handler;
	}

	if (getppid() == 1)
		return 0; /* already a daemon */
	i = fork();
	if (i < 0)
		return 1; /* fork error */
	if (i > 0)
		return 0; /* parent exits */
	/* child (daemon) continues */
	setsid(); /* obtain a new process group */
	for (i = getdtablesize(); i >= 0; --i)
		close(i); /* close all descriptors */
	i = open("/dev/null", O_RDWR);
	dup(i);
	dup(i); /* handle standart I/O */
	umask(027); /* set newly created file permissions */
	chdir(runningdir); /* change running directory */

	/*
	 * lock file allocation
	 */
	lfp = open(LOCK_FILE, O_RDWR | O_CREAT, 0640);
	if (lfp < 0)  {
		log_message(ERROR, "could not open lockfile");
		return 1; /* can not open */
	}
	if (lockf(lfp, F_TLOCK, 0) < 0) {
		log_message(WARNING, "could not lock lockfile");
		return 0; /* can not lock */
	}
	/* first instance continues */
	sprintf(str, "%d\n", getpid());
	write(lfp, str, strlen(str)); /* record pid to lockfile */

	/* signal handler */
	signal(SIGCHLD, sig_handler); /* ignore child */
	signal(SIGTSTP, sig_handler); /* ignore tty signals */
	signal(SIGTTOU, sig_handler);
	signal(SIGTTIN, sig_handler);
	signal(SIGHUP, sig_handler); /* catch hangup signal */
	signal(SIGTERM, sig_handler); /* catch kill signal */

	return 0;
}
