#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>
#include "common.h"
#include "bootloader.h"
#include "bl_can.h"
#include "usart.h"

#define EEPROM_QUEUE_SIZE 5
#define EEPROM_QUEUE_TYPE page_eeprom_t

#define CAN_QUEUE_SIZE 5
#define CAN_QUEUE_TYPE can_data_t

#define BYTE_QUEUE_SIZE 18
#define BYTE_QUEUE_TYPE unsigned char

/**
 * Queue used for eeprom memory pages 
 */

typedef struct eeprom_queue {
  volatile unsigned int head;
  volatile unsigned int tail;
  volatile bool full;
  volatile EEPROM_QUEUE_TYPE buffer[EEPROM_QUEUE_SIZE];
} page_eeprom_queue_t;

extern page_eeprom_queue_t eeprom_page_buf;

void eeprom_queue_init(struct eeprom_queue *q);
bool eeprom_queue_enqueue(struct eeprom_queue *q, EEPROM_QUEUE_TYPE *obj);
void eeprom_queue_dequeue(struct eeprom_queue *q, EEPROM_QUEUE_TYPE *obj);
bool eeprom_queue_is_empty(struct eeprom_queue *q);

/**
 * Queue used for incoming/outgoing CAN data
 */

typedef struct can_queue {
  volatile unsigned int head;
  volatile unsigned int tail;
  volatile bool full;
  volatile CAN_QUEUE_TYPE buffer[CAN_QUEUE_SIZE];
} can_queue_t;

extern can_queue_t can_rx_queue;
extern can_queue_t can_tx_queue;

void can_queue_init(can_queue_t *q);
bool can_queue_enqueue(can_queue_t *q, CAN_QUEUE_TYPE *obj);
void can_queue_dequeue(can_queue_t *q, CAN_QUEUE_TYPE *obj);
bool can_queue_is_empty(can_queue_t *q);

/**
 * Queue used for byte data, for example incoming USART data
 */

typedef struct byte_queue {
  volatile unsigned int head;
  volatile unsigned int tail;
  volatile bool full;  
  volatile BYTE_QUEUE_TYPE buffer[BYTE_QUEUE_SIZE];
} byte_queue_t;

extern byte_queue_t usart_rx_queue;

void byte_queue_init(byte_queue_t *q);
bool byte_queue_enqueue(byte_queue_t *q, BYTE_QUEUE_TYPE *obj);
void byte_queue_dequeue(byte_queue_t *q, BYTE_QUEUE_TYPE *obj);
bool byte_queue_is_empty(byte_queue_t *q);

#endif
