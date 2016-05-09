/*
 * linkedlist.h
 *
 *  Created on: 27.08.2015
 *      Author: tecdroid
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

#define LL_FRONT	0x01
#define LL_BACK		0x02
#define LL_SORT		0x03

/**
 * the linked list entries;
 */
typedef struct s_linkedlist {
	struct s_linkedlist *last;
	struct s_linkedlist *next;
	void *data;
} t_linkedlist, *p_linkedlist;

/**
 * sort function if neccessary
 */
typedef int (*_ll_compare) (p_linkedlist a, p_linkedlist b);

/**
 * creates a liked list item
 */
p_linkedlist ll_create(void *data, size_t size);
/**
 * destroys a linked list item
 */
void ll_destroy(p_linkedlist item);
/**
 * recursively destroys a linked list
 */
void ll_destroy_list(p_linkedlist list);

/**
 * gets the last item of a linked list
 */
p_linkedlist ll_getlast(p_linkedlist list);
/**
 * adds an item to a linked list
 */
p_linkedlist ll_add(p_linkedlist list, p_linkedlist items, int where, _ll_compare comparefunc);

/** 
 * sorts a linked list
 */
p_linkedlist ll_sort(p_linkedlist list, _ll_compare comparefunc);  
/**
 * finds an item in a linked list
 */
p_linkedlist ll_find(p_linkedlist list, p_linkedlist item, _ll_compare comparefunc);
/**
 * removes an item from a linked list
 */
p_linkedlist ll_remove(p_linkedlist list, p_linkedlist item);
/**
 * removes the first or last item from list
 */
p_linkedlist ll_pop (p_linkedlist list, p_linkedlist *item, int where, _ll_compare comparefunc);

#endif /* LINKEDLIST_H_ */
