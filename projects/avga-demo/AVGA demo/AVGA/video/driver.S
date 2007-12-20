/*

	file:	driver.s
	desc:	Video Graphic Driver
 
 	note:	Optimized version.
 
  			This is __very__ time critical code. every 5th clock cycle, out instruction with new pixel must be executed.
  			beginning of the function must take constant number of cycles.

  			Please only modify if really know what you're doing!

	author: Jaromir Dvorak (md@unicode.cz)
 
*/




#define __ASM__
#define __SFR_OFFSET 0

#include <avr/io.h>
#include "driver.h"
#include "video.h"


.extern __ramp
.extern _pgmp
.extern _scrp

; register usage
; http://www.nongnu.org/avr-libc/user-manual/FAQ.html#faq_reg_usage
#define r_ram_bitmap_L  r20
#define r_ram_bitmap_H  r23
#define r_pgm_bitmap_L  r21
#define r_pgm_bitmap_H  r25

#define tmp1            r24
#define tmp2            r19
#define tmp3            r18

#define c_arg0          r24
#define c_arg1          r22

; r0, r1, X, Z



driver_throw_scanline:
.global driver_throw_scanline
  
		 timer_fine_sync

		 lds XL, _scrp
         lds XH, _scrp+1
         lds r_ram_bitmap_L, __ramp
         lds r_ram_bitmap_H, __ramp+1
         lds r_pgm_bitmap_L, _pgmp
         lds r_pgm_bitmap_H, _pgmp+1

#if DRIVER_BLOCK_HEIGHT > 8
         ldi tmp3, 2048/DRIVER_BLOCK_HEIGHT+1 ;c_arg0/DRIVER_BLOCK_HEIGHT
         mul c_arg0, tmp3
         mov tmp3, r1
#else
         mov tmp3,c_arg0
#endif
         lsr tmp3
         lsr tmp3
         lsr tmp3

         ldi tmp2, DRIVER_COLUMNS
         mul tmp3, tmp2
         add XL, r0
         adc XH, r1

         ldi tmp2, DRIVER_BLOCK_HEIGHT
         mul tmp3, tmp2
         sub c_arg0, r0
         lsl c_arg0
         lsl c_arg0

         clr r1
         add r_pgm_bitmap_L, c_arg0
         adc r_pgm_bitmap_H, r1
         add r_ram_bitmap_L, c_arg0
         adc r_ram_bitmap_H, r1


         ldi tmp3, DRIVER_BLOCK_SIZE
         ld tmp2, X+                         ;1.block
         mul tmp2, tmp3
         movw ZL,r0

         ld tmp1, X+                         ;2.block
         mul tmp1, tmp3
         subi tmp1, DRIVER_PGM_BLOCK_COUNT
         set
         brlo ppgm
         add r0, r_ram_bitmap_L
         adc r1, r_ram_bitmap_H
         rjmp pnext
ppgm:    add r0, r_pgm_bitmap_L
         adc r1, r_pgm_bitmap_H
         clt                                 ;1. in RAM ?

pnext:
#ifdef HSCROLL
         andi c_arg1, 7
         mov tmp1, c_arg1
         lsr tmp1
         add ZL,tmp1                         ;start scroll shift
         ldi tmp1,0
         adc ZH,tmp1

         ldi tmp1, 8
         sub tmp1, c_arg1                    ;tmp1 is the only free
         inc c_arg1
#else
         ldi tmp1, 8
         ldi c_arg1, 8
#endif

         ldi tmp3, DRIVER_COLUMNS-2
         subi tmp2, DRIVER_PGM_BLOCK_COUNT
         brsh ramstrt

pgmstrt: add ZL, r_pgm_bitmap_L              ;PGM entry point.
         adc ZH, r_pgm_bitmap_H
         lsr tmp1
         lpm tmp2,Z+
         brcs sspeven                        ;can't be 0
         nop
         nop
         rjmp sspodd                         ;e.g. 1.pixel
ssploop: wdr
         out VIDEO_PORT, tmp2                ;even

         lpm tmp2,Z+                         ;1 reg for px
sspodd:  dec tmp1
         out VIDEO_PORT, tmp2                ;odd

sspeven: swap tmp2
         brne ssploop
         brts ramloop
         wdr
pgmloop: out VIDEO_PORT,tmp2                 ;8

         movw ZL, r0
         lpm r0, Z+
pstart:  out VIDEO_PORT,r0                   ;1

         ldi tmp1, DRIVER_BLOCK_SIZE
         ld tmp2, X+
         swap r0
         out VIDEO_PORT,r0                   ;2

         mov r0, tmp1
         lpm tmp1, Z+
         out VIDEO_PORT,tmp1                 ;3

         swap tmp1
         clt
         mul tmp2, r0
         out VIDEO_PORT,tmp1                 ;4

         lpm tmp1, Z+
         subi tmp2, DRIVER_PGM_BLOCK_COUNT
         out VIDEO_PORT,tmp1                 ;5

         lpm tmp2, Z+
         swap tmp1
ram2pgm: out VIDEO_PORT,tmp1                 ;6

         brsh pgm2ram
         add r0, r_pgm_bitmap_L
         adc r1, r_pgm_bitmap_H
         movw ZL, r0
         out VIDEO_PORT,tmp2                 ;7

         dec tmp3
         swap tmp2
         brne pgmloop
esploop: subi c_arg1, 2
         out VIDEO_PORT,tmp2                 ;even

         brlo pgmdone
         lpm tmp2,Z+
         out VIDEO_PORT,tmp2                 ;odd

         swap tmp2
         brne esploop
pgmdone: rjmp done
         
ramstrt: add ZL, r_ram_bitmap_L              ;RAM entry point.
         adc ZH, r_ram_bitmap_H
         lsr tmp1
         ld tmp2,Z+
         brcs ssreven
         nop
         rjmp ssrodd
ssrloop: wdr
         out VIDEO_PORT, tmp2                ;2 even

         ld tmp2,Z+
ssrodd:  nop
         dec tmp1
         out VIDEO_PORT, tmp2                ;1 odd

ssreven: swap tmp2
         brne ssrloop
         brtc pgmloop
         wdr
ramloop: out VIDEO_PORT,tmp2                 ;8

         clt
         movw ZL,r0
         ld tmp1, Z+
         out VIDEO_PORT,tmp1                 ;1

         nop
         ldi tmp2, DRIVER_BLOCK_SIZE
         mov r0, tmp2
         swap tmp1
         out VIDEO_PORT,tmp1                 ;2

         ld tmp1, Z+
         ld tmp2, X+
         out VIDEO_PORT,tmp1                 ;3

         swap tmp1
         mul tmp2, r0
         subi tmp2, DRIVER_PGM_BLOCK_COUNT
         out VIDEO_PORT,tmp1                 ;4

         ld tmp1, Z+
         ld tmp2, Z+
         out VIDEO_PORT,tmp1                 ;5

         swap tmp1
         movw ZL, r0
         brlo ram2pgm
         nop
         out VIDEO_PORT,tmp1                 ;6

         nop
         wdr
pgm2ram: add r0, r_ram_bitmap_L
         adc r1, r_ram_bitmap_H
         out VIDEO_PORT,tmp2                 ;7

         dec tmp3
         swap tmp2
         brne ramloop
         movw ZL, r0
esrloop: out VIDEO_PORT,tmp2                 ;even

         subi c_arg1, 2
         brlo ramdone
         ld tmp2, Z+
         out VIDEO_PORT,tmp2                 ;odd

         nop
         swap tmp2
         brne esrloop
ramdone: wdr
done:    out VIDEO_PORT,tmp3                 ;last

         ret

