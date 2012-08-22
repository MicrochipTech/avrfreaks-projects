
// file: initializations.c
//
//	atxmega 32A4/128A1 port, VG Feb 2011


#include "all.h"


typedef void tskTCB;
extern volatile tskTCB * volatile pxCurrentTCB;


// USART data structs
USART_data_t USARTC0_data;
USART_data_t USARTC1_data;
USART_data_t USARTE0_data;


// local function prototypes
//
static void usart_setup(USART_data_t* usartData, USART_t * usart,
                 s16 bsel, s8 bscale,
				 dirctrl_t dirctrl,
				 USART_DREINTLVL_t dreIntLevel);



//-----------------------------------------------------------
// all usart preparation including port pin setup
//
//
void initUsarts(void)
{
	// The demo code has two serial receivers. The data is multiplexed into one single
	// transmitter output.

	//-----------------------------
	// USARTC0 setup ( RXD0, TXD0 )

	/* PC3 (TXD0) as output. */
	PORTC.DIRSET = PIN3_bm;
	/* PC2 (RXD0) as input. */
	PORTC.DIRCLR = PIN2_bm;

	usart_setup( &USARTC0_data, &USARTC0, BSEL_115200, BSCALE_115200, transmitonly,TRANSMITTER_INTERRUPT_LEVEL);

	//-----------------------------
	// USARTC1 setup ( RXD1, TXD1 )

	/* PC3 (TXD1) as output. */
	PORTC.DIRSET = PIN7_bm;
	/* PC2 (RXD1) as input. */
	PORTC.DIRCLR = PIN6_bm;

	usart_setup( &USARTC1_data, &USARTC1, BSEL_9600, BSCALE_9600, receiveonly,RECEIVER_INTERRUPT_LEVEL);

	//-----------------------------
	// USARTE0 setup ( RXD0, TXD0 )

	/* PC3 (TXD0) as output. */
	PORTE.DIRSET = PIN3_bm;
	/* PC2 (RXD0) as input. */
	PORTE.DIRCLR = PIN2_bm;

	usart_setup( &USARTE0_data, &USARTE0, BSEL_38400, BSCALE_38400, receiveonly,RECEIVER_INTERRUPT_LEVEL);
}

//-----------------------------------------------------------
//
// usartData    usart data structure
// usart        address of usart to be used (predefined in Atmel delivered .h file)
// bsel, bscale usart params setting the baudrate.
// dirctrl    	enables the sender, receiver or both.
// dreIntLevel	interrupt level to be used by the usart (low,med,high).
//
static void usart_setup(USART_data_t* usartData, 
						USART_t * usart,
                 		s16 bsel, 
						s8 bscale,
				 		dirctrl_t dirctrl,
				 		USART_DREINTLVL_t dreIntLevel)
{
	unsigned portBASE_TYPE uxQueueLength = SERIAL_PORT_BUFFER_LEN;

	// Use USARTC0 and initialize buffer queues
	USART_InterruptDriver_Initialize(usartData, uxQueueLength, dirctrl, usart, dreIntLevel);

	// 8 Data bits, No Parity, 1 Stop bit.
	USART_Format_Set(usartData->usart, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);

	// Enable RXC interrupt.
	USART_RxdInterruptLevel_Set(usartData->usart, USART_RXCINTLVL_LO_gc);

	// set baud via driver function
	USART_Baudrate_Set(usartData->usart, bsel, bscale);

	// Enable RX and TX.
	if((dirctrl == receivetransmit) || (dirctrl == receiveonly))
	{
		USART_Rx_Enable(usartData->usart);
	}
	if((dirctrl == receivetransmit) || (dirctrl == transmitonly))
	{
		USART_Tx_Enable(usartData->usart);
	}
}


//-----------------------------------------------------------
//

#define INTERNAL_CLOCK_32MHZ 0
#define EXTERNAL_CLOCK_15MHZ 0
#define EXTERNAL_CLOCK_30MHZ 1

void vInitClock(void)
{
#if INTERNAL_CLOCK_32MHZ == 1
	/* put osc to 32Mhz PIN1 = OSC_RC32MEN */
	OSC.CTRL |= 0x02;
	/* wait until osc stability and set the flag RC32MRDY */
	while (!(OSC.STATUS & OSC_RC32MRDY_bm))
		;
	/* I/O protection */
	CCP = 0xD8;
	/* set clock to 32MHz */
	CLK.CTRL = 0x01;
#endif

#if EXTERNAL_CLOCK_15MHZ == 1 
	// use the crystal oszillator with 14,7456 Mhz
#if (F_CPU != 14745600)
// this warning is important because this mode works only in the range 12-16Mhz
# warning ("using crystal osillator with other than 14,7456 MHz!")	
#endif

	// Enable for external 12-16 MHz crystal with 256 clks startup time.
	// Check if it's stable and set the external
	// oscillator as the main clock source.
	//
	CLKSYS_XOSC_Config( OSC_FRQRANGE_12TO16_gc,false,OSC_XOSCSEL_XTAL_256CLK_gc );
	CLKSYS_Enable( OSC_XOSCEN_bm );
	do {} while ( CLKSYS_IsReady( OSC_XOSCRDY_bm ) == 0 );
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_XOSC_gc );
	CLKSYS_Disable( OSC_RC32MEN_bm );
#endif

#if EXTERNAL_CLOCK_30MHZ == 1
	// use the crystal oszillator with 14,7456 Mhz and double via PLL
	//
#if (F_CPU != 29491200)
// this warning is important because this mode works only in the range 12-16Mhz
# warning ("using crystal osillator with other than 29,491200 MHz!")	
#endif

	// Enable for external 12-16 MHz crystal with 256 clks startup time.
	// Check if it's stable and set the external
	// oscillator as the main clock source.
	//
	CLKSYS_XOSC_Config( OSC_FRQRANGE_12TO16_gc,false,OSC_XOSCSEL_XTAL_256CLK_gc );
	CLKSYS_Enable( OSC_XOSCEN_bm );
	do {} while ( CLKSYS_IsReady( OSC_XOSCRDY_bm ) == 0 );

	// the PLL can be started as soon as the clock is stable
	// select the external xtal osc as source for the PLL
	// and multiply by 2
	CLKSYS_PLL_Config( OSC_PLLSRC_XOSC_gc, 2 );
	CLKSYS_Enable( OSC_PLLEN_bm );
	// all prescaler divide by 1
	CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );
	do {} while ( CLKSYS_IsReady( OSC_PLLRDY_bm ) == 0 );
	// select the PLL as system clock
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_PLL_gc );
	// disable the internal oscillator
	CLKSYS_Disable( OSC_RC32MEN_bm );
#endif
}

//-----------------------------------------------------------
//
void vPortPreparation(void)
{
	//	PORTCFG.MPCMASK=0xFF;	// set the mask for pin configurations, all pins are masked
	//	PORTF.PIN0CTRL=0x38;	// activate a pullup for all masked pins
	//	PORTF.DIR=0x00;			// set port F as input
	//	PORTE.DIR=0xFF;			// set port E as output
	//    PORTE.OUT=0xFF;         // set all LEDS on Port E to OFF state
}

//-----------------------------------------------------------



// eof initializations.c
