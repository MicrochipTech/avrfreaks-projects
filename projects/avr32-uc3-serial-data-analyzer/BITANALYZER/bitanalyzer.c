#include <avr32/io.h>
#if __GNUC__
#  include "intc.h"
#endif
#include "compiler.h"
#include "board.h"
#include "pm.h"
#include "gpio.h"
#include "tc.h"
#include "usart.h"
#include "process.h"
#include "terminal.h"

#define INPUT_CHANNEL 1		// Capture timer channel (NOTE: io mapping is separate)
#define OUTPUT_CHANNEL 0	// PWM timer channel (NOTE: io mapping is separate)

#define GPIO_PIN_EXAMPLE_2  LED3_GPIO	// For external trigger

int int_src; 		// For storing timer interrupt source
int sampling = 1;	// Flag telling if sampling or processing
int ra = 0;			// Used to read the RA value for the input timer/counter instance.
int mode = 1;		// "user edit mode" or decoder output mode
					
volatile avr32_tc_t *tc = &AVR32_TC;			// Address of timer module
volatile avr32_usart_t *usart = &AVR32_USART1;	// Address of usart m0dule
  
#if __GNUC__		// Interrupts: Capture timer RA load and counter overflow
__attribute__((__interrupt__)) void tc_irq( void )
#elif __ICCAVR32__
/* TC Interrupt  */
#pragma handler = AVR32_TC_IRQ_GROUP, 1
__interrupt void tc_irq( void )
#endif
{
	int_src=tc->channel[INPUT_CHANNEL].sr;	// Read status ( clear interrupts)
	ra = tc_read_ra(tc, INPUT_CHANNEL);
	if (int_src&32) {
		if ((pulses < MAX_PULSES)&&(ra > 0)) { 
			if (pulses>-1) {				// First edge only reset counter.
				p[pulses].len = ra;			// Store pulselength
				p[pulses].type = int_src;	// Store counter status
			}
			pulses++;
		}
	} else if (pulses>-1) {
		tc->channel[INPUT_CHANNEL].ccr = AVR32_TC_CLKDIS_MASK; // Stop counter.
		sampling = 0;	// Pass the status to main
	}
}

static void init_tc_input(volatile avr32_tc_t *tc) {
	tc_capture_opt_t capture_opt =	// Options for capture mode.
	{
		.channel  = INPUT_CHANNEL,                // Channel selection. 
		.ldrb     = TC_SEL_NO_EDGE,               // RB loading selection.
		.ldra     = TC_SEL_EACH_EDGE,	          // RA loading selection.
		.cpctrg   = TC_NO_TRIGGER_COMPARE_RC,     // RC compare trigger enable.
		.abetrg   = TC_EXT_TRIG_SEL_TIOA,         // TIOA or TIOB external trigger selection.
		.etrgedg  = TC_SEL_EACH_EDGE,          	  // External trigger edge selection.
		.ldbdis   = FALSE,                        // Counter clock disable with RB loading.
		.ldbstop  = FALSE,                        // Counter clock stopped with RB loading.
		.burst    = TC_BURST_NOT_GATED,           // Burst signal selection.
		.clki     = TC_CLOCK_RISING_EDGE,         // Clock inversion.
		.tcclks   = TC_CLOCK_SOURCE_TC5           // Internal source clock 5  - connected to PBA/32
	};
	tc_init_capture(tc, &capture_opt);	// Initialize the timer/counter capture.
}

static void init_tc_output(volatile avr32_tc_t *tc) {
	tc_waveform_opt_t waveform_opt =	// Options for waveform generation.
	{
		.channel  = OUTPUT_CHANNEL,               // Channel selection.
		.bswtrg   = TC_EVT_EFFECT_NOOP,           // Software trigger effect on TIOB.
		.beevt    = TC_EVT_EFFECT_NOOP,           // External event effect on TIOB.
		.bcpc     = TC_EVT_EFFECT_NOOP,           // RC compare effect on TIOB.
		.bcpb     = TC_EVT_EFFECT_NOOP,           // RB compare effect on TIOB.
		.aswtrg   = TC_EVT_EFFECT_NOOP,           // Software trigger effect on TIOA.
		.aeevt    = TC_EVT_EFFECT_NOOP,           // External event effect on TIOA.
		.acpc     = TC_EVT_EFFECT_SET,            // RC compare effect on TIOA.
		.acpa     = TC_EVT_EFFECT_CLEAR,          // RA compare effect on TIOA.
		.wavsel   = TC_WAVEFORM_SEL_UP_MODE,      // Waveform selection: Up mode without automatic trigger on RC compare.
		.enetrg   = FALSE,                        // External event trigger enable.
		.eevt     = TC_EXT_EVENT_SEL_TIOB_INPUT,  // External event selection.
		.eevtedg  = TC_SEL_NO_EDGE,               // External event edge selection.
		.cpcdis   = FALSE,                        // Counter disable when RC compare.
		.cpcstop  = FALSE,                        // Counter clock stopped with RC compare.
		.burst    = TC_BURST_NOT_GATED,           // Burst signal selection.
		.clki     = TC_CLOCK_RISING_EDGE,         // Clock inversion.
		.tcclks   = TC_CLOCK_SOURCE_TC4           // Internal source clock 4 -  - connected to PBA/16
	};
	tc_init_waveform(tc, &waveform_opt);	// Initialize the timer/counter waveform.
}
  
void start(void) {
 		pulses = -1; 	// Ready to capture new bitstream 
		types = 0;
		sampling = 1;	// No process in main
		Enable_global_interrupt();
		int_src=tc->channel[INPUT_CHANNEL].sr;	//  Clear pending interrupts
		tc_start(tc, INPUT_CHANNEL);	// Start the timers/counters.
		//tc_start(tc, OUTPUT_CHANNEL);// Pulse output not used yet
		if (mode) usart_write_line(usart, "\n\n\nWaiting for input...\n");
}

int main(void) {
	static const gpio_map_t TC_GPIO_MAP = { // Timer pins
		{AVR32_TC_A1_0_PIN, AVR32_TC_A1_0_FUNCTION},	// TIOA1 pulse input on PB25
		{AVR32_TC_A0_0_PIN, AVR32_TC_A0_0_FUNCTION}		// TIOA0 pulse output on PB23 (not used yet)
	};
	
 	static const gpio_map_t USART_GPIO_MAP = { 		// USART pins
		{AVR32_USART1_RXD_0_PIN, AVR32_USART1_RXD_0_FUNCTION},
		{AVR32_USART1_TXD_0_PIN, AVR32_USART1_TXD_0_FUNCTION}
	};
	
	static const usart_options_t USART_OPTIONS = { 	// USART options
		.baudrate     = 57600,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};
	
	static const tc_interrupt_t TC_INTERRUPT = { 	// Interrupt settings
		.etrgs = 0,
		.ldrbs = 0,
		.ldras = 1,	// Pulse edge detect - load RA and reset
		.cpcs  = 0,
		.cpbs  = 0,	
		.cpas  = 0,
		.lovrs = 0,
		.covfs = 1	// Counter overflow - no more pulses
	};  
  
	pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP); // Main clock
  
	gpio_enable_module(TC_GPIO_MAP,	    // Assign timer I/O.
		sizeof(TC_GPIO_MAP) / sizeof(TC_GPIO_MAP[0]));
      
	gpio_enable_module(USART_GPIO_MAP,	// Assign USART I/O
                     sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

	usart_init_rs232(usart, &USART_OPTIONS, FOSC0);	// Initialize USART in RS232 mode.

	Disable_global_interrupt();
	
	#if __GNUC__	// The INTC driver has to be used only for GNU GCC for AVR32.
	INTC_init_interrupts();	// Initialize interrupt vectors.
	INTC_register_interrupt(&tc_irq, AVR32_TC_IRQ1, AVR32_INTC_INT0);
	#endif

	Enable_global_interrupt(); 
	
	init_tc_input(tc);	// Initialize the timers/counters.
	//init_tc_output(tc);	// Pulse output not in use yet
  
	//tc_write_ra(tc, OUTPUT_CHANNEL, 0x2000);	// Set the compare trigger. 
	//Pulse output not used yet
  
	tc_configure_interrupts(tc, INPUT_CHANNEL, &TC_INTERRUPT);
	  
	start();	// Start timers and pulse capture
			
	char cmd;	// User command
	int ed_type =0;	// The pulse type the user is editing
	int temp;
	
	clr_dec();	// Clear all decode data  
	
	while (TRUE) {
		if (!sampling) {	//Wait for samples
			gpio_set_gpio_pin(GPIO_PIN_EXAMPLE_2);
			Disable_global_interrupt();
			if (pulses>0) {
				if (mode) usart_write_line(usart, "Sort.. ");
				sort();
				if (mode) usart_write_line(usart, "Group.. ");
				group();
				if (mode) usart_write_line(usart, "Quantize.. ");
				quantize();
				if (mode) usart_write_line(usart, "Decode.. ");
				decode();
				if (mode) usart_write_line(usart, "Done!\n\n");
			}
			ed_type=0;	
			if (mode) {	// User interface
				print_summary(usart);	// Show result and error messages
				while (TRUE) {
					if (ed_type>9) temp=ed_type+55; else temp=ed_type+48;
					print_menu(usart,temp);	// Menu w/edit type and modulation
					cmd = usart_getchar(usart);		// Wait for user input
					temp=ctoi(cmd);					// For type change 0...9 and A...Z
					if (temp<=types) ed_type=temp;	//Change type
					switch (cmd) {			// Manual decoding
						case 'v': mdecode(ed_type, 11); break;
						case 'l': mdecode(ed_type, 10); break;
						case 't': mdecode(ed_type, 9); break;
						case 's': mdecode(ed_type, 32); break;
						case 'm': mdecode(250,1); break; // Select modulation
						case 'p': print_pulses(usart); break;
						case 'z': getkey(usart,0,dec.zero); decode(); break; // Get search string and replace
						case 'o': getkey(usart,1,dec.one); decode(); break; // Get search string and replace
						case 'g': setgroup(cmd); break;
						case '+': setgroup(cmd); decode(); break; // Move a separator right
						case '-': setgroup(cmd); decode(); break; // Move a separator left
						case 'c': clr_dec(); break;
						case 'u': mdecode(undo_code,0); break;	// Undo change
					}
					if (cmd=='r') {start(); break;}	// Leave edit and  restart
					if (cmd=='q') {mode=0; break;}	// Leave edit and loop decoder
				}
			} else {	// Computer interface (decode - output loop)
				usart_write_line(usart, decode_str);
				usart_write_line(usart,"\n");
				start();
			}
		}
	}
}
