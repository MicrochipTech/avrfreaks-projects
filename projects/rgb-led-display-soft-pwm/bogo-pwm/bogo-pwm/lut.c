/*@****************************** RCS HEADER ********************************/
/*
 * $RCSfile: lut.c,v $
 * $Author: chris $
 * $Date: 2007-11-05 11:36:16 $
 * $Revision: 1.5 $
 *
 */ 
//@********************************* lut.c ***********************************
/*
 * Light Up $THING  --  LED lightshow using RGB LED with software PWM on Atmel ATTiny13
 *
 ******************************************************************************
 * WARNING: modern multi-colour LEDs can be PAINFULLY bright.   
 *
 *	If children will play with this, choose one of the following:
 *
 *	* ensure your LED is not too bright, or
 *	* diffuse LED (eg put it inside a table tennis ball), or
 *	* use higher-value resistor to limit current, or
 *	* lower PWM_OC_MAX to limit duty-cycle (LED brightness) via software
 *******************************************************************************
 *
 * The goal is to create a coloured light that cyles gradually through random 
 * colours, but changes colour instantly at the press of a switch.
 *
 * Why?   My kids had this $2 toy pen that did this with 3 SMD LEDs and some coin-cells.
 *        They broke it almost instantly, losing some critical small parts.
 *        "Daddy, my light-up pen broke!  WAaaaaaaaaaaaaa!!!"
 *        Plus, they then drew on the walls with the instrument-formerly-known-as-the-light-up-pen.
 *
 * Since the ATtiny13 only has one timer with 2 pwm channels, we do the PWM logic in 
 * software, using a regular tick (interrupt) from the hardware timer.
 *
 * If I could get the ATtiny45 easily in Oz (which has 3 PWM channels), I'd probly just use 
 * that instead, cos doing PWM in software turns out to be really quite painful.
 *
 * THEORY OF OPERATION:
 *
 * There are several kinds of events happening at different rates:
 *
 * Lowest level:
 *     3 independent PWM channels generate waveforms that drive an RGB LED.
 *     The 3 PWM duty cycle values define the perceived colour (when diffused or seen at a distance)
 *
 *     (I use 3 distinct channels rather than 1ch with 3 Output-compare values, out of some voodoo idea that
 *     running the channels out-of-phase will give more even current draw)
 *
 * Colour change level:
 *     Every CYCLERATEx PWM cycles, the duty cycle of channel X changes by a small amount.
 *
 * User interface level:
 *     If a button is pressed, the LEDs are reset to all off, and the cycle-rate of one channel 
 *     is changed to a new random value, giving a new display cycle
 *
 *     A second button acts as an "off switch" (actually low-power sleep mode)
 *
 * TODO:
 *
 *	Add a macro to select between sourcing or sinking current for LEDS 
 *	(common-anode LEDs need sink mode, common-cathode or discrete LEDS generally use source mode)
 *
 *	Add an idle-timeout with dim-to-off after 5 minutes (allowing use as a night-light)
 *	(also prevents draining batteries when kids lose interest and wander off)
 *
 *	Design a 'shake to change' switch
 *
 */ 

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#ifndef F_CPU
#error define F_CPU in makefile
#endif

#include <util/delay.h>

/*@**************************** forward decls *******************************/

void pwm_setup(uint8_t ch_id, uint8_t pin, uint8_t cnt, uint8_t oc, uint8_t mut_interval, int8_t mut_delta);

/*@****************************** constants *********************************/
/* 
 *@@ pin assignments
 * 
 */

#ifdef __AVR_ATmega168__
#error mega168 version not implemented yet
#elif defined(__AVR_ATtiny2313__)
#error tiny2313 version not implemented yet
#elif defined(__AVR_ATtiny13__)
/* 
 *@@@ Pin assignments for ATtiny13
 * 
 */

// hello-world LED is PB0
// this is used for testing, and omitted in deployment
// note, if enabling this, disable any button input that shares same pin
#if 0
#define HWL_DDR    DDRB
#define HWL_PORT   PORTB
#define HWL_PIN    PINB
#define HWL_MASK   _BV(PB0)
#endif

// soft-power input is PB0
// We use an asynchronous  pin-change interrupt to wake from sleep 
// (On tiny13, shares pin with HWL, so do not enable if HWL in use)
//
#if !defined(HWL_PIN)
#define PWR_DDR    DDRB
#define PWR_PORT   PORTB
#define PWR_PIN    PINB
#define PWR_MASK   _BV(PB0)
#define PWR_INTSETUP() PCMSK|=PWR_MASK;
#define PWR_INTON()  PCMSK|=PWR_MASK; GIMSK|=_BV(PCIE)
#define PWR_INTOFF() PCMSK&=~PWR_MASK;
#else
#define PWR_INTSETUP() 
#define PWR_INTON()  
#define PWR_INTOFF() 
#endif

// push-button-input is PB1 (INT0)
//
// We use an edge-triggered interrupt to detect button press
//
#define PBI_DDR    DDRB
#define PBI_PORT   PORTB
#define PBI_PIN    PINB
#define PBI_MASK   _BV(PB1)
#ifdef PBI_PIN
#define PBI_INTSETUP() 	MCUCR |= _BV(ISC01)
#define PBI_INTON()  GIMSK|=_BV(INT0)
#define PBI_INTOFF() GIMSK&=~_BV(INT0)
#else
#define PBI_INTSETUP() 	
#define PBI_INTON()  
#define PBI_INTOFF() 
#endif

// soft-PWM output A is PB2, pin 7
// soft-PWM output B is PB3, pin 2
// soft-PWM output C is PB4, pin 3
//
// These pin assignments are made when the SPWM table is initialized
//
#define PWM_DDR  DDRB
#define PWM_PORT PORTB
#define PWM_PIN  PINB

#else
#error unsupported chip
#endif

/*@********************************* data ***********************************/
 /* 
 *@@ register-bound variables
 * 
 */
// Counter limits
//
// pwm_cnt_top = top value for pseudo-timers.  
//
//	Don't necessarily need 8-bit pwm for LEDs, 
//	might get away with, say, 5-bits (32 brightness levels)
// 
//	Lowering this value gives a faster PWM frequency, 
//	at expense of less colour resolution
//
// pwm_oc_max = maximum LED brightness
//
//	If using high-brightness LEDs, they may be TOO bright.
//
//	Either use a higher resistor value, or change PWM_OC_MAX
//	to prevent running LEDs at full-brightness
//
#define PWM_TOP		0xFF
#define PWM_OC_MAX	0xFF
#if (PWM_OC_MAX>PWM_TOP)
#error Invalid PWM_OC_MAX
#endif

// FIXME: register commented out for testing
/*register*/ uint8_t pwm_cnt_top /*asm("r2")*/; 
/*register*/ uint8_t pwm_oc_max /*asm("r3")*/;

/* 
 *@@ RAM variables 
 */
//
// counter values for the pwm pseudo-timers.
//
// Initial version used all register variables, but it turns out
// that leads to a lot of replicated code for each channel that 
// blows the 1k size limit.   Having a table in RAM is slower, but
// allows more compact code, and we have sufficient cycles to spare.
//
// If pushed, we /could/ get away with a single counter and merely use
// three separate OC values, but by running three complete 
// pseudo-timers 'out of phase' (i.e. different initial cnt values), 
// peak current draw may be reduced.
//
// That is, when duty cycle of all channels is less than 1-in-3 
// the 3 LEDs will never be on at same time.
//
// We simulate the AVR hardware timers' "inverted fast PWM" mode.
// (Inverted because I am using a common-anode LED array)
// (FIXME: add a #define to choose normal/inverted mode)
//
// Counter starts at zero and counts to TOP, then goes back to 0
// Overflow event (pseudo-interrupt) occurs when timer passes TOP
// Output pin is set at 0, and cleared when CNT==OC
//
// pwm_cnt  = the PWM counter value, ranges from 0 to pwm_cnt_top
// pwm_oc   = the PWM output-compare value (duty cycle) [0..pwm_oc_max]
// pin_mask = the bit-mask selecting the output pin in the IO port.
//            (ATtinyXX only have one IO port, so port ID is global ATM.)
// 
// mut_interval = mutate OC every N overflows
// mut_delta    = value to add to OC at mutation time
// mut_ctr      = number of overflows since last mutation
//

typedef struct _pwm_ctr_s 
{
	uint8_t pwm_cnt;
	uint8_t pwm_oc;
	uint8_t pin_mask; // bitmask in PWM_PORT for this channel

	// colour cycle mode
	uint8_t mut_interval; // change only every N overflows (0=do not change)
	int8_t mut_delta;     // value to add to OC after each change interval
	uint8_t mut_ctr;      // number of overflows since last change
} pwm_ch_t;

#define CH_RED 0
#define CH_GRN 1
#define CH_BLU 2
#define CH_MAX 3 

pwm_ch_t SPWM[CH_MAX];
	

//@@@ power state
//
// Set to 0 when we go to deep-sleep mode
//
#ifdef PWR_PIN
uint8_t pwr_state = 1;
uint8_t pwr_down = 0;
#endif

//@@@ debounce interval - number of PWM cycles for which input is ignored 
//                        after any button press
//
// At button press, button interrupt is disabled and debounce_ctr set to
// DEBOUNCE_INTERVAL
//
// At Every overflow event, debounce_ctr is decremented.  When it reaches
// zero, the button interrupt is re-enabled
//
#define DEBOUNCE_INTERVAL 32
uint8_t debounce_ctr;

/*@******************************** MACROS **********************************/

// pseudo-halt for testing
#define HANG 	cli(); for(;;) _delay_ms(100);

/*@***************************** subroutines ********************************/
/*@@---------------------- initialization routines -------------------------*/
/* 
 *@@@ ioinit - set up input/output pin configurations
 */
void
ioinit (void)
{
	/* 
	 * Configure the push-buttons pin as input with internal pullup
	 *
	 * INT0 pin will use low-level interrupt
	 * Other pins will use pin-change interrupt
	 *
	 */
#ifdef PWR_PIN
	PWR_DDR &= ~PWR_MASK; // input (default state after reset, actually)
	PWR_PORT |= PWR_MASK; // internal pull-up resistor enabled
	pwr_state = 1;        // we are currently [ON].  Duh.
	PWR_INTSETUP();
	PWR_INTON();
#endif

#ifdef PBI_PIN
	PBI_DDR &= ~PBI_MASK; // input (default state after reset, actually)
	PBI_PORT |= PBI_MASK; // internal pull-up resistor enabled
	PBI_INTSETUP();
	PBI_INTON();
#endif
	
	/* 
	 * Set up the timer 0 with the hardware PWM disabled (which is the
	 * default), in "normal" free running mode.  We will use the overflow
	 * interrupt to drive the software PWM.
	 *
	 * Select clock divisor 1 (8MHz), which with default TOP of 255
	 * gives overflow interrupts at a rate of about 32kHz.
	 *
	 * Tick-interrupts at 32kHz combined with a pwm_cnt_top of 255 gives a 
	 * soft-PWM cycle frequency of around 128Hz, which is quite slow
	 * but has no visible flicker to my (old) eyes.
	 *
	 * If a higher frequency is needed, the hardware timer's TOP can be 
	 * lowered using CTC mode (but make sure the inter-interrupt interval is
	 * long enough for the ISR to run) or pwm_cnt_top can be lowered 
	 * (which will give less PWM resolution).
	 *
	 * Fastest possible soft-pwm could be achieved by running in a tight
	 * delay loop instead of using timer interrupts, but would probably 
	 * eat more battery.
	 *
	 * Note: The rapidly changing hardware timer's TCNT value will make
	 * a nice pseudo-random number when someone presses the colour 
	 * change button.
	 */

	/* Enable timer 0 overflow interrupt. */
	TIMSK0 = _BV(TOIE0);
	/* Start timer 0. */
	TCCR0B = _BV(CS00); // divisor==1
	//TCCR0B = _BV(CS01); // divisor==8, for testing gives fast visible flash
	//TCCR0B = _BV(CS01)|_BV(CS00); // divisor==256, for testing, gives slow flash
}

/* 
 *@@@ hwl_init - hello_world pin setup
 * 
 * Control a simple on-off led used as a debugging aid.
 *
 * Not present in deployed version
 */
#ifdef HWL_PIN
#define HWL_ON() HWL_PORT|=HWL_MASK
#define HWL_OFF() HWL_PORT&=~HWL_MASK
#define HWL_FLIP() HWL_PIN|=HWL_MASK
inline void hwl_init() 
{
	HWL_DDR|=HWL_MASK;
	HWL_ON();
}
#else
#define HWL_ON() 
#define HWL_OFF() 
#define HWL_FLIP()
#define hwl_init()
#endif

/* 
 *@@@ pwminit - set up software pwm counters
 * 
 */
void pwminit(void) 
{
	pwm_cnt_top	= PWM_TOP;    // PWM resolution vs frequency
	pwm_oc_max	= PWM_OC_MAX; // sprag max brightness for safety

	/* 
	 * Set up initial pwm state table
	 *
	 * Start the PWM counters out-of-phase to maybe lower peak current draw:
	 * if all 3 leds are at less than 1/3 brightness, they should never be 
	 * on at the same time!
	 *
	 * Start with all LEDs off (duty cycle 0%)
	 *
	 * For each channel SPWM[i] the LED brightness (pwm_oc) will change
	 * by mut_delta once every mut_interval overflows.   
	 * This produces a shifting colour display, esp when the values are
	 * different for each channel.
	 * 
	 * A button press should modify one or more of the mut_XX values to produce a new
	 * pattern of colour changes.
	 */
	pwm_setup(CH_RED, PB2, 0x00, 0x20, 2, -1);
	pwm_setup(CH_GRN, PB4, 0x55, 0x10, 3, 1);
	pwm_setup(CH_BLU, PB3, 0xAA, 0x30, 1, -1);
}

/*@@---------------------- application subroutines -------------------------*/
/* 
 *@@@ pwm_setup - initialize a pwm table entry
 * 
 */
void pwm_setup(uint8_t ch_id, uint8_t pin, uint8_t cnt, uint8_t oc, uint8_t mut_interval, int8_t mut_delta)
{
	pwm_ch_t *pc = &(SPWM[ch_id]);
	
	pc->pin_mask = _BV(pin);
	pc->pwm_cnt = cnt;
	pc->pwm_oc = oc;
	
	pc->mut_interval = mut_interval;
	pc->mut_delta = mut_delta;
	pc->mut_ctr = 0;
	
	PWM_DDR |= pc->pin_mask;   // configure pin as output
	PWM_PORT |= pc->pin_mask;  // LED off (sink mode) (fixme: macroize use of sink/source mode)
}

/* 
 *@@@ pwm_overflow - handle overflow of a software-pwm channel's counter
 * 
 */
void pwm_overflow(uint8_t ch_id) 
{
	int16_t oc_new;
	int8_t delta_new = 0;
	pwm_ch_t *pc = &(SPWM[ch_id]);
	
	/* 
	 * The counter for SPWM channel 'i' has overflowed.  
	 * Check if it's time to do the 'colour drift' action
	 */
	if (pc->mut_interval == 0)
		return; // this channel's duty-cycle does not drift
	
	pc->mut_ctr++;
	if (pc->mut_ctr < pc->mut_interval) 
		return; // mut_interval has not yet elapsed since last change

	// 
	// It is time to bump the duty cycle for this channel by a small bit
	//
	pc->mut_ctr = 0;
	oc_new = pc->pwm_oc;
	oc_new += pc->mut_delta;
	
	// reverse direction of drift if max or min was hit
	if (oc_new <= 0) {
		oc_new = 1;
		delta_new = 0 - pc->mut_delta;
	}
	else if (oc_new > pwm_oc_max) {
		oc_new = pwm_oc_max;
		delta_new = 0 - pc->mut_delta;
	}

	// save the updated value(s)
	pc->pwm_oc = oc_new & 0xFF;
	if (delta_new != 0)
		pc->mut_delta = delta_new;
}

/* 
 *@@@ colour_jump - make a discontinuous colour change, and alter rate of colour cycling
 *
 * Kids love having a button to frob.   At each button press we change to a new 
 * pattern of colour cycling.
 *
 */
void colour_jump(void)
{
	uint8_t rand, tc, i;
	pwm_ch_t *pc;

	// Use (TCNT0 mod 3) as a random value to determine which LED to mogrify
	// Don't actually care about real division, so use bogo-mod
	rand = 0; //TCNT0;
	tc = rand&0x03; 
	if (tc>=CH_MAX) tc=0;
	
	/*
	 * Turn off all LEDs, and randomly change the drift-rate of ONE of the PWM channels.  
	 *
	 * This should yield a new colour cycle with a different variation.
	 *
	 * This routine is also called at power-down (where it just extinguishes the LEDs)
	 **/
	for (i=0; i<CH_MAX; i++) {
		pc = &(SPWM[i]);

		pc->pwm_oc = 1;	            // Set almost 0% duty cycle (dark)
		PWM_PORT |= pc->pin_mask;   // LED off immediately (pin high, sink mode)

		if (i!=tc) 
			continue; // nothing more to do for this channel

		//
		// This is the channel we will mutate
		//
		// We either halve or double the rate of change of colour of this LED,
		// using the high-bit of TCNT0 as a "coin flip" (random binary digit)
		//
		// If drift rate is already zero, we always set it back to 1 
		// 
		//
		if (pc->mut_delta == 0) 
		{
			pc->mut_delta = 1;
		}
		else {
			if (rand & 0x80) 
				pc->mut_delta /= 2;
			else
				pc->mut_delta *= 2;

			// ensure new rate is within reasonable limits
			// note that 'stopped' (0) is a valid state
			if (pc->mut_delta > 0x10)
				pc->mut_delta = 0x10;
		}
	}
}

/*@********************** Interrupt Service routines ************************/
/* 
 *@@ Hardware timer overflow event 
 *
 * This happens at a rate of (CLK/TOP), so this routine must take no longer than
 * 256 cycles!      (An occasional overrun is OK, but will cause PWM phase to drift).
 *
 * With an 8Mhz clock this is called at rate of about 32kHz.
 *
 * To speed up, lower the hardware timer's TOP (or use CTC mode)
 * To slow down, select a larger (8x) IO clock divisor.  Use CTC mode to obtain
 *    intermediate speeds (say 2, or 4x slower).
 *
 */
ISR(TIM0_OVF_vect)	
{
	uint8_t i;
	uint8_t debounce_check = 0;
	pwm_ch_t *pc;

	/* 
	 * We got a timer overflow interrupt.  Time to bump our software-PWM
	 * counters, and test the output compares.
	 *
	 * We simulate inverting fast PWM mode (common-anode LEDs, in sink mode).  
	 *
	 * OC pin is cleared at BOTTOM (led ON) and set  at OC match (LED off).  
	 *
	 * So PWM value 0 is "always off", PWM value 255 is (almost) always on.
	 */
	for (i=0; i<CH_MAX; i++) 
	{
		pc = &(SPWM[i]);

		// bump the counter and wrap if past top 
		// (wrap is a no-op if top is 0xFF)
		++pc->pwm_cnt;
		if (pc->pwm_cnt > pwm_cnt_top) pc->pwm_cnt = 0;

		// set/clear output pin based on PWM mode rules
		// run overflow hooks if overflow happened.
		if (pc->pwm_cnt == pc->pwm_oc) {
			PWM_PORT |= pc->pin_mask;	// set output pin (LED off) when ctr==OC
		}
		else if (pc->pwm_cnt == 0) {
			PWM_PORT &= ~(pc->pin_mask);	// clear output pin (LED on) when ctr==BOTTOM
			pwm_overflow(i);		// run overflow hook
			if (i==0) debounce_check = 1;	// use CH0 overflow to clock debounce timer
		}
	}

	/* 
	 * Check if it's time to reenable pushbutton interrupts
	 * This is done at channel-0 overflow (purely because that's a convenient regular event)
	 *
	 * We disable interrupts for a while after each button press (for switch debouncing) and set
	 * debounce_ctr.   We decrement debounce_ctr every time channel 0 overflows.
	 *
	 * When debounce_ctr reaches zero, we re-enable interrupts.   
	 *
	 * Default debounce interval gives about 1ms of debounce after each press.
	 */
	if (debounce_check && debounce_ctr && (--debounce_ctr == 0))
	{
		// re-enable interrupts
		PWR_INTON();
		PBI_INTON();
	}
}

/* 
 *@@ INT0 - Change Button press event
 */
#ifdef PBI_PIN
ISR(INT0_vect)
{
	/* 
	 * We got a negative-edge on the INT0 pin, i.e. colour button press
	 *
	 * Do a "colour change" procedure
	 */	
	cli();
	colour_jump();
	for (char j=50;j;j--)
		_delay_ms(10);
	sei();

	/*
	 * Disable further INT0 events for DEBOUNCE_INTERVAL soft-PWM cycles.
	 * (About 1ms with default values)
	 *
	 * This is done to aid debouncing of the pushbutton input.
	 */
	PBI_INTOFF();
	debounce_ctr = DEBOUNCE_INTERVAL;
}
#endif

/* 
 *@@ PCINT0 - Power button (via general-purpose pin change) interrupt
 * 
 * The only pin we ever enable for pin-change interrupt is the power button, 
 * so even though all pin-change interrupts are routed to this single ISR, 
 * we don't actually need to check /which/ pin triggered the interrupt 
 * during this service routine.
 */
#ifdef PWR_PIN
ISR(PCINT0_vect) 
{
	/* 
	 * something changed on a monitored pin.
	 *
	 * In this application, this can only be the 'power' button
	 *
	 * Power button is active-low, we go to 'power-off' (actually deep sleep) 
	 * at the RELEASE event, cos powering off at the PRESS event would power 
	 * us right back on again when RELEASED!
	 *
	 * If power is OFF, we'll take whatever event we get as signal to turn on
	 */
	if (pwr_state == 1) 
	{
		if ((PWR_PIN & PWR_MASK) == 0) 
		{
			// button pressed, ignore (wait for release event)
			pwr_down = 1;
			return;
		}
		if (!pwr_down) {
			// button released after 'power on' press.   Ignore.
			return;
		}
		pwr_down=0;

		/* 
		 * Turn off (deep sleep mode)
		 *
		 * Hope that switch bounce does not power us right back on again!
		 */
		colour_jump(); // ALL LEDs off
		pwr_state = 0;
		//set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		//set_sleep_mode(SLEEP_MODE_ADC);
		// let the main loop put us to bed

		/* 
		 * When we wake up due to another button press, we will get another PCINT interrupt, 
		 * and when we exit from that we will resume here.  
		 *
		 * All the work is done, so we just exit from this routine, skipping else-case
		 */
	}
	else 
	{
		/* 
		 * Turn on (wake from sleep)
		 *
		 * Disable button-interrupts for a period in case of switch-bounce
		 * (About 8ms with default values)
		 */
		pwr_state = 1;
		// leave pwr_down==0, so that release doesn't put us straight back to sleep!
		set_sleep_mode(SLEEP_MODE_IDLE);
		PWR_INTOFF();
		debounce_ctr = 0xFF; // maximum debounce for power events
	}
}
#endif



/*@********************************* main ***********************************/
int
main (void)
{
	/* 
	 * Hello, World
	 */
	hwl_init();

	/* 
	 * set up application IO resources 
	 */
	ioinit ();
	pwminit();
	sei();		// Enable interrupts, go!
	
	/* loop forever, the interrupts are doing the rest */
	for (;;) 
	{
		if (pwr_state == 0) 
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		// sleep until next timer tick (every 256 cycles, so more of a blink, really!)
		sleep_mode(); 

		// fixme: hook up a scope and find out how much time (if any) we actually spend asleep,
		// this will give power-use comparison against polled-mode.
	}

	return (0);
}

//@****************************** end of lut.c *******************************
