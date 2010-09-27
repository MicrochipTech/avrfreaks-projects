/******************************************************************************************************************
 * RGB Controller 
 * Plattform: ATTINY2313
 * Date: 07/2010
 * Performed by: Pablo Liberman
 * palmering@hotmail.com
 * Buenos Aires - Argentina
 * 
 ******************************************************************************************************************/

/******************************************************************************************************************
 * File: RGBCont.c
 * Version: 1.0
 * Date: 07/2010 
 * 
 ******************************************************************************************************************/

/* Edit makefile for configurations */

/******************************************************************************************************************
 * Description:
 * The program generates 3 PWM outputs with differents patterns whose can be selected sequentially by pressing 
 * SW1.
 * Patterns are stored in Flash and they can be created and modified easily.
 * SW2 makes sequentially the leds blink at different speeds. Also, if it is pressed for a while, 
 * the microcontroller goes to low power mode. The other button wakes up the system.
 ******************************************************************************************************************/

/******************************************************************************************************************
 * Hardware:
 *                           _________
 *                  RESET  -|1      20|-  VCC
 *                         -|2      19|-
 *                         -|3      18|-
 *             _|_         -|4      17|-
 *     |---SW1-_|_----------|5      16|------/\/\/-R1----> B
 *     |---SW2-   ----------|6      15|------/\/\/-R2----> G
 *     |                   -|7      14|------/\/\/-R3----> R
 *    GND                  -|8      13|-
 *                         -|9      12|-
 *                   GND   -|10     11|-  
 *                          |_________|
 *
 * 
 ******************************************************************************************************************/
 

/******************************************************************************************************************
 * Included files
 ******************************************************************************************************************/


#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define ENA_IRQ sei()
#define DIS_IRQ cli()

/* RGB sequence structure */
typedef struct{
	unsigned char fade_time;		// fade time in 5mS steps (255=end of sequence)
	unsigned char hold_time;		// hold time in 5mS steps (255=end of all sequences)
	unsigned char red_level;		// red level 0-255
	unsigned char green_level;		// green level 0-255
	unsigned char blue_level;		// blue level 0-255
}RGB_SEQ;

/* sequence table

Format: 
Fade time, Hold time, Red level, Green level, Blue level

Fade time = timer period * Fade time 
Hold time = timer period * Hold time

255,  x,  x,  x,  x -> end of sequence
255,255,  x,  x,  x -> end of all sequences

*/
const unsigned char RGB_SEQ_TBL[] = {	
										// SLOW R - G - B
										250,250,255,  0,  0,
										250,250,  0,255,  0,
										250,250,  0,  0,255,
										255,  0,  0,  0,  0,   
										// FAST R - G - B		
										100,0,255,  0,  0,
										100,0,  0,255,  0,
										100,0,  0,  0,255,
										255,  0,  0,  0,  0,   
										// WHITE - BLACK
										250,250,  0,  0,  0,
										250,250, 255, 255, 255,
										255,255
									};
									

#define FADE 0 
#define HOLD 1

RGB_SEQ *rgbseq=(RGB_SEQ*)RGB_SEQ_TBL;	/* current sequence pointer */
RGB_SEQ *seqptr=(RGB_SEQ*)RGB_SEQ_TBL;	/* current loop sequence */

struct {
	unsigned stage:1; 		// 
	unsigned flash:1; 	// 
	unsigned toggle:1; 		// 
	unsigned b4:1; 		// 
	unsigned b3:1; 		// 
	unsigned b2:1; 		// 
	unsigned b1:1; 		// 
	unsigned b0:1; 		// 
} flag;

unsigned char rgb_ctr;			/* fade/hold counter */
unsigned char flash_ctr=0;		/* flash counter */
unsigned char flash_count=0;	/* flash count */
 
unsigned int rval=0;			/* Fade acumulator */
unsigned int gval=0;
unsigned int bval=0;

unsigned char rpwm;
unsigned char gpwm;
unsigned char bpwm;

int redfade;					/* fade increment for red */
int greenfade;					/* fade increment for green */
int bluefade;					/* fade increment for blue */

#define PWM_RED		OCR0A		/* PWM registers */
#define PWM_GREEN	OCR1A
#define PWM_BLUE 	OCR1B

/************************************** KEYBOARD **********************************/
#define HOLD_TIME 		50		/* Hold time 		*/
#define HOLD_DELAY 		10		/* Hold delay 		*/
#define	DEBOUNCE_TIME 	10		/* Debounce time 	*/

#define KEY_MASK		(_BV(PD2) | _BV(PD3))

#define KEY1_PRESSED    (KEY_MASK & ~_BV(PD2))
#define KEY1_HOLD  		(0x80|KEY1_PRESSED)
#define KEY2_PRESSED	(KEY_MASK & ~_BV(PD3))
#define KEY2_HOLD		(0x80|KEY2_PRESSED)

#define KEY_READY		0		/* no key */
#define KEY_PRESS		1 		/* key pressed */
#define KEY_HOLD		2		/* key hold */

#define TRUE 1
#define FALSE 0

unsigned char key_buf = KEY_MASK;					/* key scan code */
unsigned char key_stat = KEY_READY; 				/* key status */

/*
	KEYScan: Key scan
	This routine should be called periodically (every 10mS)
*/
void KEYScan (void)
{
	static unsigned char ena_key = TRUE;				// keyboard enable flag
	static unsigned char new_key = KEY_MASK;			// new keyscan code 
	static unsigned char prev_key = KEY_MASK;			// previous keyscan code 
	static unsigned char count_debounce = DEBOUNCE_TIME;	// key debounce counter 
	static unsigned char count_hold = HOLD_TIME;		// key hold counter 

	if (ena_key == TRUE)
	{
		// key scan enable
		new_key = PIND&KEY_MASK;
		if (new_key != KEY_MASK)
		{
			// some key was pressed
			if (new_key == prev_key)
			{
				// same key
				count_hold--;					
				if (count_hold == 0)
				{
					count_hold = HOLD_DELAY;
					key_buf|=0x80;
					key_stat=KEY_HOLD;
				}
			}
			else
			{
				// another key
				prev_key = new_key;
				key_buf = new_key;
				ena_key = FALSE;
				count_debounce = DEBOUNCE_TIME;
				count_hold = HOLD_TIME;
				key_stat=KEY_PRESS;
			}
		}
		else
		{
			//there is not pressed key
			count_hold = HOLD_TIME;
			prev_key = KEY_MASK;
		}
	}
	else
	{
		// key scan disabled
		count_debounce--;
		if (count_debounce == 0)
		{
			count_debounce = DEBOUNCE_TIME;
			ena_key = TRUE;
		}
	}
}

unsigned char getch (void)
{
	while (key_stat == KEY_READY);
	key_stat = KEY_READY;
	return (key_buf);
}


/* RGB_FadeInit:  
 Calculate the value to be added in each timer interrupt
 from the initial level to the end level in signed 16-bit
 Parameters:
 x2: time in timer periods
 y1: initial level
 y2: end level
 */
int RGB_FadeInit (unsigned char x2, unsigned char y1, unsigned char y2)
{
	int dy;
	dy = (((int)(y2-y1))<<7)/x2;
	return dy;
}

/******************************************************************************************************************
 * Interrupt handlers
 ******************************************************************************************************************/
/* INT0 Interrupt Service Routine (ISR) */
ISR(INT0_vect)
{
 	/* clear pending interrupts */
	EIFR |= _BV (INTF0);
	wdt_enable(WDTO_15MS);		// enable wdt to reset
}

/* Timer 0 overflow Interrupt Service Routine */
ISR(TIMER0_OVF_vect)
{
	unsigned char red_prev,green_prev,blue_prev;
	
	KEYScan();						// keyboard scan

	/* RGB controller core */

	if(flash_count!=0)
	{
		flash_ctr--;
		if(flash_ctr==0)
		{
			flash_ctr=flash_count;
			DDRB ^= (_BV(DDB2) | _BV(DDB3) | _BV(DDB4));
		}
	}
	
	if (flag.stage==FADE)
	{
		if (rgb_ctr==0)
		{
			flag.stage=HOLD;
			rgb_ctr=rgbseq->hold_time;
		}
		else
		{
			rgb_ctr--;
			rval += redfade;
			rpwm = rval>>7;
			PWM_RED = rpwm;
			gval += greenfade;
			gpwm = gval>>7;
			PWM_GREEN = gpwm;
			bval += bluefade;
			bpwm = bval>>7;
			PWM_BLUE = bpwm;
		}
	}
	else
	{
		if (rgb_ctr==0)
		{
			red_prev=rgbseq->red_level;		// save current values
			green_prev=rgbseq->green_level;
			blue_prev=rgbseq->blue_level;
			
			rgbseq++;							// next sequence
			if(rgbseq->fade_time == 255)		// if end of sequence
			{
				rgbseq=seqptr;					// restart sequence loop
			}

			flag.stage=FADE;					// load values to next sequence
			rgb_ctr=rgbseq->fade_time;
			redfade = RGB_FadeInit(rgb_ctr,red_prev,rgbseq->red_level);
			greenfade = RGB_FadeInit(rgb_ctr,green_prev,rgbseq->green_level);
			bluefade = RGB_FadeInit(rgb_ctr,blue_prev,rgbseq->blue_level);
		}
		else
			rgb_ctr--;
	}

}


void SystemInit (void)
{
	/* Set prescaler /1 F_CPU=8Mhz*/
	CLKPR = _BV(CLKPCE);		// enable clock prescale write
	CLKPR = 0x0;
}

void PortsInit (void)
{
	/* turn on internal pull-up resistor for the switch */
	PORTD |= (_BV(PD2)|_BV(PD3));
	/* clear PORTB.2/.3/.4 */
	PORTB &= ~(_BV(PB2) | _BV(PB3) | _BV(PB4));

	/* set PORTB.2/.3/.4 for output */
	DDRB = _BV(DDB2) | _BV(DDB3) | _BV(DDB4);
	DDRD = _BV(DDD5);
}

/* TImerInit: initialize timer in Fast PWM mode and it generates an overflow IRQ
   timer IRQ and PWM freq = IOclk/Psc/256 = 8000000/256/256 = 122Hz => 8.2mSeg
*/
void TimerInit (void)
{	

	/***************************** Timer/Counter 0 ******************************/
	/* Timer/Counter 0 control register A 
		BIT 7:6 COM0A[1:0] : Compare match output A mode (FAST PWM MODE ONLY DESCRIPTED)
			00: normal operation OC0A disconnected
			01: toggle OC0A on compare match (WGM02=1 in TCCR0B)
			10: clear OC0A on compere match, set OC0A at TOP (*)
			11: set OC0A on compere match, clear Oc0A at TOP		
		BIT 5:4 COM0B[1:0] : Compare match output B mode
			idem ant.
		BIT 3:2 RESERVED
		BIT 1:0 WGM0[1:0] : Waveform Generation Mode
			00: Normal
			01: PWM phase correct (*)
			10: CTC
			11: FAST PWM											
	*/
	TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00); 

	/* Timer/Counter 0 control register B 
		BIT 7 FOC0A: Force Output Compare A
		BIT 6 FOC0B: Force Output Compare B
		BIT 5:4 RESERVED
		BIT 3 WGM02
		BIT 2:0 CS0[2:0] Clock Select
			000: No clock
			001: No prescaling
			010: /8
			011: /64
			100: /256 (*)
			101: /1024
			110: External clock source on T0 pin falling edge
			111: External clock source on T0 pin rising edge
	*/
	TCCR0B = _BV(CS02);
	
	/* Timer/Counter Interrupt Mask Register (shared with Timer1)
		BIT 4: RESERVED
		BIT 2: Output Compare Match B Interrupt Enable
		BIT 1: Overflow Interrupt Enable
		BIT 0: Output Compare Match A Interrupt Enable
	*/
//	TIMSK = _BV(TOIE0);
	
	OCR0A = 0;				// initialize duty cycle output 0A 0%
	OCR0B = 0;				// initialize duty cycle output 0B 0%

	/***************************** Timer/Counter 1 ******************************/
	/* Timer/Counter 1 control register A 
		BIT 7:6 COM1A[1:0] : Compare match output A mode (FAST PWM MODE ONLY DESCRIPTED)
			00: normal operation OC1A disconnected
			01: toggle OC1A on compare match (WGM12=1 in TCCR1B)
			10: clear OC1A on compere match, set OC1A at TOP		(*)
			11: set OC1A on compere match, clear OC1A at TOP		
		BIT 5:4 COM1B[1:0] : Compare match output B mode
			idem ant.
		BIT 3:2 RESERVED
		BIT 1:0 WGM1[1:0] : Waveform Generation Mode (@@@see table above)
	*/
	TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10); 

	/* Timer/Counter 1 control register B 
		BIT 7 ICNC1: Input Capture Noice Canceler
		BIT 6 ICES1: Input Capture Edge Select
		BIT 5 RESERVED
		BIT 4:3 WGM1[3:2] : Waveform Generation Mode (@@@see table above)
		BIT 2:0 CS0[2:0] Clock Select
			000: No clock
			001: No prescaling
			010: /8
			011: /64
			100: /256 (*)
			101: /1024
			110: External clock source on T0 pin falling edge
			111: External clock source on T0 pin rising edge
	*/
	TCCR1B = _BV(CS02);

	/* @@@ Table Timer/Counter 1 Waveform Generation Unit bits
		WGM1  3 2 1 0
		MODE
		5	  0 1 0 1	Fast PWM, 8-bit
	*/

	/* Timer/Counter 1 control register B 
		BIT 7 FOC1A: Force Output Compare A
		BIT 6 FOC1B: Force Output Compare B
		BIT 5:0 RESERVED
	*/
	TCCR1C = 0;

	
	/* Timer/Counter Interrupt Mask Register (Timer0 and Timer1)
		BIT 7 TOIE1: T1 Overflow Interrupt Enable (*)
		BIT 6 OCIE1A: T1 Output Compare Match A Interrupt Enable
		BIT 5 OCIE1B: T1 Output Compare Match B Interrupt Enable
		BIT 4 RESERVED
		BIT 2 OCIE0B: T0 Output Compare Match B Interrupt Enable
		BIT 1 TOIE0: T0 Overflow Interrupt Enable 
		BIT 0 OCIE0A: T0 Output Compare Match A Interrupt Enable
	*/
	TIMSK = _BV(TOIE0);
	
	OCR1A = 0;				// initialize duty cycle output 1A 0%
	OCR1B = 0;				// initialize duty cycle output 1B 0% (not used)


}


/******************************************************************************************************************
 * main program
 ******************************************************************************************************************/
void main (void)
{
	unsigned char key;
	DIS_IRQ;

	MCUCR &= ~(_BV(SE));
	/* Clear WDRF in MCUSR */
	MCUSR &= ~_BV(WDRF);
	/* Write logical one to WDCE and WDE */
	/* Keep old prescaler setting to prevent unintentional time-out */
	WDTCSR |= _BV(WDCE) | _BV(WDE);
	/* Turn off WDT */
	WDTCSR = 0x00;
	
	flash_count=0;

	/* Initialize MCU */
	SystemInit();
	/* Initialize i/o ports */
	PortsInit();
	/* Initialize timer */
	TimerInit();
	
	/* Enable Interrupts */
	ENA_IRQ;
	
	/* Main Loop */
	for(;;)
	{
		key=getch();
		switch(key)
		{
			case KEY1_PRESSED:
				/* next sequence pattern */
				do{
					seqptr++;
				}while(seqptr->fade_time!=255);	// search end of sequence
				
				if(seqptr->hold_time==255)			// end of all sequences
					seqptr=(RGB_SEQ*)RGB_SEQ_TBL;	// start from the begining
				else
					seqptr++;
				rgbseq=seqptr;

				flag.stage=FADE;
				rgb_ctr=rgbseq->fade_time;

				redfade = RGB_FadeInit(rgbseq->fade_time,0,rgbseq->red_level);
				greenfade = RGB_FadeInit(rgbseq->fade_time,0,rgbseq->green_level);
				bluefade = RGB_FadeInit(rgbseq->fade_time,0,rgbseq->blue_level);

				rval=0;
				gval=0;
				bval=0;

				break;
			case KEY1_HOLD:
				break;
			case KEY2_PRESSED:
				flash_count+=20;
				if(flash_count==100)
				{
					flash_count=0;
					DDRB = _BV(DDB2) | _BV(DDB3) | _BV(DDB4);
				}
				flash_ctr=flash_count;
				break;
			case KEY2_HOLD:
				/* turn off */
				TIMSK = 0;
				TCCR0B=0;
				TCCR1B=0;
				DDRB = 0;
				DIS_IRQ;
				MCUCR &= ~(_BV(ISC01)|_BV(ISC00));	// low level INT0
				GIMSK = _BV(INT0);				
				/* set power down mode */
				set_sleep_mode(SLEEP_MODE_PWR_DOWN);
				sleep_enable();
				ENA_IRQ;
				sleep_cpu();				
				break;
		}		
	}
}
