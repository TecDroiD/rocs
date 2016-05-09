/*
 * log.h
 *
 *  Created on: 30.09.2015
 *      Author: tecdroid
 */

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>


#define LOG_LEVEL_DEBUG		0
#define LOG_LEVEL_INFO		1
#define LOG_LEVEL_WARNING 	2
#define LOG_LEVEL_ERROR 	3
#define COUNT_LOG_LEVELS	4

#define DEBUG 	LOG_LEVEL_DEBUG
#define INFO 	LOG_LEVEL_INFO
#define WARNING LOG_LEVEL_WARNING
#define ERROR 	LOG_LEVEL_ERROR

/**
 * opens a logfile to write into..
 * @param name of logging
 * @param filename to open
 * @return 0 if success
 */
int openlog(char const *name, char const *filename);
/**
 * closes the log, if allowed to
 */
void closelog();
/**
 * initializes the log with an existing file
 * @param name of logging
 * @param fp pointer to a file, eg. stdout
 */
void log_init(char const *name, FILE *fp );

/**
 * logs a message
 * @param level to log. if higher than actual loglevel, message will be discarded
 * @param text formated text
 */
void log_message(int level, char const *text, ...);

/**
 * sets the loglevel
 * @param level the log level to set.
 */
void log_setlevel(int level);

/**
 * gets the loglevel by string
 */ 
int log_getlevel(char *level);
#endif /* LOG_H_ */
