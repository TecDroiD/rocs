#include "cli_functions.h"

/**
 * execute an order
 * @param the order name
 * @param the order input
 * @param func the function
 */
int execute(char const *name, char *input, _orderfunc_t func) {
	int len = strlen(name);
	char *pos = input;
	// find first char
	while (*pos == ' ' || *pos == '\t') pos++;
	
	if (strncasecmp(pos, name, len) == 0) {
		// set pointer behind the order to find parameters 
		pos += len + 1;		
		return func(pos);
	}
	
	return 0;
}
