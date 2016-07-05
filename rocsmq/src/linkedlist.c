/*
 * linkedlist.c
 *
 *  Created on: 27.08.2015
 *      Author: tecdroid
 */

#include "../include/linkedlist.h"

p_linkedlist ll_create(void *data, size_t size) {
	p_linkedlist item = (p_linkedlist) malloc(sizeof(t_linkedlist));
	// copy data
	if (item == 0) return 0;
	item->data = malloc(size);
	memcpy(item->data, data, size);

	// set item siblings to zero
	item->next = 0;
	item->last = 0;

	return item;
}

void ll_destroy(p_linkedlist item) {
	free(item->data);
	free(item);
}

void ll_destroy_list (p_linkedlist list) {
	if (list == 0) return;
	ll_destroy_list(list->next);
	ll_destroy(list);
}

p_linkedlist ll_getlast(p_linkedlist list) {
	if (list == 0) return 0;
	
	if (list->next == 0) {
		return list;
	}
//	log_message(DEBUG,"list->next : %d", list->next);
	return ll_getlast(list->next);
}

p_linkedlist ll_add(p_linkedlist list, p_linkedlist items, int where,
		_ll_compare comparefunc) {
	p_linkedlist iterator;
	switch (where) {
	case LL_FRONT:
		iterator = ll_getlast(items);
		iterator->next = list;
		list->last = iterator;
		return items;
	case LL_BACK:
		iterator = ll_getlast(list);
		if (iterator == 0) {
			return items;
		} else {
			iterator->next = items;
			items->last = iterator;
			return list;
		}
	case LL_SORT:
//		log_message(DEBUG, "adding item");
		iterator = ll_getlast(items);
		iterator->next = list;
		if (list != 0)
			list->last = iterator;
//		log_message(DEBUG, "sorting");

		iterator = ll_sort(items, comparefunc);
//		log_message(DEBUG, "done");

		return iterator;
	default:
		return items;
	}
}

void exchange (p_linkedlist a, p_linkedlist b) {
 
 void *dummy = a->data;
 a->data = b->data;
 b->data = dummy;
 		
}

p_linkedlist ll_sort(p_linkedlist list, _ll_compare comparefunc) {
	p_linkedlist start, cmp;	
	
	if (list == 0) return list;
	start = list;
	cmp = list;

//	log_message(DEBUG, "tets");

	/*
	 * iteratively sort a bubble
	 */ 
	while (cmp->next != 0) {
//		log_message(DEBUG, "ta");
		cmp = cmp->next;
		
		if (comparefunc(start, cmp) < 0) {
			exchange(start, cmp);
		}
	}
	
	
	/*
	 * recursively bubble up
	 */ 
	if (start->next != 0) {
//		log_message(DEBUG, "iterating down");

		start->next = ll_sort(start->next,comparefunc);
		start->next->last = start;
	}
	
//	log_message(DEBUG, "sorted");

	return start;
}

p_linkedlist ll_find(p_linkedlist list, p_linkedlist item, _ll_compare comparefunc) {
	if (0 == list) {
		return 0;
	}
	if (0 == comparefunc(list, item)) {
		return list;
	}

	if (list->next) {
		return ll_find(list->next, item, comparefunc);
	}

	return 0;
}

p_linkedlist ll_remove(p_linkedlist list, p_linkedlist item) {
	p_linkedlist ll = list;

	if (item == 0) {
		return list;
	}
	
	if (list == item) {
		ll = list->next;
	}

	if (item->last) {
		item->last->next = item->next;
		item->last = 0;
	}
	if (item->next)	{
		item->next->last = item->last;
		item->next = 0;
	}
	
	return ll;
}

p_linkedlist ll_pop(p_linkedlist list, p_linkedlist *item, int where, _ll_compare comparefunc) {
	p_linkedlist dummy;
	switch(where) {
	case LL_FRONT:
		*item = list;
		if(list == 0) return 0;
		if(list->next)
			list->next->last = 0;
		return list->next;
	case LL_BACK:
		dummy = ll_getlast(list);
		*item = dummy;
		dummy->last->next = 0;
		return list;
	case LL_SORT:
		dummy = ll_find(list, *item, comparefunc);
		item = &dummy;
		if (dummy) {
			ll_remove(list,dummy);
		}
		return list;
	default:
		return list;
	}
}

