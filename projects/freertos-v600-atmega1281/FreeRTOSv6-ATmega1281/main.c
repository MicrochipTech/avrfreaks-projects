////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////    main.c 
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

// Anders Green // LinaRacing.com
// port of FreeRTOS v6.0.0 for the ATMega1281
// Tested on a board and verified with a scope.
// All it does is toggle a couple pins, though, so any other
// stuff you're doing, you've got to figure out.
//
// Apologies for renaming the files, but for me, prefixing them
// with "os_" allows me to group them within my IDE. If you
// rename them back you'll need to fix a couple of includes.


#include <avr/io.h>
#include <avr/pgmspace.h> 
#include "os_FreeRTOS.h"
#include "os_task.h"


void InitPorts(void);
void InitTimers(void);

static void TaskBlinkPortC(void *pvParameters);
static void TaskBlinkPortA(void *pvParameters);

    int 
main(void) 
    {     
    InitPorts();
    InitTimers();

    xTaskCreate(
		TaskBlinkPortC
		,  (const signed portCHAR *)"BlinkC"
		,  128
		,  NULL
		,  2
		,  NULL );


    xTaskCreate(
		TaskBlinkPortA
		,  (const signed portCHAR *)"BlinkA"
		,  128
		,  NULL
		,  1
		,  NULL );



    vTaskStartScheduler();

    //and finally, to eliminat compiler warnings....
    return(0);
    }



    static void 
TaskBlinkPortC(void *pvParameters)
    {
    char bits=0;
    while(1)
        {
        PORTC=bits;
        bits=~bits;
        vTaskDelay(5);
        }
    }

    static void 
TaskBlinkPortA(void *pvParameters)
    {
    char bits=0;
    while(1)
        {
        PORTA=bits;
        bits=~bits;
        vTaskDelay(1);
        }
    }



    //--------Init Ports --------------------
	// Input/Output Ports initialization
    // Setting DDRx to 1 sets as output
    void 
InitPorts(void)
    {

	// Port A initialization
    // 0 - output - lcd
    // 1 - output - lcd
    // 2 - output - lcd
    // 3 - output - lcd
    // 4 - output - lcd
    // 5 - output - lcd
    // 6 - output - lcd contrast adjust
    // 7 - output - serial powerdown
    // 0b1111111=0xff
	PORTA=0x00;
	DDRA=0xff;

	// Port B initialization
    // 0 - output - no connection - but internally, spi SS should be output
    // 1 - output - spi clock
    // 2 - output - spi data - MOSI
    // 3 - input  - spi data - MISO
    // 4 - output - chip enable - spi rtc
    // 5 - output - chip enable - spi flash
    // 6 - output - lcd backlight power enable
    // 7 - output - ibutton led power enable
    // 0b1111 0111 = 0xf7
	PORTB=0x30;
	DDRB=0xf7;

	// Port C initialization
    // 0 - output - lcd data
    // 1 - output - lcd data
    // 2 - output - lcd data
    // 3 - output - lcd data
    // 4 - output - lcd data
    // 5 - output - lcd data
    // 6 - output - lcd data
    // 7 - output - lcd data
	PORTC=0x00;
	DDRC=0xff;

	// Port D initialization
    // 0 - input  - big red button
    // 1 - input  - nav1
    // 2 - input  - uart1 RX
    // 3 - output - uart1 TX
    // 4 - input  - nav2
    // 5 - input  - nav3
    // 6 - input  - nav4
    // 7 - input  - nav5
    // 0b00000100 = 0x04
   	PORTD=0x00;
	DDRD=0x08;

	// Port E initialization
    // 0 - input  - uart0 RX
    // 1 - output - uart0 TX
    // 2 - output - buzzer
    // 3 - output - enable serial output
    // 4 - input  - trip
    // 5 - output - lcd bias control
    // 6 - input/output - ibutton
    // 7 - input  - rtc square wave
    // 0b00101110 = 0x2e
	PORTE=0x00;
	DDRE=0x2e;

	// Port F initialization
    // 0 - input - analog - temperature sensor
    // 1 - output - temp sensor shutdown
    // 2 - no connection
    // 3 - no connection
    // 4 - jtag
    // 5 - jtag
    // 6 - jtag
    // 7 - jtag
    // 0b00000010
	PORTF=0x02;
	DDRF=0x02;

	// Port G initialization
	PORTG=0x00;
	DDRG=0x00;

    }

    //--------Init Timers --------------------
    void 
InitTimers(void)
    {

	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: Timer 0 Stopped
	// Mode: Normal top=FFh
	// OC0 output: Disconnected
	///ASSR=0x00;
	///TCCR0=0x00;
	///TCNT0=0x00;
	///OCR0=0x00;

	// Timer/Counter 1 initialization
	// Mode: Normal top=FFFFh


	// OC1A output: Discon.
	// OC1B output: Discon.
	// OC1C output: Discon.
	TCCR1A=0x00;

	// Clock source: none               = 00
	// Clock source: system clock       = 01
	// Clock source: system clock/64    = 03
	// Clock source: system clock/256   = 04
	// Clock source: system clock/1024  = 05
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	TCCR1B=0x00;

    // set the inital counts to zero
	TCNT1H=0x00;
	TCNT1L=0x00;

    // amount to compare the timer to
	OCR1AH=0x00;
	OCR1AL=0x00;

	OCR1BH=0x00;
	OCR1BL=0x00;

	OCR1CH=0x00;
	OCR1CL=0x00;



	// Timer/Counter 2 initialization
	// Clock source: T2 pin Rising Edge
	// Mode: Normal top=FFh
	// OC2 output: Disconnected
	TCCR2B=0x07;
	TCNT2=0x00;
	OCR2A=0x00;

	// Timer/Counter 3 initialization
	// Clock source: System Clock
	// Clock value: 2000,000 kHz
	// Mode: Normal top=FFFFh
	// OC3A output: Discon.
	// OC3B output: Toggle
	// OC3C output: Toggle
	//TCCR3A=0x14;
	//TCCR3B=0x02;
	//TCNT3H=0x00;
	//TCNT3L=0x00;
	//OCR3AH=0x00;
	//OCR3AL=0x00;
	//OCR3BH=0x00;
	//OCR3BL=0xFA;
	//OCR3CH=0x00;
	//OCR3CL=0xFA;

	// External Interrupt(s) initialization
	// INT0: Off
	// INT1: Off
	// INT2: Off
	// INT3: Off
	// INT4: Off
	// INT5: Off
	// INT6: Off
	// INT7: Off
	///EICRA=0x00;
	///EICRB=0x00;
	///EIMSK=0x00;

	// Timer(s)/Counter(s) Interrupt(s) initialization
	///TIMSK=0x00;
	///ETIMSK=0x00;

    } 
