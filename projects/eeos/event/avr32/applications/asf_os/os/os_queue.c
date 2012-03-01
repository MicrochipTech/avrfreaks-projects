/*! \file
 * \brief eeOS Queues
 * \author Blaise Lengrand (blaise.lengrand@gmail.com)
 * \version 0.1
 * \date 2011
 *
 * \section eeos_license License
 * \ref group_os is provided in source form for FREE evaluation, for
 * educational use or for peaceful research. If you plan on using \ref group_os
 * in a commercial product you need to contact the author to properly license
 * its use in your product. The fact that the  source is provided does
 * NOT mean that you can use it without paying a licensing fee.
 */

#include "os_core.h"

bool os_queue_sort_fifo(struct os_queue *a, struct os_queue *b)
{
	/* Always return true, means a should be always placed before b */
	return true;
}

bool os_queue_sort_lifo(struct os_queue *a, struct os_queue *b)
{
	/* Always return false, means b should be always placed before a */
	return false;
}

#if CONFIG_OS_USE_PRIORITY == true
bool os_queue_process_sort_priority(struct os_queue *a, struct os_queue *b)
{
	/* Get the process associated with the queue element */
	struct os_process *proc1 = os_queue_process_from_queue(a)->proc;
	struct os_process *proc2 = os_queue_process_from_queue(b)->proc;
	/* The first element is the one with the highest priority */
	return (proc1->priority <= proc2->priority);
}

bool os_queue_doubly_process_sort_priority(struct os_queue_doubly *a,
		struct os_queue_doubly *b)
{
	/* Get the process associated with the queue element */
	struct os_process *proc1 = os_queue_doubly_process_from_queue_doubly(a)->proc;
	struct os_process *proc2 = os_queue_doubly_process_from_queue_doubly(b)->proc;
	/* The first element is the one with the highest priority */
	return (proc1->priority <= proc2->priority);
}
#endif

void os_queue_add_sort(struct os_queue **first_elt, struct os_queue *new_elt,
		os_queue_sort_t sort_fct)
{
	struct os_queue *current_elt;
	struct os_queue *prev_elt = NULL;

	/* Add the element to the list */
	current_elt = *first_elt;
	/* Look for the position in the list where to insert the new element.
	 * The search will start from the begining until the end. This process
	 * uses a sorting function to proceed.
	 */
	while (current_elt && sort_fct(current_elt, new_elt)) {
		prev_elt = current_elt;
		current_elt = current_elt->next;
	}
	/* If the element is not at the beginning of the list */
	if (prev_elt) {
		os_queue_insert_after(prev_elt, new_elt);
	}
	/* else the element is at the beginning of the list */
	else {
		os_queue_insert_first(first_elt, new_elt);
	}
}

void os_queue_doubly_add_sort(struct os_queue_doubly **first_elt,
		struct os_queue_doubly *new_elt,
		os_queue_doubly_sort_t sort_fct)
{
	struct os_queue_doubly *current_elt;
	struct os_queue_doubly *prev_elt = NULL;

	/* Add the element to the list */
	current_elt = *first_elt;
	/* Look for the position in the list where to insert the new element.
	 * The search will start from the begining until the end. This process
	 * uses a sorting function to proceed.
	 */
	while (current_elt && sort_fct(current_elt, new_elt)) {
		prev_elt = current_elt;
		current_elt = current_elt->next;
	}
	/* If the element is not at the beginning of the list */
	if (prev_elt) {
		os_queue_doubly_insert_after(prev_elt, new_elt);
	}
	/* else the element is at the beginning of the list */
	else {
		os_queue_doubly_insert_first(first_elt, new_elt);
	}
}

bool os_queue_remove(struct os_queue **first_elt, struct os_queue *elt)
{
	struct os_queue *current_elt = *first_elt;
	struct os_queue *prev_elt = NULL;

	/* To remove an element from a queue, the algorithm needs to search from
	 * the first element until the end. If it reaches the end, it stops its
	 * research.
	 */
	while (current_elt) {
		/* If the element has been found */
		if (current_elt == elt) {
			/* If the element is not the first one */
			if (prev_elt) {
				prev_elt->next = current_elt->next;
			}
			/* Else it is the first one, just pop it */
			else {
				os_queue_pop(first_elt);
			}
			/* Return true to notify that the element has been found
			 * in the list, and has been removed.
			 */
			return true;
		}
		/* The current element is not a match, get the next one */
		prev_elt = current_elt;
		current_elt = current_elt->next;
	}
	/* No element has been found, notify by returning false */
	return false;
}

void os_queue_doubly_remove(struct os_queue_doubly *elt)
{
	struct os_queue_doubly *prev_elt = elt->prev;
	struct os_queue_doubly *next_elt = elt->next;

	/* Update the previous element if any */
	if (prev_elt) {
		prev_elt->next = next_elt;
	}
	/* Update the next element if any */
	if (next_elt) {
		next_elt->prev = prev_elt;
	}
}

void os_queue_doubly_remove_ex(struct os_queue_doubly **first_elt,
		struct os_queue_doubly *elt)
{
	/* If the element is the first element of the list */
	if (*first_elt == elt) {
		os_queue_doubly_pop(first_elt);
	}
	/* Else use another algorithm */
	else {
		os_queue_doubly_remove(elt);
	}
}
