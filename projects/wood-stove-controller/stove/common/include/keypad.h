/*
 * keypad.h
 *
 * Created: 12/31/2012 2:51:40 PM
 *  Author: sasquach
 */ 


#ifndef KEYPAD_H_
#define KEYPAD_H_

#define KEYPAD_PORT PORTB
#define KEYPAD_DDR DDRB
#define KEYPAD_PIN PINB
#define KEYPAD_PIN0 PB0
#define KEYPAD_PIN1 PB1
#define KEYPAD_PIN2 PB2
#define KEYPAD_PIN3 PB3
#define KEYPAD_MASK (_BV(KEYPAD_PIN0) | _BV(KEYPAD_PIN1) | _BV(KEYPAD_PIN2) | _BV(KEYPAD_PIN3))

#define KEYPAD_RST		KEYPAD_PIN3
#define KEYPAD_RST_BASEMENT		KEYPAD_PIN2
#define KEYPAD_UP		KEYPAD_PIN0
#define KEYPAD_DOWN		KEYPAD_PIN1


#define KEYPAD_DEBOUNCE_COUNT 100
#define KEYPAD_AUTO_KEY_COUNT 1000

#endif /* KEYPAD_H_ */


extern uint8_t keypad_out;
extern uint8_t keypad_temp;
extern uint16_t keypad_count;
extern uint8_t keypad_multi;

void keypad(void);
void keypad_init(void);
void menu(void);
void menu_init(void);