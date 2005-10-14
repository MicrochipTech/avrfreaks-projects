/**************************************************************
 **             - IO_AVR.H -
 **
 **     I/O definitions for iccAVR
 **
 **     Used with iccAVR.
 **
 **     Copyright IAR Systems 2001. All rights reserved.
 **
 **     $Name:  $
 **
 ** MODIFIED AND DISTRIBUTED BY LARRY BARELLO FOR USE WITH AVRX
 ** UNTIL DISTRIBUTED BY IAR SYSTEMS WITH THEIR PRODUCTS.
 **
 ** NB This is old stuff from 2001, it may or may not be complete
 ** or even correct at a later date.
 **
 **************************************************************/

#ifndef __IO_AVR_H
#define __IO_AVR_H

#if defined(__AT90tiny22__)
#include "iotiny22.h"
#elif defined(__AT90S2313__)
#include "io2313.h"
#elif defined(__AT90S2323__)
#include "io2323.h"
#elif defined(__AT90S2333__)
#include "io2333.h"
#elif defined(__AT90S2343__)
#include "io2343.h"
#elif defined(__AT90S4414__)
#include "io4414.h"
#elif defined(__AT90S4433__)
#include "io4433.h"
#elif defined(__AT90S4434__)
#include "io4434.h"
#elif defined(__AT90S8515__)
#include "io8515.h"
#elif defined(__AT90S8534__)
#include "io8534.h"
#elif defined(__AT90S8535__)
#include "io8535.h"
#elif defined(__AT90Mega103__)
#include "iom103.h"
#elif defined(__AT90Mega161__)
#include "iom161.h"
#elif defined(__AT90Mega603__)
#include "iom603.h"
#elif defined(__AT90Mega83__)
#include "iom83.h"
#elif defined(__AT90Mega163__)
#include "iom163.h"
#elif defined(__AT94Kxx__)
#include "ioat94k.h"
/* These files are included for assembler use only */
#elif defined (__ATtiny28__)
#  include <iotn28.h>
#elif defined (__AT90S1200__)
#  include <io1200.h>
#elif defined (__ATtiny15__)
#  include <iotn15.h>
#elif defined (__ATtiny12__)
#  include <iotn12.h>
#elif defined (__ATtiny11__)
#  include <iotn11.h>
#else
#error Unknown CPU type, or --cpu option not used
#endif

#endif /* __IO_AVR_H */
