#include "configparser.h"

int get_objval		(json_object *json, char *key, json_object **val) {
	if (FALSE == json_object_object_get_ex(json, key, val)) {
		printf( "Json-Value not found (%s).\n", key);
		return -1;
	}

	return 0;	
}

int get_intval	(json_object *json, char *key, int *val) {
	json_object *object;
	enum json_type type;
	
	if(0 != get_objval(json, key, &object))
		return -1;
	
	type = json_object_get_type(object);
	if (type != json_type_int) {
		printf(  "Not a number (%s)\n", key);
		return -2;
	}
	
	*val = json_object_get_int(object);
	
	return 0;	
}
	
int get_doubleval	(json_object *json, char *key, double *val) {
	json_object *object;
	enum json_type type;
	
	if(0 != get_objval(json, key, &object))
		return -1;
	
	type = json_object_get_type(object);
	if (type != json_type_double) {
		printf( "Not a real (%s)\n", key);
		return -2;
	}
	
	*val = json_object_get_double(object);
	
	return 0;	
}
	
int get_boolval(json_object *json, char *key, json_bool *val) {
	json_object *object;
	enum json_type type;
	
	if(0 != get_objval(json, key, &object))
		return -1;
	
	type = json_object_get_type(object);
	if (type != json_type_boolean) {
		printf(  "Not a boolean (%s)\n", key);
		return -2;
	}
	
	*val = json_object_get_boolean(object);
	
	return 0;	
}
int get_stringval	(json_object *json, char *key, char *val, int maxlen) {
	json_object *object;
	enum json_type type;
	char const *v;
	
	
	if(0 != get_objval(json, key, &object))
		return -1;
	
	type = json_object_get_type(object);
	if (type != json_type_string) {
		printf(  "Not a string (%s)\n", key);
		return -2;
	}
	
	v = json_object_get_string(object);
	strncpy(val, v, maxlen);
		
	return 0;	
}
	

int parseconfig(char const *filename, p_rocsmq_baseconfig baseconfig, custom_config_t customs, void *p_datastruct) {
	FILE *fp;
	char *text;
	char loglevel[20] = "";
	
	printf("opening file %s\n", filename);
	
	size_t filesize;
	json_object * json;
	// open file
	fp = fopen(filename, "r");
	if (fp == 0) {
		printf(  "Could not open config file %s", filename);
		return -1;
	}

	// get file size
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	printf("config file size %d\n", filesize);
	
	// allocate memory for file
	text = malloc(filesize);
	fread(text, filesize, 1, fp);
	
	printf("config file %s\n", text);
	
	// parse json text
	json = json_tokener_parse(text);
	
	json_object_object_foreach (json, key, val) {
		printf("  -> '%s' : %s\n", key, val);
	}
	
	// get config base data
		get_stringval(json, KEY_SERVERIP, baseconfig->serverip, 15);
		get_intval(json,KEY_PORT, &(baseconfig->port));
		get_boolval(json, KEY_DAEMON, &(baseconfig->rundaemon));
		get_stringval(json,KEY_LOGLEVEL, loglevel, 20);
		printf("loglevel = '%s'", loglevel);
		baseconfig->loglevel = log_getlevel(loglevel); 
		
		get_stringval(json,KEY_LOGFILE, baseconfig->logfile,255);
		 
	// get custom data	 
	if(customs != 0) {
		customs(json, p_datastruct);
	}	 
	
	// release memory for file
	json_object_put(json);
	free(text);
	return 0;
}