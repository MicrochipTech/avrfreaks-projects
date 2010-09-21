// ad_leds.h
//
// header file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//
#include <iom16v.h>
#include <macros.h>
#include <stdio.h>
#define OK_LEDS

extern char sa_led;            // bits to write to PORTA lower
extern char led_mask;          // mask for interrupt handler
extern char led_cnt;           // counter for interrupt handler

// ----------- function prototypes
void leds(char, char);        // turn individual LEDs on/off
char get_led(char);           // read individual LED state
void init_leds(void);         // initialize LEDs

#define LED_TOG   2             // toggle LEDs
#define LED_ON    1             // LED on
#define LED_OFF   0             // LED off
