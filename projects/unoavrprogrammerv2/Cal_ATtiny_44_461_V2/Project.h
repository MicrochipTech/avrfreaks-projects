


//Baud rate set for 38400: replace 38400 with wanted baud rate (Application only not the programmer)
//Options are 960, 1920, 38400, 57600 and 76800

#define Tx_clock						Tx_clock_38400
#define Rx_clock						Rx_clock_38400
#define Start_clock						Start_clock_38400



/******Use these definitions for the ATtiny 461 familay of devices*********

#define	TCNT0							TCNT0L
#define	TIFR0							TIFR
#define	WDTCSR							WDTCR
#define	PCIF0							PCIF
#define TIMSK0							TIMSK
#define TIFR1							TIFR
#define PCINT0_vect						PCINT_vect
#define TIM1_COMPA_vect					TIMER1_COMPA_vect
#define TIM0_COMPA_vect					TIMER0_COMPA_vect

#define set_PCI_mask_on_SCK				PCMSK1	= (1 << PCINT10)					//Used to calibrate internal clock
#define clear_PCI_mask_on_SCK			PCMSK1 &= (~(1 << PCINT10))
#define calibration_active				PCMSK1 & (1 << PCINT10)

#define set_PCI_mask_on_DI				PCMSK0	= (1 << PCINT0)						//Used to detect start bit
#define clear_PCI_mask_on_DI			PCMSK0	&= (~(1 << PCINT0))

#define Enable_PCI_on_DI_pin			GIFR |= 1 << PCIF; GIMSK |= 1 << PCIE1;
#define Disable_PCI_on_DI_pin			GIMSK &= (~(1 << PCIE1));

#define Enable_Timer_1_Interrupt		TIMSK |= (1 << OCIE1A);
#define Disable_Timer_1_Interrupt		TIMSK &= (~(1 << OCIE1A));

#define prescaller_setting				0x44

#define DI_pin							0
#define DO_pin							1
#define USCK_pin						2
#define setup_IO						setup_IO_461
#define DDR_USI							DDRA
#define PIN_USI							PINA
#define PORT_USI						PORTA
***************************************************************************/



/******Use these definitions for the ATtiny 44 familay of devices*********/

#define set_PCI_mask_on_SCK			PCMSK0	= (1 << PCINT4)							//Used to calibrate internal clock
#define clear_PCI_mask_on_SCK		PCMSK0 &= (~(1 << PCINT4))
#define calibration_active			PCMSK0 & (1 << PCINT4)

#define set_PCI_mask_on_DI			PCMSK0	= (1 << PCINT6)							//Used to detect start bit
#define clear_PCI_mask_on_DI		PCMSK0	&= (~(1 << PCINT6))

#define Enable_PCI_on_DI_pin		enable_PCI_on_SCK_pin;
#define Disable_PCI_on_DI_pin		disable_PCI_on_SCK_pin;

#define Enable_Timer_1_Interrupt		TIMSK1 |= (1 << OCIE1A);
#define Disable_Timer_1_Interrupt		TIMSK1 &= (~(1 << OCIE1A));

#define prescaller_setting			2

#define DI_pin						6
#define DO_pin						5
#define USCK_pin					4
#define setup_IO					setup_IO_44
#define DDR_USI						DDRA
#define PIN_USI						PINA
#define PORT_USI					PORTA
/*******************************************************************************/


#define enable_PCI_on_SCK_pin		GIFR |= 1 << PCIF0; GIMSK |= 1 << PCIE0;
#define disable_PCI_on_SCK_pin		GIMSK &= (~(1 << PCIE0));



#include "../Resources/ATtiny_cal.h"
#include "../Resources/USI_subroutines.c"
#include "../Resources/ASKII_and_timer_subroutines.c"
#include "../Resources/cal_subroutines.c"



const char message_1[] PROGMEM = {"\r\n\r\nCalibrating ATtiny"};
const char message_2[] PROGMEM = "\r\nDV/WV, previous OSCCAL values  ";
const char message_3[] PROGMEM = "User cal? Enter 1 to F then x if OK\r\n";







