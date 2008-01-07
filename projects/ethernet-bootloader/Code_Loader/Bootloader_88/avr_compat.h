/* vim: sw=8 ts=8 si et: */
/****************************************************************************
* Title   :   compatibility module for forward compatibility with
              newer ARV C compiler which does not have older
              macros
* Authors:    Guido Socher
* Copyright: GPL
*
*        
*****************************************************************************/
#ifndef AVR_COMPAT_H
#define AVR_COMPAT_H

// uncomment the following line if you have avr-1.2.x or less:
//#define ALIBC_OLD 1

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef inb
#define inb(sfr) _SFR_BYTE(sfr) 
#endif

#ifndef outb
#define outb(sfr, val) (_SFR_BYTE(sfr) = (val))
#endif

#ifndef inw
#define inw(sfr) _SFR_WORD(sfr)
#endif

#ifndef outw
#define outw(sfr, val) ( _SFR_WORD(sfr) = (val)) 
#endif

#ifndef outp
#define outp(val, sfr) outb(sfr, val) 
#endif

#ifndef inp
#define inp(sfr) inb(sfr) 
#endif

#ifndef BV
#define BV(bit) _BV(bit)
#endif

/* compatibility macro for libc 1.0 to 1.2 */
#ifndef PRG_RDB
#define PRG_RDB(addr)       pgm_read_byte(addr)
#endif

#endif //AVR_COMPAT_H

