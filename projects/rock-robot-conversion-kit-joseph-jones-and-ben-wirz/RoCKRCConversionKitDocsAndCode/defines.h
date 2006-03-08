/* defines.h -- All the "external" defines */

// Store the RAM address of the sensors array and the parameters array
// so the host computer knows what address to look at.  YOU HAVE TO LOOK
// AT THE .MAP FILE TO SET THESE NUMBERS FOR EVERY RELEASE!!!
#define SENSOR_ADDR_H  0x00
#define SENSOR_ADDR_L  0xBA
#define PARAMS_ADDR_H  0x00
#define PARAMS_ADDR_L  0x92

// Behavior IDs
#define STOP_ID		 0	// ID of default stop behavior
#define DANCE_ID	 1	// Dance a canned dance
#define IR_FOLLOW_ID 2	// Use IRs to IR_follow/follow things
#define VL_FOLLOW_ID 3  // Follow/avoid visible light
#define BOSTON_ID	 4	// Perform a random motion at a random time
#define CRUISE_ID	 5	// Move at a constant speed/rotation
#define ESCAPE_ID	 6	// Responde to collisions
#define TRIGGER_ID	 7	// Halt until trigger condition is met
#define JOYSTICK_ID	 8	// Direct control by user
#define WIRE_ID		 9  // Help the user connect motor wires
#define MAX_BEH 	 10 // One more than the highest behavior ID

// Task IDs
#define THEREMIN	 0
#define DANCE		 1
#define WIMP		 2
#define SCHIZOID	 3
#define POUNCE		 4
#define MOTH		 5
#define MOUSE		 6
#define CHICKEN		 7
#define ROACH		 8
#define REMOTE		 9
#define USER1		10
#define USER2		11
#define USER3		12
#define USER4		13
#define DIFDRV		14
#define DRVSTR		15

#define DIFDRV_CONFIG 0xFF
#define DRVSTR_CONFIG 0x00

// Buzzer behaviors, implemented in tones.c buzzer_beh
#define BUZZ_OFF     0
#define BUZZ_PHO_DIF 1
#define BUZZ_PHO_SUM 2
#define BUZZ_BUMP    3
#define BUZZ_IR   	 4
#define BUZZ_TUNE	 5
#define BUZZ_EEPROM	 6
#define BUZZ_DIFDRV	 7
#define BUZZ_DRVSTR	 8
#define BUZZ_BATLOW	 9
// Defined for completeness, don't call Buzzer behavior with ee_ or fl_ dance
#define BUZZ_EEPROM_DANCE 10
#define BUZZ_FLASH_DANCE  11

/* Every behavior must include in its declarations the macro:
   behavior(ID).  Where ID is the unique identifier for that behavior
   Behaviors return one of the following values:
	 CONTROL => Request control of the motors. 
     PUNT => Release control to lower priority behaviors
   CONTROL and PUNT only work inside the context of a behavior.
	 */
   
#define behavior(id) unsigned char behavior_id = id
#define PUNT (beh_ctl[behavior_id] = 0)
#define CONTROL (beh_ctl[behavior_id] = behavior_id)

#define fast 255	
#define slow ((3 * fast) / 4)

#define drive(vl,vr) (_drive(vl,vr,behavior_id))
#define drive_angle(angle,lspd) (_drive_angle(angle,lspd,behavior_id))

// ADC channel assignments
#define PC_LEFT_CH   0	// Left photocell mux channel
#define PC_RIGHT_CH  1	// Right photocell mux chanel   
#define MOT_BAT_CH   2	// Voltage measurement for motor battery
#define LOGIC_BAT_CH 3  // Voltage measurement for logic battery (9v)
#define USER_BTN_CH  4	// User btn is could be analog but is interpreted digitally
#define USER_POT_CH  5  // Input channel of user pot
#define USER_ANA0_CH 6  // User analog channel 1
#define USER_ANA1_CH 7  // User analog channel 2

// Other assignments
#define LEFT_MOTOR 1	   // Index for PWM
#define RIGHT_MOTOR 0	   // Index for PWM
#define DRIVE_MOTOR 1	   // Alternate definition for motors
#define STEER_MOTOR 0

#define USER_BUTTON_BIT 0x10  // User Button mapped to PA4 
#define LEFT_PORT  OCR1BL  // Left motor output port
#define RIGHT_PORT OCR1AL  // Right motor output port
#define LEFT_DIR   0x01	   // Direction bit for left motor
#define RIGHT_DIR  0x02    // Direction bit for right motor
#define BUZZER_BIT BIT(6)  // Buzzer connected to bit 3 of port B
#define IR_RIGHT   0x02	   // Right IR bit in IR_detect variable
#define IR_LEFT    0x01	   // Left IR bit in IR_detect variable

// Macros for manipulating sounds
#define note(a,b) (((7 & a) << 5) | (b & 0x1F))
#define beat(notex) (notex >> 5)
#define pitch(notex) (notex & 0x1F)

// Macros for manipulating path steps
#define step(a,b) (note(a,b))
//#define heading(stepx) (pitch(stepx) << 3)
#define heading(stepx) (stepx << 3)
#define distance(stepx) (beat(stepx))

// Wrap a number around a power of two less one
#define wrap_2(var) var = (var + 1) & 1
#define wrap_4(var) var = (var + 1) & 3
#define wrap_8(var) var = (var + 1) & 7
#define wrap_F(var) var = (var + 1) & 0x0F

// So you don't have to remember...
#define bit_on(port,bit_mask)  (port |=  bit_mask)
#define bit_off(port,bit_mask) (port &= ~bit_mask)
#define bit_tog(port,bit_mask) (port ^=  bit_mask)
// bit_force: port=>Output port, mask=>all bits to affect, value=>affect
#define bit_force(port,mask,value) (port = (port & ~mask) | (value & mask))

#define LOGIC_BAT_THRESH 94		   // At Vbat = 6.5, Vcc ~5.0, 1.85v at bat ADC
// ACD_mot_bat = V_mot_bat / 11 (not 9); 3.5/11 = 0.318
// 0.318/5.0 = x/255 => x = 16 for ~3.5 V
#define MOTOR_BAT_HI_THRESH 20
#define MOTOR_BAT_LO_THRESH 12

#define tog_buzzer bit_tog(PORTD,BUZZER_BIT)
#define MIN_BUZZER_PERIOD 12	   // 12 => 5 kHz
#define BUZZER_DEFAULT 255		   // Don't set freq

#define div_by_2(arg) (arg >> 1)
#define div_by_4(arg) (arg >> 2)
#define div_by_8(arg) (arg >> 3)
#define div_by_16(arg) (arg >> 4)

//#define enable_interrupts asm("SEI")

///////////////////////////////////////////////////////////////
// Parameter location definitions

// SRAM loacation:


/* EEPROM Memeory Map [Assuming 10 behavior, 40 parameters]
     0 -   9 Priority list User Program 1
	10 -  49 Paramerer list User Program 1
	50 -  59 Priority list User Program 2
	60 -  99 Paramerer list User Program 2
   100 - 109 Priority list User Program 3
   110 - 149 Paramerer list User Program 3
   150 - 159 Priority list User Program 4
   160 - 199 Paramerer list User Program 4
   200 - 349 Song [150 notes]
   350 - 499 Dance [150 steps]
   500 - 505 Reserved
   506 	 	 EE_SENSOR_ADDR_H: Hi byte of sensor[] address in RAM
   507		 EE_SENSOR_ADDR_L: Lo byte of sensor[] address in RAM
   508		 EE_PARAMS_ADDR_H: Hi byte of p_arr[] address in RAM
   509		 EE_PARAMS_ADDR_L: Lo byte of p_arr[] address in RAM
   510 	 	 DRVSTR_CONFIG_ADDR: Steering configuration flag
   511 		 TASK_INDEX_ADDR: Index of the last task robot performed
*/

// EEPROM locations
// EEPROM address that the host looks at to find the RAM address of the
//  sensor[] and p_arr[] arrays.
#define EE_SENSOR_ADDR_H 506
#define EE_SENSOR_ADDR_L 507
#define EE_PARAMS_ADDR_H 508
#define EE_PARAMS_ADDR_L 509

// Address of the flag that determines Diff Drive vs Drive/Steer
#define DRVSTR_CONFIG_ADDR 510
// Address of task index (task that was running at power down) in EEPROM
#define TASK_INDEX_ADDR 511

// Behavior priority list: (EE_PRI_OS goes, EE_PRI_OS + MAX_BEH)
#define EE_PRI_OS   0

// Behavior parameter values start here:
// We allow the user only 10 behaviors, more would mess up the EEPROM map
#define EE_PARAM_OS MAX_BEH

// Each task steps over all priority list and the parameter list
#define EE_TASK_OS (MAX_BEH + PARAM_MAX)

// Song/dance in EEPROM starts this many bytes from the front
#define EE_SONG_OS  200

// Dance spec starts here
#define EE_DANCE_OS 350


#define photo_left  (sensor[0]) // [0] Left photo cell
#define photo_right (sensor[1]) // [1] Right photo cell
#define motor_bat   (sensor[2]) // [2] "Voltage" of motor battery
#define logic_bat   (sensor[3]) // [3] "Voltage" of 9V logic battery
#define user_pot    (sensor[4]) // [4] User potentiameter
#define IR_detect   (sensor[5]) // [5] IR values
#define bump        (sensor[6]) // [6] Collision detector
#define user_button (sensor[7])	// [7] User select button


// Access functions for behavior parameters

#define ig_speed				 		        0
#define pg_speed	   	 (p_arr[ig_speed].u)		// [0 - 255]
// [1] Dance
#define                        id_dance_index   1
#define pd_dance_index 	(p_arr[id_dance_index].u)	// [0 - ~5]
#define 					   id_dist_fact     2
#define pd_dist_fact   	(p_arr[id_dist_fact].u)
// [2] IR_follow
#define pi_a 	  	   (p_arr[ 3].s)
#define pi_b 	  	   (p_arr[ 4].s)
#define pi_c 	  	   (p_arr[ 5].s)
#define pi_d 	  	   (p_arr[ 6].s)
#define pi_e 	 	   (p_arr[ 7].s)
#define pi_f		   (p_arr[ 8].s)
#define pi_g 	 	   (p_arr[ 9].s)
#define pi_h 	  	   (p_arr[10].s)
#define pi_i 	  	   (p_arr[11].s)
// [3] VL_follow
#define pv_a 	  	   (p_arr[12].s)
#define pv_b 	  	   (p_arr[13].s)
#define pv_c 	  	   (p_arr[14].s)
#define pv_d 	  	   (p_arr[15].s)
#define pv_e 	  	   (p_arr[16].s)
#define pv_f 	  	   (p_arr[17].s)
#define pv_g 	  	   (p_arr[18].s)
#define pv_h 	  	   (p_arr[19].s)
#define pv_i 	  	   (p_arr[20].s)
// [4] Boston
#define 	 		          ir_time          21
#define pr_time    	   (p_arr[ir_time].u)
#define 	 		          ir_dur   		   22
#define pr_dur 	   	   (p_arr[ir_dur].u)
// [5] Cruise
#define 	 		          ic_dir  		   23
#define pc_dir    	   (p_arr[ic_dir].u)
// [6] Escape
#define 	 		          ie_backup		   24
#define pe_backup  	   (p_arr[ie_backup].u)
#define 	 		          ie_spin		   25
#define pe_spin    	   (p_arr[ie_spin].u)
#define 	 		          ie_halt		   26
#define pe_halt    	   (p_arr[ie_halt].u)
// [7] Trigger
//#define 	  		          it_source		   27
//#define pt_source      (p_arr[it_source].u)
// Trigger is now defunct so reuse its parasm:
#define	   	   		   	  	  iw_config        27
#define	pw_config      (p_arr[iw_config].u)
#define 	  		          it_thresh		   28
#define pt_thresh  	   (p_arr[it_thresh].u)
#define 	  		          it_action		   29
#define pt_action 	   (p_arr[it_action].u)
// [8] Joystick
#define 	   		   		  ij_angle	 	   30
#define pj_angle  	   (p_arr[ij_angle].u)
#define 	   		   		  ij_speed		   31
#define pj_speed  	   (p_arr[ij_speed].u)
#define 	   		   		  ij_active 	   32
#define pj_active 	   (p_arr[ij_active].u)
// Beeper behavior parameters [tones.c]
#define   		               ib_tempo   	   33
#define pb_tempo       	(p_arr[ib_tempo].u)
#define 					   ib_select 	   34
#define pb_select 	   	(p_arr[ib_select].u)
// Index of the parameter updated by the frob knob
#define	 				      ix_frob	 	   35
#define px_frob		   (p_arr[ix_frob].u)
// There is no need save ee params to EEPROM 
// EEwriter parameters [util.c]
#define pee_high_addr  (p_arr[36].u)
#define pee_low_addr   (p_arr[37].u)
#define pee_data 	   (p_arr[38].u)
#define pee_flag 	   (p_arr[39].u)

#define PARAM_MAX 40

#define EEPARAMS 4