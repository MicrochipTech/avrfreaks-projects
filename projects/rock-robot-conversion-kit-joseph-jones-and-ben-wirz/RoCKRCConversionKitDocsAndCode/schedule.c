// Schedule defines behaviors, runs the behaviors, and performs arbitration

/*
TO DO:
  It is possible to reclaim 16 eeprom locations since we don't need to save
    the 4 parameters that enable the host to write to EEPROM
  Inverted the sense of the user_pot but didn't test 5/1/02	
  Verify each behavior with drive/steer platform
  Task Status:  Homebrew		57 Chevy
    0 Theremin  Works			Works
	1 Dance	  	Works			Works
	2 Wimp	  	Works			Works
	3 Schizoid  Works			Works
	4 Pounce	Works			Works
	5 Moth		Works			Works
	6 Mouse		Works			Works
	7 Chicken	Works			Works poorly, most object make it hang
	8 Roach		Works			Works poorly, dosen't escape walls
	9 Joystick  [Not tested]
   10 User1
   11 User2
   12 User3
   13 User4
   14 DiffSel	Works
   15 SteerSel	Maybe works
	  [Low Motor Bat]  Lockout at 3.5 volts to avoid transistor damage
	  
Byte saving possibilities:
  Convert velocities from ints to signed chars => save a whole bunch
  Convert analog sensor vars to array then alter acquire_analog to just
     step through 8 analog channels.  => Reduce function to ~1/4 current size
  In init_tasks start by setting all matrix values to 0, then only set
     the non-zero value,  => Save about 24 bytes.
  Write LEDs routine in assembly code
	  
*/

#include <io8535.h>
#include <macros.h>
#include <stdlib.h>
#include "defines.h"

//int cycles = 0;				  // Iteration counter
unsigned char led_var; // = 0;	  // Use this var to write to the LEDs
// Use this global to halt motion and sound while the users chooses a task
unsigned char choose_halt = 0; 
unsigned char drive_config;	   	  // Select diff drive vs drive/steer
unsigned char task_index;	 	  // Let eveyrone know the most recent task
extern union { unsigned char u; signed char s;} p_arr[PARAM_MAX]; // from acquire.c

// ******  Motion enabling variables and functions ****** //

int right_vel[MAX_BEH];// = {0}; // Velocity arrays
int left_vel[MAX_BEH]; //  = {0};
unsigned char beh_ctl[MAX_BEH]; // = {0}; // Beh n wants control if beh_ctl[n] != 0
unsigned char beh_pri[MAX_BEH]; // = {0};// Task writes pritority list here

// Functions:
// _drive(l,r,i) Store vels in velocity array for later execution
// move(i)      Make robot move now.  Arg is winning index.
// Debug function:  move(_drive(l,r,i));  Where i < (MAX_BEH - 1)

int _drive(int left, int right, unsigned char index)
{ left_vel[index] = left;   // Signed 16-bit velocity
  right_vel[index] = right;
  beh_ctl[index] = index;  // Behavior index wants control
  return index;
  }

// Angle is offset is such a way that an input angle of 128 => forward motion
// Compute and store L and R velocities from angle and local speed
int _drive_angle(unsigned char angle, unsigned char lspd, unsigned char index)
{ int r_vel = (cosp(0xff&(angle - 96))<<1) - 256;	// Range [-128,128]
  int l_vel = (sinp(0xff&(angle - 96))<<1) - 256;
  int max_vel = abs(max_mag(r_vel, l_vel));	   // Keep velocities high
  r_vel = scale(r_vel,max_vel,lspd);  		   // Scale to local speed
  l_vel = scale(l_vel,max_vel,lspd);
  return _drive(l_vel, r_vel, index);
}

/*
BEHAVIORS

 First all the behaviors are given a chance to run.  Each behavior has
 a unique ID.  If the behavior wants control, it writes that ID to the beh_ctl 
 array, AND it write the velocity it wants to the behavior's slot
 in the velocity array.  Otherwise, the behavior writes 0 to its control slot.
 Arbitration:  
  beh_pri - Ordered list of priorities, 0=>Highest, 1=>2nd highest, etc.
  beh_ctl - If slot contains beh index, beh wants control, else not.
  arbiter - Arbiter checks each slot of beh_pri in turn.  The slot tells
            which behavior has the next highest priority.  That number
            is used as the index for the beh_ctl array.  If the value
            found there is 0, then the arbiter goes to the next behavior.
            If the value is a behavior index, the arbiter uses that index
            to look up the velocity to send to the motors.
 
PARAMETERS

Each Behavior can have any number of adjustable parameter.  A default value 
for each parameter is loaded on startup.  A task may overide the parameter
with a different value.

FROB

Each task selects exactly one parameter for control by the frob knob.

TASKS

A task is composed of an ordered list of behavior indices.  The order
of the list indicates the order of priorites.  The behavior list can have
as many as 8 behaviors.  Any number of behavior parameters can be
altered by the task.  One behavior parameter is linked to the frob
knob.

There are about 40 defined parameters.  
*/

/*
// Command the motors to move, look up velocity of winning behavior (index)
void move(unsigned char index) {
  motor(LEFT_MOTOR, left_vel[index]);
  motor(RIGHT_MOTOR,right_vel[index]);
  }
*/

//#define config 127  // PWM limit will be 2*config
//#define config 0x80 // 
#define s_gain 16 //64 //32 // 8
#define ds_limit 255 // Max PWM for steer motor (could be stored in EEPROM)
  
void move_dual(unsigned char index) {
//  unsigned char config = EEPROMread(DRVSTR_CONFIG_ADDR);
  int mag; 			   // Magnitude of Left/Right velocity
  extern unsigned char motor_bat_OK;
  signed char dir;	   // Direction of drive motor
  int drive, steer;	   // Drive steer magnitudes
  int l_vel = left_vel[index];
  int r_vel = right_vel[index];
  static signed char l_old_s, r_old_s;	// Signum applied to old velocity
  // Eliminate H-bridge shoot-through
  // Always have 1 cycle of 0 vel if vel tries to switch sign
  if (l_old_s + sig(l_vel) == 0) l_vel = 0;
  if (r_old_s + sig(r_vel) == 0) r_vel = 0;
  l_old_s = sig(l_vel);
  r_old_s = sig(r_vel);
  
  if (motor_bat_OK) {  // Transistors may fry without this test
    if (DIFDRV_CONFIG == drive_config) {	  
	// DIFFERENTIAL DRIVE configuration
  	  motor(LEFT_MOTOR, l_vel);
      motor(RIGHT_MOTOR,r_vel);
      }
  	else {
	// DRIVE/STEER configuration
	  // Spins to the left should include a backup component if r_vel comes first!
	  mag = max_mag(l_vel, r_vel);	// Max of L/R velocities (keep sign)
	  dir = sig(mag);  			// Direction of drive motor
	  drive = mag; // * dir;		// Mag is already signed
	  steer = clip((dir * s_gain * (r_vel - l_vel)), -ds_limit, ds_limit);
	  motor(DRIVE_MOTOR, drive);
	  motor(STEER_MOTOR, steer);
	  }
	}
	else {
	  motor(LEFT_MOTOR,0);		// Here we turn off the motors if
	  motor(RIGHT_MOTOR,0); 	//   motor battery voltage is too low.
	  }
  }

// Behavior Definitions //////////////////////////////////////////////////

#define WIRE_WAIT 3000	// Number of mS to wait for each state

void show_wire(void) {
  behavior(WIRE_ID);
  static unsigned char index = 0;
  static unsigned int sw_time = 0;
  extern unsigned int clock;
  signed char l_vel, r_vel;
  unsigned char i_led;
  if (time_out(sw_time, WIRE_WAIT)) {
    sw_time = clock;	// Update clock
    index++;  			// Update index
	if (index > 5) index = 0;
	}
  switch(index) {
	case 0:  l_vel =  0; r_vel =  0; i_led = 0;   break; // Stop
	case 1:  l_vel =  1, r_vel =  1; i_led = 10;  break; // L+R+; D+So
	case 2:  l_vel =  1; r_vel =  0; i_led = 8;   break; // L+Ro; D+Sr
	case 3:  l_vel =  0; r_vel =  1; i_led = 2;   break; // LoR+; D+Sl
	case 4:  l_vel = -1; r_vel =  0; i_led = 4;   break; // L-Ro;
	default: l_vel =  0; r_vel = -1; i_led = 1;   break;// LoR-; 
	}
	drive(l_vel * pg_speed, r_vel * pg_speed);
	leds(i_led);
}
  
// A Behavior to help user connect motors and set configuration in EEPROM 
void wire(void) {
  behavior(WIRE_ID);
  static unsigned char state = 0;
  static unsigned int time;
  extern unsigned int clock;
  extern unsigned char winner;
  static unsigned char config_song;
  extern unsigned char motor_bat_OK;
  extern unsigned char task_index;
  CONTROL;		  // Always demand control of robot
//  debug = pw_config;
  if (choose_halt) 	  // If user pushes the button again
    state = 0;		  //  then we didn't really mean it
  switch(state) {
    // See if either of the wiring behaviors is the winner
    case 0: if (winner == WIRE_ID) { // AND NOT choose-halt (no)
		      state++;
			  time = clock;
			  pb_select = BUZZ_OFF;	 // Turn off the music while deciding
			  drive(0,0); // Maybe we need to make sure motors are off...
			  }
			break;
	// Wait for n seconds to make sure the user wants to change modes
	case 1: if (time_out(time, WIRE_WAIT)) { //(clock - time > WIRE_WAIT) {
		 	  if (task_index == DIFDRV) {  // Differential drive is the winner
			    drive_config = DIFDRV_CONFIG;
				config_song = BUZZ_DIFDRV; //pb_select = BUZZ_DIFDRV;   // Play Differential drive tune
				}
			  else {	  				   // Drive/Steer is the winner
			   	drive_config = DRVSTR_CONFIG;
				config_song = BUZZ_DRVSTR; //pb_select = BUZZ_DRVSTR;
			    }
			  EEPROMwrite(DRVSTR_CONFIG_ADDR,drive_config);	 // Store in EEPROM
			  time = clock;
			  state++;
			  }
			break;
	// Step through the lights and motions
	default: show_wire();
			 if (motor_bat_OK)
			   pb_select = config_song;
			 else
			   pb_select = BUZZ_OFF;
			 break;
	}
  }	 

//unsigned char debug;

// Cruise -- move in an arbitrary arc at arbitrary speed.
// When pc_dir = 128 move forward, <128 arc left, >128 arc right
void cruise(void) {
  behavior(CRUISE_ID);	  // Declare behavior_id  
//  debug = pg_speed;
  drive_angle(pc_dir,pg_speed);
  CONTROL;
  }
  
// Joystick -- User controls robot motion  
void joystick(void) {
  behavior(JOYSTICK_ID);	 // Setup for drive command
  if (pj_active) {	   		   		  // Drive the robot if user wants control
    drive_angle(pj_angle,pj_speed);	  // User controls robot heading and speed
	CONTROL; }
  else
    PUNT;
	}
  
// Trigger -- start or stop other behavior based on a sensed value  

// Trigger parameters:
//   pt_source: 0 CdS, 1 IR, 2 clock, 3 bump
//   pt_thresh: Pos (>128) => rising edge, Neg => falling edge
//   pt_action: 0=> Release on event, 1=> halt on event

/*
#define trigger_time 250  // Wait this many mS before initing trigger values

void trigger(void) {
  behavior(TRIGGER_ID);		// This is a motor-controlling behavior
//  extern unsigned char p_arr[];
  static int source_int;	// Initial value or trigger source
  static char state = 0;	// State of behavior
  int thresh_int;	  		// Internal threshold, adjusted to source format
  extern unsigned int  clock;
  extern unsigned char photo_left;
  extern unsigned char photo_right;
  extern unsigned char IR_detect;
  extern unsigned char bump;
  drive(0,0); 		  		// When active Trigger always wants robot to stop
  switch (state) {
  	// State 0 -- Wait for analog values to be collected
    case 0: if (clock > trigger_time)
	  state++; 
	  CONTROL; 		  // Make robot stop
	  break;
	// State 1 -- Compare source with threshold selected source
	case 1: 
	  switch (pt_source) {
	    // Source: photocells, trigger on incerasing or decreasing light
    	case 0: 
		  thresh_int = (pt_source - 128) << 2; 	  // -512 to +508
		  source_int = photo_left + photo_right;  // Sum of light
		  break;
		// Source: IR, trigger on increasing or decreasing count
		case 1:
		  thresh_int = (pt_source - 128); // Useful range: -3 to +3
		  source_int = IR_detect;		  // Range is 0 to 3
		  break; // 0-2
		// Source: clock, trigger only on increasing time
		case 2: 
		  thresh_int = pt_thresh >> 7;	// Range: 0 to 256*128 = 32768
		  source_int = clock;	 		// Smallest time step is 128 mS
		   break;
		// Source: bump, trigger only on 0 to 1 transition
		default:
		  thresh_int = pt_thresh;	    // Useful range: 0 to 1
		  source_int = bump;	 	    // Range: 0 to 1
		  break;
		}
	  // Check source versus threshold just computed
	  if (((source_int >= abs(thresh_int)) && (thresh_int >= 0)) || // s>|t|,t>1
		  ((source_int <  abs(thresh_int)) && (thresh_int <  0))) //  s<|t|,t<1
			 state++;		// Game over, execute action
	 // Untriggered action
	  if (pt_action)	   // Untriggered action is opposite of triggered action
	    PUNT;
	  else
	    CONTROL;
	  break;
	// Final state -- Stay here forever
	default: 
	  if (pt_action)
	    CONTROL; // Event has occured, let other behaviors control
      else
	    PUNT; // Event has occured, stop the robot
	}
}
*/

// IR_follow -- Control the robot based on info from the IR obstacle detector
void IR_follow(void) {
  behavior(IR_FOLLOW_ID);
  extern unsigned char IR_detect;
  int l_vel, r_vel; // action;
  unsigned char L = (IR_detect & IR_LEFT ) && 1; // 0 or 1
  unsigned char R = (IR_detect & IR_RIGHT) && 1; // 0 or 1
  l_vel =  mat_scale(L,R,  pi_a, pi_b, pi_c); 	 // L/R velocity and action
  r_vel =  mat_scale(L,R,  pi_d, pi_e, pi_f);	 //  are controlled by a general
//  action = mat_scale(L,R,  pi_g, pi_h, pi_i);	 //  linear transformation
  drive(scale(l_vel,255,pg_speed), scale(r_vel,255,pg_speed));
  if (mat_scale(L,R,  pi_g, pi_h, pi_i) > 0) //(action > 0)
    CONTROL;
  else
    PUNT;
  } 
  
// vl_follow -- Follow visible light.  Control the robot based on the 
//   signal from the light sensors.
void vl_follow(void) {
  behavior(VL_FOLLOW_ID);
//  extern unsigned char photo_left, photo_right; // YES action is an int!
  extern unsigned char sensor[];
//  int action;
  signed char sum, dif;
  int v_vel, v_omega, iv;
  iv = (photo_left + photo_right)>>3; 	 // [0,63]
  sum = (char) iv;
  iv = (photo_left - photo_right)>>2; 	 // [-64,63]
  dif = (char) iv;
  v_vel   =  mat_scale(sum, dif,  pv_a, pv_b, pv_c);
  v_vel = scale(v_vel,255,pg_speed);	// Scale v_vel with frob knob
  v_omega = (mat_scale(sum, dif,  pv_d, pv_e, pv_f))>>3;
//  action  =  mat_scale(sum, dif,  pv_g, pv_h, pv_i);
  drive(v_vel + v_omega, v_vel - v_omega);
  
  if (mat_scale(sum, dif,  pv_g, pv_h, pv_i) > 0) //(action > 0) 
    CONTROL;
  else
    PUNT;
  }

// Escape -- A balistic behavior executed when a collision is detected.
// The robot will backup, turn, go forward before releasing control.
// Parameters in units of ~mS * 16 => max time = 256 * 0.016 => 4.096 seconds
// That is each unit of pe_x is 16 mS.

// Use motor current to determine a bump.  Backup and spin if detected.
void escape(void) {
  behavior(ESCAPE_ID);  // Declare behavior_id
//  extern unsigned char bump;
  extern unsigned char sensor[];
  static unsigned char state;
  extern unsigned int  clock;
  static unsigned int  e_time;
  static unsigned int  duration;  // Odd behavior results without the variable???
  switch(state) {
    case 0: if (bump) { // There is now a collision
		 	  e_time = clock;
			  duration = ((unsigned int) pe_backup) << 4;
			  drive(-pg_speed,-pg_speed); 		  // Backup fast
			  state++; }				  // Advance to next state
			else
			  PUNT;	   					  // No bump => give up control
			break;
	case 1: if (time_out(e_time, duration)) {//(clock - e_time > duration) { //(pe_backup<<4)) {  // Backup e_timeout
		 	   e_time = clock;
			   duration = ((unsigned int) pe_spin) << 4;
			   drive(-pg_speed,pg_speed);		  // Spin in place
			   state++; }
			CONTROL;
			break;
//	case 2: if (time_out(e_time, duration)) {//(clock - e_time > duration) {
//		 	   e_time = clock;
//			   duration = ((unsigned int) pe_halt) << 4;
//			   drive(0,0);	// Just stop for a while...
//	   	 	   state++; }
//			CONTROL;
//			break;
	default: if (time_out(e_time, duration)) //(clock - e_time > duration)
			  	state = 0;
			CONTROL;
			break;
	}
}

// Boston -- the boston behavior causes the robot to drive eradically
//   pr_time: Time between events (balanced on 128)
//   pr_dur:  Duration of event (balanced on 128)

/*
// Delta_event time = Random or const, Heading = random, Dur = Random or const
void boston(void) {
  behavior(BOSTON_ID);
  static int angle;	  // Angle we will leave 
  static unsigned char state = 0;
  extern unsigned char p_arr[];
  extern unsigned int  clock;
  static unsigned int  del_time;
  static unsigned int  b_time;
  switch(state) {
    // State 0 - Compute time until random event
    case 0: 
	  b_time = clock;	 // When did we start?
	  if (pr_time > 128) //Parameter > 128 => Compute constant time
		del_time = ((pr_time - 128)<<8); //0 to 32.6 S in steps of 256 mS
	  else	 	   		 //  Compute random time  [0,256] * [0,128] = [0,32768] 
	    del_time = ((rand()>>7) * pr_time);
      PUNT;	   // Don't try to control robot
	  state++; // Go to next state
	  break;
	// State 1 - Wait for event to start
	case 1:
	  if (clock - b_time > del_time) {// Done waiting, time to act
	    angle = rand()>>7;  		  // Pick a random angle, range = [0, 256]
		drive_angle(angle,pg_speed);
	    if (pr_dur > 128)
		  b_time = clock + ((pr_dur - 128)<<8); // Add 0 to 32687 mS to clock
	  	else	 //         [0,256] * [0,128] = [0,32768]
	      b_time = clock + ((rand()>>7) * pr_dur);	 // Random time
	  }
      CONTROL;	   // Try to control robot
	  state++;     // Go to next state
	  break;
	// State 2 - Random heading until timeout
	default:
	  if (clock > b_time)
	    state = 0;
	  break;
	}
}
*/
// rand() is a random number between 0 and 2^15 - 1

// Delta_event time = Random, Heading = random, Dur = Random
void boston(void) {
  behavior(BOSTON_ID);
  static int angle;	  // Angle we will leave 
  static unsigned char state = 0;
  extern unsigned int  clock;
  static unsigned int  del_time;
  static unsigned int  b_time;
  switch(state) {
    // State 0 - Compute time until random event
    case 0: 
	  b_time = clock;	 // Starting time
	  del_time = ((rand()>>10) * (unsigned int) pr_time);  // [0,32] * pr_time
      PUNT;	   // Don't try to control robot
	  state++; // Go to next state
	  break;
	  
	// State 1 - Wait for event to start
	case 1:
	  if (time_out(b_time, del_time)) { //(clock - b_time > del_time) {// Done waiting, time to act
		b_time = clock;				  // New timeout time
		del_time = ((rand()>>7) * (unsigned int) pr_dur); // [0,256] * [0,128] = [0,32768]mS
	    angle = rand()>>7;  		  // Pick a random angle, range = [0, 256]
		drive_angle(angle,pg_speed);
      	CONTROL;	   // Try to control robot
	    state++;       // Go to next state
	  }
	  break;
	  
	// State 2 - Random heading until timeout
	default:
	  if (time_out(b_time, del_time)) //(clock - b_time > del_time)
	    state = 0;
	  break;
	}
}

// Dance -- Make the robot move in a pattern defined by a motion file
//   pd_dance_index: Dance file source, 5=> EEPROM, 6 => Flash
//unsigned char pd_dist_fact = 50; // Distance factor...
// IF  dance_time_unit = 100 => 1 unit = 100 mS, 0.1 sec is shortest 
// stop time => longest rest time is 31 * 0.1 = 3.1 seconds
unsigned char dance_time_unit = 25; 

void dance(void) {
  behavior(DANCE_ID);
  extern unsigned int  clock;
  static unsigned int  time; // = 0;
  static unsigned char lstep;		// Local step
  static unsigned int  duration;
  static unsigned char dstate = 0;
  static unsigned char index = 0;
  CONTROL;		  // Always request control
  switch(dstate) {
    // Interpret next step
    case 0: index++;   // Index = 0 is step count, os start with ++
		    lstep = get_step(pd_dance_index, index);	// Get next step
			time = clock;
		 	if (distance(lstep) == 0)	{	 // Zero dist => stop
			  drive(0,0);  // Stop right here
			  duration = (heading(lstep) * dance_time_unit);
			  dstate++; }
			else {  // Dist was not zero
			  drive_angle(heading(lstep),pg_speed);	// Pick a heading
			  duration = (distance(lstep) * pd_dist_fact);
			  dstate++; }
			break;
    // Wait for step to end
	default: if (time_out(time, duration)) { //(clock - time > duration) {
			   dstate = 0;	   // This step over, get next step
			   if ( index >= get_step(pd_dance_index, 0) )
			     index = 0;
			 }
	}
}

// End behavior definitions///////////////////////////////////////////////

// Task initialization from EEPROM and Flash.
// On startup the robot looks up the selected task from an index stored 
// in EEPROM.  It then initializes the behavior priority list, the frob knob 
// selection, and any non-defaulted parameter values.

// Initialize behavior parameters from data stored in EEPROM by the user
void init_task_eeprom(unsigned char task_OS) {
  extern unsigned char beh_pri[];
  unsigned char idx;
  for (idx = 0; idx < MAX_BEH; idx++) {		// EE_PRI_OS = 0, Pri list comes first
    beh_pri[idx] = EEPROMread(idx + EE_PRI_OS + task_OS); // Get pri list from EEPROM
	}
  for (idx = 0; idx < (PARAM_MAX - EEPARAMS); idx++) {	// EE_PARAM_OS = 10
    p_arr[idx].u = EEPROMread(idx + EE_PARAM_OS + task_OS); // Get params from EEPROM
	}
}

// Initialize parameters from data stored in flash
void init_tasks(unsigned char task_index_local) {
  beh_pri[1] = 0;		 // Undo commands of previously selected behaviors
  beh_pri[2] = 0;
  beh_pri[3] = 0;
  switch(task_index_local) {
   // Theremin
   default: beh_pri[0] = 0;	      // STOP is the only motor controlling behavior
   		   pb_select  = BUZZ_PHO_DIF; // Beeper freq = f(light difference)
   break;

   // Waltz
   case 1: beh_pri[0] = ESCAPE_ID;	   // Try not to get stuck
   		   beh_pri[1] = DANCE_ID;
	//	   beh_pri[2] = 0;
		   pb_select  = BUZZ_TUNE;	   // Tune in flash, Entertainer
		   px_frob    = ig_speed;  	   // User_pot controls speed/size of dance
   break;
   
   // Wimp
   case 2: beh_pri[0] = ESCAPE_ID;
   		   beh_pri[1] = IR_FOLLOW_ID;
//		   beh_pri[2] = 0;
//		   pi_a = 0;    pi_b = -128; pi_c = 64;	   // Spin in place
//		   pi_d = -128; pi_e = 0;    pi_f = 64;
		   pi_a = -64;  pi_b = 0;    pi_c = 0;   // Arc backward
		   pi_d = 0;    pi_e = -64;  pi_f = 0;
		   pi_g = 1;    pi_h = 1;    pi_i = 0;
		   pb_select  = BUZZ_IR;		// IRs make the sound
		   px_frob = ig_speed;			// User pot controls robot speed
   break;
   
   // Schizoid
   case 3: beh_pri[0] = ESCAPE_ID;
   		   beh_pri[1] = BOSTON_ID;		// This helps RoCK get out of trouble
		   beh_pri[2] = IR_FOLLOW_ID;
		   beh_pri[3] = CRUISE_ID;
		   pi_a = -64; pi_b = -64; pi_c = 64;   // From Mouse
	  	   pi_d = 0;   pi_e = 0;   pi_f = 64;
		   pi_g = 1;   pi_h = 1;   pi_i = 0;
		   px_frob = ig_speed; 	   	  // Frob controls speed here too
		   pb_select  = BUZZ_PHO_DIF; // Theremin makes it sould weird too
		   pr_time = 32; //16; //8;
		   pr_dur = 2; //3;	   
		   
   break;
   
   // Pounce
   case 4: beh_pri[0] = ESCAPE_ID;
   		   beh_pri[1] = IR_FOLLOW_ID;
	//	   beh_pri[2] = 0;
		   //pi_a = mp(127); pi_b = mp(64);  pi_c = mp(-64);
		   //pi_d = mp(64);  pi_e = mp(127); pi_f = mp(-64);
		   // This seems backward...but it works correctly(?)
		   pi_a = 64;  pi_b = 127; pi_c = -64;	   
		   pi_d = 127; pi_e = 64;  pi_f = -64;
		   pi_g = 1;   pi_h = 1;   pi_i = 0;
		   pb_select = BUZZ_IR;	    // IRs make the sound
		   px_frob = ig_speed;		// User pot controls robot speed
   break;
   
   // Moth
   case 5: beh_pri[0] = ESCAPE_ID;
   		   beh_pri[1] = VL_FOLLOW_ID;
		   beh_pri[2] = CRUISE_ID;   // Stop if vl_follow not active
		   pv_a = 0;  pv_b =  0; pv_c = 64;
		   pv_d = 0;  pv_e =  2; pv_f = 0;  // 1 not enough, 4 too much
		   pv_g = 1;  pv_h =  0; pv_i = -10; //-12; //Day/Night values at odds
		   pc_dir = 135;   // Arc in place until you see some light
		   px_frob = ig_speed;
		   pb_select  = BUZZ_PHO_SUM;  // More light, more sound
   break;
   
   // Mouse
   case 6: beh_pri[0] = ESCAPE_ID;
   		   beh_pri[1] = IR_FOLLOW_ID;
		   beh_pri[2] = CRUISE_ID;
		   pi_a = -64; pi_b = -64; pi_c = 64;
	  	   pi_d = 0;   pi_e = 0;   pi_f = 64;
		   pi_g = 1;   pi_h = 1;   pi_i = 0;
		   //px_frob = ic_dir;
		   pc_dir = 130; //136 & 144=> Rvel=0	  // Arc right
		   pb_select = BUZZ_IR;

   break;
   
   // Chicken
   case 7: beh_pri[0] = ESCAPE_ID;
   		   beh_pri[1] = IR_FOLLOW_ID;
		   beh_pri[2] = CRUISE_ID;
		   pi_a = 0;    pi_b = -128; pi_c = 64;
		   pi_d = -128; pi_e = 0;    pi_f = 64;
		   pi_g = 1;    pi_h = 1;    pi_i = 0;
		   pb_select = BUZZ_IR;
		   px_frob = ig_speed;		// User pot controls robot speed
   break;
   
   // Roach -- When the lights go on, seek darkenss
   case 8: beh_pri[0] = ESCAPE_ID;
   		   beh_pri[1] = IR_FOLLOW_ID;
		   beh_pri[2] = VL_FOLLOW_ID;
		   
		   pi_a = -64; pi_b = -64; pi_c = 64;  // IR from Mouse
	  	   pi_d = 0;   pi_e = 0;   pi_f = 64;
		   pi_g = 1;   pi_h = 1;   pi_i = 0;
		   
		   pv_a = 0;  pv_b =  0; pv_c = 64;	   // Opposite turn and higher
		   pv_d = 0;  pv_e = -3; pv_f = 0;	   //   gain compared
		   pv_g = 1;  pv_h =  0; pv_i = -6;    //   to moth

		   pb_select = BUZZ_BUMP;	// Beep on Bump
		   px_frob = ig_speed;		// User pot controls robot speed
   break;
   
   // Remote
   case 9: beh_pri[0] = JOYSTICK_ID;
   		   pb_select = BUZZ_OFF;
   //		   beh_pri[1] = 0;
   break;
   
   // Four User programs
   case 10: init_task_eeprom(0);
   break;
   
   case 11: init_task_eeprom(1 * EE_TASK_OS);
   break;
   
   case 12: init_task_eeprom(2 * EE_TASK_OS);
   break;
   
   case 13: init_task_eeprom(3 * EE_TASK_OS);
   break;
   
   // Configure for differential drive
   case 14: beh_pri[0] = WIRE_ID;	
   			drive_config = DIFDRV_CONFIG;  
   			//pw_config = DIFDRV;
			px_frob = ig_speed;		  // Let user control speed, starts high
			// wire() sets pb_select...
   break;
   // Configure for drive/steer
   case 15: beh_pri[0] = WIRE_ID;
   			drive_config = DRVSTR_CONFIG;
   			//pw_config = DRVSTR;
			px_frob = ig_speed;		  // Let user control speed, starts high
   break;
  }
}
#define TASK_COUNT 16				 // Total number of defined tasks


// Choose_task:
// When the user presses the user button choose_task() becomes active.
// By adjusting the user_pot with the button pressed, the user can 
// select a new task.  When the user releases the user button the 
// index of the chosen task is written to EEPROM.  That task then 
// becomes the default startup task.
void choose_task(void) {
  static unsigned char ct_state = 0;
  extern unsigned char sensor[];
    switch(ct_state) {
	  // Button is not pressed
	  case 0: if (user_button)	{ // If button becomes pressed, next state
	  	   	  	 choose_halt = 1; // User is choosing a new task
      	   	  	 ct_state++;
			  }
	  break;
	  // Button is pressed
	  case 1: task_index = user_pot>>4;  // I know there are 16 tasks...
	  	   	  led_var = task_index;		 // Show user the selected task
			  if (user_button == 0) {	 // If the button becomes unpressed
			     init_params_flash();	 // First set all param values
			     init_tasks(task_index); // Then init task, overwrite some params
			  	 choose_halt = 0;		 // OK to run again
				 if (task_index < DIFDRV)	// Wire functions aren't saved
			  	 	EEPROMwrite(TASK_INDEX_ADDR,task_index); // Save for next start
				 ct_state = 0;				   // Back to looking for the button
				 // Restart the processor [Doesn't work???]
				 //CLI();		// Disable interrupts
				 //asm("Call 0");
			  }
	   break;
	  }
}

// Arbitrate -- Arbitrate among all behaviors in the task list to determine 
//  who wins control of the motors

int arbitrate(void) {
  unsigned char pri_index;	// Check each index
  unsigned char beh_index;	// Get the behavior index stored there
  // Step through the behaviors in the beh_pri array
  if (choose_halt)
    return 0;	  // This makes the STOP behavior win
  else {
    for(pri_index = 0; pri_index < MAX_BEH; pri_index++) {
      beh_index = beh_pri[pri_index]; // Get slot # of next hst beh
	  if (beh_ctl[beh_index])	// If this behavior wants control
	    return beh_index;	   	    // ...return it as winner
	}
    return 0;	 // Nobody won
  }
}

// Wakeup time must equal tune play time or it will be chopped off...
// Default tempo is 200, notes of duration 1 => 200 ms each, 5 notes => 1s
#define wake_time 1000

// Wakeup routine.  Execute before starting scheduler, hog the processor,
//   only interrupt routines are active during wakeup.
void wakeup(void) {
  extern unsigned int clock;
  extern unsigned char sensor[];
  //extern unsigned char logic_bat;
  unsigned int time = clock + wake_time;  // Wait in a tight loop this number of miliseconds
  leds(task_index);	 // Show the task we are executing
  logic_bat = 255;						 // No init => wakeup tune is bat low tune.
  while (time > clock) {
     if (DIFDRV_CONFIG == drive_config) 
	 	buzzer_beh(BUZZ_DIFDRV);		 // Wake up to Differential drive tune 
     else
	    buzzer_beh(BUZZ_DRVSTR);		 // Wake up to Drive/Steer tune
  }
  sing(255);		  // Reset sing's intenral state...
}

unsigned char winner = 0;		   	   // ID of the winning behavior  

// Execute behaviors and choose an arbitration winner.
void main(void) {
  // Decelerations
//  extern unsigned char user_button;
//  extern unsigned char photo_left;
//  extern unsigned char photo_right;
//  extern unsigned char moter_bat;
//  extern unsigned char logic_bat;
//  extern unsigned char user_pot;
//  extern unsigned char user_ana_0;
//  extern unsigned char user_ana_1;
//  extern unsigned char IR_detect;
//  extern unsigned char bump;
//  extern unsigned char user_button;
  
  extern unsigned char sensor[];
  extern unsigned char winner;
  extern unsigned char motor_bat_OK;
  
  // Initializations

  init_multi();
  init_params_flash(); // Initialize default parameter values
  init_serial();	   // Connection with host
  drive_config = EEPROMread(DRVSTR_CONFIG_ADDR); //Remember drive configuration
  task_index = EEPROMread(TASK_INDEX_ADDR);
  init_tasks(task_index); // On startup pick the task to run
  SEI();			// Enable interrupts
  // Tell the host where the sensor and parameter arrays are in RAM
  //          EEPROM address    EEPROM data
  EEPROMwrite(EE_SENSOR_ADDR_H, SENSOR_ADDR_H);
  EEPROMwrite(EE_SENSOR_ADDR_L, SENSOR_ADDR_L);
  EEPROMwrite(EE_PARAMS_ADDR_H, PARAMS_ADDR_H);
  EEPROMwrite(EE_PARAMS_ADDR_L, PARAMS_ADDR_L);
  
  wakeup();			// Play wakeup tune and show running behavior
 
  //Schedule behaviors and arbitrate
  while(1) { 
	acquire_sensors(); 			  // Read all analog and digital sensors
	p_arr[px_frob].u = user_pot;	  // Move data to selected parameter
	
	eewriter();	  	// Maybe host wants to write to EEPROM
	choose_task();	// Maybe user wants to pick a different task

	// All robot behaviors:
	buzzer_beh(pb_select);	// The behavior that controls the buzzer
	cruise();		// Move at a constant selected velocity
	joystick();		// Let the user control the robot
//	trigger();		// Start or stop action based on some sensor
	IR_follow();	// IR_follow behavior
	vl_follow();	// Visible Light follow behavior
	escape();		// Escape from bump behavior
	boston();		// Randomly do something
	dance();		// Move to music
	wire();			// Maybe help user to connect the motors
	
	winner = arbitrate();	// Find the highes priority beh that wants control
	
	// LED control
	if (choose_halt)   		// If user is choosing a task
	  leds(led_var);		//   Display the current choice
	else if (winner < WIRE_ID) {
      led(0x01, (IR_detect & IR_LEFT )); // Left IR sees an object
	  led(0x08, (IR_detect & IR_RIGHT)); // Right IR sees an object
	  led(0x02, !motor_bat_OK); // Glow if motor bat too low
	  led(0x04, bump);			// Declared bump
	}
	//move(winner);			// Move the way that behavior wants
	
	move_dual(winner);		// Dif drive or drive steer

//    cycles++;		   	   	      // Count iterations
  }
}