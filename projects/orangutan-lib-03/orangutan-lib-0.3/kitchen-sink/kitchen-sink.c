// main.c
//
// Example application using Orangutan-lib
//
// This example is primarily for an Orangutan, though it will
// run on a Baby-Orangutan or an Orangutan-X2 with slightly
// less functionality (no LCD stuff on the Baby-O, no motors
// on the X2, etc.)  There's not really much to this example,
// but it should use most of the functions in the library so you
// can see how evreything works.
//
// Tom Benedict


// When using Orangutan-lib, you MUST set your device type 
// (Orangutan, Baby-Orangutan, or Orangutan-X2) in device.h.  All
// the other .c files in Orangutan-lib include that one file.
// Among other things it sets your F_CPU value if you haven't set
// it on the command line.  This is required for the LCD to work,
// for delays to be accurate, etc.
//
// One other function of the "device.h" file is to determine what
// optional hardware your device has.  For example, the Baby-
// Orangutan has no LCD.  That's ok.  By setting the correct
// device, the LCD code won't load if you're building it for a
// Baby-Orangutan.

#include "device.h"

#include <avr/io.h>

// Now include the subsystems you want to use:

// Commands to use the A/D subsystem (must include analog.c in
// Source Files)
#include "analog.h"

// Commands to use the LCD on the Orangutan (must include lcd.c
// in Source Files)
#include "lcd.h"

// For motor control you can only use one motor control system
// at a time.  Right now the two available are relay-style (fwd,
// rev, brake, coast), and PWM speed control using TIMER0. 
// Include only one!  If you include both you'll get compile
// errors and unpredictable behavior.

// Commands to do PWM speed control on motors (must include 
// pwm.c in Source Files)
#include "pwm.h"

// Commands to do relay-style motor control (must include 
// relay-motor.c in Source Files)
// #include "relay-motor.h"

// Commands to drive the onboard buzzer
#include "buzzer.h"

// Past here the library has been loaded, and the rest is up to you.


// For this example I want to be able to do timed delays, so
// I'm including util/delay.h as well:
#include <util/delay.h>

// And I'm including a delay routine that will let us pause for a 
// couple of seconds.

// Delay for N seconds
void delay_sec(unsigned char sec)
{
	unsigned int cycles;

	// Delay 25ms at a time (38.4ms is the most we can delay with a
	// 20MHz processor, unfortunately.  See the delay.h include file
	// for more info.)

	for(cycles = 0; cycles < (sec * 40); cycles ++)
	{
		_delay_ms(25);
	}
}


// Now for our main routine.  This program doesn't do a whole lot.
// It writes to the LCD, if present, reads the pushbuttons, if
// present, reads from the onboard potentiometer, and uses it to
// set motor speed.  Not much in terms of really DOING anything,
// but it does go through all the commands in the library (with the
// exception of the relay-motor routines), and makes sure they
// work.

int main(void)
{
	// The variables we'll need
	int
		pot = 0,		// Potentiometer
		oldpot = 0,		// Last value of the potentiometer
		dir = 1,		// Motor direction (1 = fwd, -1 = rev)
		olddir = 1;		// Last value of the motor direction

	// Start by defining the onboard hardware we're going to use:

	// The onboard potentiometer is on AD7.  We're reading it in
	// 8-bit mode.  This uses the stuff in analog.h and analog.c:
	#define POT (analog8(7))

// Only include this next bit if we have push-buttons

#ifdef _ENABLE_BUTTONS_

	// And they need to be set up as inputs:
	BUTTON_DDR &= ~((1 << BUTTON0) | (1 << BUTTON1) | (1 << BUTTON2));

#endif // _ENABLE_BUTTONS_

#ifdef _ENABLE_LCD_

	// Start up the LCD
	lcd_init();

	// Write a welcome message, wait two seconds
	lcd_clear();
	lcd_line1();
	lcd_string("O-Lib");
	lcd_line2();
	lcd_string("0.2");
	delay_sec(2);

	// Clear the display and set it up for reading
	// buttons and the potentiometer.
	lcd_clear();
	lcd_line1();
	lcd_string("Spd: 000");
	lcd_line2();
	lcd_string("Dir: Fwd");

#endif // _ENABLE_LCD_

	// Initialize the ADC subsystem:
	analog_init();

#ifdef _ENABLE_LB1836M_MOTORS_

	// Initialize the PWM motor control subsystem:
	pwm_init();

#endif // _ENABLE_LB1836M_MOTORS_

	// Endless loop
	for(;;)
	{
		// Get the value of the potentiometer
		pot = POT;

		// If it or our direction have changed, report it on the LCD,
		// set the new motor speeds, and record the new values.
		if((pot != oldpot) | (dir != olddir))
		{

// Only do LCD stuff on the Orangutan or Orangutan-X2
#ifdef _ENABLE_LCD_

			lcd_gotoxy(4,0);
			lcd_int(pot);
			lcd_gotoxy(4,1);
			if (dir > 0)
				lcd_string("Fwd");
			else
				lcd_string("Rev");

#endif // _ENABLE_LCD

			oldpot = pot;
			olddir = dir;

// Only do PWM motor stuff on the Orangutan or Baby-Orangutan
#ifdef _ENABLE_LB1836M_MOTORS_

			pwm_a(pot * dir);
			pwm_b(pot * dir);

#endif // _ENABLE_LB1836M_MOTORS_

		}

// We can only read the push buttons if this is an Orangutan or
// Orangutan-X2
#ifdef _ENABLE_BUTTONS_

		// If pushbutton 0 is pressed, go forward
		if(BUTTON_PIN & (1 << BUTTON0))
		{
			dir = 1;
		}

		// If pushbutton 1 is pressed, go in reverse.
		if(BUTTON_PIN & (1 << BUTTON1))
		{
			dir = -1;
		}

// The buzzer code won't work on the Baby-O or the Orangutan-X2
// since the Baby-O lacks a buzzer and the Orangutan-X2 controls
// its buzzer through SPI calls
#ifdef _ENABLE_BUZZER_

		// If pushbutton 2 is pressed, sound the buzzer for
		// 0.2 seconds.
		if(BUTTON_PIN & (1 << BUTTON2))
		{
			buzzer(440, 200);
		}

#endif // _ENABLE_BUZZER_

#endif // _ENABLE_BUTTONS_

	}

	return 0;
}
