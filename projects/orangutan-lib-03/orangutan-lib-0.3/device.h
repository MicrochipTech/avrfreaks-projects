#ifndef _DEVICE_
#define _DEVICE_



// Declare your device.  The choices are ORANGUTAN, BABY_ORANGUTAN,
// and ORANGUTAN_X2.  Uncomment only one of the following three
// lines:

#define ORANGUTAN
// #define BABY_ORANGUTAN
// #define ORANGUTAN_X2


// Declare what timer you want to use for PWM.  Choices are TIMER0
// or TIMER2 for the ATMega168:

#define PWM_TIMER0
// #define PWM_TIMER2





/////////////////////////////////////////////////////////////////////
//                                                                 //
// Make no changes past here.  Once you set the device, all the    //
// other assumptions can be made.                                  //
//                                                                 //
/////////////////////////////////////////////////////////////////////



// Include stdint.h - This has definitions for standard integer types
// broken down by signed/unsigned, and how many bits the type 
// occupies in memory.  Since that's typically what we're interested
// in knowing for a microprocessor application, that's a much more
// logical way of defining variables.

#include <stdint.h>

/////////////////////////////////////////////////////////////////////
// Baby-Orangutan                                                  //
/////////////////////////////////////////////////////////////////////

#ifdef BABY_ORANGUTAN

#ifndef F_CPU
#define F_CPU 20000000UL
#endif //!F_CPU

// Enable or disable different subsystems based on the supported
// hardware:

#define _ENABLE_LB1836M_MOTORS_
// #define _ENABLE_LCD_
// #define _ENABLE_BUZZER_
// #define _ENABLE_BUTTONS_
#define _ENABLE_LEDS_

// For the supported hardware, define ports and pins:

// Serial Clock:

#define XCK0_DDR				DDRD
#define XCK0					PD4

// ADC:

#define ADC_DDR				DDRC
#define ADC_PORT			PORTC

// Onboard LED(s):

#define LED_DDR				DDRD
#define LED_PORT			PORTD
#define LED0				PD1	// Red

// Onboard Button(s):

// Not Applicable

// Motor Pinouts:

// Motor A:
#define IN1_DDR				DDRB
#define IN1_PORT			PORTB
#define IN1_BIT				PB1
#define IN2_DDR				DDRD
#define IN2_PORT			PORTD
#define IN2_BIT				PD5

// Motor B:
#define IN3_DDR				DDRB
#define IN3_PORT			PORTB
#define IN3_BIT				PB2
#define IN4_DDR				DDRD
#define IN4_PORT			PORTD
#define IN4_BIT				PD6

// PWM Definitions:

#ifdef PWM_TIMER0

#define _TCCRA				TCCR0A
#define _TCCRB				TCCR0B
#define _TIMSK				TIMSK0
#define _OCRA				OCR0A
#define _OCRB				OCR0B
#define _PRESCALE_MODE		4
#define _OCIEA				OCIE0A
#define _OCIEB				OCIE0B
#define _TOIE				TOIE0
#define _OUTPUT_COMPARE_A	SIG_OUTPUT_COMPARE0A
#define _OUTPUT_COMPARE_B	SIG_OUTPUT_COMPARE0B
#define _OVERFLOW			SIG_OVERFLOW0

#endif // PWM_TIMER0

#ifdef PWM_TIMER2

#define _TCCRA				TCCR2A
#define _TCCRB				TCCR2B
#define _TIMSK				TIMSK2
#define _OCRA				OCR2A
#define _OCRB				OCR2B
#define _PRESCALE_MODE		6
#define _OCIEA				OCIE2A
#define _OCIEB				OCIE2B
#define _TOIE				TOIE2
#define _OUTPUT_COMPARE_A	SIG_OUTPUT_COMPARE2A
#define _OUTPUT_COMPARE_B	SIG_OUTPUT_COMPARE2B
#define _OVERFLOW			SIG_OVERFLOW2

#endif // PWM_TIMER2

// Pin Change Interrupt Definitions:

#define PCI_PIN0	PINB
#define PCI_PIN1	PINC
#define PCI_PIN2	PIND
#define PCI_NUM		3

// SPI Definitions

#define SPI_DDR		DDRB
#define SPI_MISO	PB4
#define SPI_MOSI	PB3
#define SPI_SCK		PB5
#define SPI_SS		PB2

#define SPI_SPCR	SPCR
#define SPI_SPDR	SPDR
#define SPI_SPSR	SPSR
#define SPI_SPIF	SPIF
#define SPI_SPE		SPE
#define SPI_MSTR	MSTR
#define SPI_SPR0	SPR0

#endif //BABY_ORANGUTAN


/////////////////////////////////////////////////////////////////////
// Orangutan                                                       //
/////////////////////////////////////////////////////////////////////

#ifdef ORANGUTAN

#ifndef F_CPU
#define F_CPU 8000000UL
#endif //!F_CPU

// Enable or disable different subsystems based on the supported
// hardware:

#define _ENABLE_LB1836M_MOTORS_	
#define _ENABLE_LCD_
#define _ENABLE_BUZZER_
#define _ENABLE_BUTTONS_
#define _ENABLE_LEDS_

// For the supported hardware, define ports and pins:

// Serial Clock:

#define XCK0_DDR				DDRD
#define XCK0					PD4

// ADC:

#define ADC_DDR				DDRC
#define ADC_PORT			PORTC

// Onboard buzzer:

#define BUZZER_DDR			DDRB
#define BUZZER_PORT			PORTB
#define BUZZER_BIT			PB0

// Onboard LED(s):

#define LED_DDR				DDRD
#define LED_PORT			PORTD
#define LED0				PD1	// Red

// Onboard Button(s):

#define BUTTON_DDR			DDRB
#define BUTTON_PIN			PINB
#define BUTTON0				PB3
#define BUTTON1				PB4
#define BUTTON2				PB5

// LCD Pinouts:

// On the Orangutan all the LCD control lines live on a single
// port, PORTD:

#define LCD_CTRL_DDR	DDRD
#define LCD_CTRL_PORT	PORTD
#define LCD_RW			PD3
#define LCD_RS			PD2
#define LCD_E			PD4

// On the Orangutan the LCD's data lines are split between two
// ports, PORTB and PORTD.

#define LCD_PORTB_MASK		((1 << PB3) | (1 << PB4) | (1 << PB5))
#define LCD_PORTD_MASK		(1 << PD7)

// Since we're only using four data lines, and since the pins they're
// wired up to don't start with 0, we need to shift them into
// position in order to load their values into the LCD.  Port B uses
// bits 3, 4, and 5, so we take our data and shift it by three
// positions to make things line up.  At that point our data
// looks like this:
//
//	PortB:     7 6 5 4 3 2 1 0
//  LCD Data:    3 2 1 0
//
// The last bit for the LCD needs to go out PortD on pin 7, so we
// do one last shift to make its data line up.
//
// Sorry this is so confusing.  It'll make more sense in the 
// lcd_nibble() routine in lcd.c/.h, which is where these shift 
// values actually get used.

#define LCD_PORTB_SHIFT		3
#define LCD_PORTD_SHIFT		1

// Data port bit where the LCD's busy flag lives

#define LCD_BF			7

// Size of the LCD (each starts at zero, so an 8x2 display would use
// the numbers 7 and 1.)

#define LCD_MAX_X		7
#define LCD_MAX_Y		1

// Motor Pinouts:

// Motor A:
#define IN1_DDR				DDRB
#define IN1_PORT			PORTB
#define IN1_BIT				PB1
#define IN2_DDR				DDRD
#define IN2_PORT			PORTD
#define IN2_BIT				PD5

// Motor B:
#define IN3_DDR				DDRB
#define IN3_PORT			PORTB
#define IN3_BIT				PB2
#define IN4_DDR				DDRD
#define IN4_PORT			PORTD
#define IN4_BIT				PD6

// PWM Definitions:

#ifdef PWM_TIMER0

#define _TCCRA				TCCR0A
#define _TCCRB				TCCR0B
#define _TIMSK				TIMSK0
#define _OCRA				OCR0A
#define _OCRB				OCR0B
#define _PRESCALE_MODE		4
#define _OCIEA				OCIE0A
#define _OCIEB				OCIE0B
#define _TOIE				TOIE0
#define _OUTPUT_COMPARE_A	SIG_OUTPUT_COMPARE0A
#define _OUTPUT_COMPARE_B	SIG_OUTPUT_COMPARE0B
#define _OVERFLOW			SIG_OVERFLOW0

#endif // PWM_TIMER0

#ifdef PWM_TIMER2

#define _TCCRA				TCCR2A
#define _TCCRB				TCCR2B
#define _TIMSK				TIMSK2
#define _OCRA				OCR2A
#define _OCRB				OCR2B
#define _PRESCALE_MODE		6
#define _OCIEA				OCIE2A
#define _OCIEB				OCIE2B
#define _TOIE				TOIE2
#define _OUTPUT_COMPARE_A	SIG_OUTPUT_COMPARE2A
#define _OUTPUT_COMPARE_B	SIG_OUTPUT_COMPARE2B
#define _OVERFLOW			SIG_OVERFLOW2

#endif // PWM_TIMER2

// Pin Change Interrupt Definitions:

#define PCI_PIN0	PINB
#define PCI_PIN1	PINC
#define PCI_PIN2	PIND
#define PCI_NUM		3

// SPI Definitions

#define SPI_DDR		DDRB
#define SPI_MISO	PB4
#define SPI_MOSI	PB3
#define SPI_SCK		PB5
#define SPI_SS		PB2

#define SPI_SPCR	SPCR
#define SPI_SPDR	SPDR
#define SPI_SPSR	SPSR
#define SPI_SPIF	SPIF
#define SPI_SPE		SPE
#define SPI_MSTR	MSTR
#define SPI_SPR0	SPR0


#endif //ORANGUTAN


/////////////////////////////////////////////////////////////////////
// Orangutan-X2                                                    //
/////////////////////////////////////////////////////////////////////

#ifdef ORANGUTAN_X2

#ifndef F_CPU
#define F_CPU 20000000UL
#endif //!F_CPU

// Enable or disable different subsystems based on the supported
// hardware.  The X2 has a completely different LCD, motor, and buzzer
// setup, so these are defined separately:

#define _ENABLE_X2_MOTORS_
#define _ENABLE_LCD_
#define _ENABLE_X2_BUZZER_
#define _ENABLE_BUTTONS_
#define _ENABLE_LEDS_

// For the supported hardware, define ports and pins:

// Serial Clock:

#define XCK0_DDR				DDRB
#define XCK0					PB0

// ADC:

#define ADC_DDR				DDRA
#define ADC_PORT			PORTA

// Onboard LED(s):

#define LED_DDR				DDRC
#define LED_PORT			PORTC
#define LED0				PC0	// Yellow
#define LED1				PC2	// Green
#define LED2				PC3	// Red
#define LED3				PC5	// Green
#define LED4				PC7	// Red

// Onboard Button(s):

#define BUTTON_DDR			DDRC
#define BUTTON_PIN			PINC
#define BUTTON0				PC1
#define BUTTON1				PC4
#define BUTTON2				PC6

// LCD Pinouts:

// On the Orangutan-X2 all the LCD control lines live on a single
// port, PORTB:

#define LCD_CTRL_DDR	DDRB
#define LCD_CTRL_PORT	PORTB
#define LCD_RW			PB1
#define LCD_RS			PB0
#define LCD_E			PB3

// On the Orangutan-X2 the LCD's data lines are on a single port, PORTC.
// The entire port is used, so no special masks are required, and no
// shifts are associated with any of the data lines.

#define LCD_DATA_DDR	DDRC
#define LCD_DATA_PORT	PORTC
#define LCD_DATA_PIN	PINC

// Data port bit where the LCD's busy flag lives

#define LCD_BF			7

// Size of the LCD (each starts at zero, so a 20x4 display would use
// the numbers 19 and 3.)

#define LCD_MAX_X		19
#define LCD_MAX_Y		3

// Pin Change Interrupt Definitions:

#define PCI_PIN0	PINA
#define PCI_PIN1	PINB
#define PCI_PIN2	PINC
#define PCI_PIN3	PIND
#define PCI_NUM		4

// SPI Definitions

#define SPI_DDR		DDRB
#define SPI_MISO	PB6
#define SPI_MOSI	PB5
#define SPI_SCK		PB7
#define SPI_SS		PB4

#define SPI_SPCR	SPCR0
#define SPI_SPDR	SPDR0
#define SPI_SPSR	SPSR0
#define SPI_SPIF	SPIF0
#define SPI_SPE		SPE0
#define SPI_MSTR	MSTR0
#define SPI_SPR0	SPR00

#endif //ORANGUTAN_X2


/////////////////////////////////////////////////////////////////////
// subset ofData Types from AVRLIB for code imported from AVRLIB   //
/////////////////////////////////////////////////////////////////////
#ifndef AVRLIBTYPES_H
#define AVRLIBTYPES_H
// datatype definitions macros
typedef unsigned char  u08;
typedef   signed char  s08;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned long  u32;
typedef   signed long  s32;
typedef unsigned long long u64;
typedef   signed long long s64;

#define TRUE 1
#define FALSE 0

#endif // AVRLIBTYPES_H
 
#endif //_DEVICE_
