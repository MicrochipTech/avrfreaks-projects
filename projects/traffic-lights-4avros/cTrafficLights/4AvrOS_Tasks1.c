//----------------------------------------------------------------------------
//  4AvrOS_Tasks1.c                                                                                     //
//----------------------------------------------------------------------------
// This is a traffic light intersection simulation using the ECROS Technology
// Traffic Lights Board or a protoboard with at least six LEDs and three push
// buttons and an application RS-232 port.
//
// There are several traffic light modes that assume that the N/S direction
// is a major road while the E/W direction is a minor road.
//
//  - Daytime with heavy E/W traffic.  A standard Green/Yellow/Red sequence
//    for both directions.
//
//  - Daytime with light E/W traffic.  The N/S light remains green unless
//    the E/W vehicle sensor (push button) is tripped.
//
//  - Nighttime.  N/S flashes yellow and E/W flashes red.
//
//  - Mixed.  Daytime mode during day, nighttime mode at night.  Controlled
//    by two alarm clock settings.
//
//  - Four-way Stop.  All lights flash red.
//
//  - Pedestrians crossing.  All lights red.  Tripped by pedestrian push
//    button.  Overrides all other modes and can't be switched off at
//    control console.
//
// The system is controlled by a dumb terminal (e.g., PC running Tera Term
// Pro) connected through the RS-232 port.  When not in command mode,
// the terminal receives status messages on the traffic light mode,
// vehicle sensors, and pedestrian push button with time stamps.  The 
// commands are described at any time by the Help command h.  
// The commands available are:
//
//        h = Help
//        c = Enter command mode
//        f = Start four-way stop light mode
//        d = Start daylight (normal) light mode
//        n = Start night flashing light mode
//        m = Start mixed (day/night) light mode
//        H = Set day mode for heavy E/W traffic
//        L = Set day mode for light E/W traffic
//        t = Set system clock time
//        g = Set green light on time interval
//        y = Set yellow light on time interval
//        1 = Set mixed mode day-to-night time (Alarm 1)
//        2 = Set mixed mode night-to-day time (Alarm 2)
//        s = Show system status
//        q = Quit command mode
//
// Hardware:
//   AVRmega168 with 8.000 MHz crystal on STK-500 or similar protoboard.
//
//   RS-232 driver and port connected to PC running Tera Term Pro.  
//
//   ECROS Technology Traffic Lights Board or protoboard.  See
//      http://www.ecrostech.com/General/TrafficLights/index.htm
//   for a description of the ERCOS Technology Traffic Lights board.
//
//   In order to work with the low current source capability on some AVR
//   pins, the push button pullup and current limiting resistors on the
//   ECROS board were changed as follows:  
//       R7, R11, R20, and R21 changed from 3.3K to 10K ohms
//       R12, R13, R22, and R23 changed from 2K to 150 ohms
//
//   Alternately, a protoboard such as the STK-500 can be used for the
//   LED lights and pushbuttons.  In this case, the output to the LEDs may
//   have to be complemented.
//
// AVR pin assignments:
//   PORTD0:1 - USART RX and TX connected to RS-232 driver
//   PORTD4:7 - Connection to ERCOS Technology Traffic Lights
//              project board, as follows (0 = pushed):
//                PD4 - E/W vehicle sensor push button
//                PD5 - N/S vehicle sensor push button
//                PD6 - Pedestrian push button
//   PORTB0:5 - Connection to ERCOS Technology Traffic Lights
//              project board, as follows (1 = on):
//                PB0 - E/W green light driver
//                PB1 - E/W yellow light driver
//                PB2 - E/W red light driver
//                PB3 - N/S green light driver
//                PB4 - N/S yellow light driver
//                PB6 - N/S red light driver
//   PORTB6:7 - Crystal at 8.000 MHz
//
// Notes:
//  This version uses the 4AvrOS cooperative scheduler.  There are ten
//  tasks and five semaphores, as follows:
//
//      Day_Lights_Task.  Operates normal GYR light sequences. Priority 4,
//      starts OFF.
//
//      Night_Lights_Task.  Operates night mode flashing lights. Priority
//      4, starts OFF.
//
//      Mixed_Lights_Task.  Operates switch between day and night light
//      modes using two clock alarm settings. Priority 3, starts OFF.
//
//      Four_Way_Stop_Task.  Operates four-way stop light mode. Priority
//      4, starts ON.  (If power cycles, this is the mode lights go to.)
//
//      Ped_Cross_Task.  Operates pedestrian crossing light mode.  Priority
//      3, starts OFF.
//
//      System_Clock_Task.  Operates system clock (separate from 4AvrOS
//      system tick).  Priority 0, starts ON.  Linked to ISR.
//
//      Send_Status_Msg_Task.  Sends status messages with time stamp when
//      not in command mode.  Priority 7, starts ON.
//
//      Command_Task.  Manages terminal interface for command inputs and
//      command messages.  Priority 5, starts OFF.  Linked to ISR.
//
//      EW_Veh_Sensor_Task.  Debounces E/W vehicle sensor on both down
//      and up stroke, sends status messages on vehicles.  Priority 2,
//      starts OFF.  Linked to ISR and Day_Lights_Task.
//
//      NS_Veh_Sensor_Task.  As above, but for N/S direction, except not
//      linked to Day_Lights_Task.
//
//      Ped_Button_Task.  As above, but for pedestrian push button. Triggers
//      Ped_Cross_Task.
//
//      semaSMAQueue   - Protects shared status message address queue.
//      semaTime       - Protects shared clock time variables.
//      semaTX         - Protects shared USART transmitter.
//      semaAlarmTime  - Protects shared alarm time variables.
//      semaModeSwitch - Controls when a light mode task can be disabled
//                       and is also used by Ped_Cross_Task to suspend
//                       and restore operation of current light task.
//
// Andrew Palm
// 2010.02.12
//----------------------------------------------------------------------------
// 4AvrOS
// Coop Scheduler
//
// by curtvm
//                                                                                                     //
// Copyright (c) 2007,2008, Curt Van Maanen                                                            //
//                                                                                                     //
// Permission to use, copy, modify, and/or distribute this software for any                            //
// purpose with or without fee is hereby granted, provided that the above                              //
// copyright notice and this permission notice appear in all copies.                                   //
//                                                                                                     //
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES                            //
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF                                    //
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR                             //
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES                              //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN                               //
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF                             //
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.                                      //
//                                                                                                     //
// Version 2007.10.06                                                                                  //
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// INCLUDES

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdlib.h>
#include "4AvrOS_Tasks1.h"       // needs to be before 4AvrOS.h
#include "4AvrOS.h"

//----------------------------------------------------------------------------
// DEFINES

// General bit handling
#define clear_bit( reg, bitnum )    ((reg) &= ~(1<<(bitnum)))
#define set_bit( reg, bitnum )  ((reg) |= (1<<(bitnum)))
#define test_bit( reg, bitnum )   ((reg) & (1<<(bitnum)))
#define toggle_bit( reg, bitnum ) ((reg) ^= (1<<(bitnum)))

// Sensor port and bits
#define SENSOR_PIN  PIND

#define EW_VEH_BIT PD4    // EW vehicle sensor
#define NS_VEH_BIT PD5    // NS vehicle sensor
#define PED_PB_BIT PD6    // Pedestrian button

// Light port and bits
#define LIGHT_PORT PORTB
#define LIGHT_DDR  DDRB

#define EW_GRN_BIT PB0
#define EW_YEL_BIT PB1
#define EW_RED_BIT PB2
#define NS_GRN_BIT PB3
#define NS_YEL_BIT PB4
#define NS_RED_BIT PB5

// Normal sequence light state outputs
#define LIGHT_STATE_0 ((1<<EW_GRN_BIT)|(1<<NS_RED_BIT)) // EW = Grn, NS = Red
#define LIGHT_STATE_1 ((1<<EW_YEL_BIT)|(1<<NS_RED_BIT)) // EW = Yel, NS = Red
#define LIGHT_STATE_2 ((1<<EW_RED_BIT)|(1<<NS_GRN_BIT)) // EW = Red, NS = Grn
#define LIGHT_STATE_3 ((1<<EW_RED_BIT)|(1<<NS_YEL_BIT)) // EW = Red, NS = Yel
// Light state for four-way stop and pedestrian crossing
#define LIGHT_STATE_4 ((1<<EW_RED_BIT)|(1<<NS_RED_BIT)) // EW = Red, NS = Red

// Limits for light intervals
#define GRN_IVL_MIN 12    // Must be two digits
#define GRN_IVL_MAX 30    // Must be two digits
#define YEL_IVL_MIN 4     // Must be one digit
#define YEL_IVL_MAX 9     // Must be one digit

#define T1_PRESCALE 256       // Timer1 prescaler value
#define T1_START    65536 - (F_CPU / T1_PRESCALE)  // For 1 second

#define BAUD_RATE 9600UL    // Baud rate for USART

// Status message address (SMA) buffer
#define SMA_QUEUE_SIZE 8  // Circular buffer for status msg addresses
#define mod_sma_queue_size & 0x07  // To avoid conflicts with % op

#define INPUT_BUFFER_SIZE 7   // For command inputs
//----------------------------------------------------------------------------
// GLOBAL VARIABLES

// Display strings for reporting system status
char prompt_msg[] PROGMEM = "\r\n>";
PGM_P prompt_msg_ptr PROGMEM = prompt_msg;
char startup_msg[] PROGMEM = "System activated";
PGM_P startup_msg_ptr PROGMEM = startup_msg;

char light_state0_msg[] PROGMEM = "EW = G, NS = R";
PGM_P light_state0_msg_ptr PROGMEM = light_state0_msg;
char light_state1_msg[] PROGMEM = "EW = Y, NS = R";
PGM_P light_state1_msg_ptr PROGMEM = light_state1_msg;
char light_state2_msg[] PROGMEM = "EW = R, NS = G";
PGM_P light_state2_msg_ptr PROGMEM = light_state2_msg;
char light_state3_msg[] PROGMEM = "EW = R, NS = Y";
PGM_P light_state3_msg_ptr PROGMEM = light_state3_msg;

char ew_veh_det_msg[] PROGMEM = "EW veh det";
PGM_P ew_veh_det_msg_ptr PROGMEM = ew_veh_det_msg;
char ew_veh_pass_msg[] PROGMEM = "EW veh pass";
PGM_P ew_veh_pass_msg_ptr PROGMEM = ew_veh_pass_msg;
char ns_veh_det_msg[] PROGMEM = "NS veh det";
PGM_P ns_veh_det_msg_ptr PROGMEM = ns_veh_det_msg;
char ns_veh_pass_msg[] PROGMEM = "NS veh pass";
PGM_P ns_veh_pass_msg_ptr PROGMEM = ns_veh_pass_msg;

char ped_pb_msg[] PROGMEM = "Ped button push";
PGM_P ped_pb_msg_ptr PROGMEM = ped_pb_msg;
char ped_cross_msg[] PROGMEM = "Pedestrians crossing";
PGM_P ped_cross_msg_ptr PROGMEM = ped_cross_msg;

// Display strings for commands
char enter_cmd_mode_msg[] PROGMEM = "Enter command mode\r\n"
                                    ">Status messages suspended";
PGM_P enter_cmd_mode_msg_ptr PROGMEM = enter_cmd_mode_msg;
char in_cmd_mode_msg[] PROGMEM = "Already in command mode";
PGM_P in_cmd_mode_msg_ptr PROGMEM = in_cmd_mode_msg;
char exit_cmd_mode_msg[] PROGMEM = "Quit command mode";
PGM_P exit_cmd_mode_msg_ptr PROGMEM = exit_cmd_mode_msg;

char unknown_cmd_msg[] PROGMEM = "Unknown command";
PGM_P unknown_cmd_msg_ptr PROGMEM = unknown_cmd_msg;

char start_4way_cmd_msg[] PROGMEM = "Start 4-way stop light mode";
PGM_P start_4way_cmd_msg_ptr PROGMEM = start_4way_cmd_msg;
char start_night_cmd_msg[] PROGMEM = "Start night light mode";
PGM_P start_night_cmd_msg_ptr PROGMEM = start_night_cmd_msg;
char start_day_cmd_msg[] PROGMEM = "Start day light mode";
PGM_P start_day_cmd_msg_ptr PROGMEM = start_day_cmd_msg;
char start_mixed_cmd_msg[] PROGMEM = "Start mixed (day/night) light mode";
PGM_P start_mixed_cmd_msg_ptr PROGMEM = start_mixed_cmd_msg;
char alarms_equal_cmd_msg[] PROGMEM = "Alarm times equal, cannot start";
PGM_P alarms_equal_cmd_msg_ptr PROGMEM = alarms_equal_cmd_msg;

char enter_time_msg[] PROGMEM = "Enter time in 24-hr format HH:MM:SS, hit CR";
PGM_P enter_time_msg_ptr PROGMEM = enter_time_msg;
char time_set_msg[] PROGMEM = "New time set";
PGM_P time_set_msg_ptr PROGMEM = time_set_msg;
char show_time_msg[] PROGMEM = "Current system time is";
PGM_P show_time_msg_ptr PROGMEM = show_time_msg;
char show_mode_msg[] PROGMEM = "Current light mode is";
PGM_P show_mode_msg_ptr PROGMEM = show_mode_msg;
char check_mode_msg[] PROGMEM = "Check light mode with s command";
PGM_P check_mode_msg_ptr PROGMEM = check_mode_msg;
char show_alarm1_msg[] PROGMEM = "Mixed mode day-to-night time is";
PGM_P show_alarm1_msg_ptr PROGMEM = show_alarm1_msg;
char show_alarm2_msg[] PROGMEM = "Mixed mode night-to-day time is";
PGM_P show_alarm2_msg_ptr PROGMEM = show_alarm2_msg;

char show_grn_time_msg[] PROGMEM = "Green light interval is";
PGM_P show_grn_time_msg_ptr PROGMEM = show_grn_time_msg;
char show_yel_time_msg[] PROGMEM = "Yellow light interval is";
PGM_P show_yel_time_msg_ptr PROGMEM = show_yel_time_msg;

char enter_alarm1_msg[] PROGMEM = "Enter mixed mode day-to-night time\r\n"
                                  ">in 24-hr format HH:MM, hit CR";
PGM_P enter_alarm1_msg_ptr PROGMEM = enter_alarm1_msg;
char enter_alarm2_msg[] PROGMEM = "Enter mixed mode night-to-day time\r\n"
                                  ">in 24-hr format HH:MM, hit CR";
PGM_P enter_alarm2_msg_ptr PROGMEM = enter_alarm2_msg;

char enter_grn_time_msg[] PROGMEM = "Enter green light time, 12 to 30 sec, hit CR";
PGM_P enter_grn_time_msg_ptr PROGMEM = enter_grn_time_msg;
char enter_yel_time_msg[] PROGMEM = "Enter yellow light time, 4 to 9 sec, hit CR";
PGM_P enter_yel_time_msg_ptr PROGMEM = enter_yel_time_msg;

char invalid_entry_msg[] PROGMEM = "Invalid entry, command cancelled";
PGM_P invalid_entry_msg_ptr PROGMEM = invalid_entry_msg;

char day_mode_H_cmd_msg[] PROGMEM = "Day mode set for heavy E/W traffic";
PGM_P day_mode_H_cmd_msg_ptr PROGMEM = day_mode_H_cmd_msg;
char day_mode_L_cmd_msg[] PROGMEM = "Day mode set for light E/W traffic";
PGM_P day_mode_L_cmd_msg_ptr PROGMEM = day_mode_L_cmd_msg;
char show_day_mode_msg[] PROGMEM = "E/W traffic day mode is";
PGM_P show_day_mode_msg_ptr PROGMEM = show_day_mode_msg;

// Help messages
char help_msg1[] PROGMEM = " h = Help\r\n"
                           "> c = Enter command mode\r\n"
                           "> f = Start four-way stop light mode\r\n"
                           "> d = Start daylight (normal) light mode\r\n"
                           "> n = Start night flashing light mode";
char help_msg2[] PROGMEM = " m = Start mixed (day/night) light mode\r\n"
                           "> H = Set day mode for heavy E/W traffic\r\n"
                           "> L = Set day mode for light E/W traffic\r\n"
                           "> t = Set system clock time\r\n"
                           "> g = Set green light on time interval";
char help_msg3[] PROGMEM = " y = Set yellow light on time interval\r\n"
                           "> 1 = Set mixed mode day-to-night time\r\n"
                           "> 2 = Set mixed mode night-to-day time\r\n"
                           "> s = Show system status\r\n"
                           "> q = Quit command mode";
PGM_P help_msg1_ptr PROGMEM = help_msg1;
PGM_P help_msg2_ptr PROGMEM = help_msg2;
PGM_P help_msg3_ptr PROGMEM = help_msg3;

// Global ram variables
static uint16_t prompt_msg_addr;   // Prog mem addr of standard prompt msg
static uint16_t sma_queue[SMA_QUEUE_SIZE];  // Status msg addr queue
static uint8_t  sma_get_loc, sma_put_loc;   // SMA queue indices

static uint8_t hours, minutes, seconds;     // Time stamp clock values
static uint8_t new_hrs, new_mins, new_secs; // For setting new times
static uint8_t alarm1_hrs, alarm1_mins;     // Alarm 1 time
static uint8_t alarm2_hrs, alarm2_mins;     // Alarm 2 time
static uint8_t check_hrs, check_mins;       // For alarm time check

static uint8_t grn_ivl, yel_ivl;            // Light cycle times

volatile static uint8_t ew_veh_state, ns_veh_state;  // Vehicle sensor states
volatile static uint8_t ped_pb_state;         // Pedestrian pushbutton state

static uint8_t cmd_mode_flag;               // Command mode flag
volatile static char c_rx;                  // Character received from USART

static char light_mode0, light_mode1;       // Light mode status indicator

static char day_mode;                       // Type of daytime operation
static uint8_t ew_det_flag;                 // EW traffic detection flag

//----------------------------------------------------------------------------
// FUNCTIONS
//----------------------------------------------------------------------------
static uint8_t put_sma_queue(uint16_t entry) // Put entry in queue
{
  if (((sma_put_loc + 1) mod_sma_queue_size) == sma_get_loc)
    return 0;  // queue full
  else
  {
    sma_queue[sma_put_loc] = entry;
    sma_put_loc = (sma_put_loc + 1) mod_sma_queue_size;
    return 1;
  }
}

//----------------------------------------------------------------------------
static uint16_t get_sma_queue(void)  // Retrieve queue entry
{
  uint16_t temp;
  if (sma_get_loc == sma_put_loc)
    return 0;  // queue empty
  else
  {
    temp = sma_queue[sma_get_loc];
    sma_get_loc = (sma_get_loc + 1) mod_sma_queue_size;
    return temp;
  }
}

//----------------------------------------------------------------------------
static uint8_t num_elt_sma_queue(void)  // Get number of elements in queue
{
  if (sma_put_loc >= sma_get_loc)
    return sma_put_loc - sma_get_loc;
  else
    return SMA_QUEUE_SIZE - (sma_get_loc - sma_put_loc);
}

//----------------------------------------------------------------------------
static void send_prompt(void)     // Send prompt to terminal
{
  uint8_t i;
  char c;

  for(i=0;; i++)
  {
    c = pgm_read_byte(prompt_msg_addr+i) ;
    if (c!=0) 
    {
      while(!(test_bit(UCSR0A, UDRE0)));
      UDR0 = c;
    }
    else break;
  }
}

//----------------------------------------------------------------------------
static void send_cmd_msg(uint16_t msg_addr)  // Send command mode message
{                                            // from program memory
  uint8_t i;
  char c;

  for(i=0;; i++)                             // Send msg to USART
  {
    c = pgm_read_byte(msg_addr+i) ;
    if (c!=0) 
    {
      while(!(test_bit(UCSR0A, UDRE0)));
      UDR0 = c;
    }
    else break;
  }
  send_prompt();
}

//----------------------------------------------------------------------------
static uint8_t not_digit(char c)   // Test that character is digit
{
  uint8_t n;
  n = c - '0';
  if((n < 0)||(n > 9))
    return 1;
  else
    return 0;
}
//----------------------------------------------------------------------------
static uint8_t system_time_OK(char input_buffer[])    // Convert entered time
{                                                     // in format HH:MM:SS
  char c;

  c = input_buffer[0];
  if(not_digit(c))
    return 0;
  new_hrs = c - '0';
  c = input_buffer[1];
  if(not_digit(c))
    return 0;
  new_hrs = (10 * new_hrs) + (c - '0');

  c = input_buffer[3];
  if(not_digit(c))
    return 0;
  new_mins = c - '0';
  c = input_buffer[4];
  if(not_digit(c))
    return 0;
  new_mins = (10 * new_mins) + (c - '0');

  c = input_buffer[6];
  if(not_digit(c))
    return 0;
  new_secs = c - '0';
  c = input_buffer[7];
  if(not_digit(c))
    return 0;
  new_secs = (10 * new_secs) + (c - '0');

  return 1;   // Entered time is OK
}
//----------------------------------------------------------------------------
static uint8_t light_time_OK(char input_buffer[], uint8_t num_digits, \
                             uint8_t color_state) 
// Check light time interval input, one or two digits
{
  static char c;
  static uint8_t i;

  new_secs = 0;

  if(num_digits > (INPUT_BUFFER_SIZE+1))  // Check for num_digits error
    return 0;
  
  for(i=0; i<num_digits; i++)
  {
    c = input_buffer[i];
    if(not_digit(c))
      return 0;
    new_secs = (10 * new_secs) + (c - '0');
  }

// Check limits of entered values for green and yellow lights
  if(color_state == 0)    // For green light
    if((new_secs < GRN_IVL_MIN)||(new_secs > GRN_IVL_MAX))
      return 0;

  if(color_state == 1)    // For yellow light
    if((new_secs < YEL_IVL_MIN)||(new_secs > YEL_IVL_MAX))
      return 0;

  return 1;   // Entered time is OK
}
//----------------------------------------------------------------------------
static uint8_t alarm_time_OK(char input_buffer[])   // Convert entered time
{                                                   // in format HH:MM
  char c;

  c = input_buffer[0];
  if(not_digit(c))
    return 0;
  new_hrs = c - '0';
  c = input_buffer[1];
  if(not_digit(c))
    return 0;
  new_hrs = (10 * new_hrs) + (c - '0');

  c = input_buffer[3];
  if(not_digit(c))
    return 0;
  new_mins = c - '0';
  c = input_buffer[4];
  if(not_digit(c))
    return 0;
  new_mins = (10 * new_mins) + (c - '0');

  return 1;   // Entered time is OK
}
//----------------------------------------------------------------------------
//  Initialize ports, registers, etc. called from main()
//
void init_mystuff(void)
{
  LIGHT_DDR |= ((1<<EW_GRN_BIT)|(1<<EW_YEL_BIT)     // Traffic light
               |(1<<EW_RED_BIT)|(1<<NS_GRN_BIT)     // LED outputs
               |(1<<NS_YEL_BIT)|(1<<NS_RED_BIT));

  LIGHT_PORT = LIGHT_STATE_0;     // Complement for protoboard

// Set up USART for sending and receiving messages
  UCSR0A = 0x00;
  UCSR0B = (1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0); // Enable RX, TX, RX Int
  UBRR0L = (F_CPU / (16UL * BAUD_RATE)) - 1;  // Load baud rate value

// Set up pin change interrupts for sensor inputs
  PCICR |= (1<<PCIE2);
  PCMSK2 |= ((1<<PCINT20)|(1<<PCINT21)|(1<<PCINT22));

// Initialize normal light cycle times
  grn_ivl = 12;     // Green on 12 seconds
  yel_ivl = 3;      // Yellow on 4 seconds (task adds 1 sec)

// Initialize status message address queue indices
  sma_get_loc = 0;
  sma_put_loc = 0;

// Set up for sending status messages
  prompt_msg_addr = pgm_read_word(&prompt_msg_ptr);
  put_sma_queue(pgm_read_word(&startup_msg_ptr)); // Start up msg

// Initialize light mode indicator (depends on initial task status)
  light_mode0 = 'F';    // Start up in 4-way stop mode since
  light_mode1 = ' ';    // Four_Way_Stop_Task is initially ON

// Send first prompt
  send_prompt();

// Initialize vehicle and pedestrian sensor states for debounce
// state machines
  ew_veh_state = 0;
  ns_veh_state = 0;
  ped_pb_state = 0;

// Initialize command mode flag for command processing state machine
  cmd_mode_flag = 0;

// Set up initial system clock time
  hours = 12;
  minutes = 0;
  seconds = 0;

// Set up initial alarm times for mixed mode
  alarm1_hrs = 21;    // Start night mode
  alarm1_mins = 0;
  alarm2_hrs = 6;     // Start day mode
  alarm2_mins = 0;

// Set up for daytime mode of operation
  day_mode = 'H';   // Set for heavy E/W traffic
  ew_det_flag = 0;  // Flag that E/W traffic not present

// Set up Timer1 as system clock, interrupt every 1 second
  TCCR1B = (1<<CS12)|(0<<CS11)|(0<<CS10);   // 256x prescaler
  TCNT1 = T1_START;                         // Load starting value
  TIMSK1 = (1<<TOIE1);                      // Enable overflow interrupt

  sei();          // Enable global interrupts

}

//----------------------------------------------------------------------------
// INTERRUPTS
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ISR(TIMER1_OVF_vect)
//  Interrupt once per second by Timer1
{
  TCNT1 = T1_START;
  osENABLE_TASK(System_Clock_Task);
}

//----------------------------------------------------------------------------
ISR(PCINT2_vect)
//  Interrupt on pin change for sensor inputs
{
  switch(ew_veh_state)    // EW vehicle sensor state machine
  {                       // Other half is in associated task
    case 0:                                     // Look for down-stroke
      if(!(test_bit(SENSOR_PIN, EW_VEH_BIT)))   // Down?
      {
        ew_veh_state = 1;                       // Start down debounce wait
        osENABLE_TASK(EW_Veh_Sensor_Task);
      }
      break;
    case 2:                                     // Look for up-stroke
      if(test_bit(SENSOR_PIN, EW_VEH_BIT))      // Up?
      {
        ew_veh_state = 3;                       // Start up debounce wait
        osENABLE_TASK(EW_Veh_Sensor_Task);
      }
      break;
    default:
      break;
  }

  switch(ns_veh_state)    // NS vehicle sensor state machine
  {                       // Other half is in associated task
    case 0:                                     // Look for down-stroke
      if(!(test_bit(SENSOR_PIN, NS_VEH_BIT)))   // Down?
      {
        ns_veh_state = 1;                       // Start down debounce wait
        osENABLE_TASK(NS_Veh_Sensor_Task);
      }
      break;
    case 2:                                     // Look for up-stroke
      if(test_bit(SENSOR_PIN, NS_VEH_BIT))      // Up?
      {
        ns_veh_state = 3;                       // Start up debounce wait
        osENABLE_TASK(NS_Veh_Sensor_Task);
      }
      break;
    default:
      break;
  }

  switch(ped_pb_state)    // Pedestrian button state machine
  {                       // Other half is in associated task
    case 0:                                     // Look for down-stroke
      if(!(test_bit(SENSOR_PIN, PED_PB_BIT)))   // Button down?
      {
        ped_pb_state = 1;                       // Start down debounce wait
        osENABLE_TASK(Ped_Button_Task);
      }
      break;
    case 2:                                     // Look for up-stroke
      if(test_bit(SENSOR_PIN, PED_PB_BIT))      // Button up?
      {
        ped_pb_state = 3;                       // Start up debounce wait
        osENABLE_TASK(Ped_Button_Task);
      }
      break;
    default:
      break;
  }

}

//----------------------------------------------------------------------------
ISR(USART_RX_vect)
//  Interrupt on USART RX complete
{
  c_rx = UDR0;
  osENABLE_TASK(Command_Task);
}

//----------------------------------------------------------------------------
// TASKS
//----------------------------------------------------------------------------
//  tasks
//    BEGIN(taskname)
//      //code here
//      //make sure an os call is in here somewhere, else will be stuck here
//    END_TASK
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//  System_Clock_Task
//----------------------------------------------------------------------------

BEGIN_TASK(System_Clock_Task)   // Clock for status msg time stamps

  osWAIT_IRQ();                     // Wait for Timer1 overflow

  osGET_SEMA(semaTime);          // Protect clock data
  seconds++;                        // Increment time, military format
  if (seconds == 60)
  {
    seconds = 0;
    minutes++;
    if (minutes == 60)
    {
      minutes = 0;
      hours++;
      if (hours == 24)
      {
        hours = 0;
      }
    }
  }  
  osRELEASE_SEMA(semaTime);

END_TASK

//----------------------------------------------------------------------------
//  Day_Lights_Task
//----------------------------------------------------------------------------
BEGIN_TASK(Day_Lights_Task)   // Operates daylight (normal) light sequence

  static uint8_t i;

  osGET_SEMA(semaModeSwitch);       // Prevent mode switch during green

  LIGHT_PORT = LIGHT_STATE_2 ;      // NS green, EW red
                                    // Complement bits for protoboard
  osGET_SEMA(semaSMAQueue);         // Load status msg addr into queue
  put_sma_queue(pgm_read_word(&light_state2_msg_ptr));
  osRELEASE_SEMA(semaSMAQueue);     // with semaphore protection

  for(i=0; i<grn_ivl; i++) { osWAIT(100); }

  if(day_mode == 'L')     // If day mode is for EW light traffic, keep
  {                       // NS green until EW traffic is detected
    osWAIT_UNTIL((ew_det_flag == 1)||(day_mode == 'H'));
  }

  LIGHT_PORT = LIGHT_STATE_3 ;    // NS yellow, EW red
                                  // Complement bits for protoboard
  osGET_SEMA(semaSMAQueue);
  put_sma_queue(pgm_read_word(&light_state3_msg_ptr));
  osRELEASE_SEMA(semaSMAQueue);

  for(i=0; i<yel_ivl; i++) { osWAIT(100); }

  osRELEASE_SEMA(semaModeSwitch);   // Allow mode switch here
  osWAIT(100);                      // for 1 sec period


  osGET_SEMA(semaModeSwitch);       // Prevent mode switch during green

  LIGHT_PORT = LIGHT_STATE_0 ;      // EW green, NS red
                                    // Complement bits for protoboard
  osGET_SEMA(semaSMAQueue);
  put_sma_queue(pgm_read_word(&light_state0_msg_ptr));
  osRELEASE_SEMA(semaSMAQueue);   

  for(i=0; i<grn_ivl; i++) { osWAIT(100); }

  LIGHT_PORT = LIGHT_STATE_1 ;      // EW yellow, NS red
                                    // Complement bits for protoboard
  osGET_SEMA(semaSMAQueue);
  put_sma_queue(pgm_read_word(&light_state1_msg_ptr));
  osRELEASE_SEMA(semaSMAQueue);  

  for(i=0; i<yel_ivl; i++) { osWAIT(100); }

  osRELEASE_SEMA(semaModeSwitch);   // Allow mode switch here
  osWAIT(100);                      // for 1 sec period

  ew_det_flag = 0;   // Clear EW veh detection flag as NS goes to green

END_TASK

//----------------------------------------------------------------------------
//  Four_Way_Stop_Task
//----------------------------------------------------------------------------
BEGIN_TASK(Four_Way_Stop_Task)   // All traffic lights flash red

  osGET_SEMA(semaModeSwitch);    // Stop disable of this task

  LIGHT_PORT = LIGHT_STATE_4;    // All traffic lights to red
                                 // Complement bits for protoboard
  osWAIT(50);                    // 1 Hz frequency
  
  osRELEASE_SEMA(semaModeSwitch);   // Can now disable this task

  LIGHT_PORT = 0;                // All off for flash

  osWAIT(50);

END_TASK

//----------------------------------------------------------------------------
//  Night_Lights_Task
//----------------------------------------------------------------------------
BEGIN_TASK(Night_Lights_Task)    // Operate night flashing lights

  osGET_SEMA(semaModeSwitch);    // Stop disable of this task

  LIGHT_PORT = LIGHT_STATE_3;    // Flash EW = Red, NS = Yellow
                                 // Complement bits for protoboard
  osWAIT(50);                    // 1 Hz frequency

  osRELEASE_SEMA(semaModeSwitch);   // Can now disable this task

  LIGHT_PORT = 0;                // All off for flash

  osWAIT(50);

END_TASK

//----------------------------------------------------------------------------
//  Mixed_Lights_Task
//----------------------------------------------------------------------------
BEGIN_TASK(Mixed_Lights_Task) // Change light mode at alarm times

  static uint8_t i;

  osGET_SEMA(semaTime);          // Protect clock data
  check_hrs = hours;             // Get current time
  check_mins = minutes;
  osRELEASE_SEMA(semaTime);
  osGET_SEMA(semaAlarmTime);

  if((check_hrs == alarm1_hrs) && (check_mins == alarm1_mins))
  {
    ew_det_flag = 1;            // Release day_mode=L EW traffic hold
    osGET_SEMA(semaModeSwitch); // Prevents mode switch
    osDISABLE_TASK(Day_Lights_Task);    // Start night light mode
    osDISABLE_TASK(Four_Way_Stop_Task);
    osRELEASE_SEMA(semaModeSwitch);
    osENABLE_TASK(Night_Lights_Task);
    light_mode1 = 'N';
    osGET_SEMA(semaSMAQueue);
    put_sma_queue(pgm_read_word(&start_night_cmd_msg_ptr));
    osRELEASE_SEMA(semaSMAQueue);
  }

  if((check_hrs == alarm2_hrs) && (check_mins == alarm2_mins))
  {
    osGET_SEMA(semaModeSwitch);             // Prevents mode switch
    osDISABLE_TASK(Four_Way_Stop_Task);     // Start daytime light mode
    osDISABLE_TASK(Night_Lights_Task);
    osRELEASE_SEMA(semaModeSwitch);
    osENABLE_TASK(Day_Lights_Task);
    light_mode1 = 'D';    
    osGET_SEMA(semaSMAQueue);
    put_sma_queue(pgm_read_word(&start_day_cmd_msg_ptr));
    osRELEASE_SEMA(semaSMAQueue);
  }

  osRELEASE_SEMA(semaAlarmTime);
  for(i=0; i<30; i++) { osWAIT(100); }  // Wait 30 sec to be sure to
                                        // catch alarm time. Duplicate
                                        // switch has no effect on lights.

END_TASK

//----------------------------------------------------------------------------
//  Ped_Cross_Task
//----------------------------------------------------------------------------
BEGIN_TASK(Ped_Cross_Task)    // All traffic lights red for pedestrians
                              // crossing all directions
  static uint8_t i;

  for(i=0; i<10; i++) { osWAIT(100); }  // Wait 10 sec

  ew_det_flag = 1;              // Release day_mode=L EW traffic hold
  osGET_SEMA(semaModeSwitch);   // Holds current light mode

  LIGHT_PORT = LIGHT_STATE_4;   // Set all traffic lights to red
                                // Complement bits for protoboard

  osGET_SEMA(semaSMAQueue);     // Send ped crossing msg
  put_sma_queue(pgm_read_word(&ped_cross_msg_ptr));
  osRELEASE_SEMA(semaSMAQueue);

  for(i=0; i<grn_ivl; i++) { osWAIT(100); }

  osRELEASE_SEMA(semaModeSwitch);   // Back to current light mode
  osSUSPEND;

END_TASK

//----------------------------------------------------------------------------
//  Send_Status_Msg_Task
//----------------------------------------------------------------------------

BEGIN_TASK(Send_Status_Msg_Task)   // Send system status messages from queue
  static uint8_t i;
  static uint8_t n;
  static char c;                      // Character to send
  static uint16_t status_msg_addr;    // Prog mem addr of a status msg
  static char h, m, s;                // Local hrs, mins, secs for time stamp

  osGET_SEMA(semaSMAQueue);           // Get number of msg addresses in
  n = num_elt_sma_queue();            // queue with semaphore protection
  osRELEASE_SEMA(semaSMAQueue);

  while(n)                            // Loop while queue is not empty
  {                                   // num_elt_sma_queue not put in
                                      // while() arg to allow semaphore use
    osGET_SEMA(semaTX);               // Hold USART TX

// Send message text
    osGET_SEMA(semaSMAQueue);             // Get next msg address
    status_msg_addr = get_sma_queue();    // with semaphore protection
    osRELEASE_SEMA(semaSMAQueue);

    for(i=0;; i++)                        // Send msg to USART
    {
      c = pgm_read_byte(status_msg_addr+i) ;
      if (c!=0) 
      {
        osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
        UDR0 = c;
      }
      else break;
    }

// Get time with semaphore protection
    osGET_SEMA(semaTime);
    h = hours;
    m = minutes;
    s = seconds;
    osRELEASE_SEMA(semaTime);

// Send time stamp in format HH:MM:SS
    c = ' ';                          // Send leading space
    osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
    UDR0 = c;

    c = h / 10;                       // Hours first digit
    osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
    UDR0 = c + '0';
    c = h % 10;                       // Hours second digit
    osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
    UDR0 = c + '0';
    c = ':';
    osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
    UDR0 = c;

    c = m / 10;                       // Minutes first digit
    osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
    UDR0 = c + '0';
    c = m % 10;                       // Minutes second digit
    osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
    UDR0 = c + '0';
    c = ':';
    osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
    UDR0 = c;

    c = s / 10;                       // Seconds first digit
    osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
    UDR0 = c + '0';
    c = s % 10;                       // Seconds second digit
    osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
    UDR0 = c + '0';

// Send new prompt
    send_prompt();

    osRELEASE_SEMA(semaTX);           // Release USART TX

    osGET_SEMA(semaSMAQueue);         // Get number of msg addresses in
    n = num_elt_sma_queue();          // queue with semaphore protection
    osRELEASE_SEMA(semaSMAQueue);
  }

  osWAIT(10);     // Wait 100 ms and recheck queue

END_TASK

//----------------------------------------------------------------------------
//  EW_Veh_Sensor_Task
//----------------------------------------------------------------------------
BEGIN_TASK(EW_Veh_Sensor_Task)   // Actions for EW vehicle sensor

  switch(ew_veh_state)    // EW vehicle sensor state machine
  {                       // Other half is in associated ISR
    case 1:
      osWAIT(2);                    // Down-stroke debounce
      if(test_bit(SENSOR_PIN, EW_VEH_BIT))  // Back up?
        ew_veh_state = 0;           // False signal, reset
      else
      {
        osGET_SEMA(semaSMAQueue);   // Send veh detected msg
        put_sma_queue(pgm_read_word(&ew_veh_det_msg_ptr));
        osRELEASE_SEMA(semaSMAQueue);

        ew_det_flag = 1;            // Flag for light traffic day mode

        ew_veh_state = 2;           // ISR looks for up-stroke
      }
      break;
    case 3:
      osWAIT(2);                    // Up-stroke debounce
      if(!(test_bit(SENSOR_PIN, EW_VEH_BIT)))   // Back down?
        ew_veh_state = 2;           // False signal, go back
      else
      {
        osGET_SEMA(semaSMAQueue);   // Send veh passed msg
        put_sma_queue(pgm_read_word(&ew_veh_pass_msg_ptr));
        osRELEASE_SEMA(semaSMAQueue);

        ew_veh_state = 0;           // ISR looks for down-stroke
      }
      break;
    default:
      break;
  }

  osWAIT_IRQ();

END_TASK

//----------------------------------------------------------------------------
//  NS_Veh_Sensor_Task
//----------------------------------------------------------------------------
BEGIN_TASK(NS_Veh_Sensor_Task)   // Actions for NS vehicle sensor

  switch(ns_veh_state)    // NS vehicle sensor state machine
  {                       // Other half is in associated ISR
    case 1:
      osWAIT(2);                    // Down-stroke debounce
      if(test_bit(SENSOR_PIN, NS_VEH_BIT))  // Back up?
        ns_veh_state = 0;           // False signal, reset
      else
      {
        osGET_SEMA(semaSMAQueue);   // Send veh detected msg
        put_sma_queue(pgm_read_word(&ns_veh_det_msg_ptr));
        osRELEASE_SEMA(semaSMAQueue);

        ns_veh_state = 2;           // ISR looks for up-stroke
      }
      break;
    case 3:
      osWAIT(2);                    // Up-stroke debounce
      if(!(test_bit(SENSOR_PIN, NS_VEH_BIT)))   // Back down?
        ns_veh_state = 2;           // False signal, go back
      else
      {
        osGET_SEMA(semaSMAQueue);   // Send veh passed msg
        put_sma_queue(pgm_read_word(&ns_veh_pass_msg_ptr));
        osRELEASE_SEMA(semaSMAQueue);

        ns_veh_state = 0;           // ISR looks for down-stroke
      }
      break;
    default:
      break;
  }

  osWAIT_IRQ();

END_TASK

//----------------------------------------------------------------------------
//  Ped_Button_Task
//----------------------------------------------------------------------------
BEGIN_TASK(Ped_Button_Task)   // Actions for pedestrian push buttons

  switch(ped_pb_state)    // Pedestrian push button state machine
  {                       // Other half is in associated ISR
    case 1:
      osWAIT(2);                    // Down-stroke debounce
      if(test_bit(SENSOR_PIN, PED_PB_BIT))  // Back up?
        ped_pb_state = 0;           // False signal, reset
      else
        ped_pb_state = 2;           // ISR looks for up-stroke
      break;
    case 3:
      osWAIT(2);                    // Up-stroke debounce
      if(!(test_bit(SENSOR_PIN, PED_PB_BIT)))   // Back down?
        ped_pb_state = 2;           // False signal, go back
      else
      {
        osGET_SEMA(semaSMAQueue);   // Send veh passed msg
        put_sma_queue(pgm_read_word(&ped_pb_msg_ptr));
        osRELEASE_SEMA(semaSMAQueue);

        osENABLE_TASK(Ped_Cross_Task);  // Start pedestrian crossing task

        ped_pb_state = 0;           // ISR looks for down-stroke
      }
      break;
    default:
      break;
  }

  osWAIT_IRQ();

END_TASK

//----------------------------------------------------------------------------
//  Command_Task
//----------------------------------------------------------------------------
BEGIN_TASK(Command_Task)   // Manage command interface

  static char c, c_tx;
  static uint16_t msg_addr;
  static uint8_t input_buffer_index, i;
  static uint8_t num_digits, color_state;
  static char input_buffer[INPUT_BUFFER_SIZE];  // Input data buffer
  static uint16_t a1, a2, now;  // Times in minutes past midnight
  static uint8_t now_hrs, now_mins, now_secs;
  static uint8_t alarm_num;

  c = c_rx;           // Fetch received character
  osGET_SEMA(semaTX);

  if(c == 'h')        // Print help msg
  {
    osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));    // Echo character
    UDR0 = c;
    send_prompt();
    msg_addr = pgm_read_word(&help_msg1_ptr);
    send_cmd_msg(msg_addr);
    msg_addr = pgm_read_word(&help_msg2_ptr);
    send_cmd_msg(msg_addr);
    msg_addr = pgm_read_word(&help_msg3_ptr);
    send_cmd_msg(msg_addr);
  }
  else
  {
    switch(cmd_mode_flag)
    {
      case 0:
        if(c == 'c')      // Enter command mode
        {
          while(!(test_bit(UCSR0A, UDRE0)));    // Echo character
          UDR0 = c;
          osDISABLE_TASK(Send_Status_Msg_Task); // Disable status msg sending
          send_prompt();
          msg_addr = pgm_read_word(&enter_cmd_mode_msg_ptr);
          send_cmd_msg(msg_addr);
          cmd_mode_flag = 1;      // Put state machine in command mode
        }
        break;

      case 1:   // Respond to other commands in command mode
        while(!(test_bit(UCSR0A, UDRE0)));    // Echo character
        UDR0 = c;
        switch(c)
        {
          case '\r':    // Carriage return
            send_prompt();
            break;
          case 'q':     // Quit command mode
            send_prompt();
            msg_addr = pgm_read_word(&exit_cmd_mode_msg_ptr);
            send_cmd_msg(msg_addr);
            sma_get_loc = 0;          // Flush status message addr queue
            sma_put_loc = 0;
            cmd_mode_flag = 0;
            osENABLE_TASK(Send_Status_Msg_Task);
            break;
          case 'f':     // Go to four-way stop mode
            ew_det_flag = 1;          // Release day_mode=L EW traffic hold
            osGET_SEMA(semaModeSwitch); // Prevents mode switch
            osDISABLE_TASK(Day_Lights_Task);
            osDISABLE_TASK(Night_Lights_Task);
            osDISABLE_TASK(Mixed_Lights_Task);
            osRELEASE_SEMA(semaModeSwitch);
            osENABLE_TASK(Four_Way_Stop_Task);
            light_mode0 = 'F';
            light_mode1 = ' ';
            send_prompt();
            msg_addr = pgm_read_word(&start_4way_cmd_msg_ptr);
            send_cmd_msg(msg_addr);
            break;
          case 'n':     // Go to night flashing lights mode
            ew_det_flag = 1;          // Release day_mode=L EW traffic hold
            osGET_SEMA(semaModeSwitch); // Prevents mode switch
            osDISABLE_TASK(Day_Lights_Task);
            osDISABLE_TASK(Four_Way_Stop_Task);
            osDISABLE_TASK(Mixed_Lights_Task);
            osRELEASE_SEMA(semaModeSwitch);
            osENABLE_TASK(Night_Lights_Task);
            light_mode0 = 'N';
            light_mode1 = ' ';
            send_prompt();
            msg_addr = pgm_read_word(&start_night_cmd_msg_ptr);
            send_cmd_msg(msg_addr);
            break;
          case 'd':     // Go to daylight (normal) light sequence mode
            osGET_SEMA(semaModeSwitch);
            osDISABLE_TASK(Four_Way_Stop_Task);
            osDISABLE_TASK(Night_Lights_Task);
            osDISABLE_TASK(Mixed_Lights_Task);
            osRELEASE_SEMA(semaModeSwitch);
            osENABLE_TASK(Day_Lights_Task);
            light_mode0 = 'D';
            light_mode1 = ' ';
            send_prompt();
            msg_addr = pgm_read_word(&start_day_cmd_msg_ptr);
            send_cmd_msg(msg_addr);
            break;
          case 'H':     // Set day mode for heavy E/W traffic
            send_prompt();
            msg_addr = pgm_read_word(&day_mode_H_cmd_msg_ptr);
            send_cmd_msg(msg_addr);
            day_mode = 'H';
            break;
          case 'L':     // Set day mode for light E/W traffic
            send_prompt();
            msg_addr = pgm_read_word(&day_mode_L_cmd_msg_ptr);
            send_cmd_msg(msg_addr);
            day_mode = 'L';
            break;
          case 'm':     // Go to mixed day/night light mode
            // Determine if time now is to be day or night mode
            // Set to night mode if alarm1 <= now < alarm2
            osGET_SEMA(semaTime);   // Get current time
            now_hrs = hours;
            now_mins = minutes;
            osRELEASE_SEMA(semaTime);
            // Convert times to minutes since midnight
            now = (60 * (uint16_t) now_hrs) + now_mins;
            a1 = (60 * (uint16_t) alarm1_hrs) + alarm1_mins; // Alarm 1
            a2 = (60 * (uint16_t) alarm2_hrs) + alarm2_mins; // Alarm 2
            // Shift alarms back 5 mins to avoid near misses
            // Can cause setting of mode up to 5 minutes early
            if (a1 < 5) a1 = a1 + 1435;   
            else a1 = a1 - 5;
            if (a2 < 5) a2 = a2 + 1435;
            else a2 = a2 - 5;
            // Determine where current time falls between alarms
            if(a1 == a2)     // Alarm times equal, can't do mixed mode
            {
              send_prompt();
              msg_addr = pgm_read_word(&alarms_equal_cmd_msg_ptr);
              send_cmd_msg(msg_addr);
            }
            else
            {
              if(((a1>a2)&&((now>=a1)||(now<a2)))
                  ||((a1<a2)&&((now>=a1)&&(now<a2))))
              {
                ew_det_flag = 1;   // Release day_mode=L EW traffic hold
                osGET_SEMA(semaModeSwitch);
                osDISABLE_TASK(Four_Way_Stop_Task);   // It is night time
                osDISABLE_TASK(Day_Lights_Task);
                osRELEASE_SEMA(semaModeSwitch);
                osENABLE_TASK(Night_Lights_Task);
                osENABLE_TASK(Mixed_Lights_Task);
                light_mode1 = 'N';
                send_prompt();
                msg_addr = pgm_read_word(&start_night_cmd_msg_ptr);
                send_cmd_msg(msg_addr);
              }
              else
              {
                osGET_SEMA(semaModeSwitch);
                osDISABLE_TASK(Four_Way_Stop_Task);   // It is day time
                osDISABLE_TASK(Night_Lights_Task);
                osRELEASE_SEMA(semaModeSwitch);
                osENABLE_TASK(Day_Lights_Task);
                osENABLE_TASK(Mixed_Lights_Task);
                light_mode1 = 'D';
                send_prompt();
                msg_addr = pgm_read_word(&start_day_cmd_msg_ptr);
                send_cmd_msg(msg_addr);
              }
              msg_addr = pgm_read_word(&start_mixed_cmd_msg_ptr);
              send_cmd_msg(msg_addr);
              light_mode0 = 'M';
            }
            break;
          case 't':     // Set system clock time
            send_prompt();
            msg_addr = pgm_read_word(&enter_time_msg_ptr);
            send_cmd_msg(msg_addr);
            for(i=0; i<=INPUT_BUFFER_SIZE; i++)
              input_buffer[i] = ' ';
            input_buffer_index = 0;
            cmd_mode_flag = 2;
            break;
          case 'g':    // Enter green light time
            send_prompt();
            msg_addr = pgm_read_word(&enter_grn_time_msg_ptr);
            send_cmd_msg(msg_addr);
            for(i=0; i<=INPUT_BUFFER_SIZE; i++)
              input_buffer[i] = ' ';
            input_buffer_index = 0;
            cmd_mode_flag = 3;
            color_state = 0;
            num_digits = 2;
            break;
          case 'y':    // Enter yellow light time
            send_prompt();
            msg_addr = pgm_read_word(&enter_yel_time_msg_ptr);
            send_cmd_msg(msg_addr);
            for(i=0; i<=INPUT_BUFFER_SIZE; i++)
              input_buffer[i] = ' ';
            input_buffer_index = 0;
            cmd_mode_flag = 3;
            color_state = 1;
            num_digits = 1;
            break;
          case '1':     // Set mixed mode day-to-night time
            send_prompt();
            msg_addr = pgm_read_word(&enter_alarm1_msg_ptr);
            send_cmd_msg(msg_addr);
            for(i=0; i<=INPUT_BUFFER_SIZE; i++)
              input_buffer[i] = ' ';
            input_buffer_index = 0;
            alarm_num = 1;
            cmd_mode_flag = 4;
            break;
          case '2':     // Set mixed mode night-to-day time
            send_prompt();
            msg_addr = pgm_read_word(&enter_alarm2_msg_ptr);
            send_cmd_msg(msg_addr);
            for(i=0; i<=INPUT_BUFFER_SIZE; i++)
              input_buffer[i] = ' ';
            input_buffer_index = 0;
            alarm_num = 2;
            cmd_mode_flag = 4;
            break;
          case 's':     // Show current system status
            osGET_SEMA(semaTime); // Get time with semaphore protection
            now_hrs = hours;
            now_mins = minutes;
            now_secs = seconds;
            osRELEASE_SEMA(semaTime);
            send_prompt();
            msg_addr = pgm_read_word(&show_time_msg_ptr);
            send_cmd_msg(msg_addr);
            c_tx = now_hrs / 10;        // Hours first digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            c_tx = now_hrs % 10;        // Hours second digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            c_tx = ':';
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx;
            c_tx = now_mins / 10;       // Minutes first digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            c_tx = now_mins % 10;       // Minutes second digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            c_tx = ':';
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx;
            c_tx = now_secs / 10;       // Seconds first digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            c_tx = now_secs % 10;       // Seconds second digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0' ;
            send_prompt();

            // Show alarm times
            msg_addr = pgm_read_word(&show_alarm1_msg_ptr);
            send_cmd_msg(msg_addr);
            osGET_SEMA(semaAlarmTime);
            c_tx = alarm1_hrs / 10;        // Hours first digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            c_tx = alarm1_hrs % 10;        // Hours second digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            c_tx = ':';
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx;
            c_tx = alarm1_mins / 10;       // Minutes first digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            c_tx = alarm1_mins % 10;       // Minutes second digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            osRELEASE_SEMA(semaAlarmTime);
            send_prompt();
            msg_addr = pgm_read_word(&show_alarm2_msg_ptr);
            send_cmd_msg(msg_addr);
            osGET_SEMA(semaAlarmTime);
            c_tx = alarm2_hrs / 10;        // Hours first digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            c_tx = alarm2_hrs % 10;        // Hours second digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            c_tx = ':';
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx;
            c_tx = alarm2_mins / 10;       // Minutes first digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            c_tx = alarm2_mins % 10;       // Minutes second digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            osRELEASE_SEMA(semaAlarmTime);
            send_prompt();

            msg_addr = pgm_read_word(&show_grn_time_msg_ptr);
            send_cmd_msg(msg_addr);
            c_tx = grn_ivl / 10;        // First digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            c_tx = grn_ivl % 10;        // Second digit
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            send_prompt();
            msg_addr = pgm_read_word(&show_yel_time_msg_ptr);
            send_cmd_msg(msg_addr);
            c_tx = yel_ivl + 1;
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = c_tx + '0';
            send_prompt();

            msg_addr = pgm_read_word(&show_mode_msg_ptr);
            send_cmd_msg(msg_addr);
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = light_mode0;
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = light_mode1;
            send_prompt();

            msg_addr = pgm_read_word(&show_day_mode_msg_ptr);
            send_cmd_msg(msg_addr);
            osWAIT_UNTIL(test_bit(UCSR0A, UDRE0));
            UDR0 = day_mode;
            send_prompt();
            break;
          case 'c':     // Already in command mode
            send_prompt();
            msg_addr = pgm_read_word(&in_cmd_mode_msg_ptr);
            send_cmd_msg(msg_addr);
            break;
          default:      // Command unknown
            send_prompt();
            msg_addr = pgm_read_word(&unknown_cmd_msg_ptr);
            send_cmd_msg(msg_addr);
            msg_addr = pgm_read_word(&help_msg1_ptr);
            send_cmd_msg(msg_addr);
            msg_addr = pgm_read_word(&help_msg2_ptr);
            send_cmd_msg(msg_addr);
            msg_addr = pgm_read_word(&help_msg3_ptr);
            send_cmd_msg(msg_addr);
            break;
        }
        break;

      case 2:     // Get new system time
        switch(c)
        {
          case '\b':    // Backspace entered
            if(input_buffer_index > 0)    // Backspace OK
            {
              input_buffer_index--;
              while(!(test_bit(UCSR0A, UDRE0)));    // Echo backspace
              UDR0 = c;
              while(!(test_bit(UCSR0A, UDRE0)));    // Blank space
              UDR0 = ' ';
              while(!(test_bit(UCSR0A, UDRE0)));    // Back again
              UDR0 = '\b';
            }
            break;
          case '\r':    // Carriage return entered
            send_prompt();
            if(system_time_OK(input_buffer))   // Decode time values
            {
              osGET_SEMA(semaTime);
              hours = new_hrs;
              minutes = new_mins;
              seconds = new_secs;
              osRELEASE_SEMA(semaTime);
              msg_addr = pgm_read_word(&time_set_msg_ptr);
              send_cmd_msg(msg_addr);
              msg_addr = pgm_read_word(&check_mode_msg_ptr);
              send_cmd_msg(msg_addr);
            }
            else    // Entry invalid, cancel command
            {
              msg_addr = pgm_read_word(&invalid_entry_msg_ptr);
              send_cmd_msg(msg_addr);
            }
            cmd_mode_flag = 1;    // Back to getting commands
            break;
          default:  // Get new time value
            if(input_buffer_index <= 7)
            {
              while(!(test_bit(UCSR0A, UDRE0)));    // Echo character
              UDR0 = c;
              input_buffer[input_buffer_index] = c;
              input_buffer_index++;
            }
            break;       
        }
        break;

      case 3:     // Get new light time interval
        switch(c)
        {
          case '\b':    // Backspace entered
            if(input_buffer_index > 0)    // Backspace OK
            {
              input_buffer_index--;
              while(!(test_bit(UCSR0A, UDRE0)));    // Echo backspace
              UDR0 = c;
              while(!(test_bit(UCSR0A, UDRE0)));    // Blank space
              UDR0 = ' ';
              while(!(test_bit(UCSR0A, UDRE0)));    // Back again
              UDR0 = '\b';
            }
            break;
          case '\r':    // Carriage return entered
            send_prompt();
            if(light_time_OK(input_buffer, num_digits, color_state))
            {                         // No protection needed
              if (color_state == 0)
                grn_ivl = new_secs;
              if (color_state == 1)
                yel_ivl = new_secs - 1;   // Task adds one second
              msg_addr = pgm_read_word(&time_set_msg_ptr);
              send_cmd_msg(msg_addr);
            }
            else    // Entry invalid, cancel command
            {
              msg_addr = pgm_read_word(&invalid_entry_msg_ptr);
              send_cmd_msg(msg_addr);
            }
            cmd_mode_flag = 1;    // Back to getting commands
            break;
          default:  // Get new value
            if(input_buffer_index <= (num_digits-1))
            {
              while(!(test_bit(UCSR0A, UDRE0)));    // Echo character
              UDR0 = c;
              input_buffer[input_buffer_index] = c;
              input_buffer_index++;
            }
            break;       
        }
        break;

      case 4:     // Get new alarm time
        switch(c)
        {
          case '\b':    // Backspace entered
            if(input_buffer_index > 0)    // Backspace OK
            {
              input_buffer_index--;
              while(!(test_bit(UCSR0A, UDRE0)));    // Echo backspace
              UDR0 = c;
              while(!(test_bit(UCSR0A, UDRE0)));    // Blank space
              UDR0 = ' ';
              while(!(test_bit(UCSR0A, UDRE0)));    // Back again
              UDR0 = '\b';
            }
            break;
          case '\r':    // Carriage return entered
            send_prompt();
            if(alarm_time_OK(input_buffer))   // Decode time values
            {
              osGET_SEMA(semaAlarmTime);
              if(alarm_num == 1)
              {
                alarm1_hrs = new_hrs;
                alarm1_mins = new_mins;
              }
              if(alarm_num == 2)
              {
                alarm2_hrs = new_hrs;
                alarm2_mins = new_mins;
              }
              osRELEASE_SEMA(semaAlarmTime);
              msg_addr = pgm_read_word(&time_set_msg_ptr);
              send_cmd_msg(msg_addr);
              msg_addr = pgm_read_word(&check_mode_msg_ptr);
              send_cmd_msg(msg_addr);
            }
            else    // Entry invalid, cancel command
            {
              msg_addr = pgm_read_word(&invalid_entry_msg_ptr);
              send_cmd_msg(msg_addr);
            }
            cmd_mode_flag = 1;    // Back to getting commands
            break;
          default:  // Get new time value
            if(input_buffer_index <= 4)
            {
              while(!(test_bit(UCSR0A, UDRE0)));    // Echo character
              UDR0 = c;
              input_buffer[input_buffer_index] = c;
              input_buffer_index++;
            }
            break;       
        }
        break;

      default:
        break;
    }

  }

  osRELEASE_SEMA(semaTX);
  osWAIT_IRQ();     // Suspend until next character received

END_TASK
