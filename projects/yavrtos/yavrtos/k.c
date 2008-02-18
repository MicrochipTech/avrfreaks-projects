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

#include "task.h"

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

/**
 * \brief Has a semaphore been triggered?
 *
 * It is quite critical that this algorithm be correct, even when the semaphore values roll over
 */
#define semaphore_triggered(current, required) ((current) - (required) >= 0)

/**
 * \ingroup taskstategroup
 * \brief Starting
 *
 * The task is starting, and the stack only contains a return address for task_starter()
 *
 * This state is set by create_task()
 */
#define TASK_STATE_STARTING 1
/**
 * \ingroup taskstategroup
 * \brief Running
 *
 * The task is running, and the stack contains the entire CPU context.
 *
 * This state is set by the scheduler the first time the task is scheduled
 */
#define TASK_STATE_RUNNING 2
/**
 * \ingroup taskstategroup
 * \brief Waiting for the task to release all of its mutexes before stopping
 *
 * The task is running, and the stack contains the entire CPU context
 *
 * This state is set by stop_task() when the \c wait_for_mutexes parameter is set and the task owns mutexes
 */
#define TASK_STATE_WAITING_TO_STOP 3
/**
 * \ingroup taskstategroup
 * \brief Stopping
 *
 * The task is stopping, and the stack only contains a return address for task_stopper()
 *
 * This state is set by stop_task() when there is no need, or desire, to wait for mutexes. Note that a task
 * will commit suicide on lock_off() if it is in TASK_STATE_WAITING_TO_STOP and has just released its last
 * mutex.
 */
#define TASK_STATE_STOPPING 4
/**
 * \ingroup taskstategroup
 * \brief Cleaning up
 *
 * The task is stopping, and the stack contains the entire CPU context
 *
 * This state is set by the scheduler the first time the task is scheduled after having had its state set to
 * TASK_STATE_STOPPING
 */
#define TASK_STATE_CLEANING_UP 5
/**
 * \ingroup taskstategroup
 * \brief Stopped
 *
 * The task is completely dead, and this entry in the task list is available for new tasks.
 *
 * This state is set by reserve_task(), and when the task has completely stopped.
 */
#define TASK_STATE_STOPPED 6

/// \internal Set the state of a task - this macro makes it easier to put other information into task.status
#define set_task_state(taskptr,req_state) (taskptr)->status = (req_state)
/// \internal Get the state of a task - this macro makes it easier to put other information into task.status
#define get_task_state(taskptr) ((taskptr)->status)
/// \internal Are we currently executing an ISR?
#define executing_isr() (system.interrupted_task)
/// \internal Has the RTOS been started?
#define rtos_started() (current_task)

/// \internal Pointer to the first task in our linked list
task_t *first_task = 0;
// The current task
task_t *current_task = 0;
/// \internal A semaphore we use to not return from stop_task() until the task in question has actually stopped
semaphore_t task_stopping_semaphore;
/// \internal The system stack, and a flag indicating whether the CPU is currently processing an interrupt
struct system_struct system;

/**
 * \brief \internal The entry point for all tasks
 *
 * First, interrupts are enabled, so that the tick interrupt can happen. Then, the task procedure is
 * executed. When the task procedure exits, if the task priority is zero, it is re-executed. If the task
 * priority isn't zero, the task is shut down.
 */
void task_starter() __attribute__ ((naked));

void task_starter(void *p) {
	sei();
	do {
		(*(current_task->proc))(p);
	} while (!current_task->pri);
	stop_task(current_task, 0);
}

/**
 * \brief \internal The entry point for all tasks that are stopping
 *
 * If this task is being stopped by a call to stop_task() with the \c wait_for_mutexes parameter set, then
 * the task will continue to run as normal until the last mutex is released. When that happens, then this
 * task stopper will start running on the task.
 *
 * First, interrupts are enabled, then any mailbox being read is released
 *
 * If the task has a cleanup procedure defined (the \c cleanup argument to create_task()), it is called
 *
 * Then, the task state is set to stopped, all mutexes and mailboxes are released, the (internal) task stopping
 * semaphore is signalled, and a task switch is executed.
 */
void task_stopper() __attribute__ ((naked));

void task_stopper() {
	sei();
	release_mbox_read();
	if (*current_task->cleanup) {
		(*(current_task->cleanup))();
	}
	cli();
	set_task_state(current_task, TASK_STATE_STOPPED);
	if (current_task->waiting_on_mbox) {
		current_task->waiting_on_mbox->writing_semaphore.value++;
		current_task->waiting_on_mbox = 0;
	}
	while (current_task->owned_mutex) {
		current_task->owned_mutex->notification.value++;
		current_task->owned_mutex->owner = 0;
		current_task->owned_mutex = current_task->owned_mutex->next;
	}
	task_stopping_semaphore.value++;
	SP = (uint16_t) system.stack_top;
	switch_task();
}

// (documentation in task.h)
int8_t stop_task(task_t *t, uint8_t wait_for_mutexes) {
	interrupt_store_t interrupts;

	// We can only be stopped by ourselves, or by a higher-priority task (see below)
	if ((t != current_task) && (t->pri > current_task->pri) && (!executing_isr())) {
		return -1;
	}
	if (!t->pri) return -2; // Cannot stop an idle task!
	interrupts = disable_interrupts();
	switch (get_task_state(t)) {
		case TASK_STATE_STARTING:
			set_task_state(t,TASK_STATE_STOPPED);
			break;
		case TASK_STATE_RUNNING:
		case TASK_STATE_WAITING_TO_STOP:
			if ((wait_for_mutexes) && (t->owned_mutex)) {
				set_task_state(t, TASK_STATE_WAITING_TO_STOP); // The task will commit suicide when it calls lock_off() on its last mutex
			} else { // (!wait_for_mutexes) || (!t->owned_mutex) -- the task is ready to die!
				t->waiting_semaphore = 0; // Otherwise, the task would never get re-scheduled!
				t->sp = t->stack;
				// The next time the task is scheduled, it will start executing task_stopper()
				*((t->sp)--) = (((uint16_t)task_stopper) & 0xFF);
				*((t->sp)--) = (((uint16_t)task_stopper) >> 8) & 0xFF;
				set_task_state(t,TASK_STATE_STOPPING);
			}
			// NOTE THE FALL THROUGH!
		case TASK_STATE_STOPPING:
		case TASK_STATE_CLEANING_UP:
			if (executing_isr()) break;
			if (t != current_task) {
				do {
					// This is why we can only be stopped by a higher-priority task - the task must not re-start
					// until after we have processed the fact that it has stopped
					switch (get_task_state(t)) {
						case TASK_STATE_WAITING_TO_STOP:
						case TASK_STATE_STOPPING:
						case TASK_STATE_CLEANING_UP:
							// (The task could be re-started by a higher pri task than us, but it won't get any
							//  further than "starting")
							current_task->waiting_semaphore = &task_stopping_semaphore;
							current_task->waiting_semaphore_min_value = task_stopping_semaphore.value+1;
							restore_interrupts(interrupts);
							yield();
							cli();
							break;
						default:
							restore_interrupts(interrupts);
							return 0;
					}
				} while (1);
			} else {
				SP = (uint16_t) system.stack_top;
				switch_task();
			}
			break;
		case TASK_STATE_STOPPED:
			restore_interrupts(interrupts);
			return 1;
		default: ;
	}
	restore_interrupts(interrupts);
	return 0;
}

// (documentation in task.h)
task_t *reserve_task(uint16_t stacklen, uint8_t pri, mutex_t *memory_mutex) {
	interrupt_store_t interrupts = 0;
	task_t *ans = 0;

	if ((memory_mutex) && (!executing_isr()) && (rtos_started()) && (current_task->pri)) {
		lock_on(memory_mutex);
	} else {
		memory_mutex = 0;
		interrupts = disable_interrupts();
	}
	ans = malloc(sizeof(task_t));
	if (memory_mutex) {
		lock_off(memory_mutex);
	} else {
		restore_interrupts(interrupts);
	}
	if (!ans) return 0;
	if (memory_mutex) {
		lock_on(memory_mutex);
	} else {
		cli();
	}
	ans->stack = malloc(stacklen) + stacklen - 1;
	if (memory_mutex) {
		lock_off(memory_mutex);
	} else {
		restore_interrupts(interrupts);
	}
	if (!ans->stack) return 0;
	ans->stacklen = stacklen;
	ans->pri = pri;
	ans->next = 0;
	ans->waiting_semaphore = 0;
	ans->waiting_on_mbox = 0;
	ans->owned_mutex = 0;
	set_task_state(ans, TASK_STATE_STOPPED);
	interrupts = disable_interrupts();
	if (!first_task) {
		first_task = ans;
	} else {
		task_t *next = first_task;
		while (next->next) {
			next = next->next;
		}
		next->next = ans;
	}
	restore_interrupts(interrupts);
	return ans;
}

// (documentation in task.h)
task_t *create_task(void (*proc)(void*), void (*cleanup)(), void *init_data, uint16_t stacklen, uint8_t pri, mutex_t *memory_mutex) {
	interrupt_store_t interrupts;
	task_t *ans = 0;

	interrupts = disable_interrupts();
	while (!ans) {
		task_t *t = first_task;
		while (t) {
			// We must find a task struct not only with a big enough stack, but also with the same pri, so
			// that any task stopping this task will get to process the stop before the task re-starts
			if ((get_task_state(t) == TASK_STATE_STOPPED) && (t->pri == pri) && (t->stacklen >= stacklen)) {
				if (ans) {
					if (ans->stacklen > t->stacklen) ans = t;
				} else {
					ans = t;
				}
			}
			t = t->next;
		}
		if (!ans) {
			restore_interrupts(interrupts); // We want interrupts disabled as little as possible
			reserve_task(stacklen, pri, memory_mutex); // Next time through the loop, we should "find" this task entry!
			cli();
		}
	}
	ans->proc = proc;
	ans->cleanup = cleanup;
	ans->sp = ans->stack;
 	// When the task gets scheduled, it will start executing task_starter()
 	*((ans->sp)--) = (((uint16_t)task_starter) & 0xFF);
	*((ans->sp)--) = (((uint16_t)task_starter) >> 8) & 0xFF;
	// Set up the init data on the stack
	*((ans->sp)--) = (((uint16_t)init_data) & 0xFF);
	*((ans->sp)--) = (((uint16_t)init_data) >> 8) & 0xFF;
	ans->waiting_semaphore = 0;
	ans->waiting_on_mbox = 0;
	ans->owned_mutex = 0;
	set_task_state(ans, TASK_STATE_STARTING);
	restore_interrupts(interrupts);
	if ((rtos_started()) && (!executing_isr()) && (pri > current_task->pri)) yield();
	return ans;
}

// (documentation in task.h)
uint8_t lock_on(mutex_t *m) {
	interrupt_store_t interrupts;
	if ((!current_task->pri) || (executing_isr())) {
		return 1;
	}
	interrupts = disable_interrupts();
	while (m->owner != current_task) {
		if (m->owner) {
			int16_t value = m->notification.value + 1; // We need to do this while interrupts are disabled!
			restore_interrupts(interrupts);
			wait_for_min_value(&(m->notification), value);
			cli();
		} else {
			mutex_t *owned = current_task->owned_mutex;
			m->owner = current_task;
			m->next = 0;
			if (!owned) {
				current_task->owned_mutex = m;
			} else {
				while (owned->next) owned = owned->next;
				owned->next = m;
			}
		}
	}
	restore_interrupts(interrupts);
	return 0;
}

// (documentation in task.h)
uint8_t lock_off(mutex_t *m) {
	interrupt_store_t interrupts;
	if (executing_isr()) return 1;
	interrupts = disable_interrupts();
	if (m->owner == current_task) {
		mutex_t *owned = current_task->owned_mutex;
		m->owner = 0;
		m->notification.value++;
		if (owned == m) {
			current_task->owned_mutex = m->next;
		} else {
			while (owned->next != m) owned = owned->next;
			owned->next = m->next;
		}
		if ((!current_task->owned_mutex) && (get_task_state(current_task) == TASK_STATE_WAITING_TO_STOP)) {
			stop_task(current_task, 0);
		}
	} else {
		restore_interrupts(interrupts);
		return 1;
	}
	restore_interrupts(interrupts);
	yield();
	return 0;
}

// (documentation in task.h)
int16_t get_semaphore_value(semaphore_t *s) {
	interrupt_store_t interrupts = disable_interrupts(); // We are reading a 16-bit value!
	int16_t ans = s->value;
	restore_interrupts(interrupts);
	return ans;
}

// (documentation in task.h)
uint8_t wait_for_min_value(semaphore_t *p, int16_t value) {
	interrupt_store_t interrupts;
	if ((!current_task->pri) || (executing_isr())) {
		return 1;
	}
	interrupts = disable_interrupts();
	if (!semaphore_triggered(p->value, value)) {
		current_task->waiting_semaphore = p;
		current_task->waiting_semaphore_min_value = value;
		restore_interrupts(interrupts);
		yield();
	} else {
		restore_interrupts(interrupts);
	}
	return 0;
}

// (documentation in task.h)
uint8_t wait_for_increment_of(semaphore_t *p, uint16_t amount) {
	interrupt_store_t interrupts;
	int16_t v;
	if ((!current_task->pri) || (executing_isr())) {
		return 1;
	}
	interrupts = disable_interrupts();
	v = p->value + amount;
	restore_interrupts(interrupts);
	return wait_for_min_value(p, v);
}

/**
 * \brief \internal Perform a task switch
 *
 * Note that interrupts must be disabled before we enter, and the stack pointer should be set up to use
 * the system stack.
 */
void switch_task() { /*  __attribute__ ((naked)) */
	uint8_t min_priority = 0;
	// ans will contain the next task to run
	task_t *ans = 0;
	// Our round-robin algorithm is to check the task list starting at the current task. Hence the task we will end up
	// with will be the current task only if it is the only task available.
	task_t *checking = current_task;

	// We set the system.interrupted_task flag so that if any ISR executes during the brief re-enabling
	// of interrupts below, they won't attempt a task switch.
	system.interrupted_task = 1;
	__asm__ volatile ("sei\n nop\n nop\n cli\n" ::); // Give any pending interrupts a chance to run
	do {
		if (get_task_state(checking) != TASK_STATE_STOPPED) {
			// First, if the task is waiting on a semaphore, and the semaphore is triggered, then mark it as no
			// longer waiting on the semaphore
			if (checking->waiting_semaphore) {
				if (semaphore_triggered((checking->waiting_semaphore)->value, checking->waiting_semaphore_min_value)) {
					checking->waiting_semaphore = (semaphore_t*)0;
				}
			}
			// Now, if the task is available (i.e. not waiting on a semaphore), and if it's priority is at least the
			// minimum priority of the available tasks found so far, then it is our candidate for the next task to run
			if (checking->pri >= min_priority && !checking->waiting_semaphore) {
				min_priority = checking->pri;
				ans = checking;
			}
		}
		checking = checking->next;
		if (!checking) {
			// If we have reached the end of the task list, then go back to the start
			checking = first_task;
		}
	} while (checking != current_task); // Stop when we have looped through the task list completely
	current_task = ans; // This is why it is so important to have an always-available idle task!!!
	SP = (uint16_t) ans->sp; // Restore the stack pointer
	system.interrupted_task = 0;
	switch (get_task_state(current_task)) {
		case TASK_STATE_RUNNING:
		case TASK_STATE_CLEANING_UP:
		case TASK_STATE_WAITING_TO_STOP:
			restore_cpu_context(); // The task has previously saved its context onto its stack
			__asm__ volatile ("ret\n" ::);
		case TASK_STATE_STARTING:
			set_task_state(current_task, TASK_STATE_RUNNING);
			// Retrieve the init data from the stack, placing it into the registers that mark the value of the argument
			// to the called function. Then, perform a ret, which will start executing task_starter()
			__asm__ volatile ("pop r25\npop r24\nret\n" ::);
			break;
		case TASK_STATE_STOPPING:
			set_task_state(current_task, TASK_STATE_CLEANING_UP);
			break;
		default: ;
	}
	__asm__ volatile ("ret" ::); // This will start executing task_stopper()
}

// (documentation in task.h)
void increment_semaphore_by(semaphore_t *s, uint16_t amount) {
	interrupt_store_t interrupts = disable_interrupts();
	s->value += amount;
	restore_interrupts(interrupts);
	if ((!executing_isr()) && (rtos_started())) {
		yield(); // There may now be a higher-priority task that can run
	}
}

// (documentation in task.h)
int16_t get_current_mbox_version(mailbox_t *m) {
	interrupt_store_t interrupts = disable_interrupts();
	int16_t ans = m->reading_semaphore.value; // Reading a 16-bit value!
	restore_interrupts(interrupts);
	return ans;
}

// (documentation in task.h)
void *read_mbox(mailbox_t *m, int16_t *version) {
	interrupt_store_t interrupts;
	void *ans = 0;
	if ((!rtos_started()) || (executing_isr())) return 0;
	interrupts = disable_interrupts();
	if (!current_task->waiting_on_mbox) { // We can only "lock" one mailbox at a time
		ans = m->data;
		// Now, "lock" the mailbox against writing
		current_task->waiting_on_mbox = m;
		current_task->waiting_on_mbox_version = m->reading_semaphore.value;
		// Return the version number (if required)
		if (version) *version = m->reading_semaphore.value;
	}
	restore_interrupts(interrupts);
	return ans;
}

// (documentation in task.h)
void *read_mbox_min_version(mailbox_t *m, int16_t *version) {
	interrupt_store_t interrupts;
	void *ans = 0;
	if ((!rtos_started()) || (executing_isr()) || (!(current_task->pri))) return 0;
	interrupts = disable_interrupts();
	if (!current_task->waiting_on_mbox) {
		current_task->waiting_on_mbox = m;
		current_task->waiting_on_mbox_version = *version;
		while (!semaphore_triggered(m->reading_semaphore.value, *version)) {
			restore_interrupts(interrupts);
			increment_semaphore_by(&(m->writing_semaphore), 1); // Writers can be waiting for readers!
			wait_for_min_value(&(m->reading_semaphore), *version);
			cli();
		}
		ans = m->data;
		*version = m->reading_semaphore.value;
	}
	restore_interrupts(interrupts);
	return ans;
}

// (documentation in task.h)
mailbox_t *release_mbox_read() {
	interrupt_store_t interrupts;
	mailbox_t *ans = 0;
	if (!rtos_started()) return 0;
	interrupts = disable_interrupts();
	ans = current_task->waiting_on_mbox;
	current_task->waiting_on_mbox = 0; // Writing a 16-bit value!
	restore_interrupts(interrupts);
	if (ans) {
		increment_semaphore_by(&(ans->writing_semaphore), 1);
	}
	return ans;
}

// (documentation in task.h)
void initialise_mbox(mailbox_t *m, void *data, const int16_t version) {
	m->data = data;
	m->reading_semaphore.value = version;
	m->writing_semaphore.value = 0;
}

/**
 * \internal \brief Find out if a mailbox is "empty" (i.e. if there is no-one waiting to read it)
 *
 * Note that interrupts must be disabled before we enter
 */
int8_t mbox_is_empty(mailbox_t *m) {
	task_t *t = first_task;
	while (t) {
		if (t->waiting_on_mbox == m) {
			if (semaphore_triggered(m->reading_semaphore.value, t->waiting_on_mbox_version)) {
				// Task t is waiting for the current version of this mailbox
				return 0;
			}
		}
		t = t->next;
	}
	return 1;
}

// (documentation in task.h)
void write_mbox(mailbox_t *m, void *data, uint8_t wait_for_receivers, uint8_t wait_for_empty_nullify) {
	interrupt_store_t interrupts;
	uint8_t receivers = 0;
	uint8_t wait_for_empty = wait_for_empty_nullify;
	if ((!rtos_started()) || (executing_isr()) || (!(current_task->pri))) return;
	interrupts = disable_interrupts();
	// First, wait for the mailbox to be empty - i.e. wait for everyone that wants the current version
	// of the mailbox to get it - we are not allowed to over-write whats in the mailbox until everyone
	// is finished with the old data that is in there.
	while (!mbox_is_empty(m)) {
		int16_t v = m->writing_semaphore.value + 1; // Need to read 16-bit value with disabled interrupts
		restore_interrupts(interrupts);
		wait_for_min_value(&(m->writing_semaphore), v);
		cli();
	}
	// Now, we wait for the specified number of receivers
	while (wait_for_receivers > receivers) {
		task_t *t = first_task;
		receivers = 0;
		while (t && (wait_for_receivers > receivers)) {
			if ((t->waiting_on_mbox == m) && (!semaphore_triggered(m->reading_semaphore.value, t->waiting_on_mbox_version))) {
				// Task t is waiting on this mailbox
				receivers++;
			}
			t = t->next;
		}
		if (wait_for_receivers > receivers) {
			// We haven't got the required number of receivers yet - wait on the writing semaphore
			int16_t v = m->writing_semaphore.value + 1;
			restore_interrupts(interrupts);
			wait_for_min_value(&(m->writing_semaphore), v);
			cli();
		}
	}
	// The mailbox is empty, and we have the required number of receivers, so put the data into the mailbox,
	// and increment the mailbox version (which doubles as the reading semaphore value)
	m->data = data;
	m->reading_semaphore.value++;
	// Have we been requested to wait for everyone to read the data?
	if (wait_for_empty) {
		// v is the version of the data we have published
		int16_t v = m->reading_semaphore.value;
		while (wait_for_empty) {
			int8_t empty = 1;
			task_t *t = first_task;
			while (t && empty) {
				if (t->waiting_on_mbox == m) {
					if (semaphore_triggered(v, t->waiting_on_mbox_version)) {
						// Task t is waiting to read our data
						empty = 0;
					}
				}
				t = t->next;
			}
			if (empty) {
				wait_for_empty = 0;
				if (wait_for_empty_nullify > 1) {
					// Publish another version of the mailbox data - one with a null pointer
					// (It is safe to do this, as the mailbox is "empty" - i.e. no-one is waiting
					//  for what we've just published).
					m->data = 0;
					m->reading_semaphore.value++;
				}
			} else {
				// There are still tasks waiting to read our data - wait on the writing semaphore
				int16_t val = m->writing_semaphore.value + 1;
				restore_interrupts(interrupts);
				wait_for_min_value(&(m->writing_semaphore), val);
				cli();
			}
		}
	}
	restore_interrupts(interrupts);
	yield();
}

// (documentation in task.h)
int8_t write_mbox_now(mailbox_t *m, void *data) {
	interrupt_store_t interrupts;
	if (!rtos_started()) return 1;
	interrupts = disable_interrupts();
	if (!mbox_is_empty(m)) {
		restore_interrupts(interrupts);
		return 2;
	}
	m->data = data;
	m->reading_semaphore.value++;
	restore_interrupts(interrupts);
	if (!executing_isr()) yield();
	return 0;
}

// (documentation in task.h)
void wait_for_receiver(mailbox_t *m) {
	interrupt_store_t interrupts;
	if ((!rtos_started()) || (executing_isr()) || (!(current_task->pri))) return;
	interrupts = disable_interrupts();
	while (1) {
		task_t *t = first_task;
		int16_t v;
		while (t) {
			if ((t->waiting_on_mbox == m) && (!semaphore_triggered(m->reading_semaphore.value, t->waiting_on_mbox_version))) {
				restore_interrupts(interrupts);
				return;
			}
			t = t->next;
		}
		v = m->writing_semaphore.value + 1;
		restore_interrupts(interrupts);
		wait_for_min_value(&(m->writing_semaphore), v);
		cli();
	}
}

// (documentation in task.h)
interrupt_store_t disable_interrupts() {
	interrupt_store_t ans = (SREG & _BV(SREG_I));
	cli();
	return ans;
}

// (documentation in task.h)
void restore_interrupts(interrupt_store_t interrupts) {
	if (interrupts) {
		sei();
	} else {
		cli();
	}
}

// (documentation in task.h)
void yield() { /*  __attribute__ ((naked)) */
	save_cpu_context();
	current_task->sp = (uint8_t *)SP;
	SP = (uint16_t) system.stack_top;
	switch_task();
}

// (documentation in task.h)
void task_switcher_start(void (*idle)(void*), void *idle_data, uint16_t idle_stacklen, uint16_t system_stacklen) { /*  __attribute__ ((naked)) */
	cli();
	create_task(idle, 0, idle_data, idle_stacklen, 0, 0);
	system.stack_top = malloc(system_stacklen);
	system.stack_top += system_stacklen - 1;
	system.interrupted_task = 0;
	task_stopping_semaphore.value = 0;
	current_task = first_task;
	SP = (uint16_t) system.stack_top;
	switch_task();
}
