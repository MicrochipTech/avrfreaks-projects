#ifndef PWM_H
#define PWM_H

#include <avr/io.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#define F_CPU 8000000UL
#include <util/delay.h>


/** 
 *  @name  Definitions for MCU Clock Frequency
 *  Adapt the MCU clock frequency in Hz to your target. 
 */
#ifndef XTAL
#define XTAL 8000000              /**< clock frequency in Hz, used to calculate delay timer */
#endif

#define RED(what) OCR0A = what;
#define GRN(what) OCR0B = what;
#define BLU(what) OCR2A = what;
#define WHT(what) OCR2B = what;
#define PWMWRITE(a,b,c,d) {RED(a);GRN(b);BLU(c);WHT(d);};
//PWM DEFINITIONS
static uint8_t red     = 0x00;
static uint8_t grn     = 0x00;
static uint8_t blu     = 0x00;
static uint8_t wht     = 0x00;
//END PWM DEFINITIONS

extern void pwm_setup(void);
extern void pwm_write(uint8_t ein, uint8_t zwi, uint8_t dri, uint8_t vir);
#endif //PWM_H
