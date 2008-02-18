/*
 * Copyright (C) 2007-2008 Chris O'Byrne
 *
 * This file is part of YAVRTOS (see http://www.chris.obyrne.com/yavrtos/)
 *
 * YAVROTS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YAVROTS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with YAVROTS.  If not, see <http://www.gnu.org/licenses/>.
 *
 * The author can be contacted at <chris@obyrne.com>
 *
 * version 1.6b2, 2008 Jan 25
 */

#ifndef TASK_H_
#define TASK_H_

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * \brief Structure describing a semaphore
 *
 * \sa semaphore
 */
typedef struct semaphorestruct {
	/** \brief \internal The value of the semaphore
	 *
	 * If you do decide to read or write the value of the semaphore directly, keep in mind
	 * \li it is a 16-bit value, and so interrupts need to be disabled when reading/writing
	 * \li after writing, a yield() may need to be performed
	 */
	int16_t value;
} semaphore_t;

/**
 * \brief Structure describing mailboxes
 *
 * \sa mailbox
 */
typedef struct mbox_struct {
	/// \internal The address of the current mailbox data
	void *data;
	/// \internal Readers wait on this semaphore. This semaphore's value is the "version" of the mailbox data
	semaphore_t reading_semaphore;
	/// \internal Writers wait on this semaphore for readers to finish reading, or for readers to suspend themselves on the mailbox
	semaphore_t writing_semaphore;
} mailbox_t;

// mutexstruct is used in taskstruct, and taskstruct is used in mutexstruct!
struct mutexstruct;

/**
 * \brief Structure describing a task
 *
 * \sa task
 */
typedef struct taskstruct {
	/// \internal Pointer to the top of the task stack
	uint8_t *stack;
	/// \internal Length of the task stack
	uint16_t stacklen;
	/// \internal The stack pointer as at the last time this task was suspended
	uint8_t *sp;
	/// \internal The task priority
	uint8_t pri;
	/// \internal The function that performs the tasks' activities
	void (*proc)(void*);
	/// \internal The function to execute when the task is stopping
	void (*cleanup)();
	/// \internal The first mutex this task owns
	struct mutexstruct *owned_mutex;
	/// \internal The mailbox we are waiting on or reading from
	mailbox_t *waiting_on_mbox;
	/**
	 * \brief \internal The version of the mailbox that we are reading or waiting on
	 *
	 * If this value is greater than the mailbox version, then we are waiting, otherwise we are reading
	 */
	int16_t waiting_on_mbox_version;
	/// \internal The semaphore the task is waiting on
	semaphore_t *waiting_semaphore;
	/// \internal The value of the semaphore that the task is waiting on
	int16_t waiting_semaphore_min_value;
	/**
	 * \brief \internal Task status
	 *
	 * \sa taskstategroup
	 */
	uint8_t status;
	/// \internal Pointer to the next task in the list
	struct taskstruct *next;
} task_t;

/**
 * \brief A structure describing a mutex
 *
 * \sa mutex
 */
typedef struct mutexstruct {
	/// \internal The task that currently owns the mutex
	struct taskstruct *owner;
	/// \internal The semaphore that other tasks waiting to own the mutex will suspend themselves against
	semaphore_t notification;
	/// \internal The next mutex that is owned by this task - used to release mutexes owned by a stopping task
	struct mutexstruct *next;
} mutex_t;

/**
 * \ingroup task
 *
 * \brief The current task.
 *
 * This value may be used in stop_task() to stop the current task
 */
extern task_t *current_task;

/**
 * \ingroup task
 *
 * \brief Tasks are kept in a linked list in memory - this function reserves an "empty" task on that list, ready to be
 * subsequently utilised by a call to create_task().
 *
 * \attention reserve_task() calls malloc(), and disables interrupts during the call to malloc(). Therefore,
 * depending on the amount of time your malloc() algorithm takes, and depending on what your time margin for the
 * launch of ISRs is, reserve_task() could disable interrupts for "too long". See the description of the
 * \c memory_mutex argument, and \ref malloc
 *
 * The arguments are
 * \arg \c stacklen - the number of bytes to reserve for the task stack. Note that it must be long enough to hold a copy
 *     of the value of all 32 registers when a task switch occurs, and it needs a couple of extra bytes to handle
 *     interrupts, function calls etc.
 * \arg \c pri - the priority of the task. The highest-priority available task will "hog" the CPU. If the task has a priority
 *     of zero (an "idle" task), then it may not suspend itself on a mutex, semaphore or mailbox.
 * \arg \c memory_mutex - if not null, and if called from within a non-idle task, this mutex will be locked on to during the
 *		call to malloc(), otherwise interrupts will be disabled during the call to malloc() - see \ref malloc.
 */
task_t *reserve_task(uint16_t stacklen, uint8_t pri, mutex_t *memory_mutex);

/**
 * \ingroup task
 *
 * \brief Create a task, ready to be run.
 *
 * \attention create_task() can call malloc(), and disables interrupts during any call to malloc(). Therefore,
 * depending on the amount of time your malloc() algorithm takes, and depending on what your time margin for the
 * launch of ISRs is, create_task() could disable interrupts for "too long". See the description of the
 * \c memory_mutex argument, and \ref malloc
 *
 * The arguments are
 * <ul>
 * <li> \c proc - this is the task function. Upon entry, interrupts will be enabled, and the value of the argument will
 *			be that of \c init_data. This function does not need to
 *			contain an infinite loop - if it returns (and if the priority is greater than zero), a stop will be
 *			performed on the task.</li>
 * <li> \c cleanup - this is the task cleanup function, which will be called as the task is dying. May be null if the task
 *			doesn't need to clean up after itself. See task_stopper()</li>
 * <li> \c init_data - this is the value that will be given in the argument to \c proc when it starts</li>
 * <li> \c stacklen - the number of bytes to reserve for the task stack. Note that it must be long enough to hold a copy
 *     of the value of all 32 registers when a task switch occurs, and it needs a couple of extra bytes to handle
 *     interrupts, function calls etc.</li>
 * <li> \c pri - the priority of the task. The highest-priority available task will "hog" the CPU. If the task has a priority
 *     of zero (an "idle" task), then it may not suspend itself on a mutex, semaphore or mailbox, and it may not be stopped.</li>
 * <li> \c memory_mutex - if not null, and if called from within a non-idle task, this mutex will be locked on to during the
 *		call to malloc(), otherwise interrupts will be disabled during the call to malloc() - see \ref malloc</li>
 * </ul>
 *
 * This function will scan through the task list in memory, looking for one that is not being used with a matching pri and a stacklen
 * that is at least as large as that required. If it cannot find such a task, it will create a brand new one. See reserve_task()
 */
task_t *create_task(void (*proc)(void*), void (*cleanup)(), void *init_data, uint16_t stacklen, uint8_t pri, mutex_t *memory_mutex);

/**
 * \ingroup task
 *
 * \brief Stop a task.
 *
 * The arguments are
 * \arg \c t the task to stop
 * \arg \c wait_for_mutexes - if not zero, the task will not be stopped until it has released all of its mutexes. If zero,
 *			then the tasks will stop immediately. Note that if you are using a memory mutex, you \b must set
 *			\c wait_for_mutexes when stopping any task that uses the memory mutex - see \ref malloc
 *
 * This function can be called by the current task (stop_task(current_task, ...)), or by a higher-priority task, or by an ISR,
 * and may be called on any non-zero-priority task.
 *
 * If this function is called by the current task, and \c wait_for_mutexes isn't set, then the "cleaning up" of the task
 * (task_stopper()) will start executing immediately - i.e. the stop_task(current_task, 0) call won't "return". If
 * \c wait_for_mutexes is set, then the lock_off() that releases the tasks' last mutex won't return - the task_stopper()
 * will run instead.
 *
 * If this function is called by a higher priority task, then it will not return until the task in question has
 * completely stopped executing.
 *
 * If this function is called from within an ISR, it will return immediately.
 *
 * A return value of -1 means that the calling task doesn't have the required permission. A return value of -2 means
 * that an attempt was made to stop a zero-priority task. A return value of zero indicates success, and a return value of
 * 1 means that the task was already stopped.
 *
 * See task_stopper() for a description of what happens to the task that is being stopped.
 */
int8_t stop_task(task_t *t, uint8_t wait_for_mutexes);

/**
 * \ingroup task
 *
 * \brief Stop executing the current task and try and execute a higher-priority task or another task of the same priority.
 *
 * Note that all API calls that could theoretically cause a higher-priority task to be re-enabled will
 * call yield(), which will cause an automatic and immediate task switch to that higher-priority task. (This
 * also means that if there is another task of the same priority that hasn't been disabled, all such API
 * calls will cause a task switch to that task).
 *
 * Obviously this function can only be called by tasks, and it will "return" the next time it is the turn of the
 * current task to execute.
 */
void yield() __attribute__ ((naked));

/**
 * \ingroup mutex
 *
 * \brief Lock on a mutex.
 *
 * The return value is zero for success.
 *
 * Since the task may be suspended while waiting for another task to release the mutex, this function may only be
 * called by tasks with a non-zero priority.
 */
uint8_t lock_on(mutex_t *m);

/**
 * \ingroup mutex
 *
 * \brief Unlock a mutex.
 *
 * This function can only usefully be called by the task that locked the mutex in the first place, and it
 * may end up yielding control to a task that is waiting on the mutex. The return value is zero for success.
 *
 * Note that if someone has called stop_task() on this task with the \c wait_for_mutexes parameter set, and if the
 * call to this function is the one that releases the tasks' last mutex, then this function won't "return" - the
 * task_stopper() will run on this task instead.
 */
uint8_t lock_off(mutex_t *m);

/**
 * \ingroup mailbox
 *
 * \brief Get the current version of a mailbox
 *
 * This function can be called by an ISR, by any task, or even before the RTOS starts
 */
int16_t get_current_mbox_version(mailbox_t *mbox);

/**
 * \ingroup mailbox
 *
 * \brief Wait for a mailbox to reach at least a certain version, and then start reading from it
 *
 * \attention It is \b vital that release_mbox_read() be called to release the mailbox for other tasks to write to
 *		it when the calling task has finished reading the mailbox data
 *
 * The arguments are
 * \arg \c mbox - the mailbox to read from
 * \arg \c version - the minimum version of the mailbox that we require. Note that the version of the mailbox actually read
 *			will be written to this address
 *
 * The return value is a pointer to the mailbox data. Note that a zero return value does not mean that the mailbox
 * read failed - it means that the mailbox was empty - and hence release_mbox_read() must still be called.
 *
 * Since this function can cause a suspension (i.e. if the mailbox hasn't reached the specified version), it can only be
 * called from a task with a non-zero priority
 */
void *read_mbox_min_version(mailbox_t *mbox, int16_t *version);

/**
 * \ingroup mailbox
 *
 * \brief Read a mailbox
 *
 * This function reads a mailbox regardless of the version of the mailbox.
 *
 * \attention It is \b vital that release_mbox_read() be called to release the mailbox for other tasks to write to
 *		it when the calling task has finished reading the mailbox data
 *
 * The arguments are
 * \arg \c mbox - the mailbox to read
 * \arg \c version - if not null, the version of the mailbox will be written to this address
 *
 * The return value is a pointer to the mailbox data. Note that a zero return value does not mean that the mailbox
 * read failed - it means that the mailbox was empty - and hence release_mbox_read() must still be called.
 *
 * Since this function can cause a suspension (i.e. if the mailbox hasn't reached the specified version), it can only be
 * called from a task with a non-zero priority
 */
void *read_mbox(mailbox_t *mbox, int16_t *version);

/**
 * \ingroup mailbox
 *
 * \brief Function to call when finished reading from a mailbox
 *
 * Since tasks are not allowed to write to a mailbox while there are other tasks reading from it, this function
 * \b must be called when a task has finished reading from a mailbox - i.e. it must be called after every call to
 * read_mbox() or read_mbox_min_version()
 *
 * Since a task can only read one mailbox at a time, there are no arguments to this function. The return value is
 * the mailbox that the task was reading from, or zero if it was not reading from any mailbox.
 *
 * Note that calling this function may cause a higher-prioroty task that is waiting to write to the mailbox to be
 * scheduled.
 */
mailbox_t *release_mbox_read();

/**
 * \ingroup mailbox
 *
 * \brief Initialise a mailbox - this \b must be called on every mailbox before it is used
 *
 * The arguments are
 * \arg \c mbox - the mailbox to initialise
 * \arg \c data - the pointer to the mailbox data. This is the value that will be returned by read_mbox() and
 *			read_mbox_min_version(). It is acceptable to set this value to zero - though callers to read_mbox()
 *			and read_mbox_min_version() would need to be aware that the return value could be zero.
 * \arg \c version - the initial version of the mailbox
 */
void initialise_mbox(mailbox_t *mbox, void *data, const int16_t version);

/**
 * \ingroup mailbox
 *
 * \brief Write to a mailbox
 *
 * A write to a mailbox will
 * \li wait for all tasks that are reading the mailbox to call release_mbox_read()
 * \li if \c wait_for_receivers is not zero, it will wait until there are at least that many tasks that have suspended themselves
 *		on read_mbox_min_version() while waiting for fresh data to be put into this mailbox. Otherwise, the data that is put into
 *		the mailbox might end up not being picked up by any task (e.g. if another write is made to the mailbox before
 *		any task attempts to read from it, that second write will over-write what is put into the mailbox by this function call)
 * \li then, the mailbox data will be updated, and the version will be incremented by one
 * \li if there are any higher-priority tasks suspended on read_mbox_min_version(), they will start executing
 * \li if \c wait_for_empty_nullify is not zero, the function will not return until all tasks that had been suspended on
 *		read_mbox_min_version() have called release_mbox_read().
 * \li if \c wait_for_empty_nullify is greater than one, a new version of the mailbox data, containing a null pointer, will be
 *		published
 * 
 * If the task that writes to the mailbox can be stopped with stop_task(), then you could end up in a situation where the memory
 * location of the task data goes out of scope while the mailbox is being read. If this is a possibility, then it is recommended
 * that a \ref mutex "mutex" be created for the mailbox, that it be locked on to while the mailbox is being written to, that the
 * task only ever be stopped with the \c wait_for_mutexes parameter of stop_task() set, and that
 * \c wait_for_empty_nullify be set to at least one (it would need to be set at two if there are more than one tasks that could
 * read from the mailbox).
 *
 * The arguments are
 * <ul>
 * <li> \c mbox - the mailbox to write to</li>
 * <li> \c data - the pointer to the mailbox data. This is the value that will be returned by read_mbox() and
 *			read_mbox_min_version(), so it is vital that it points to valid data for the lifetime of this particular
 *			version of the mailbox.</li>
 * <li> \c wait_for_receivers - if non-zero, the write to the mailbox will not occur until there are at least
 *			\c wait_for_receivers task(s) waiting to read data from the mailbox. This could be used if it is
 *			vital that at least a specified number of
 *			receiving task(s) receive the information - it would give the receiving task(s) a chance to initialise</li>
 * <li> \c wait_for_empty_nullify
 *			<ul>
 * 			<li>if greater than zero, the function will not return until all tasks that were waiting to read the
 *			mailbox have done so. This could be used if it is vital that the sender knows that all messages have been received by
 *			the receiving task(s).</li>
 *			<li>if greater than one, the mailbox data will be set to null when all the tasks that were waiting to
 *			read the mailbox have done so (which means that subsequent reads of the mailbox will return a null pointer),
 *			so the data in the mailbox data buffer can safely be changed</li>
 *			</ul>
 * </ul>
 *
 * Since this function can cause a task suspension, it can only be called from a task with a non-zero priority
 */
void write_mbox(mailbox_t *mbox, void *data, uint8_t wait_for_receivers, uint8_t wait_for_empty_nullify);

/**
 * \ingroup mailbox
 *
 * \brief Attempt to write to a mailbox
 *
 * This function will attempt to write to a mailbox - if the mailbox is being read, and is therefore unavailable for writing
 * to as per the rules of mailboxes, this function will return
 * immediately without having written anything to the mailbox.
 *
 * The arguments are -
 * \arg \c mbox - the mailbox to write to
 * \arg \c data - the pointer to the mailbox data. This is the value that will be returned by read_mbox() and
 *			read_mbox_min_version().
 *
 * The return value is zero on success
 *
 * Since this function will never cause a task suspension, it can be called from an ISR, from the idle task, or from anywhere
 * else in the application. Also, if this function is called from within a task, and if the task is potentially stoppable
 * by another task using stop_task() on it, then it is important that the memory location pointed to by \c data doesn't
 * go out of scope when the task is stopped.
 */
int8_t write_mbox_now(mailbox_t *mbox, void *data);

/**
 * \ingroup mailbox
 *
 * \brief Wait for a task to be suspended while trying to read from a mailbox
 *
 * This function will suspend the caller until another task is suspended while calling read_mbox_min_version(). This could
 * be used to give a task that needs to read the mailbox a chance to initialise.
 */
void wait_for_receiver(mailbox_t *mbox);

/**
 * \ingroup semaphore
 *
 * \brief Wait for a semaphore to reach at least a particular value.
 *
 * Since the calling task could be suspended, this can only be called by a task with a non-zero priority
 */
uint8_t wait_for_min_value(semaphore_t *s, int16_t value);

/**
 * \ingroup semaphore
 *
 * \brief Wait for a semaphore to increment its value by a certain amount
 *
 * The arguments are
 * \arg \c p - the semaphore to wait on
 * \arg \c amount - the amount by which the semaphore should increment before returning. Note that semaphore
 *			values are actually signed 16-bit numbers, so the maximum value for this argument is about 32,000
 *
 * Since the calling task could be suspended, this can only be called by a task with a non-zero priority
 */
uint8_t wait_for_increment_of(semaphore_t *p, uint16_t amount);

/**
 * \ingroup semaphore
 *
 * \brief Get the current value of a semaphore
 *
 * This function may be called anywhere
 */
int16_t get_semaphore_value(semaphore_t *s);

/**
 * \ingroup semaphore
 *
 * \brief Increment the value of a semaphore by the given amount.
 *
 * The arguments are
 * \arg \c s - the semaphore to increment
 * \arg \c amount - the amount by which the semaphore should be incremented. Note that semaphore
 *			values are actually signed 16-bit numbers, so the maximum value for this argument is about 32,000
 *
 * This may be called by any task, by an ISR, or even before the task
 * system is running.
 *
 * When called from a task, this function may end up yielding control to a higher-priority task that is waiting on the
 * semaphore.
 */
void increment_semaphore_by(semaphore_t *s, uint16_t amount);

/**
 * \ingroup task
 *
 * \brief Start the whole process running.
 *
 * Note that this method will never return. The arguments are
 * \arg \c idle - the "idle" function (the function to execute when we have nothing else to do)
 * \arg \c idle_data - the value of the argument to \c idle when it starts
 * \arg \c idle_stacklen - the length of the stack for the "idle" function - see create_task()
 * \arg \c system_stacklen - the length of the system stack - this is the stack that will be in use during all ISRs
 *
 * A recommended idle task is -
 * \code
 * void idle_task() {
 *   set_sleep_mode(SLEEP_MODE_IDLE);
 *   sleep_enable();
 *   sei();
 *   sleep_cpu();
 * }
 * \endcode
 *
 * See \ref Example "the example application" for an example of how to start the task switcher.
 *
 * Note that, if you are using the TASK_ISR() macro to give your ISRs access to the system stack (and you should!),
 * then all interrupts must remain disabled right up until this function is called. Interrupts will be enabled as soon
 * as the first task switch starts.
 */
void task_switcher_start(void (*idle)(void*), void *idle_data, uint16_t idle_stacklen, uint16_t system_stacklen) __attribute__ ((naked));


/**
 * \ingroup task
 *
 * \brief A flag indicating whether interrupts were enabled - used by disable_interrupts() and restore_interrupts()
 */
#define interrupt_store_t uint8_t

/**
 * \ingroup task
 *
 * \brief Disable interrupts system-wide.
 *
 * Returns a value which is non-zero if interrupts were enabled. Can be called
 * from anywhere.
 */
interrupt_store_t disable_interrupts();

/**
 * \ingroup task
 *
 * \brief Restore the state of the system-wide interrupts.
 *
 * A non-zero argument enables interrupts. Can be called from
 * anywhere.
 */
void restore_interrupts(interrupt_store_t interrupts);

/*
 * Everything below here is for the benefit of the TICK_ISR(vector) and TASK_ISR(vector,proc) macros
 */

/**
 * \brief \internal Structure used by the system to hold the system stack, and a flag indicating whether an ISR is currently
 * being executed.
 */
struct system_struct {
	/// non-zero if we are executing an ISR. Also, bit 1 is set if a task switch is required.
	uint8_t interrupted_task;
	/// The top of the system stack
	uint8_t *stack_top;
};

/**
 * \brief \internal The system_struct used to hold the system stack and a flag indicating whether an ISR is currently being
 * executed
 */
extern struct system_struct system;

/**
 * \brief \internal Save the CPU context to the stack, and disable interrupts
 */
#define save_cpu_context() __asm__ volatile( \
		"push  r0\n in r0, 0x3f\n cli\n" \
		"push  r1\n push  r2\n push  r3\n push  r4\n push  r5\n push  r6\n push  r7\n" \
		"push  r8\n push  r9\n push r10\n push r11\n push r12\n push r13\n push r14\n push r15\n" \
		"push r16\n push r17\n push r18\n push r19\n push r20\n push r21\n push r22\n push r23\n" \
		"push r24\n push r25\n push r26\n push r27\n push r28\n push r29\n push r30\n push r31\n" \
		"push  r0\n" ::)

/**
 * \brief \internal Restore the CPU context from the stack, possibly re-enabling interrupts
 */
#define restore_cpu_context() __asm__ volatile ( \
		"pop r0\n" \
		"pop r31\n pop r30\n pop r29\n pop r28\n pop r27\n pop r26\n pop r25\n pop r24\n" \
		"pop r23\n pop r22\n pop r21\n pop r20\n pop r19\n pop r18\n pop r17\n pop r16\n" \
		"pop r15\n pop r14\n pop r13\n pop r12\n pop r11\n pop r10\n pop  r9\n pop  r8\n" \
		"pop  r7\n pop  r6\n pop  r5\n pop  r4\n pop  r3\n pop  r2\n pop  r1\n out 0x3f, r0\n pop  r0\n" ::)

// The all-important task switch function
void switch_task() __attribute__ ((naked));

/**
 * \ingroup isr
 *
 * \brief The macro for ISRs
 *
 * The arguments are
 * \arg \c vector the ISR vector
 * \arg \c do_task_switch something that evaluates non-zero if the ISR should trigger a task switch. This
 * 		could be a constant, a function call, a macro, anything. Note that the tick interrupt \b must,
 * 		by definition, trigger a task switch. It is highly recommended that this macro always evaluates
 * 		to non-zero - the only time where it would be safe for it to evaluate to zero is if it does
 * 		absolutely nothing to any task, semaphore, mailbox or mutex (i.e. if it doesn't do anything that
 * 		could un-suspend a suspended task).
 *
 * See \ref Example "the example application" and \ref how-write-isr "how do I write an ISR" for
 * examples of usage.
 *
 * The WinAVR&tm; ISR() macro can also be used to define ISRs - however, when using ISR() as opposed to
 * TASK_ISR(), you cannot make use of any YAVRTOS API call from within your ISR, and your ISR will not
 * (necessarily) be using the system stack.
 *
 * Taking this macro apart line-by-line, we have
 *
 * \code
 * // The standard way of calling an ISR
 * void vector(void) __attribute__ ((signal,naked,__INTR_ATTRS));
 * // The ISR itself
 * void vector(void) {
 * 	// Save the entire CPU context to the stack (which could be a task stack or the system stack)
 * 	save_cpu_context();
 * 	// Global interrupts were enabled just before this ISR was launched (otherwise it would not have
 * 	// launched), and are disabled upon entry into the ISR. Set the interrupt enable bit at the location of
 * 	// the saved status register on the saved stack so that when we restore the CPU context, interrupts will
 * 	// be re-enabled.
 * 	*(((uint8_t*)SP)+1) |= _BV(SREG_I);
 * 	// Now, we have either interrupted a task or interrupted another ISR. system.interrupted_task will
 * 	// be non-zero if a task has already been interrupted - i.e. if we have interrupted an ISR
 * 	// Note that task_switch() also sets system.interrupted_task - any ISRs that manage to run during
 * 	// the brief period when task_switch() enables interrupts should not subsequently run task_switch()!
 * 	if (system.interrupted_task) { // We have interrupted an ISR (or we have interrupted task_switch())
 * 		// Execute the macro to see if we should do a task switch
 * 		if (do_task_switch) {
 * 			// The macro may have enabled interrupts - disable them again
 * 			cli();
 * 			// Set bit 1 of system.interrupted_task - the bit that signals that a task switch is
 * 			// required. We don't do the task switch just yet, as we have interrupted another ISR,
 * 			// so we need to return to that ISR first. The ISR that interrupted the task will be the one
 * 			// to actually perform the task switch (see below)
 * 			// If we have actually interrupted task_switch(), then setting bit 1 of system.interrupted_task
 * 			// will have no effect - but we were doing a task switch anyway!
 * 			system.interrupted_task = 3;
 * 		} else {
 * 			// Task switch not required (this time). Make sure that interrupts are still disabled
 * 			cli();
 * 		}
 * 	} else { // We have interrupted a task
 * 		// Save the stack pointer
 * 		current_task->sp = (uint8_t *)SP;
 * 		// Switch to the system stack
 * 		SP = (uint16_t) system.stack_top;
 * 		// Set the system.interrupted_task so that subsequent ISRs will know that they have interrupted
 * 		// an ISR, not a task. This has the side-effect of disabling switch_task() - all API calls check
 * 		// system.interrupted_task and do NOT perform a task switch if it is non-zero.
 * 		system.interrupted_task = 1;
 * 		// Execute the macro and see if we should do a task switch
 * 		if (do_task_switch) {
 * 			// The macro could have enabled interrupts - disable them
 * 			cli();
 * 			// Signal to ourselves that we need a task switch - bit 1 of system.interrupted_task is set
 * 			// when a task switch is required
 * 			system.interrupted_task = 3;
 * 		} else {
 * 			// Make sure that interrupts are disabled
 * 			cli();
 * 		}
 * 		// At this point, any ISRs that interrupted us have finished.
 * 		// Now, if a task switch is required, perform it!
 * 		if (system.interrupted_task & 2) {
 * 			// switch_task() will (eventually) reset system.interrupted_task, and never "returns"
 * 			switch_task();
 * 		}
 * 		// A task switch is not required - restore the stack pointer so that we return to the task
 * 		SP = (uint16_t) current_task->sp;
 * 		// We are about to return to the task - reset system.interrupted_task
 * 		system.interrupted_task = 0;
 * 	}
 * 	// Return to whatever we were doing before this ISR was called
 * 	restore_cpu_context();
 * 	__asm__ volatile ("ret" ::);
 * }
 * \endcode
 */
#ifdef DOXYGEN
#define TASK_ISR(vector, do_task_switch)
#else
#define TASK_ISR(vector,do_task_switch) \
void vector(void) __attribute__ ((signal,naked,__INTR_ATTRS)); \
void vector(void) { \
	save_cpu_context(); \
	*(((uint8_t*)SP)+1) |= _BV(SREG_I); \
	if (system.interrupted_task) { \
		if (do_task_switch) { \
			cli(); \
			system.interrupted_task = 3; \
		} else { \
			cli(); \
		} \
	} else { \
		current_task->sp = (uint8_t *)SP; \
		SP = (uint16_t) system.stack_top; \
		system.interrupted_task = 1; \
		if (do_task_switch) { \
			cli(); \
			system.interrupted_task = 3; \
		} else { \
			cli(); \
		} \
		if (system.interrupted_task & 2) { \
			switch_task(); \
		} \
		SP = (uint16_t) current_task->sp; \
		system.interrupted_task = 0; \
	} \
	restore_cpu_context(); \
	__asm__ volatile ("ret" ::); \
}
#endif /* DOXYGEN */

#endif /*TASK_H_*/
