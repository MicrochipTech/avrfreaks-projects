
/*********************************************************************************************************
 Title  :   C header file for the ONE_WIRE FUNCTIONS library (one_wire.h)
 Author:    Chris efstathiou  
 E-mail:    hendrix@otenet.gr
 Homepage:  ........................
 Date:      25/1/2003 12:01:11 μμ
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  any AVR MCU device
 Comments:  This software is FREE but without any warranty of any kind.
*********************************************************************************************************/

/*
   After many hours and days i finally made it in writing this library for 1 wire that is stable,
   reliable and most of all timing accurate.
   I have double checked the timing accuracy and it seems to be correct.
   However some bugs might still exist but in general this driver works fine in most cases.
   If you find something strange or wrong please drop me a note.
   Edit the configuration block below to adapt settings to your environment.
   The timing constants defined later in this file are in general correct
   so you shouldn't need to edit them. If you do edit them warning messages will be issued
   to warn you if you enter a value that is outside the 1 wire specifications.

   NOTICE: The Calculation of crc8 algorithm is mainly taken from the  
           Daniel Mooney's and Colin O'Flynn's crc libraries.
           Have a look at AVRFREAKS->ACADEMY->USER_PROJECTS...
*/


#ifndef ONE_WIRE_H
#define ONE_WIRE    1

/*######################################################################################################*/
/*			         START OF CONFIGURATION BLOCK                                           */
/*######################################################################################################*/

#ifndef  F_CPU
#define  F_CPU		              3686400   /* The cpu clock frequency in Hertz */
#endif

#define ONE_WIRE_PORT	              D	     /* The 1 wire port. Use capital letter (A,B,C,D... etc.) */
#define ONE_WIRE_PIN 	              0	     /* The 1 wire port pin */

#define ONE_WIRE_DYNAMIC_IO_CHANGE    0      /* 0=the above settings apply, 1=can dynamically change */
#define OW_CRC8_ERROR_RETRIES         3      /* The max number of retries if a crc8 error occur. */

/* WAYS TO REDUCE CODE SIZE BY NOT COMPILING UNWANTED FUNCTIONS OR CODE PORTIONS */
#define OW_SEARCH_ROM_NEEDED          1      /* 0=function not available */
#define OW_READ_ROM_NEEDED            1      /* 0=function not available */
#define OW_CALCULATE_CRC8_NEEDED      1      /* 0=function not available */
#define OW_PULL_HARD_DQ_NEEDED        1      /* 0=function not available */
#define OW_SAVE_MORE_CODE_SPACE       0      /* 1=more code is ommited without affecting operation  */

/* Normally you shouldn't need to change anything in the below timing definitions */

#define ONE_WIRE_RECOVERY_TIME_US     1      /* The time for the pullup resistor to get the line high */
#define ONE_WIRE_READ_POSITION_US     14     /* When to sample the DQ line for rx, after DQ going low */
#define ONE_WIRE_WRITE_SLOT_TIME_US   90     /* The total time of the WRITE SLOT  */ 
#define ONE_WIRE_READ_SLOT_TIME_US    90     /* The total time of the READ SLOT  */ 
#define ONE_WIRE_RESET_TIME_US        750    /* The total reset pulse time */
#define ONE_WIRE_WAIT_FOR_PRESENSE_US 60     /* The max time to wait for a presence pulse after reset */
#define ONE_WIRE_PRESENSE_DURATION_US 60     /* The minimum presense pulse width in order to be valid. */
#define ONE_WIRE_ALARM_TIME_US        2000   /* The required presense pulse duration for a valid alarm */
#define ONE_WIRE_TIMEOUT_US           4000   /* After that time elapses a fault is detected */
#define ONE_WIRE_DEFAULT_TIMING       0      /* 1=the above values are auto set to the default ones. */

#define OW_TOTAL_R0M_CODE_SIZE        8      /* For the future. 8 bytes might prove to be not enough */
#define OW_ROM_CODE_CRC_SIZE          1      /* For the future. 1 byte  might prove to be not enough */
/*######################################################################################################*/
/*			            END OF CONFIGURATION BLOCK                                          */
/*######################################################################################################*/



/********************************************************************************************************/
/*                              KEYWORD DEFINITIONS                                                     */
/********************************************************************************************************/
/* One wire messages */
#define OW_NO_ERROR             0
#define OW_BUS_ERROR            1
#define OW_NO_DEVICE_PRESENT    2
#define OW_CRC_ERROR            3
#define OW_SEARCH_ROM_ERROR     4
#define OW_ALARM_DETECTED       5
#define OW_SEARCH_ROM_IS_RESET  6
#define OW_SEARCH_ROM_ACTIVE    7
#define OW_SEARCH_ROM_FINISHED  8



/* ONE WIRE COMMANDS ONLY */
#define OW_SKIP_ROM             0xCC
#define OW_MATCH_ROM            0x55
#define OW_READ_ROM             0x33

/* ONE WIRE COMMANDS and "ow_search_rom()" function, second argument commands */
#define OW_SEARCH_ROM           0xF0
#define OW_ALARM_SEARCH         0xEC

/* "ow_search_rom()" function, second argument commands only */
#define OW_RESET_SEARCH_ROM     1


/********************************************************************************************************/
/*                             PREPROCESSOR DIRECTIVES                                                  */
/********************************************************************************************************/





/********************************************************************************************************/
/*                             PUBLIC FUNCTION PROTOTYPES                                               */
/********************************************************************************************************/

/*           NO SPECIAL INITIALIZATION IS NEEDED. YOU CAN USE THE FUNCTIONS IMMEDIATELY.                */ 

/*
   ow_search_rom() returns the total number of roms found so far and it fills an 8 byte char array
   with the rom code every time it is called. Use the array for reading only.  
   You can call it repeatedly until all roms have been found. If all roms have been found,
   it issues a "OW_SEARCH_ROM_FINISHED" message, else a "OW_SEARCH_ROM_ACTIVE" message.
   Use the ow_get_msg() command to catch those messages.
   ow_search_rom() takes as arguments 1) a pointer to an 8 byte char array and 2) a command.
   It returns the number of rom codes found so far.
  
   The commands are:
   1) "OW_RESET_SEARCH_ROM" resets the search rom variables and exits. No rom searching is performed.
      Rom searching begins from the next function call with the command "OW_CONTINUE_SEARCH_ROM". 
   2) "OW_SEARCH_ROM" fills an 8 byte array with one rom code each time the "ow_search_rom()" is called.
      When all rom codes are found the "ow_search_rom()" function exits returning the total
      rom codes found and a "OW_SEARCH_ROM_FINISHED" message no matter how many times it is called
      after that. To perform a SEARCH_ROM again a call to ow_search_rom() with the "OW_RESET_SEARCH_ROM"
      command is needed.
      For example if there are 3 devices in the 1 wire bus the three first calls to 
      ow_search_rom(rom_code, OW_SEARCH_ROM) fill the rom_code array with the three rom codes
      and the return value of the function is 1,2,3 respectively.
      The message "OW_SEARCH_ROM_FINISHED" is issued and the return value remains 3 no matter
      how many times you call the function after that.
   3) "OW_ALARM_SEARCH" is the same as the "OW_SEARCH_ROM" but only devices with the alarm flag on are
      detected.

   IMPORTANT: The returned rom codes are crc8 verified so they are always correct.
              If a crc8 error occur during the search rom execution the search is
              automatically repeated untill a correct rom code is received or maximum number
              of retries is exceeded. This process is transparent to the user.
*/ 
#if OW_SEARCH_ROM_NEEDED == 1
extern unsigned char        ow_search_rom(unsigned char *mem_array, unsigned char command);
#endif
#if OW_READ_ROM_NEEDED == 1
/* Fills the mem_array with the rom code. Only one device must be connected on the 1 wire bus */
extern void                 ow_read_rom(unsigned char *mem_array);
#endif
/*
   Sends a 1 wire reset pulse. Usefull also for determining if there is any error on the 1 wire bus.
   If a function exits abnormally give a ow_reset() and then a ow_get_msg() to determine the error.
*/ 
extern void                 ow_reset(void);

/* Retuns the CRC8 of an array or char whose address is "data_pointer" and length is "number_of_bytes" */
#if OW_CALCULATE_CRC8_NEEDED == 1
extern unsigned char        calculate_crc8(unsigned char* data_pointer, unsigned char number_of_bytes);
#endif
/* Reads a bit from the 1 wire bus. */
extern inline unsigned char ow_get_bit(void);

/* Writes a bit to the 1 wire bus. */
extern inline void          ow_put_bit(unsigned char value);

/* Writes a byte on the 1 wire bus. */
extern void                 ow_put_byte(unsigned char data);

/* Reads a byte from the 1 wire bus. */
extern unsigned char        ow_get_byte(void);

/* Returns any 1 wire related message and error. */
extern unsigned char        ow_get_msg(void);

/* Pulls hard high the DQ line. Used if paracite powered devices are on the 1 wire bus */
extern inline void          ow_pull_hard_dq(unsigned char on_off);

#if ONE_WIRE_DYNAMIC_IO_CHANGE == 1
/* Sets the one wire port and pin dynamically if enabled. Use the macro ow_config_io(port,pin) */
extern void                 ow_config_io(unsigned char port, unsigned char pin);
#endif
/*
ONE WIRE MACROS THAT ARE USED AS FUNCTIONS

void                        ow_command(cmd)

GENERAL USEFULL AND ACCURATE DELAY MACROS

DELAY_US(us)
DELAY_MS(ms)
*/
   


/********************************************************************************************************/
/*                                 FUNCTION RESEMBLING MACROS                                           */
/********************************************************************************************************/

#define ow_command(cmd)            ow_put_byte(cmd)


#if  ( defined(_SFR_ASM_COMPAT) || !defined(_SFR_MEM8) ) && !defined(_IO_REG_ASSIGN_MODE_) 

#define ow_configure_io(port,pin) ow_config_io(port,pin)

#else

#define ow_configure_io(port,pin) ow_config_io(((unsigned int) &(port)),pin)

#endif

/*
   THE REASON I USE MACROS INSTEAD OF FUNCTIONS FOR TIMING IS THAT TIMING ACCURACY IS NEEDED AND MACROS
   ARE "INLINE" AND THE COMPILER CANNOT MESS WITH THEM VERY MUCH (ALTHOUGH IT CAN SOMETIMES).
*/

/********************************************************************************************************/
/*                                TIMING MACROS AND DEFINITIONS                                         */
/********************************************************************************************************/

/* Accurate Delay macros */
#ifndef DELAY_S_ACCURATE
/* 4 cpu cycles per loop + 1 overhead when a constant is passed. */
#define DELAY_S_ACCURATE(x)  ({ unsigned short number_of_loops=(unsigned short)x; \
                              	__asm__ volatile ( "L_%=:           \n\t"         \
                                                   "sbiw %A0,1      \n\t"         \
                                                   "brne L_%=       \n\t"         \
                                                   : /* NO OUTPUT */              \
                                                   : "w" (number_of_loops)        \
                                                 );                               \
                             })                               




#define DELAY_US(us)                DELAY_S_ACCURATE( (((((us)*1000L)/(1000000000/F_CPU))-1)/4) )  

#endif /* #ifndef DELAY_S_ACCURATE */

#ifndef DELAY_L_ACCURATE
/* 6 cpu cycles per loop + 3 overhead when a constant is passed. */
#define DELAY_L_ACCURATE(x)  ({ unsigned long number_of_loops=(unsigned long)x;   \
                                __asm__ volatile ( "L_%=: \n\t"                   \
                                                   "subi %A0,lo8(-(-1)) \n\t"     \
                                                   "sbci %B0,hi8(-(-1)) \n\t"     \
                                                   "sbci %C0,hlo8(-(-1)) \n\t"    \
                                                   "sbci %D0,hhi8(-(-1)) \n\t"    \
                                                   "brne L_%= \n\t"               \
                                                   : /* NO OUTPUT */              \
                                                   : "w" (number_of_loops)        \
                                                 );                               \
                             })                                      




#define TIME_L1_MS      ( 1*(F_CPU/6000) )     /* 1 MILLISECOND  */

#define DELAY_MS(ms)    DELAY_L_ACCURATE( (TIME_L1_MS*(ms)) )

/* Do not use this for delays below 100 microseconds */
#define DELAY_L_US(us)  DELAY_L_ACCURATE( (((us)*(F_CPU/6000))/1000) )

#endif /* #ifndef DELAY_L_ACCURATE */




#endif	/*  #ifndef ONE_WIRE_H */
/*######################################################################################################*/
/*					   T H E   E N D						*/
/*######################################################################################################*/

