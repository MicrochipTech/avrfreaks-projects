#ifndef _DRIVER_H
#define _DRIVER_H

#define DATA 		PB1
#define CLK  		PB0
#define PORT_IN  	PINB
#define PORT_DIR 	DDRB
#define PORT_OUT	PORTB
#define MAX_IT 32000
#define LEARN 0xD2
#define ERASE 0xC3
#define PROG  0xB4

extern uint8_t port_init(void);
extern uint8_t request_command(void);
extern void send_octet(uint8_t);
extern void send_bit(uint8_t);
extern void get_bytes(uint8_t *,uint8_t);
extern uint8_t get_ack(void);
extern uint8_t get_ack2(void);


#endif
