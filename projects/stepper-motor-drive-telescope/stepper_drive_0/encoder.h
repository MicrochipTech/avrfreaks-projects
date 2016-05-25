//
// File: encoder.h
// 

register uint8_t current_divisor asm("r15"); 	// Current Timer2 divisor
register uint16_t RA asm("r24");
register uint16_t DEC asm("r26");					// X-register!
register uint16_t USART_string_pointer asm("r28");	// Y-register!

// Data space	

volatile uint8_t new_data;		// new encoder data
volatile uint8_t prev_data;	    // previous encoder data

volatile uint16_t PEC_ptr;		// current pointer into PEC table
volatile uint8_t ptrzero[2];	// pointer set when switch opens

volatile uint8_t string[14]; 	// ASCII string containing encoder data to be transmitted

volatile uint8_t RA_SLEW_gear;		// zero indicates on upper frequency range (RA)
volatile uint8_t DEC_SLEW_gear;	// zero indicates on upper frequency range (DEC)
volatile uint32_t RA_STEP_count;	// up/down step count (RA)
volatile uint32_t DEC_STEP_count;	// up/down step count (Dec)
volatile uint32_t RA_STEP_max;   	// max no. of steps to be done (RA)
volatile uint32_t DEC_STEP_max;   	// max no. of steps to be done (DEc)
volatile uint8_t RA_accel;    			// -1 = decel, 0 = no accel, 1 = accel
volatile uint8_t DEC_accel;   			// -1 = decel, 0 = no accel, 1 = accel
volatile uint8_t RA_keys; 	  			// RA switch state 
volatile uint8_t prev_RA_keys;  		// previous RA switch state
volatile uint8_t DEC_keys; 	  		// DEC switch state 
volatile uint8_t prev_DEC_keys;		// previous DEC switch state

volatile uint32_t clock;  				// running total of TIMER1 clock pulses, modulo 7217 cycles.
volatile uint32_t tau = 0;				// interval for quarter step of encoder
volatile uint16_t delta_tau;			// interval minus sidereal interval
volatile uint32_t tau_sidereal = 67316;	// sidereal interval between encoder pulses (64us units)

volatile uint8_t drive_flags = 0b00010110;	// drive correction flags (bit0 = encoder, bit1 = RA guiding, bit2 = PEC, bit 4 = Dec guiding)

volatile uint8_t guiding_counter; 		// Guiding count-down (0.5 sec units)
volatile uint8_t guiding_offset;			// Divisor offset for guiding
volatile uint8_t encoder_feedback;		// encoder feedback
volatile uint8_t encoder_mask = 0xFF;	// encoder mask
volatile uint8_t STATUS_byte; 			// slew-just-finished bits 1 & 2 

// Constants

const uint32_t LARGE = 0xFFFFFFFF;
const uint16_t accel_steps = 16*1024;	// No. of accel/decel steps 

