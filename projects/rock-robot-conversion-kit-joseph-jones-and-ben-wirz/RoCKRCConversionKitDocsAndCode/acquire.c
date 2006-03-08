/* acquire.c -- Acquire sensor data (interrupt driven) */

#include <io8535.h>
#include <macros.h>
#include <stdlib.h>			  // ABS is here...
#include "defines.h"

//asm(".area memory(abs)");

//#pragma abs_address: PARAM_START_ADDR

// Analog globals -- Maintained by on chip system, readable by host
/*
  unsigned char photo_left   ; //= 0;   // [0] Left photo cell
  unsigned char photo_right  ; //= 0;   // [1] Right photo cell
  unsigned char motor_bat    ; //= 0;   // [2] "Voltage" of motor battery
  unsigned char logic_bat    ; //= 0;   // [3] "Voltage" of 9V logic battery
  unsigned char user_pot     ; //= 0;   // [4] User potentiameter
  unsigned char IR_detect    ; //= 0;   // [5] IR values
  unsigned char bump         ; //= 0;   // [6] Collision detector
  unsigned char user_button  ; //= 0;	// [7] User select button

//  unsigned char user_btn_ana ; //= 0; // [x] User button (analog value) 
//  unsigned char user_ana_0   ; //= 0;	// [x] User analog line 0
//  unsigned char user_ana_1   ; //= 0; // [x] User analog line 1  
*/
unsigned char sensor[10];	   //Space for the sensor values

union {
  unsigned char u;	 // Each parameter in the parameter array can be either
  signed   char s;	 //  a signed or unsigned 8-bit value.
} p_arr[PARAM_MAX];

//union param p_arr[PARAM_MAX];

// Global initilizations for parameters not inited in init_tasks()
//unsigned char p_arr[PARAM_MAX];
/* = {
     0, // ig_speed		    0
	    // [1] Dance
     0, // id_dance_index   1
     0, // id_dist_fact     2
        // [2] IR_follow
     0, // pi_a 	(p_arr[ 3])
     0, // pi_b 	(p_arr[ 4])
     0, // pi_c 	(p_arr[ 5])
     0, // pi_d 	(p_arr[ 6])
     0, // pi_e 	(p_arr[ 7])
     0, // pi_f		(p_arr[ 8])
     0, // pi_g 	(p_arr[ 9])
     0, // pi_h 	(p_arr[10])
     0, // pi_i 	(p_arr[11])
        // [3] VL_follow
     0, // pv_a 	(p_arr[12])
     0, // pv_b 	(p_arr[13])
     0, // pv_c 	(p_arr[14])
     0, // pv_d 	(p_arr[15])
     0, // pv_e 	(p_arr[16])
     0, // pv_f 	(p_arr[17])
     0, // pv_g 	(p_arr[18])
     0, // pv_h 	(p_arr[19])
     0, // pv_i 	(p_arr[20])
        // [4] Boston
     0, // ir_time         21
     0, // ir_dur   	   22
        // [5] Cruise
     0, // ic_dir  		   23  128 => Drive forward
        // [6] Escape
     0, // ie_backup	   24  [16 => 16 * 0.016 = 256 mS]
     0, // ie_spin		   25
     0, // ie_halt		   26  
        // [7] Trigger
     0, // it_source	   27
     0, // it_thresh	   28
     0, // it_action	   29
        // [8] Joystick
     0, // ij_angle	 	   30
     0, // ij_speed		   31
     0, // ij_active 	   32
        // Beeper behavior parameters [tones.c]
     0, // ib_tempo   	   33
     0, // ib_select 	   34
        // EEwriter parameters [util.c]
     0, // pee_high_addr  (p_arr[35])
     0, // pee_low_addr   (p_arr[36])
     0, // pee_data 	  (p_arr[37])
     0, // pee_flag 	  (p_arr[38])
        // Index of the parameter updated by the frob knob
     0 //	ix_frob	 	   39
	 };
*/
   
//#pragma end_abs_address

//unsigned char motor_max = 16;	 // Maximum motor current seen since power on

unsigned int clock = 0;	// Global milisecond clock
unsigned char motor_bat_OK = 1;	  // Is motor battery voltage high enough?

void init_params_flash(void) { // Default initialize parameters from flash
  // Several behaviors use pg_speed
  
  pg_speed = fast;
  // [1] Dance parameters
  pd_dance_index = 6;	 // Dance step stored in flash [see get_step()]
  pd_dist_fact = 100;
  // [2] IR_follow parameters
  //pi_a = 0; pi_b = 0; pi_c = 0;
  //pi_d = 0; pi_e = 0; pi_f = 0;
  //pi_g = 0; pi_h = 0; pi_i = 0;
  // [3] vl_follow parameters
  //pv_a = 0; pv_b = 0; pv_c = 0;
  //pv_d = 0; pv_e = 0; pv_f = 0;
  //pv_g = 0; pv_h = 0; pv_i = 0;
  // [4] Boston parameters
  //pr_time = 8;
  //pr_dur = 3;
  // [5] Cruise
  pc_dir = 128;
  // [6] Escape			  (64 => 1 Second)
  pe_backup = 32; //48;
  pe_spin =   24; //32;
//  pe_halt =   48; //64;
  // [7] Trigger parameters
  // pt_source = 0;
  // pt_thresh = 1;
  // pt_action = 0;
  // [8] Joystick
  // pj_angle = 128;
  // pj_speed = 255;
  // pj_active = 0;
  // Beeper behavior parameters [tones.c]
  pb_tempo = 200;
  //pb_select = 5;
  // EEwriter parameters
  //pee_high_addr = 0;
  //pee_low_addr = 0;
  //pee_data = 0;
  pee_flag = 0;
  // Frob index
  px_frob = 0; // Host must never let frob knob to write to px_frob!
}

// INIT MANY PORTS
void init_multi(void) {
  // Was init_ports()
  DDRA =  0x00;		  	// All inputs
  DDRB =  0x13;			// DirA/B out, MotCur/CurSet in, LEDen out.
  DDRC =  0x0F;			// User LEDs out
  DDRD =  0xF2;			// Ser in/out, IR det in, PWM, beep, IR mod out.
  PORTA = 0x10;			// Activate internal pull up for USER_BUTTON
  PORTB = 0x00;			// No pullups
  PORTC = 0x0F;			// 0-4 hi => LED's off
  PORTD = 0x01;  //0x0D;	// Pullups for RxD and IRs, and stop motors
  
  // Was init_buzzer()
  // Init TIMER_0 for buzzer
  TCCR0 = 0x03;				// Prescaler set for 8 uS ticks

  // Was init_clock_pwm():
  // PWM setup -- Timer 1 is configured to provide dual 8-bit PWM.
  // Store the duty factor (0, 255) in OCR1A and OCR1B.  PWM freq
  // is ~15kHz.
  TCCR1A = 0xA1;	// 8-bit PWM, Clear up-count, Set down-count 
  TCCR1B = 0x01;	// 1 tick = 125 nS, (overflow every 510 clicks)
  bit_on(TIMSK,BIT(TOIE1));  // Enable Timer_1 interrupts

  // Was init_IR_source():
  TCCR2 = 0x19;	// PWM2 off, Toggle OC2, reset on ovf, PCK = 1
  OCR2 = 105;	// 105 * 125 nS = 13.16 uS, 1/2 cycle at 38 kHz
  //OCR2 = 100;	// 40 kHz, detuned to reduce electrical interference(?)
  
  // Was init_adc():
  ADCSR = 0x83;          // Enable ADC, ADC freq = 1 MHz
  ACSR = 0;				 // Turn on the analog comparitor (not the adc)
}
// END MANY PORTS

// OPTIMIZE -- make this a macro...
void IR_emit(int on_off) {	 // Turn the IR emitter on or off
  if (on_off) {
    bit_on(TCCR2,BIT(COM20));	// ..01.... => Toggle PD7
	bit_off(TCCR2,BIT(COM21));
	}
  else {
    bit_off(TCCR2,BIT(COM20));	// ..10.... => Clear PD7
	bit_on(TCCR2,BIT(COM21));
	}
}

int buzzer_period = 255; // Buzzer interrupt resets from this

//  buzzer(arg) turn the buzzer on.  Arg is number of 8 uS ticks between 
//  toggling the output.  E.g. if freq = 1000 Hz, then set 
//  half_periods to 62:  half_periods = ((1/f)/8uS)/2 or 62,500/f
//  Timer0 uses TCNT0 and 8-bit reg => Freq = (245 Hz, 62.5 kHz)
//  If arg = 0, then turn off the buzzer

void buzzer(int half_periods) {
  if (half_periods != BUZZER_DEFAULT)
    buzzer_period = (256 - half_periods); // Set the freq
  if (half_periods < MIN_BUZZER_PERIOD)
    bit_off(TIMSK,BIT(TOIE0));	  // Disable timer0 interrupts
  else
    bit_on(TIMSK,BIT(TOIE0));	  // Ensable timer0 interrupts
} 

/*
#define i_bump_time 250 //500 //250 // This value is important, 150 won't work

// Comp_thresh is a measure of how long it takes to activate the bumper
// The number here is emperical.  Comp_Max is related to how it takes to
// deactivate the bump after a bump ends.
#define COMP_THRESH 67 
#define COMP_MAX (2 * COMP_THRESH)

int bump_comp = 0;	// Integral of bump comparitor 

unsigned char i_bump(void)  // bump_comp to determine collision
{ static unsigned char state = 0;
  static unsigned int  time;
  unsigned char local_bump;
  switch(state) {
    case 0: if (clock - time > i_bump_time)
		 	  state++;
	case 1: if (bump_comp > 0)
		 	  local_bump = 1;
			else
			  local_bump = 0;
			bump_comp = 0;
			state = 0;
	}
  return local_bump;
}

// Bump_Blanking: If robot has recently made a large velocity change,
//                then don't integrate current...
unsigned char bump_blanking(void) {
  return 0;
  }
*/

// Maybe this belongs in schedule...
// Keep track of changes in motor velocity
int vel_damp (unsigned char win) {
  static int l_vel_old;
  static int r_vel_old;
  extern int left_vel[];
  extern int right_vel[];  
  int iv;
  iv = max_mag(l_vel_old - left_vel[win], r_vel_old - right_vel[win]);
  l_vel_old = left_vel[win];  // update vel
  r_vel_old = right_vel[win];
  return abs(iv);			 // Only positive values need apply
  }
  
#define bump_integral_time 250
#define bump_integral_thresh (bump_integral_time/4)
//#define bump_blanking_time 250

// Set Bump: Integrate over current, for an interval
void set_bump(void) {
  static unsigned char state = 0;
  static unsigned char bump_integral;
  static unsigned int short_time;
  static unsigned int long_time;
  static unsigned int damp_time; // Timer for delta v damping...
  static unsigned int damp_duration;
  extern unsigned int clock;
  extern unsigned char winner;
  switch(state) {
  	// Flush integral
    case 0: bump_integral = 0;		 // Flush integral, start interval timing
		 	long_time = clock;
			state++;
			break;
	// (Mostly toggle between state 1 and 2...
	// Since this code only integrates 1/2 the time, the bump_integral_time
	//  has to be 1/2 the value it would otherwise be.
	// Integrate bump
	case 1: if (ACSR & 0x20)		 // If Raw Bump, integrate
		 	   bump_integral = bump_integral + (clock - short_time);
			if (clock - long_time > bump_integral_time)
			   state = 3;
			else {
			   damp_duration = vel_damp(winner);
			   damp_time = clock;
			   state = 2; }
			break;
	// Wait for damping...
	case 2: if (clock - damp_time > damp_duration) {
		 	   short_time = clock;  // Don't integrate bump during damping
			   state = 1; }			// Go back to integrating bump
			break;
	// case 3 = default
	default: state = 0;
			 bump = (bump_integral > bump_integral_thresh);
			 break;
			 }
}

/*
unsigned char i_bump(void)	// Decide if robot is bumping based on current
{ extern unsigned char bump;
  static int comp_counts = 0; // Monitor comparitor, count overcurrent samples
  int sample_hit;
  if (ACSR & 0x20)			  // Is current over threshold?
    sample_hit = 1;
  else
    sample_hit = -1;
  comp_counts = clip(comp_counts + sample_hit, 0, COMP_MAX);
  if (comp_counts > COMP_THRESH)
    return 1;	  			 // A bump has occurred
  else
    return 0;				 // No bump has happened
}

unsigned char i_bump(void)	// Decide if robot is bumping based on current
{ static unsigned char state = 0;
  static unsigned int time = 0;
  extern unsigned char motor_cur;
  extern unsigned char bump;
  extern unsigned int clock;
  switch (state) {
    case 0: bump = 0;  // There is no bump now
	        if (ACSR & 0x20) { // Wait for bump to start
	          time = clock + i_bump_time; // Compute time out
	          state++; }	  // Advance to next state
			break;
    case 1: if (clock > time) // Wait for a bump to time out
		 	  state++;
			else if (!(ACSR & 0x20) )
			  state = 0;	   // False alarm!
			break;
    default: if (!(ACSR & 0x20) )	// Wait until bump goes away
	           state = 0;
			 bump = 1;
			 break;
	}
  return bump;
}
*/
  		   				 // 15 cycles = 1.875 uS
char get_adc(void)        // Get an 8-bit result from the ADC
{ register char adc_low, adc_high; // Assume conversion is complete
  adc_low = ADCL;        // Must read low byte first
  adc_high = ADCH;       // Next rotate bits into place
  asm("lsr %adc_high\n"
      "ror %adc_low\n"
      "lsr %adc_high\n"
      "ror %adc_low");
  return adc_low;        // Return the 8-bit result in adc_low
}

/*
void set_adc(int channel)	// Select an ACD channel to convert
{ ADMUX = channel;
  ADCSR |= 0x40;  		 	// Start a conversion
}
*/

void get_set_adc(unsigned char get_set, unsigned char ch, unsigned char *var) {
  if (get_set)			  // If get_set is 1 we set the mux channel
     //set_adc(ch);
	 { ADMUX = ch;
       ADCSR |= 0x40;  		 	// Start a conversion
      }
  else			 		  // If get_set is 0 we store the value
     *var = get_adc();
}

// Grab all analog values
void acquire_analog(void) {
  static unsigned char channel = 0;	   // Range 0 to 7
  static unsigned char tog = 0;	  		   // 0 => set ADC, 1 => get ADC
	switch(channel) {
	  case PC_LEFT_CH:   get_set_adc(tog, PC_LEFT_CH,   &photo_left);   break;
	  case PC_RIGHT_CH:  get_set_adc(tog, PC_RIGHT_CH,  &photo_right);  break;
      case MOT_BAT_CH: 	 get_set_adc(tog, MOT_BAT_CH,   &motor_bat);    break;
      case LOGIC_BAT_CH: get_set_adc(tog, LOGIC_BAT_CH, &logic_bat);    break;
//      case USER_BTN_CH:  get_set_adc(tog, USER_BTN_CH,  &user_btn_ana); break;
      case USER_POT_CH:  get_set_adc(tog, USER_POT_CH,  &user_pot); 	
	    user_pot = -user_pot;			  // Newest pot is inverted electrically
	  break;
      //case USER_ANA0_CH: get_set_adc(tog, USER_ANA0_CH, &user_ana_0);   break;
      //case USER_ANA1_CH: get_set_adc(tog, USER_ANA1_CH, &user_ana_1);   break;
	}
	if (! tog)  		       // After storing a value
	   wrap_8(channel);   	   //  advance to the next channel
  	 wrap_2(tog);		 	   // Toggle between set and get
}			 

// The Waitrony receiver must have no signal for 67 mS to reset some internal state
#define WAITRONY_WAIT 67

// CAREFUL!! The comparison (clock > local_time) fails at clock overflow
// if clock overflows BEFORE local_time overflows.  That happens if say
// delta-t = 1 and it clock advances by 2 between set and check!!

void acquire_sensors(void) {
  static char emitter = 0;	// Emitter => on = $FF, off = $00
  static char old_d = 0;	// Value of detector last time
  static unsigned int local_time; // = 0;
  static int waitrony_timer = 0;
  static char IR_state = 0;
 // IR_detect = (emit XOR detect) & (old_detect XOR detect);
  //if (clock > local_time) { //
  if (clock != local_time)  { // Update no faster than once/mS
	 IR_detect = ((PIND ^ old_d) >> 2 ) & 0x03;
	 if (time_out(waitrony_timer, WAITRONY_WAIT)) {
	 	waitrony_timer = clock;
		IR_state = ~IR_state;
		}
     if (IR_state)		
     	emitter = ~emitter;  // Toggle emitter state variable 
	 else
	    emitter = 0;		 // During rest time leave emitter off
  	 IR_emit(emitter);    // Set the actual emitter on or off.
	 local_time = clock; //+ IR_WAIT; 
     old_d = PIND; 	   	 // Save current IR reading 
	 acquire_analog();	 // Only update once per mS
	 set_bump();		 // This setqs bump
	 // Compute motor_battey_OK with hysteresis
	 if (motor_bat > MOTOR_BAT_HI_THRESH)
	    motor_bat_OK = 1;
	 else if (motor_bat < MOTOR_BAT_LO_THRESH)
	    motor_bat_OK = 0;
	 // bump = i_bump();	  		  	  // Get and store bump data
  }
  user_button = (~PINA & USER_BUTTON_BIT) && 1;//(~PINA >> 6) & 0x03; // Invert and shift over
}

// Set the pulse width modulation magnitude for one motor
//  Channel = 0 => OCR1B (Left motor) , 1 => OCR1A (Right motor)
extern void PWM_mag(int channel, int value) {
  if (channel)	 		  	  		 // channel != 0 => Right motor
    OCR1BL  = clip(value,0,255);		 // Value should be in range [0, 255]
  else	   	 						 // Channel = 0 => Left motor
    OCR1AL = clip(value,0,255);
}

int dir_mask[] = {LEFT_DIR, RIGHT_DIR};	// Ch 0 => Left mot., 1 => Right mot.

void motor(int channel, int vel) {
  if (vel >= 0) {
     bit_off(PORTB,dir_mask[channel]); 	// Physical mot dir decided here
	 PWM_mag(channel, vel); }
  else {
     bit_on(PORTB,dir_mask[channel]);	// ...and here
	 PWM_mag(channel,(- vel)); }
}

/* Declare an interrupt routine to handle the timers */
#pragma interrupt_handler	tick:9
#pragma interrupt_handler	blip:10

// Called every 125 nS * 510 = 63.7 uS => 15.686 kHz
void tick(void)			 // Interrupt driven TICK
{ static unsigned char hf_clock;	 // High frequency clock
  extern unsigned int  clock;
  hf_clock = 0x0f & (hf_clock + 1);
  if (hf_clock == 0x0f) {
    clock++;		   // Advance millisecond clock by 1
  }
}

void blip(void)	{		   // Interrupt driven buzzer function
   TCNT0 = buzzer_period;  // Start counting up from here
   tog_buzzer;	 		   // Toggle Buzzer bit
}

