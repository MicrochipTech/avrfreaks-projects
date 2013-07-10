#ifndef _INT_H_
#define _INT_H_

#include "avr_compiler_.h"
#include "lcd.h"

#define HC245_OE_DDR      PORTF_DIR
#define HC245_OE_PORT     PORTF_OUT
#define HC245_OE_PORTSET  PORTF_OUTSET
#define HC245_OE_PORTCLR  PORTF_OUTCLR
#define HC245_OE_PIN      PORTF_PIN
#define HC245_OE_BIT      3

#define EXCLK_DDR         PORTC_DIR
#define EXCLK_PORT        PORTC_OUT
#define EXCLK_PORTSET     PORTC_OUTSET
#define EXCLK_PORTCLR     PORTC_OUTCLR
#define EXCLK_PIN         PORTC_PIN
#define EXCLK_BIT         2

#define EN_245		  HC245_OE_PORTCLR=1<<HC245_OE_BIT
#define DISEN_245	  HC245_OE_PORTSET=1<<HC245_OE_BIT
#define EN_EXCLK	  EXCLK_PORTSET=1<<EXCLK_BIT 
#define DISEN_EXCLK	  EXCLK_PORTCLR=1<<EXCLK_BIT

void Init_INT0(void);

#endif

