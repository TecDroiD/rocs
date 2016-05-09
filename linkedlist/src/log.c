/*
 * log.c
 *
 *  Created on: 30.09.2015
 *      Author: tecdroid
 */

#include "../include/log.h"

static char logname[32];
static FILE *logfile = 0;
static int loglevel = LOG_LEVEL_WARNING;
static int self_opened = 0;

char const *loglevelstrings[] = { "DEBUG", "INFO", "WARNING", "ERROR" };


int openlog(char const *name, char const *filename) {
	FILE *fp = stdout;
	if (filename != 0 && strlen(filename) > 0) {
		printf("opening logfile\n");
		self_opened = 1;
		fp = fopen(filename, "a");
		if (! fp) return -1;
	}

	log_init(name,fp);
	return 0;
}

void closelog() {
	if (logfile && self_opened) {
		fclose(logfile);
	}
}

void log_init(char const *name, FILE *fp) {
	strncpy(logname, name, 32);
	self_opened = 0;
	logfile = fp;
}

void log_message(int level, char const *text, ...) {
	char date[64];

	if (!logfile) {
		logfile = stdout;
	}
	
	if (loglevel <= level) {
		va_list args;
		va_start(args, text);
		time_t now;
		now = time(0);
		struct tm *tmp;
		tmp = localtime(&now);
		strftime(date,64,"%D %T",tmp);
		fprintf(logfile, "%s [%s] %s\t- ", date, logname, loglevelstrings[level]);
		vfprintf(logfile,text, args);
		fprintf(logfile,"\n");
		fflush(logfile);
	}
}

int log_getlevel(char *level) {
	int i;
	
	for (i = 0; i < COUNT_LOG_LEVELS; i++) {
		if (0 == strcasecmp(level, loglevelstrings[i])) {
			return i;
		}
	}
	
	return -1;
}

void log_setlevel(int level) {
	if (level >= LOG_LEVEL_DEBUG && level <= LOG_LEVEL_ERROR) {
		loglevel = level;
	}
}
