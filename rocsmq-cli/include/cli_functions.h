#ifndef CLI_FUNCTIONS_H
#define CLI_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * a function type for calling parameters
 */ 
typedef int (*_orderfunc_t)(char *params);

/**
 * execute an order if the input starts with it
 */ 
int execute(char const *name, char *input, _orderfunc_t func);
#endif
