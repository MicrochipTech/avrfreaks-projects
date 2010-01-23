/*
Author:  Matt Meerian
Date:  December 22, 2004
Target Processor:  ATMEGA8535 from Atmel
Languate used:  AVR GCC compiler (in the "C" language)
Overview:
        This file will be used to test the basic operation of the 128x64 pixel,
        2.8", blue and white, LCD from Optrex.  (Part Number F-51553GNBJ-LW-AB)  The
        hardware interface will be through a demo board from Apollo Display.  (Part
        Number:  F-51553-Eval
		The display will use the serial interface to drive the LCD.  The LCD is
		set to the "80 series" (intel) interface.
*/

//defines
#define F_CPU              4000000  //4Mhz
#define UART_BAUD_RATE     9600     //9600 baud
#define UART_BAUD_SELECT (F_CPU/(UART_BAUD_RATE*16l)-1)
#define defLCDDataPort     PORTA //the port for the 8 data lines going to the LCD
#define defLCDContPort     PORTB //the port for the RS and RW lines on the LCD
#define defPORTD		   PORTD	//misc control lines
#define defPushBtnPort     PINA //the port for the 3 push buttons

//LCD Data Port defines
#define defLcdSCL		6		//LCD data port serial Clock Line (SCL)
#define defLcdS1		7		//LCD data port data line for the serial clock

//LCD Control Port defines
#define defLcdCS1       0   //LCD Chip Select, Low : Active
#define defLcdRES       1   //LCD Reset Signal, Low : Reset
#define defLcdA0        2   //LCD Address/Data:  High: D0~D7 are Display Data, Low: D0~D7 are Instructions
#define defLcdWR        3   //LCD Write signal:  Low: Active
#define defLcdRD        4   //LCD Read signal:  Low: Active

//Misc control lines for PORTD
#define defAlarmPin		0	//a high sounds the alarm clock
#define defBacklightOn	7	//a high turns on the backlight

//Push button port defines
#define defUpBtn		PA2	//the "up" push button
#define defDownBtn		PA3	//the "down" push button
#define defEnterBtn		PA1	//the "enter" push button

/////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------*/
/* **** External Hardware Definitions **** */
#define NUMBER_OF_COLUMNS 128
#define NUMBER_OF_ROWS 64
#define NUMBER_OF_PAGES 8
#define FIRST_PAGE 0
#define FIRST_COLUMN 0
#define LAST_PAGE 7
#define LAST_COLUMN 127
/* **** Global Function Prototypes **** */
//void Write_Display(unsigned char command, unsigned char data);
//unsigned char  Read_Display(unsigned char command);
/*---------------------------------------------------------------------------*/
/* These are the flags that are passed to Write_Display() or Read_Display()
routines. */
//#define CMD 0x01
//#define DATA 0xFF
/*---------------------------------------------------------------------------*/
/* The individual bit declarations of the control lines. */
/*#define CS1_BIT 0x80
#define A0_BIT 0x40
#define WR_BIT 0x20
#define RD_BIT 0x10
#define RES_BIT 0x08
#define C86_BIT 0x04
*/
/*---------------------------------------------------------------------------*/
/* The BUS_RELEASE define is used to the control bus output so that all control
signals are pulled high. This ensures that when the microcontroller gains
control of the bus, it does not change the direction of the bus to outputs
while the display is driving the bus. */
#define BUS_RELEASE 0xF8
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* The following definitions are the command codes that are passed to the
display via the data bus. */
#define DISPLAY_ON 0xAF
#define DISPLAY_OFF 0xAE
#define START_LINE_SET 0x40
#define PAGE_ADDRESS_SET 0xB0
/* The Column Address is a two byte operation that writes the most significant
bits of the address to D3 - D0 and then writes the least significant bits to
D3- D0. Since the Column Address auto increments after each write, direct
access is infrequent. */
#define COLUMN_ADDRESS_HIGH 0x10
#define COLUMN_ADDRESS_LOW 0x00
#define ADC_SELECT_NORMAL 0xA0
#define ADC_SELECT_REVERSE 0xA1
#define DISPLAY_NORMAL 0xA6
#define DISPLAY_REVERSE 0xA7
#define ALL_POINTS_ON 0xA5
#define LCD_BIAS_1_9 0xA2
#define LCD_BIAS_1_7 0xA3
#define READ_MODIFY_WRITE 0xE0
#define END 0xEE
#define RESET_DISPLAY 0xE2
#define COMMON_OUTPUT_NORMAL 0xC0
#define COMMON_OUTPUT_REVERSE 0xC8
/* The power control set value is obtained by OR'ing the values together to
create the appropriate data value. For example:
data = (POWER_CONTROL_SET | BOOSTER_CIRCUIT |
VOLTAGE_REGULATOR | VOLTAGE_FOLLOWER);
Only the bits that are desired need be OR'ed in because the initial value
of POWER_CONTROL_SET sets them to zero. */
#define POWER_CONTROL_SET 0x28
#define BOOSTER_CIRCUIT 0x04
#define VOLTAGE_REGULATOR 0x02
#define VOLTAGE_FOLLOWER 0x01
/* The initial value of the V5_RESISTOR_RATIO sets the Rb/Ra ratio to the
smallest setting. The valid range of the ratio is:
0x20 <= V5_RESISTOR_RATIO <= 0x27 */
#define V5_RESISTOR_RATIO 0x26
/* When the electronic volume command is input, the electronic volume register
set command becomes enabled. Once the electronic volume mode has been set,
no other command except for the electronic volume register command can be
used. Once the electronic volume register set command has been used to set
data into the register, then the electronic volume mode is released. */
#define ELECTRONIC_VOLUME_SET 0x81
#define ELECTRONIC_VOLUME_INIT 0x20
//------------------------------------------------------------------------------------
//PORTA equates
#define defBattVolt        0     //input, ADC battery voltage
#define defPA1             1     //input
#define defDipSw1          2     //input, DIP switch 1, on=has violator
#define defDipSw2          3     //input, DIP switch 2, on=has overspeed
#define defDipSw3          4     //input, DIP switch 3
#define defDipSw4          5     //input, DIP switch 4
#define defSegB            6     //output, 7 segment display, segment B
#define defSegA            7     //output, 7 segment display, segment A
//PortB equates
#define defOneCath         0     //The ones digit cathode line
#define defTenCath         1     //The tens digit cathode line
#define defHundCath        2     //The Hundreds digit cathode line
#define defPB3             3
#define defPB4             4
#define defMOSI            5     //in circuit programming
#define defMISO            6     //in circuit programming
#define defSCK             7     //in circuit programming
//PORTC equates
#define defSCL             0
#define defSDA             1
#define defSegC            2     //output, 7 segment display, segment C
#define defSegD            3     //output, 7 segment display, segment D
#define defSegE            4     //output, 7 segment display, segment E
#define defSegF            5     //output, 7 segment display, segment F
#define defSegG            6     //output, 7 segment display, segment G
#define defDP              7     //output, 7 segment display, decimal point
//PORTD equates
#define defRxD             0     //RS232 data in
#define defTxD             1     //RS232 data out
#define defStrobe          2     //output, strobe light output for dummy strobe
#define defPBSw1           3     //input, pushbutton switch 1, set violator
#define defPBSw2           4     //input, pushbutton switch 2, set overspeed
#define defPBSw3           5     //input, pushbutton switch 3, down
#define defPBSw4           6     //input, pushbutton switch 4, up
#define defPBSw5           7     //input, pushbutton switch 5, battery voltage
//---------------------------------------------------------------------------------


#define defTock            311      //There are 311 overflows to make 20mS on a 64.39uS timer
#define defStateTm         200      //There are 50, 20mS counts to make 1 sec

#define defBtnDbn          14       //this is the front panel button debounce, was orginally 10

#define def50Min			1
#define def50Max			5
#define defTab				10		//number of pixels to move in when displaying small text

#define defState50HighlightMin 1
#define defState50HighlightMax 7

#define defState51HighlightMin 0	//display the "ode to spot"
#define defState51HighlightMax 0

#define defState64HighlightMin 0	//display the "about this clock" information
#define defState64HighlightMax 0


#define defDlyAfterBtnPress		150	//This the blinking digit delay after a button is pressed
#define defSettingBlinkOff		80	//to get the amount of time the audio or range will be off
	// for the blinking period, use (0.02 Seconds) * (defSettingBlinkOff - defSettingBlinkOn)
#define defSettingBlinkOn		40	//to get the amount of time the audio or range will be on
	// for the blinking period, use (0.02 Seconds) * (defSettingBlinkOn)

//#undef defS1D15605			//selects the new Optrex Displays, F-51852
#define defS1D15605 1		//selects the older Optrex Displays, F-51553 (Blue on White)

//EEPROM equates:
#define defKey1            34       //
#define defKey2            28       //
#define defKey3            93       //
#define EEMonth             1     // Position 1:  Start of logging Month
#define EEDay               2     // Position 2:  Start of logging Day
#define EEYear              3     // Position 3:  Start of logging year
#define defAlarmHour __attribute__ ((section (".eeprom")))
#define defAlarmMinute __attribute__ ((section (".eeprom")))

#define EEKey1              8     // Position 8:
#define EEKey2              9     // Position 9:
#define EEKey3             10     // Position 10:



