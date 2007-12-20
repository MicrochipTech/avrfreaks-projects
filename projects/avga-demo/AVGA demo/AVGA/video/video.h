/* 

	file:	video.h
 	desc:	VIDEO signal generator heder

	note:	Pixel period is 5 clock cycles. For PAL: @20MHz, it gives 208 pixels per active picture period (52 uS).
			One frame has 288 lines. When doubling each line, a very nice 192*144 (4:3) resolution is achieved.
			This resolution is also dividable by 8, so we can use 8*8 sized blocks.
			Desired xtal: 19.6608MHz.

	author: Jaromir Dvorak (md@unicode.cz)

*/

#ifndef _VIDEO_H_
#define _VIDEO_H_


/* ########## BEGIN OF USER CONFIGURABLE SECTION ########## */


//
// CPU clock frequency in MHZ
//
#define CPU_MHZ                 19.6608

//
// Video bitstream will apear at upper 4 bits of this port. lower 4 bits can be used as input.
//
#define VIDEO_PORT              PORTD
#define VIDEO_DDR               DDRD


//
// This assembler macro should precisely synchronise the program flow with video timer (+- one cycle)
// configured here for TIMER1, atmega168.
//
#ifdef __ASM__
.macro timer_fine_sync
         lds  r0, TCNT1L
         sbrs r0, 0
         rjmp tsync
tsync:
.endm
#else //!ASM


//
// Video generator needs one full AVR 16-bit timer/counter with PWM.
// configured here for TIMER1, atmega168.
//
#define VIDEO_TIMER_IRQ_CMP		TIMER1_COMPA_vect
#define VIDEO_TIMER_IRQ_OVF		TIMER1_OVF_vect
#define VIDEO_TIMER_OCRA		OCR1A
#define VIDEO_TIMER_OCRB		OCR1B
#define VIDEO_TIMER_ICR			ICR1
#define	VIDEO_TIMER_OCRB_DDR 	DDRB		//sync DDR
#define VIDEO_TIMER_OCRB_PIN	2			//sync PIN

//
// Setting up the timer: fast PWM, no prescaling, OCRB connected.
//
#define __video_timer_select_IRQ_OVF() 		TIMSK1 = 1 << TOIE1
#define __video_timer_select_IRQ_CMP()		TIMSK1 = 1 << OCIE1A
#define	__video_timer_clear_IRQ_OVF()		TIFR1 = 0xFF
#define __video_timer_clear_IRQ_CMP()		TIFR1 = 0xFF
#define __video_timer_setup_mode14()										\
	TCCR1A = 1 << WGM11 | 0 << WGM10 | 1 << COM1B1 | 1 << COM1B0;			\
	TCCR1B = 1 << WGM13 | 1 << WGM12 | 0 << CS12 | 0 << CS11 | 1 << CS10;

//
// Define this if you want to use window tools.
//
#define VIDEO_USE_WINDOW

//
// Scan each line twice?
//
#define VIDEO_DOUBLE_SCANLINE

//
// Picture to display offset.
//
#define VIDEO_XPOS 116
#define VIDEO_YPOS 288


/* ########### END OF USER CONFIGURABLE SECTION ########### */


/*
 *
 * Video signal timings. 
 *
 *
 * PAL standard:
 * 
 * V-Sync timings
 *  DEP  __________            ___
 *    | |                 DFP |IFP|
 *    |_|    IEP       _______|   |
 * 
 *
 * H-Sync timings
 *                |\/||/\//\|
 *            BP  |    AP   | FP
 * _        ______|         |__
 *  |  SP  |
 *  |______|
 *
 * http://www.rickard.gunee.com/projects/video/sx/vsync_big.png
 *
 */

#define PAL_LP          (int)(64*CPU_MHZ)   // Nominal line period 64 us
#define PAL_DEP         (int)(2.35*CPU_MHZ) // Duration of equalizing pulses 2.35 µs
#define PAL_DFP         (int)(27.3*CPU_MHZ) // Duration of field-synchronizing pulses 27.3 µs
#define PAL_SP          (int)(4.7*CPU_MHZ)  // Synchronizing pulse 4.7 µs
#define PAL_BP          (int)(5.8*CPU_MHZ)  // PAL_BP - Back porch 5.8 us
#define PAL_AP          (int)(52*CPU_MHZ)   // Active picture area 52 us
#define PAL_FP          (int)(1.5*CPU_MHZ)  // Front porch 1.5 us

#define PAL_TIMINGS { {PAL_LP/2, PAL_DFP, 5}, 	  \
                      {PAL_LP/2, PAL_DEP, 5}, 	  \
                      {PAL_LP, PAL_SP, 1},    	  \
                      {PAL_LP, PAL_SP, 304},  	  \
                      {PAL_LP/2, PAL_DEP, 5}, 	  \
                      {PAL_LP/2, PAL_DFP, 5}, 	  \
                      {PAL_LP/2, PAL_DEP, 4}, 	  \
                      {PAL_LP, PAL_DEP, 1},   	  \
                      {PAL_LP, PAL_SP, 304}, 	  \
                      {PAL_LP/2, PAL_SP, 1},  	  \
                      {PAL_LP/2, PAL_DEP, 5}, {0} }


typedef struct 
{ 
	uint16_t period;	// T (t1+t2)
	uint16_t width;		// t1
	uint16_t times;		// repeating
} const sync_t;


#include <avr/pgmspace.h>
#define PAL ({ static sync_t PROGMEM __c[] = PAL_TIMINGS; __c; })
      


/*
 *
 * Function prototypes.
 *
 */

//
// Call for initialize the generator. Desired signal is given as argument.
//
void video_init(sync_t *s);

//
// Waits for retrace. Returns number of cycles divided by 7.
//
inline unsigned int sync(void);

//
// Enable/disable driver.
//
#define video_enable()  asm volatile("sei")
#define video_disable() asm volatile("cli")

//
// Basic effects support (when not using windows)
// note: not tested!
//
#define video_show() 	   ({ extern unsigned char video_scroll; video_scroll |= 0x08;  })
#define video_hide()  	   ({ extern unsigned char video_scroll; video_scroll &= ~0x08; })
#define video_move(n) 	   ({ extern unsigned char video_line; video_line += n; })
#define video_startline(n) ({ extern unsigned char video_line; video_line = n; })
void video_set_colormask(unsigned char mask);
void video_set_scroll(unsigned char x, unsigned char y);


#endif /* !__ASM__ */
#endif /* _VIDEO_H_ */
