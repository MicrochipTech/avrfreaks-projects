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

#ifndef __OS_QUEUE_H__
#define __OS_QUEUE_H__

/*! \defgroup group_os_queue Queues
 * \brief This page contains technical information related to a queue.
 * \ingroup group_os_public_api
 */

/* Macros *********************************************************************/

/*! \brief Macro used to define a new singly queue type.
 *
 * The queue type defined by this macro will inherit from all the following
 * functions (from the \ref os_queue):
 * - os_queue_<NAME>_pop alias of \ref os_queue_pop
 * - os_queue_<NAME>_head alias of \ref os_queue_head
 * - os_queue_<NAME>_insert_after alias of \ref os_queue_insert_after
 * - os_queue_<NAME>_insert_first alias of \ref os_queue_insert_first
 * - os_queue_<NAME>_add_sort alias of \ref os_queue_add_sort
 * - os_queue_<NAME>_remove alias of \ref os_queue_remove
 *
 * But also from 2 other functions to cast the defined type:
 * - os_queue_<NAME>_get_queue (get the \ref os_queue element from the
 * os_<NAME>_queue type)
 * - os_queue_<NAME>_from_queue (get the os_<NAME>_queue element from the
 * \ref os_queue type)
 *
 * \ingroup group_os_queue
 * \param NAME Prefix for the name of the queue. The queue type defined will be
 * a structure with the following symbol: os_queue_<NAME>
 * \param ... Parameters that will be included in the queue type.
 */
#define OS_QUEUE_DEFINE(NAME, ...) \
	/*! \ingroup group_os_queue */ \
	struct os_queue_##NAME { \
		/*! \brief Next element in the singly linked list */ \
		struct os_queue_##NAME *next; \
		/* Extra parameters for this type */ \
		__VA_ARGS__ \
	}; \
	/*! \brief Get the os_queue element from the \b os_##NAME##_queue type \
	 * \ingroup group_os_queue \
	 */ \
	static inline struct os_queue *os_queue_##NAME##_get_queue(struct os_queue_##NAME *elt) { \
		return (struct os_queue *) elt; \
	} \
	/*! \brief Get the \b os_##NAME##_queue element from the os_queue type \
	 * \ingroup group_os_queue \
	 */ \
	static inline struct os_queue_##NAME *os_queue_##NAME##_from_queue(struct os_queue *elt) { \
		return (struct os_queue_##NAME *) elt; \
	} \
	/*! \brief Alias of \ref os_queue_pop \
	 * \ingroup group_os_queue \
	 */ \
	static inline struct os_queue_##NAME *os_queue_##NAME##_pop(struct os_queue_##NAME **first_elt) { \
		return (struct os_queue_##NAME *) os_queue_pop((struct os_queue **) first_elt); \
	} \
	/*! \brief Alias of \ref os_queue_head \
	 * \ingroup group_os_queue \
	 */ \
	static inline struct os_queue_##NAME *os_queue_##NAME##_head(struct os_queue_##NAME *first_elt) { \
		return (struct os_queue_##NAME *) os_queue_head((struct os_queue *) first_elt); \
	} \
	/*! \brief Alias of \ref os_queue_insert_after \
	 * \ingroup group_os_queue \
	 */ \
	static inline void os_queue_##NAME##_insert_after(struct os_queue_##NAME *prev_elt, struct os_queue_##NAME *elt) { \
		os_queue_insert_after((struct os_queue *) prev_elt, (struct os_queue *) elt); \
	} \
	/*! \brief Alias of \ref os_queue_insert_first \
	 * \ingroup group_os_queue \
	 */ \
	static inline void os_queue_##NAME##_insert_first(struct os_queue_##NAME **first_elt, struct os_queue_##NAME *elt) { \
		os_queue_insert_first((struct os_queue **) first_elt, (struct os_queue *) elt); \
	} \
	/*! \brief Alias of \ref os_queue_add_sort \
	 * \ingroup group_os_queue \
	 */ \
	static inline void os_queue_##NAME##_add_sort(struct os_queue_##NAME **first_elt, struct os_queue_##NAME *elt, os_queue_sort_t sort_fct) { \
		os_queue_add_sort((struct os_queue **) first_elt, (struct os_queue *) elt, sort_fct); \
	} \
	/*! \brief Alias of \ref os_queue_remove \
	 * \ingroup group_os_queue \
	 */ \
	static inline bool os_queue_##NAME##_remove(struct os_queue_##NAME **first_elt, struct os_queue_##NAME *elt) { \
		return os_queue_remove((struct os_queue **) first_elt, (struct os_queue *) elt); \
	}

/*! \brief Macro used to define a new doubly queue type.
 *
 * The queue type defined by this macro will inherit from all the following
 * functions (from the \ref os_queue_doubly):
 * - os_queue_<NAME>_pop alias of \ref os_queue_doubly_pop
 * - os_queue_<NAME>_head alias of \ref os_queue_doubly_head
 * - os_queue_<NAME>_insert_after alias of \ref os_queue_doubly_insert_after
 * - os_queue_<NAME>_insert_first alias of \ref os_queue_doubly_insert_first
 * - os_queue_<NAME>_add_sort alias of \ref os_queue_doubly_add_sort
 * - os_queue_<NAME>_remove alias of \ref os_queue_doubly_remove
 *
 * But also from 2 other functions to cast the defined type:
 * - os_queue_<NAME>_get_queue (get the \ref os_queue_doubly element from the
 * os_<NAME>_queue type)
 * - os_queue_<NAME>_from_queue (get the os_<NAME>_queue element from the
 * \ref os_queue_doubly type)
 *
 * \ingroup group_os_queue
 * \param NAME Prefix for the name of the queue. The queue type defined will be
 * a structure with the following symbol: os_queue_<NAME>
 * \param ... Parameters that will be included in the queue type.
 */
#define OS_QUEUE_DOUBLY_DEFINE(NAME, ...) \
	/*! \ingroup group_os_queue */ \
	struct os_queue_##NAME { \
		/*! \brief Next element in the doubly linked list */ \
		struct os_queue_##NAME *next; \
		/*! \brief Previous element in the doubly linked list */ \
		struct os_queue_##NAME *prev; \
		/* Extra parameters for this type */ \
		__VA_ARGS__ \
	}; \
	/*! \brief Get the os_queue_doubly element from the \b os_##NAME##_queue type \
	 * \ingroup group_os_queue \
	 */ \
	static inline struct os_queue_doubly *os_queue_##NAME##_get_queue_doubly(struct os_queue_##NAME *elt) { \
		return (struct os_queue_doubly *) elt; \
	} \
	/*! \brief Get the \b os_##NAME##_queue element from the os_queue_doubly type \
	 * \ingroup group_os_queue \
	 */ \
	static inline struct os_queue_##NAME *os_queue_##NAME##_from_queue_doubly(struct os_queue_doubly *elt) { \
		return (struct os_queue_##NAME *) elt; \
	} \
	/*! \brief Alias of \ref os_queue_doubly_pop \
	 * \ingroup group_os_queue \
	 */ \
	static inline struct os_queue_##NAME *os_queue_##NAME##_pop(struct os_queue_##NAME **first_elt) { \
		return (struct os_queue_##NAME *) os_queue_doubly_pop((struct os_queue_doubly **) first_elt); \
	} \
	/*! \brief Alias of \ref os_queue_doubly_head \
	 * \ingroup group_os_queue \
	 */ \
	static inline struct os_queue_##NAME *os_queue_##NAME##_head(struct os_queue_##NAME *first_elt) { \
		return (struct os_queue_##NAME *) os_queue_doubly_head((struct os_queue_doubly *) first_elt); \
	} \
	/*! \brief Alias of \ref os_queue_doubly_insert_after \
	 * \ingroup group_os_queue \
	 */ \
	static inline void os_queue_##NAME##_insert_after(struct os_queue_##NAME *prev_elt, struct os_queue_##NAME *elt) { \
		os_queue_doubly_insert_after((struct os_queue_doubly *) prev_elt, (struct os_queue_doubly *) elt); \
	} \
	/*! \brief Alias of \ref os_queue_doubly_insert_first \
	 * \ingroup group_os_queue \
	 */ \
	static inline void os_queue_##NAME##_insert_first(struct os_queue_##NAME **first_elt, struct os_queue_##NAME *elt) { \
		os_queue_doubly_insert_first((struct os_queue_doubly **) first_elt, (struct os_queue_doubly *) elt); \
	} \
	/*! \brief Alias of \ref os_queue_doubly_add_sort \
	 * \ingroup group_os_queue \
	 */ \
	static inline void os_queue_##NAME##_add_sort(struct os_queue_##NAME **first_elt, struct os_queue_##NAME *elt, os_queue_doubly_sort_t sort_fct) { \
		os_queue_doubly_add_sort((struct os_queue_doubly **) first_elt, (struct os_queue_doubly *) elt, sort_fct); \
	} \
	/*! \brief Alias of \ref os_queue_doubly_remove \
	 * \ingroup group_os_queue \
	 */ \
	static inline void os_queue_##NAME##_remove(struct os_queue_##NAME *elt) { \
		os_queue_doubly_remove((struct os_queue_doubly *) elt); \
	} \
	/*! \brief Alias of \ref os_queue_doubly_remove_ex \
	 * \ingroup group_os_queue \
	 */ \
	static inline void os_queue_##NAME##_remove_ex(struct os_queue_##NAME **first_elt, struct os_queue_##NAME *elt) { \
		os_queue_doubly_remove_ex((struct os_queue_doubly **) first_elt, (struct os_queue_doubly *) elt); \
	}

/* Types **********************************************************************/

/*! \brief Queue type. A queue is a singly linked list
 * \ingroup group_os_queue
 */
struct os_queue {
	/*! \brief Next element in the singly linked list */
	struct os_queue *next;
};

/*! \brief Sorting function prototype for singly linked lists
 * \ingroup group_os_queue
 * \param a The first queue element
 * \param b The secont queue element
 * \return true if the first element is supposed to be placed before the second
 * element. Returns false otherwise.
 * \see The following functions are predefined and can be used:
 * - \ref os_queue_sort_fifo
 * - \ref os_queue_sort_lifo
 */
typedef bool (*os_queue_sort_t)(struct os_queue *a, struct os_queue *b);

/*! \brief Doubly queue type. A queue is a doubly linked list
 * \ingroup group_os_queue
 */
struct os_queue_doubly {
	/*! \brief Next element in the doubly linked list */
	struct os_queue_doubly *next;
	/*! \brief Previous element in the doubly linked list */
	struct os_queue_doubly *prev;
};

/*! \brief Sorting function prototype for doubly linked lists
 * \ingroup group_os_queue
 * \param a The first queue element
 * \param b The secont queue element
 * \return true if the first element is supposed to be placed before the second
 * element. Returns false otherwise.
 */
typedef bool (*os_queue_doubly_sort_t)(struct os_queue_doubly *a,
		struct os_queue_doubly *b);

/* Public API *****************************************************************/

/*! \name Singly Queues
 *
 * Singly queues are singly linked list. Singly linked lists contain nodes which
 * have a data field as well as a next field, which points to the next node in
 * the linked list.
 *
 * \{
 */

/*! \brief Helper function used to define the order of a new element added to a
 * queue (\ref os_queue). This function will add them so that the \b first \b in
 * will be the \b first \b out. This function must be used with the type
 * \ref os_queue_sort_t
 * \ingroup group_os_queue
 */
bool os_queue_sort_fifo(struct os_queue *a, struct os_queue *b);

/*! \brief Helper function used to define the order of a new element added to a
 * queue (\ref os_queue). This function will add them so that the \b last \b in
 * will be the \b first \b out. This function must be used with the type
 * \ref os_queue_sort_t
 * \ingroup group_os_queue
 */
bool os_queue_sort_lifo(struct os_queue *a, struct os_queue *b);

#if CONFIG_OS_USE_PRIORITY == true
/*! \brief Helper function used to define the order of a new element added to a
 * queue (\ref os_queue). This function will add them so that the highest
 * priority process will be at the head of the list. This function must be used
 * with the type \ref os_queue_sort_t
 * \pre \ref CONFIG_OS_USE_PRIORITY must be set
 * \ingroup group_os_queue
 */
bool os_queue_process_sort_priority(struct os_queue *a, struct os_queue *b);
#endif

/*! \brief Returns and remove the first element from the queue.
 * \ingroup group_os_queue
 * \param first_elt A pointer on the variable holding the first element in the
 * queue.
 * \return The first element from the queue.
 * \note The queue must not be empty
 */
static inline struct os_queue *os_queue_pop(struct os_queue **first_elt) {
	struct os_queue *elt = *first_elt;
	*first_elt = elt->next;
	return elt;
}

/*! \brief Returns the first element from the queue.
 * \ingroup group_os_queue
 * \param first_elt A pointer on the first element in the queue.
 * \return The first element from the queue.
 */
static inline struct os_queue *os_queue_head(struct os_queue *first_elt) {
	return first_elt;
}

/*! \brief Insert an element in the queue. The element cannot be inserted at the
 * begining of the queue but after another element only.
 * \ingroup group_os_queue
 * \param prev_elt The element after which the new element should be placed.
 * \param elt The new element to be inserted.
 */
static inline void os_queue_insert_after(struct os_queue *prev_elt,
		struct os_queue *elt) {
	elt->next = prev_elt->next;
	prev_elt->next = elt;
}

/*! \brief Insert an element at the begining of the queue.
 * \ingroup group_os_queue
 * \param first_elt A pointer on the variable holding the first element in the
 * queue.
 * \param elt The new element to be inserted.
 */
static inline void os_queue_insert_first(struct os_queue **first_elt,
		struct os_queue *elt) {
	elt->next = *first_elt;
	*first_elt = elt;
}

/*! \brief Add a new element to the queue and sort it.
 * \ingroup group_os_queue
 * \param first_elt A pointer on the variable holding the first element in the
 * queue.
 * \param elt The new element to be added.
 * \param sort_fct The sorting function.
 */
void os_queue_add_sort(struct os_queue **first_elt,
		struct os_queue *elt, os_queue_sort_t sort_fct);

/*! \brief Add a new element to the queue. The new element will be added to the
 * end of the queue.
 * \ingroup group_os_queue
 * \param first_elt A pointer on the variable holding the first element in the
 * queue.
 * \param new_elt The new element to be added.
 */
static inline void os_queue_add(struct os_queue **first_elt,
		struct os_queue *new_elt) {
	os_queue_add_sort(first_elt, new_elt, os_queue_sort_fifo);
}

/*! \brief Remove an element from the queue.
 * \ingroup group_os_queue
 * \param first_elt A pointer on the variable holding the first element in the
 * queue.
 * \param elt The element to be removed.
 * \return true if the element has be successfully removed from the queue.
 * Returns false if the element has not been found.
 */
bool os_queue_remove(struct os_queue **first_elt, struct os_queue *elt);

/*!
 * \}
 */

/*! \name Doubly Queues
 *
 * Doubly queues are doubly linked lists. In a doubly linked list, each node
 * contains, besides the next-node link, a second link field pointing to the
 * previous node in the sequence.
 *
 * \{
 */

#if CONFIG_OS_USE_PRIORITY == true
/*! \brief Helper function used to define the order of a new elements added to a
 * doubly process queue (\ref os_queue_doubly). This function will add them so
 * that the highest priority process will be at the head of the list. This
 * function must be used with the type \ref os_queue_doubly_sort_t
 * \pre \ref CONFIG_OS_USE_PRIORITY must be set
 * \ingroup group_os_queue
 */
bool os_queue_doubly_process_sort_priority(struct os_queue_doubly *a,
		struct os_queue_doubly *b);

#endif

/*! \brief Returns and remove the first element from the queue.
 * \ingroup group_os_queue
 * \param first_elt A pointer on the variable holding the first element in the
 * queue.
 * \return The first element from the queue.
 * \note The queue must not be empty
 */
static inline struct os_queue_doubly *os_queue_doubly_pop(
		 struct os_queue_doubly **first_elt) {
	struct os_queue_doubly *elt = *first_elt;
	*first_elt = elt->next;
	(*first_elt)->prev = elt->prev;
	return elt;
}

/*! \brief Returns the first element from the queue.
 * \ingroup group_os_queue
 * \param first_elt A pointer on the first element in the queue.
 * \return The first element from the queue.
 */
static inline struct os_queue_doubly *os_queue_doubly_head(
		struct os_queue_doubly *first_elt) {
	return first_elt;
}

/*! \brief Insert an element in the queue.
 * \ingroup group_os_queue
 * \param prev_elt The element after which the new element should be placed.
 * \param elt The new element to be inserted.
 */
static inline void os_queue_doubly_insert_after(
		struct os_queue_doubly *prev_elt,
		struct os_queue_doubly *elt) {
	struct os_queue_doubly *next_elt = prev_elt->next;
	/* Checks if there is a previous element */
	if (next_elt) {
		next_elt->prev = elt;
	}
	elt->next = next_elt;
	elt->prev = prev_elt;
	prev_elt->next = elt;
}

/*! \brief Insert an element at the begining of the queue.
 * \ingroup group_os_queue
 * \param first_elt A pointer on the variable holding the first element in the
 * queue.
 * \param elt The new element to be inserted.
 */
static inline void os_queue_doubly_insert_first(
		struct os_queue_doubly **first_elt,
		struct os_queue_doubly *elt) {
	elt->next = *first_elt;
	elt->prev = (struct os_queue_doubly *) first_elt;
	(*first_elt)->prev = elt;
	*first_elt = elt;
}

/*! \brief Add a new element to the queue and sort it.
 * \ingroup group_os_queue
 * \param first_elt A pointer on the variable holding the first element in the
 * queue.
 * \param elt The new element to be added.
 * \param sort_fct The sorting function.
 */
void os_queue_doubly_add_sort(struct os_queue_doubly **first_elt,
		struct os_queue_doubly *elt,
		os_queue_doubly_sort_t sort_fct);

/*! \brief Add a new element to the queue. The new element will be added to the
 * end of the queue.
 * \ingroup group_os_queue
 * \param first_elt A pointer on the variable holding the first element in the
 * queue.
 * \param new_elt The new element to be added.
 */
static inline void os_queue_doubly_add(
		struct os_queue_doubly **first_elt,
		struct os_queue_doubly *new_elt) {
	os_queue_doubly_add_sort(first_elt, new_elt,
			(os_queue_doubly_sort_t) os_queue_sort_fifo);
}


/*! \brief Remove an element from the queue. The element must not be the first
 * one in the queue.
 * \ingroup group_os_queue
 * \param elt The element to be removed.
 */
void os_queue_doubly_remove(struct os_queue_doubly *elt);

/*! \brief Remove an element from the queue.
 * \ingroup group_os_queue
 * \param first_elt A pointer on the variable holding the first element in the
 * queue.
 * \param elt The element to be removed.
 */
void os_queue_doubly_remove_ex(struct os_queue_doubly **first_elt,
		struct os_queue_doubly *elt);

/*!
 * \}
 */

/*! \name Singly Process Queues
 *
 * Singly process queues are children of singly queues (\ref os_queue). The
 * difference is that they hold a process information.
 *
 * \{
 */

/*! \brief Type definition for the structure \b os_queue_process.
 *
 * The \b os_queue_process is a singly queue which holds a process
 * information. This information is extracted from a queue element using the
 * function \ref os_queue_process_get.
 * \b os_queue_process is a child of \ref os_queue and therefore inherits from
 * all its functions. For more details, see \ref OS_QUEUE_DEFINE.
 *
 * \ingroup group_os_queue
 */
OS_QUEUE_DEFINE(process,
	/*! \brief Process information */
	struct os_process *proc;
);

/*! \brief Returns the process information held by a \b os_queue_process
 * element.
 * \ingroup group_os_queue
 * \param elt The \b os_queue_process element
 * \return The \ref os_process associated with this element.
 */
static inline struct os_process *os_queue_process_get(struct os_queue_process *elt) {
	return elt->proc;
}

/*! \brief Add a new element to the queue. The new element will be added to the
 * end of the queue. If \ref CONFIG_OS_USE_PRIORITY is set, the element will
 * be added according to the priority of the process associated with the
 * queue element. The highest priority goes first.
 * \ingroup group_os_queue
 * \param first_elt A pointer on the variable holding the first element in the
 * queue.
 * \param new_elt The new element to be added.
 */
#if CONFIG_OS_USE_PRIORITY == true
static inline void os_queue_process_add(struct os_queue_process **first_elt,
		struct os_queue_process *new_elt) {
	os_queue_add_sort((struct os_queue **) first_elt,(struct os_queue *) new_elt, os_queue_process_sort_priority);
}
#else
static inline void os_queue_process_add(struct os_queue_process **first_elt,
		struct os_queue_process *new_elt) {
	os_queue_add_sort((struct os_queue **) first_elt,(struct os_queue *) new_elt, os_queue_sort_fifo);
}
#endif

/*!
 * \}
 */

/*! \name Doubly Process Queues
 *
 * Doubly process queues are children of doubly queues (\ref os_queue_doubly).
 * The difference is that they hold a process information.
 *
 * \{
 */

/*! \brief Type definition for the structure \b os_queue_doubly_process.
 *
 * The \b os_queue_doubly_process is a singly queue which holds a process
 * information. This information is extracted from a queue element using the
 * function \ref os_queue_doubly_process_get.
 * \b os_queue_doubly_process is a child of \ref os_queue_doubly and therefore
 * inherits from all its functions. For more details, see
 * \ref OS_QUEUE_DOUBLY_DEFINE.
 *
 * \ingroup group_os_queue
 */
OS_QUEUE_DOUBLY_DEFINE(doubly_process,
	/*! \brief Process information */
	struct os_process *proc;
);

/*! \brief Returns the process information held by a \b os_queue_doubly_process
 * element.
 * \ingroup group_os_queue
 * \param elt The \b os_queue_doubly_process element
 * \return The \ref os_process associated with this element.
 */
static inline struct os_process *os_queue_doubly_process_get(
		struct os_queue_doubly_process *elt) {
	return elt->proc;
}

/*! \brief Add a new element to the queue. The new element will be added to the
 * end of the queue. If \ref CONFIG_OS_USE_PRIORITY is set, the element will
 * be added according to the priority of the process associated with the
 * queue element. The highest priority goes first.
 * \ingroup group_os_queue
 * \param first_elt A pointer on the variable holding the first element in the
 * queue.
 * \param new_elt The new element to be added.
 */
#if CONFIG_OS_USE_PRIORITY == true
static inline void os_queue_doubly_process_add(struct os_queue_doubly_process **first_elt,
		struct os_queue_doubly_process *new_elt) {
	os_queue_doubly_add_sort((struct os_queue_doubly **) first_elt,
			(struct os_queue_doubly *) new_elt,
			os_queue_doubly_process_sort_priority);
}
#else
static inline void os_queue_doubly_process_add(struct os_queue_doubly_process **first_elt,
		struct os_queue_doubly_process *new_elt) {
	os_queue_doubly_add_sort((struct os_queue_doubly **) first_elt,
			(struct os_queue_doubly *) new_elt, (os_queue_doubly_sort_t) os_queue_sort_fifo);
}
#endif

/*!
 * \}
 */

#endif // __OS_QUEUE_H__
