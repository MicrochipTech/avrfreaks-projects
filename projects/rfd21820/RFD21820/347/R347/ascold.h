// *************************************************
// ****                                         ****
// ****        I/O bits altering macros         ****
// ****         Idea by Ascold Volkov           ****
// ****   presented in ru.embedded 18-Apr-2000  ****
// ****                                         ****
// ****   Adopted to ARM by                     ****
// ****            Sergey A. Borshch, 2006      ****
// *************************************************
#ifndef ASCOLD_H__
#define ASCOLD_H__

//  #define examples:
//
// AT91SAM7:
//      LED on PIOA, bit 1, active level high,
//      key on PIOA, bit 29, active level low
// #define  AT91SAM7
// #define  LED    A, 1, H
// #define  KEY    A, 29, L
//
// LPC2xxx:
//      LED on Port1, bit 1, active level high,
//      key on Port0, bit 18, active level low
// #define  LPC
// #define  LED    1, 1, H
// #define  KEY    0, 18, L
//
// ADuC70xxx:
//      LED on P1, bit 1, active level high,
//      key on P3, bit 6, active level low
// #define  ADuC
// #define  LED    1, 1, H
// #define  KEY    3, 6, L
//
// AVR:
//      LED on port B, bit 1, active level high,
//      key on port C, bit 6, active level low
// #define  LED    PORTB, 1, H
// #define  KEY    PINC, 6, L
//
// MSP:
//      LED on port 1, bit 1, active level high,
//      key on port 2, bit 6, active level low
// #define  LED    P1OUT, 1, H
// #define  KEY    P2IN, 6, L
//
//            usage examples:
// #include "ascold.h"
//
// on(LED);     // turn LED on
// off(LED);     // turn LED off
// cpl(LED);     // toggle LED
//
// if ( signal(KEY) )   // if key pressed
// {
	//    ............
// }
//
// if ( !signal(KEY) )   // if key released
// {
	//    ............
// }
//


#if defined(AT91SAM7) \
|| defined(AT91SAM7S32) || defined(AT91SAM7S64) || defined(AT91SAM7S128) || defined(AT91SAM7S256)
    #define _setL(port,bit)         do { AT91C_BASE_PIO##port##->PIO_CODR = (1 << bit); } while(0)
    #define _setH(port,bit)         do { AT91C_BASE_PIO##port##->PIO_SODR = (1 << bit); } while(0)
    #define _clrL(port,bit)         do { AT91C_BASE_PIO##port##->PIO_SODR = (1 << bit); } while(0)
    #define _clrH(port,bit)         do { AT91C_BASE_PIO##port##->PIO_CODR = (1 << bit); } while(0)
    #define _bitL(port,bit)         (!(AT91C_BASE_PIO##port##->PIO_PDSR & (1 << bit)))
    #define _bitH(port,bit)         ( AT91C_BASE_PIO##port##->PIO_PDSR & (1 << bit))
    #define _cpl(port,bit,val)      do { AT91C_BASE_PIO##port##->PIO_ODSR ^= (1<<bit); } while(0)
    #define _dir_out(port,bit,val)  do { AT91C_BASE_PIO##port##->PIO_OER = (1<<bit); } while(0)
    #define _dir_in(port,bit,val)   do { AT91C_BASE_PIO##port##->PIO_ODR = (1<<bit); } while(0)
    #define _dir(port,bit,val)      (AT91C_BASE_PIO##port##->PIO_OSR & (1 << bit))
#elif defined(LPC) \
    || defined(LPC2119) || defined(LPC2129) || defined(LPC2212) || defined(LPC2214)
    #define _setL(port,bit)         do { IO##port##CLR = (1 << bit); } while(0)
    #define _setH(port,bit)         do { IO##port##SET = (1 << bit); } while(0)
    #define _clrL(port,bit)         do { IO##port##SET = (1 << bit); } while(0)
    #define _clrH(port,bit)         do { IO##port##CLR = (1 << bit); } while(0)
    #define _bitL(port,bit)         (!(IO##port##PIN & (1 << bit)))
    #define _bitH(port,bit)         ( IO##port##PIN & (1 << bit))
    #define _cpl(port,bit,val)      do { IO##port##PIN ^= (1 << bit); } while(0)
    #define _dir_out(port,bit,val)  do { IO##port##DIR |= (1 << bit); } while(0)
    #define _dir_in(port,bit,val)   do { IO##port##DIR &= ~(1 << bit); } while(0)
    #define _dir(port,bit,val)      ( IO##port##DIR & (1 << bit))
#elif defined(ADuC) \
    || defined(ADuC7019) || defined(ADuC7020) || defined(ADuC7021)|| defined(ADuC7022)  \
    || defined(ADuC7024)|| defined(ADuC7025)|| defined(ADuC7026)|| defined(ADuC7027)
    #define _setL(port,bit)         do { GP##port##CLR = (1 << (bit + 16)); } while(0)
    #define _setH(port,bit)         do { GP##port##SET = (1 << (bit + 16)); } while(0)
    #define _clrL(port,bit)         do { GP##port##SET = (1 << (bit + 16)); } while(0)
    #define _clrH(port,bit)         do { GP##port##CLR = (1 << (bit + 16)); } while(0)
    #define _bitL(port,bit)         (!(GP##port##DAT & (1 << bit)))
    #define _bitH(port,bit)         ( GP##port##DAT & (1 << bit))
    #define _cpl(port,bit,val)      do { GP##port##DAT ^= (1 << (bit + 16)); } while(0)
    #define _dir_out(port,bit,val)  do { GP##port##DAT |= (1 << (bit + 24)); } while(0)
    #define _dir_in(port,bit,val)   do { GP##port##DAT &= ~(1 << (bit + 24)); } while(0)
#else	// MSP430, AVR etc.
    #define _setL(port,bit)         do { port &= ~(1 << bit); } while(0)
    #define _setH(port,bit)         do { port |= (1 << bit); } while(0)
    #define _clrL(port,bit)         do { port |= (1 << bit); } while(0)
    #define _clrH(port,bit)         do { port &= ~(1 << bit); } while(0)
    #define _bitL(port,bit)         (!(port & (1 << bit)))
    #define _bitH(port,bit)         (port & (1 << bit))
    #define _cpl(port,bit,val)      do {port ^= (1 << bit); } while(0)
    #define _dir_out(port,bit)      do { DDR##port |= (1 << bit); } while(0)
    #define _dir_in(port,bit)       do { DDR##port &= ~(1 << bit); } while(0)
#endif

#define dir_out(x)              _dir_out (x)
#define dir_in(x)               _dir_in (x)
#define dir(x)                  _dir (x)

#define _set(port,bit,val)      _set##val(port,bit)
#define on(x)                   _set (x)

#define _clr(port,bit,val)      _clr##val(port,bit)
#define off(x)                  _clr (x)

#define _bit(port,bit,val)      _bit##val(port,bit)
#define signal(x)               (!! _bit (x))

#define cpl(x)                  _cpl (x)

#endif  //  ASCOLD_H__
