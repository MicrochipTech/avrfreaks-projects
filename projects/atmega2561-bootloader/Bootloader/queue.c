#include <stdlib.h>
#include <ioavr.h>
#include <intrinsics.h>
#include "queue.h"
#include "bootloader.h"

page_eeprom_queue_t eeprom_page_buf;
can_queue_t can_rx_queue;
can_queue_t can_tx_queue;
#ifdef USART
byte_queue_t usart_rx_queue;
#endif

//STATIC_QUEUE_TYPE static_queue_peek(struct static_queue *q) {
//  STATIC_QUEUE_TYPE ret;
//  __disable_interrupt();
//  ret = q->buffer[q->head];
//  __enable_interrupt();
//  return ret;
//}

//
//bool static_queue_is_full(struct static_queue *q) {
//  return q->full;
//}

//void static_queue_clear(struct static_queue *q) {
//  __disable_interrupt();
//  q->tail = q->head = 0;
//  q->full = 0;
//  __enable_interrupt();
//}

/**************
 * EEPROM queue
 */

void eeprom_queue_init(struct eeprom_queue *q) {
  q->tail = q->head = q->full = 0;
}

bool eeprom_queue_enqueue(struct eeprom_queue *q, EEPROM_QUEUE_TYPE *obj) {
  unsigned char old_int = __save_interrupt();
  __disable_interrupt();
  unsigned int i;
  if (q->full) {
    __restore_interrupt(old_int);
    return false;
  }
  q->buffer[q->tail].page_ptr = obj->page_ptr;
  q->buffer[q->tail].address = obj->address;
  for (i=0; i < EEPROM_PAGE_SIZE; i++)
    q->buffer[q->tail].data[i] = obj->data[i];
  if (q->tail == (EEPROM_QUEUE_SIZE-1))
    q->tail = 0;
  else
    q->tail++;
  if (q->head == q->tail)
    q->full = 1;
  __restore_interrupt(old_int);
  return true;
}

void eeprom_queue_dequeue(struct eeprom_queue *q, EEPROM_QUEUE_TYPE *obj) {
  unsigned int i;
  unsigned char old_int = __save_interrupt();
  __disable_interrupt();
  obj->page_ptr = q->buffer[q->head].page_ptr;
  obj->address = q->buffer[q->head].address;
  for (i=0; i < EEPROM_PAGE_SIZE; i++)
    obj->data[i] = q->buffer[q->head].data[i];
  if (q->head == (EEPROM_QUEUE_SIZE-1))
    q->head = 0;
  else
    q->head++;
  q->full = 0;
  __restore_interrupt(old_int);
}

bool eeprom_queue_is_empty(struct eeprom_queue *q) {
  bool ret;
  unsigned char old_int = __save_interrupt();
  __disable_interrupt();
  ret = !q->full && (q->head == q->tail);
  __restore_interrupt(old_int);
  return ret;
}

/***********
 * CAN queue
 */

void can_queue_init(can_queue_t *q) {
  q->tail = q->head = q->full = 0;
}
bool can_queue_enqueue(can_queue_t *q, CAN_QUEUE_TYPE *obj) {
  unsigned char old_int = __save_interrupt();
  __disable_interrupt();
  unsigned int i;
  if (q->full) {
    __restore_interrupt(old_int);
    return false;
  }
  q->buffer[q->tail].count = obj->count;
  q->buffer[q->tail].mob_no = obj->mob_no;
  q->buffer[q->tail].id_high = obj->id_high;
  q->buffer[q->tail].id_low = obj->id_low;
  for (i=0; i < 8; i++)
    q->buffer[q->tail].data[i] = obj->data[i];
  if (q->tail == (CAN_QUEUE_SIZE-1))
    q->tail = 0;
  else
    q->tail++;
  if (q->head == q->tail)
    q->full = 1;
  __restore_interrupt(old_int);
  return true;
}
void can_queue_dequeue(can_queue_t *q, CAN_QUEUE_TYPE *obj) {
  unsigned int i;
  unsigned char old_int = __save_interrupt();
  __disable_interrupt();
  obj->count = q->buffer[q->head].count;
  obj->mob_no = q->buffer[q->head].mob_no;
  obj->id_high = q->buffer[q->head].id_high;
  obj->id_low = q->buffer[q->head].id_low;
  for (i=0; i < 8; i++)
    obj->data[i] = q->buffer[q->head].data[i];
  if (q->head == (CAN_QUEUE_SIZE-1))
    q->head = 0;
  else
    q->head++;
  q->full = 0;
  __restore_interrupt(old_int);
}

bool can_queue_is_empty(can_queue_t *q) {
  bool ret;
  unsigned char old_int = __save_interrupt();
  __disable_interrupt();
  ret = !q->full && (q->head == q->tail);
  __restore_interrupt(old_int);
  return ret;
}

/************
 * Byte queue
 */

void byte_queue_init(byte_queue_t *q) {
  q->tail = q->head = q->full = 0;
}

bool byte_queue_enqueue(byte_queue_t *q, BYTE_QUEUE_TYPE *obj) {
  unsigned char old_int = __save_interrupt();
  __disable_interrupt();
  if (q->full) {
    __restore_interrupt(old_int);
    return false;
  }
  q->buffer[q->tail] = *obj;
  if (q->tail == (BYTE_QUEUE_SIZE-1))
    q->tail = 0;
  else
    q->tail++;
  if (q->head == q->tail)
    q->full = 1;
  __restore_interrupt(old_int);
  return true;
}

void byte_queue_dequeue(byte_queue_t *q, BYTE_QUEUE_TYPE *obj) {
  unsigned char old_int = __save_interrupt();
  __disable_interrupt();
  *obj = q->buffer[q->head];
  if (q->head == (BYTE_QUEUE_SIZE-1))
    q->head = 0;
  else
    q->head++;
  q->full = 0;
  __restore_interrupt(old_int);
}

bool byte_queue_is_empty(byte_queue_t *q) {
  bool ret;
  unsigned char old_int = __save_interrupt();
  __disable_interrupt();
  ret = !q->full && (q->head == q->tail);
  __restore_interrupt(old_int);
  return ret;
}
