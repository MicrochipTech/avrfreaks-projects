// Demonstrator.h Real Time Clock version

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#define  F_CPU 1000000UL
#include <util/delay.h>
#include <compat/deprecated.h>
#include <inttypes.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

// Definitions
#define KEY_UP      0
#define KEY_DOWN    1
#define KEY_LEFT    2
#define KEY_RIGHT   3
#define KEY_PUSH    4
#define KEY_INVALID 5

#define BUTTON_A    6   // UP
#define BUTTON_B    7   // DOWN
#define BUTTON_C    2   // LEFT
#define BUTTON_D    3   // RIGHT
#define BUTTON_O    4   // PUSH

#define PINB_MASK ((1<<PINB4)|(1<<PINB6)|(1<<PINB7))
#define PINE_MASK ((1<<PINE2)|(1<<PINE3))

#define TRUE 1
#define FALSE 0



void setSecond(void);
void setMinute(void);
void setHour(void);
char CHAR2BCD2(char input);
void initializer(void);
void RTC_init(void);
void PinChangeInterrupt(void);
char getkey(void);
void showClock(void);




