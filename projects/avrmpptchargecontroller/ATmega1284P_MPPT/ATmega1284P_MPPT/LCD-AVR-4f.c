/****************************************************************************
 LCD-AVR-4f.c  - Use an HD44780U based LCD with an Atmel ATmega processor

 Copyright (C) 2013 Donald Weiman    (weimandn@alfredstate.edu)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/****************************************************************************
 File:    LCD-AVR-4f.c
 Date:    September 16, 2013

 Target:    ATmega328
 Compiler:    avr-gcc (AVR Studio 6)
 Author:    Donald Weiman

 Summary:    8-bit data interface, with busy flag implemented.
 Any LCD pin can be connected to any available I/O port.
 Includes a simple write string routine.
 */
/******************************* Program Notes ******************************

 This program uses a 4-bit data interface and it uses the busy
 flag to determine when the LCD controller is ready.  The LCD
 RW line (pin 5) must therefore be connected to the uP.

 The use of the busy flag does not mean that all of the software
 time delays have been eliminated.  There are still several
 required in the LCD initialization routine where the busy flag
 cannot yet be used.  These delays are have been implemented at
 least twice as long as called for in the data sheet to
 accommodate some of the out of spec displays that may show up.
 There are also some other software time delays required to
 implement timing requirements such as setup and hold times for
 the various control signals.

 ***************************************************************************

 The four data lines as well as the three control lines may be
 implemented on any available I/O pin of any port.  These are
 the connections used for this program:

 -----------                   ----------
 | ATmega328 |                 |   LCD    |
 |           |                 |          |
 |        PD7|---------------->|D7        |
 |        PD6|---------------->|D6        |
 |        PD5|---------------->|D5        |
 |        PD4|---------------->|D4        |
 |           |                 |D3        |
 |           |                 |D2        |
 |           |                 |D1        |
 |           |                 |D0        |
 |           |                 |          |
 |        PB1|---------------->|E         |
 |        PD3|---------------->|RW        |
 |        PB0|---------------->|RS        |
 -----------                   ----------

 **************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include "LCD-AVR-4f.h"

char blank[] = "                    ";

/******************************* Main Program Code *************************/
void init_lcd_avr_4f(void) {
// configure the microprocessor pins for the data lines
	lcd_D7_ddr |= (1 << lcd_D7_bit);                  // 4 data lines - output
	lcd_D6_ddr |= (1 << lcd_D6_bit);
	lcd_D5_ddr |= (1 << lcd_D5_bit);
	lcd_D4_ddr |= (1 << lcd_D4_bit);

// configure the microprocessor pins for the control lines
	lcd_E_ddr |= (1 << lcd_E_bit);                    // E line - output
	lcd_RS_ddr |= (1 << lcd_RS_bit);                  // RS line - output
	lcd_RW_ddr |= (1 << lcd_RW_bit);                  // RW line - output

// initialize the LCD controller as determined by the defines (LCD instructions)
	lcd_init_4f();           // initialize the LCD display for a 4-bit interface
}

/******************************* End of Main Program Code ******************/

/*============================== 4-bit LCD Functions ======================*/
/*
 Name:     lcd_init_4f
 Purpose:  initialize the LCD module for a 4-bit data interface
 Entry:    equates (LCD instructions) set up for the desired operation
 Exit:     no parameters
 Notes:    uses the busy flag instead of time delays when possible
 */
void lcd_init_4f(void) {
// Power-up delay
	_delay_ms(100);                                 // initial 40 mSec delay

// IMPORTANT - At this point the LCD module is in the 8-bit mode and it is expecting to receive
//   8 bits of data, one bit on each of its 8 data lines, each time the 'E' line is pulsed.
//
// Since the LCD module is wired for the 4-bit mode, only the upper four data lines are connected to
//   the microprocessor and the lower four data lines are typically left open.  Therefore, when
//   the 'E' line is pulsed, the LCD controller will read whatever data has been set up on the upper
//   four data lines and the lower four data lines will be high (due to internal pull-up circuitry).
//
// Fortunately the 'FunctionReset' instruction does not care about what is on the lower four bits so
//   this instruction can be sent on just the four available data lines and it will be interpreted
//   properly by the LCD controller.  The 'lcd_write_4' subroutine will accomplish this if the
//   control lines have previously been configured properly.

// Set up the RS, E, and RW lines for the 'lcd_write_4' function.
	lcd_RS_port &= ~(1 << lcd_RS_bit); // select the Instruction Register (RS low)
	lcd_E_port &= ~(1 << lcd_E_bit);             // make sure E is initially low
	lcd_RW_port &= ~(1 << lcd_RW_bit);           // write to LCD module (RW low)

// Reset the LCD controller
	lcd_write_4(lcd_FunctionReset);              // first part of reset sequence
	_delay_ms(5);                                  // 4.1 mS delay (min)

	lcd_write_4(lcd_FunctionReset);             // second part of reset sequence
	_delay_us(200);                                 // 100 uS delay (min)

	lcd_write_4(lcd_FunctionReset);              // third part of reset sequence
	_delay_us(80);                    // this delay is omitted in the data sheet

// Preliminary Function Set instruction - used only to set the 4-bit mode.
// The number of lines or the font cannot be set at this time since the controller is still in the
//  8-bit mode, but the data transfer mode can be changed since this parameter is determined by one
//  of the upper four bits of the instruction.

	lcd_write_4(lcd_FunctionSet4bit);               // set 4-bit mode
//  _delay_us(80);                                  //  40 uS delay (min)
// --> from this point on the busy flag is available <--

// Function Set instruction
	lcd_check_BF_4();                       // make sure LCD controller is ready
	lcd_write_instruction_4f(lcd_FunctionSet4bit);  // set mode, lines, and font

// The next three instructions are specified in the data sheet as part of the initialization routine,
//  so it is a good idea (but probably not necessary) to do them just as specified and then redo them
//  later if the application requires a different configuration.

// Display On/Off Control instruction
	lcd_check_BF_4();
	lcd_write_instruction_4f(lcd_DisplayOff);       // turn display OFF

// Clear Display instruction
	lcd_check_BF_4();
	lcd_write_instruction_4f(lcd_Clear);            // clear display RAM

// ; Entry Mode Set instruction
	lcd_check_BF_4();
	lcd_write_instruction_4f(lcd_EntryMode); // set desired shift characteristics

// This is the end of the LCD controller initialization as specified in the data sheet, but the display
//  has been left in the OFF condition.  This is a good time to turn the display back ON.

// Display On/Off Control instruction
	lcd_check_BF_4();
	lcd_write_instruction_4f(lcd_DisplayOn);        // turn the display ON
}

/*...........................................................................
 Name:     lcd_write_string_4f
 ; Purpose:  display a string of characters on the LCD
 Entry:    (theString) is the string to be displayed
 Exit:     no parameters
 Notes:    uses the busy flag instead of time delays
 */
void lcd_write_string_4f(char theString[]) {
	volatile int i = 0;                             // character counter*/
	while (theString[i] != 0) {
		lcd_check_BF_4();                   // make sure LCD controller is ready
		lcd_write_character_4f(theString[i]);
		i++;
	}
}

/*...........................................................................
 Name:     lcd_write_character_4f
 Purpose:  send a byte of information to the LCD data register
 Entry:    (theData) is the information to be sent to the data register
 Exit:     no parameters
 Notes:    configures RW (busy flag is implemented)
 */
void lcd_write_character_4f(uint8_t theData) {
	lcd_RW_port &= ~(1 << lcd_RW_bit);           // write to LCD module (RW low)
	lcd_RS_port |= (1 << lcd_RS_bit);      // select the Data Register (RS high)
	lcd_E_port &= ~(1 << lcd_E_bit);             // make sure E is initially low
	lcd_write_4(theData);                  // write the upper 4-bits of the data
	lcd_write_4(theData << 4);             // write the lower 4-bits of the data
}

/*...........................................................................
 Name:     lcd_write_instruction_4f
 Purpose:  send a byte of information to the LCD instruction register
 Entry:    (theInstruction) is the information to be sent to the instruction register
 Exit:     no parameters
 Notes:    configures RW (busy flag is implemented)
 */
void lcd_write_instruction_4f(uint8_t theInstruction) {
    lcd_check_BF_4();
	lcd_RW_port &= ~(1 << lcd_RW_bit);           // write to LCD module (RW low)
	lcd_RS_port &= ~(1 << lcd_RS_bit); // select the Instruction Register (RS low)
	lcd_E_port &= ~(1 << lcd_E_bit);             // make sure E is initially low
	lcd_write_4(theInstruction);           // write the upper 4-bits of the data
	lcd_write_4(theInstruction << 4);      // write the lower 4-bits of the data
}

/*...........................................................................
 Name:     lcd_write_4
 Purpose:  send a nibble (4-bits) of information to the LCD module
 Entry:    (theByte) contains a byte of data with the desired 4-bits in the upper nibble
 RS is configured for the desired LCD register
 E is low
 RW is low
 Exit:     no parameters
 Notes:    use either time delays or the busy flag
 */
void lcd_write_4(uint8_t theByte) {
	lcd_D7_port &= ~(1 << lcd_D7_bit);                     // assume data is '0'
	if (theByte & 1 << 7)
		lcd_D7_port |= (1 << lcd_D7_bit);     // make data = '1' if required

	lcd_D6_port &= ~(1 << lcd_D6_bit);               // repeat for each data bit
	if (theByte & 1 << 6)
		lcd_D6_port |= (1 << lcd_D6_bit);

	lcd_D5_port &= ~(1 << lcd_D5_bit);
	if (theByte & 1 << 5)
		lcd_D5_port |= (1 << lcd_D5_bit);

	lcd_D4_port &= ~(1 << lcd_D4_bit);
	if (theByte & 1 << 4)
		lcd_D4_port |= (1 << lcd_D4_bit);

	// write the data
	// 'Address set-up time' (40 nS)
	lcd_E_port |= (1 << lcd_E_bit);                   // Enable pin high
	_delay_us(1); // implement 'Data set-up time' (80 nS) and 'Enable pulse width' (230 nS)
	lcd_E_port &= ~(1 << lcd_E_bit);                  // Enable pin low
	_delay_us(1); // implement 'Data hold time' (10 nS) and 'Enable cycle time' (500 nS)
}

/*...........................................................................
 Name:     lcd_check_BF_4
 Purpose:  check busy flag, wait until LCD is ready
 Entry:    no parameters
 Exit:     no parameters
 Notes:    main program will hang if LCD module is defective or missing
 data is read while 'E' is high
 both nibbles must be read even though desired information is only in the high nibble
 */
void lcd_check_BF_4(void) {
	uint8_t busy_flag_copy;                         // busy flag 'mirror'

	lcd_D7_ddr &= ~(1 << lcd_D7_bit);          // set D7 data direction to input
	lcd_RS_port &= ~(1 << lcd_RS_bit); // select the Instruction Register (RS low)
	lcd_RW_port |= (1 << lcd_RW_bit);          // read from LCD module (RW high)

	do {
		busy_flag_copy = 0;                     // initialize busy flag 'mirror'
		lcd_E_port |= (1 << lcd_E_bit);               // Enable pin high
		_delay_us(1); // implement 'Delay data time' (160 nS) and 'Enable pulse width' (230 nS)

		busy_flag_copy |= (lcd_D7_pin & (1 << lcd_D7_bit)); // get actual busy flag status

		lcd_E_port &= ~(1 << lcd_E_bit);              // Enable pin low
		_delay_us(1); // implement 'Address hold time' (10 nS), 'Data hold time' (10 nS), and 'Enable cycle time' (500 nS )

// read and discard alternate nibbles (D3 information)
		lcd_E_port |= (1 << lcd_E_bit);               // Enable pin high
		_delay_us(1); // implement 'Delay data time' (160 nS) and 'Enable pulse width' (230 nS)
		lcd_E_port &= ~(1 << lcd_E_bit);              // Enable pin low
		_delay_us(1); // implement 'Address hold time (10 nS), 'Data hold time' (10 nS), and 'Enable cycle time' (500 nS )

	} while (busy_flag_copy);               // check again if busy flag was high

// arrive here if busy flag is clear -  clean up and return
	lcd_RW_port &= ~(1 << lcd_RW_bit);           // write to LCD module (RW low)
	lcd_D7_ddr |= (1 << lcd_D7_bit);        // reset D7 data direction to output
}

void lcd_clear_line(uint8_t line) {
	lcd_check_BF_4();
	lcd_write_instruction_4f(lcd_SetCursor | line);
	lcd_write_string_4f(blank);
}

void lcd_clear(void) {
	lcd_clear_line(lcd_line_one);
	lcd_clear_line(lcd_line_two);
	lcd_clear_line(lcd_line_three);
	lcd_clear_line(lcd_line_four);
}

void lcd_write_float_4_decimal(float value, uint8_t line) {
	char voltage[20];
	float i_voltage = value;
	itoa(i_voltage, voltage, 10);
	lcd_clear_line(line);
	lcd_check_BF_4();
	lcd_write_instruction_4f(lcd_SetCursor | line);
	lcd_write_string_4f(voltage);
	lcd_write_string_4f(".");
	uint16_t i_voltage_f = (i_voltage - (uint16_t) i_voltage) * 10000;
	itoa(i_voltage_f, voltage, 10);
	if (i_voltage_f < 1000) {
		lcd_write_string_4f("0");
		if (i_voltage_f < 100) {
			lcd_write_string_4f("0");
			if (i_voltage_f < 10) {
				lcd_write_string_4f("0");
				if (i_voltage_f == 0) {
					lcd_write_string_4f("0");
					return;
				}
			}
		}
	}
	lcd_write_string_4f(voltage);
}

void lcd_write_float_1_decimal(float value, uint8_t line) {
	char voltage[20];
	float i_voltage = value;
	itoa(i_voltage, voltage, 10);
	lcd_clear_line(line);
	lcd_check_BF_4();
	lcd_write_instruction_4f(lcd_SetCursor | line);
	lcd_write_string_4f(voltage);
	lcd_write_string_4f(".");
	uint16_t i_voltage_f = (i_voltage - (uint16_t) i_voltage) * 10;
	itoa(i_voltage_f, voltage, 10);
	if (i_voltage_f == 0) {
		lcd_write_string_4f("0");
		return;
	}
	lcd_write_string_4f(voltage);
}
