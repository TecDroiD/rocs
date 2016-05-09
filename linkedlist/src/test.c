#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/linkedlist.h"


int strtest(p_linkedlist a, p_linkedlist b) {
	return strcmp(b->data, a->data);
}
void print_list(p_linkedlist list) {
	if (list != 0) {
		printf (" -> %s", list->data);
		print_list(list->next);
	} else {
		printf (" .\n");
	}
}

int main (int argc, char **argv) {
	char test[] = "this is a marvellously stupid sentence just for testing";
	char *tok = "zwei";
	p_linkedlist list = 0;
	p_linkedlist elem = 0;
	tok = strtok (test, " ");
	int i = 0;
	int n;
	while (0 != tok) {
		elem = ll_create(tok, strlen(tok));
		list = ll_add(list, elem, LL_BACK, 0);
//		print_list(list);
		tok = strtok(0, " ");
		i++;
	}
	printf("number of elements: %d\n", i);

	for (n = 0; n < i; n++) {
		elem = list;
		list = ll_remove(list, elem);
		print_list(list);
		list = ll_add(list, elem, LL_BACK, 0);
		print_list(list);
	}
	
	for (n = 0; n < i; n++) {
		elem = ll_getlast(list);
		list = ll_remove(list, elem);
		print_list(list);
		list = ll_add(list, elem, LL_FRONT, 0);
		print_list(list);
	}
printf ("sorting..\n");
	list = ll_sort(list,strtest);
	print_list(list);

printf ("cleaning up....\n");	
	while (list != 0) {
		elem = list;
		list = ll_remove(list, elem);
		ll_destroy(elem);
		printf("list: %d", list); print_list(list);
		
	}
	
/*	
	elem = ll_create(&test2, STRINGSIZE);
	list = ll_add(list, elem, LL_BACK, 0);
	print_list(list);
	
	printf("first element: %s\n", list->data);
	
	elem = list;
	list = ll_remove(list, list);
	print_list(list);
//	ll_destroy (elem);
	printf("first element: %s\n", list->data);

	list = ll_add(list, elem, LL_BACK, 0);
	print_list(list);
	list = ll_remove(list, elem);
	print_list(list);
	ll_destroy(elem);
*/	

	printf("done\n");
	return 0;	
}
