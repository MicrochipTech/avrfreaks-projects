//
// file: initializations.c
//
//
// xmega demo program VG Feb,Mar,Oct 2011
//
// CPU:   atxmega32A4, atxmega128a1
//
// Version: 1.2.1
//
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
		 		 USART_DREINTLVL_t recIntLevel,
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

	usart_setup( &USARTC0_data, &USARTC0, BSEL_57600, BSCALE_57600, receivetransmit,RX_INTRPT_LEVEL,TX_INTRPT_LEVEL);

	//-----------------------------
	// USARTC1 setup ( RXD1, TXD1 )

	/* PC3 (TXD1) as output. */
	PORTC.DIRSET = PIN7_bm;
	/* PC2 (RXD1) as input. */
	PORTC.DIRCLR = PIN6_bm;

	usart_setup( &USARTC1_data, &USARTC1, BSEL_9600, BSCALE_9600, receiveonly,RX_INTRPT_LEVEL,TX_INTRPT_LEVEL);

	//-----------------------------
	// USARTE0 setup ( RXD0, TXD0 )

	/* PC3 (TXD0) as output. */
	PORTE.DIRSET = PIN3_bm;
	/* PC2 (RXD0) as input. */
	PORTE.DIRCLR = PIN2_bm;

	usart_setup( &USARTE0_data, &USARTE0, BSEL_38400, BSCALE_38400, receiveonly,RX_INTRPT_LEVEL,TX_INTRPT_LEVEL);
}

//-----------------------------------------------------------
//
// usartData    usart data structure
// usart        address of usart to be used (predefined in Atmel delivered .h file)
// bsel, bscale usart params setting the baudrate.
// dirctrl    	enables the sender, receiver or both.
// recIntLevel  interrupt level to be used by the Rx usart
// dreIntLevel	interrupt level to be used by the Tx usart (low,med,high).
//
static void usart_setup(USART_data_t* usartData, 
						USART_t * usart,
                 		s16 bsel, 
						s8 bscale,
				 		dirctrl_t dirctrl,
				 		USART_DREINTLVL_t recIntLevel, // Rx
				 		USART_DREINTLVL_t dreIntLevel) // Tx     
{
	unsigned portBASE_TYPE uxQueueLength = SERIAL_PORT_BUFFER_LEN;

	// Setup for specific usart and initialize buffer queues
	//
	USART_InterruptDriver_Initialize(usartData, uxQueueLength, dirctrl, usart, dreIntLevel);


	USART_2x_Disable(usartData->usart);

	// set format: 8 Data bits, No Parity, 1 Stop bit.
    // this also implicitly sets the communication mode to asynchronous and data order to 'LSB first'
	//
	USART_Format_Set(usartData->usart, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);

	// Set communication mode to asynchronous
	//
	//USART_SetMode(usartData->usart, USART_CMODE_ASYNCHRONOUS_gc);

	// Set data order
	//USART_Set_Data_Order_LSB_first(usartData->usart);

	// Enable RXC interrupt.
	//
	USART_RxdInterruptLevel_Set(usartData->usart, recIntLevel);

	// Disable the Data Register Empty interrupt.
	// This is enabled when a byte is actually sent.
	USART_DreInterruptLevel_Set(usartData->usart, USART_DREINTLVL_OFF_gc);

	// Disable Txd interrupt. This is not used at all.
	// The Dre Interrupt is used instead to control transmission.
	USART_TxdInterruptLevel_Set(usartData->usart, USART_TXCINTLVL_OFF_gc);

	// set baud via driver function
	//
	USART_Baudrate_Set(usartData->usart, bsel, bscale);

	// Enable RX and TX.
	//
	#if SERIAL_SIMULATION == 0
	if((dirctrl == receivetransmit) || (dirctrl == receiveonly))
	{
		USART_Rx_Enable(usartData->usart);
	}
	#endif
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

	// After the bootloader was running we put everything to a defined state
	// and use the internal 2 MHz oscillator first:
	//
	CLKSYS_Enable( OSC_RC2MEN_bm );
	do {} while ( CLKSYS_IsReady( OSC_RC2MRDY_bm ) == 0 );

	// select the 2 MHz oscillator as system clock.
	//
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC2M_gc );	

	// disable all clock sources including the pll, except the 2 MHz one. 
	//
	CLKSYS_Disable( OSC_RC32MEN_bm | OSC_RC32KEN_bm | OSC_XOSCEN_bm | OSC_PLLEN_bm);

	// prescalers off 
	//
	CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );
	

//================================================================================

#if INTERNAL_CLOCK_32MHZ == 1

	// put osc to 32Mhz PIN1 = OSC_RC32MEN
	//
	OSC.CTRL |= 0x02;

	// wait until osc stability and set the flag RC32MRDY
	while (!(OSC.STATUS & OSC_RC32MRDY_bm))
		;
	// I/O protection
	//
	CCP = 0xD8;

	// set clock to 32MHz
	//
	CLK.CTRL = 0x01;
#endif

//================================================================================

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

	// select the external osc as system clock
	//
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_XOSC_gc );

	// disable all internal oszillators
    //
	CLKSYS_Disable( OSC_RC32MEN_bm | OSC_RC2MEN_bm | OSC_RC32KEN_bm);
#endif

//================================================================================

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

	// The clock is stable now and the PLL can be started.
	// Select the external xtal osc as source for the PLL
	// and multiply by 2.
	//
	CLKSYS_PLL_Config( OSC_PLLSRC_XOSC_gc, 2 );
	CLKSYS_Enable( OSC_PLLEN_bm );

	// all prescaler divide by 1
	//
	CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );
	do {} while ( CLKSYS_IsReady( OSC_PLLRDY_bm ) == 0 );

	// select the PLL as system clock
	//
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_PLL_gc );

	// disable all internal oszillators
	//
	CLKSYS_Disable( OSC_RC32MEN_bm | OSC_RC2MEN_bm | OSC_RC32KEN_bm);
#endif
}

//-----------------------------------------------------------
//
// Disable everything that is not used.
//

void disableUnusedModules(void)
{

// the power reduction settings are specific for each device
//
#if defined(__AVR_ATxmega32A4__)

	// PRGEN general power reduction register
	//
	// disable 
	//	- AES
	//  - DMA
	//  - event system
	//  - RTC
	//  - external bus interface
	//
	PR.PRGEN |= PR_AES_bm | PR_DMA_bm | PR_EVSYS_bm | PR_RTC_bm | PR_EBI_bm;


	// PRPA, PRPB power reduction port A/B register
	//
	// disable
	//	- analog comparator
	//	- ADC on port B
	//  - DAC
	//
	// leave ADCA active !
	//
	PR.PRPA |= PR_AC_bm;
	PR.PRPB |= PR_AC_bm | PR_ADC_bm | PR_DAC_bm;
	
		
	// PRPC/D/E/F Power Reduction Port C/D/E/F Register
	//
	// disable unused timers
	//
	// - high resolution extension
	// - TCD1, TCD1       on, used for nested intérrupt tests 
	// - TCC1, TCD0, TCE0 (if not simulating serial input)  
	//
	// TCC0 is the tick timer, always on!
	//
	PR.PRPC |= PR_HIRES_bm;
	PR.PRPD |= PR_HIRES_bm;
	PR.PRPE |= PR_HIRES_bm;

	#if NESTING_TEST == 0
	PR.PRPD |= PR_TC0_bm;		// TCD0 off
	PR.PRPD |= PR_TC1_bm;		// TCD1 off
	#endif

	#if SERIAL_SIMULATION == 0
                                // TCC0 is tick timer, always on
	PR.PRPC |= PR_TC1_bm;       // TCC1 off
	PR.PRPE |= PR_TC0_bm;		// TCE0 off
	                            // TCE1 does not exist in 32A4
	#endif

	// disable SPI, TWI
	//
	// all uarts are active !
	//
	PR.PRPC |= PR_SPI_bm | PR_TWI_bm;
	PR.PRPD |= PR_SPI_bm;
	PR.PRPE |=             PR_TWI_bm;

	// enable eeprom and flash power reduction
	//
	NVM.CTRLB |= NVM_EPRM_bm | NVM_FPRM_bm;

	DISABLE_JTAG();

#elif defined(__AVR_ATxmega128A1__)
//  for 128A1, there are more pins and timers to configure!
#else
#error device not supported, low power configuration is device specific!
#endif

}
        

//-----------------------------------------------------------
//
void vPortPreparation(void)
{
	// For all unused pins are the pullups enabled
	// have defined levels on these inputs (and no floating levels).
	// This is recommended for minimum power consumption and 
	// max reliability.

	//==============
	//  Port A
	//==============

	// PA0 is adc input UBATT1
	// PA1 is adc input UBATT2
	// PA2 is adc input UBATT3
	// PA3 is adc input UX
	// PA4 is adc input UY
	// PA5 is adc input UZ
	// PA6 is GND , used to calibrate the adc
	
	// PA7 is unused
	__PORT_PULLUP(PORTA, PIN7_bm);

	//==============
	//  Port B
	//==============

	// PB0 = DIRCTRL1 as output
	// set the RS485 driver to input (Nexusbus)
	//
	PORTB.DIRSET = PIN0_bm; // pin0 is output
	PORTB.OUTCLR = PIN0_bm; // DE=0, RS485 driver is off

	// PB1 = DIRCTRL2 as output, 
	// enable the RS485 Transmitter for GPS output
	//
	PORTB.DIRSET = PIN1_bm; // pin1 is output
	PORTB.OUTSET = PIN1_bm;	// DE=1 , RS484 driver is on

	// PB2 = DIRCTRL2 as output, 
	// this pin controls the reset line of the BT-222 bluetooth modem 
	//
	PORTB.DIRSET = PIN2_bm; // pin2 is output
	PORTB.OUTSET = PIN2_bm;	// BLUERES=1 , reset of BT-222 is high

	// PB3 = LINK
	// this pin is used to read the BT-222 linkstatus
	//
	PORTB.DIRCLR = PIN3_bm; // pin3 is input

	//==============
	//  Port C
	//==============

	// PC0 is SDA
	// PC1 is SCL

	// PC2 is RXBLUE, serial input of BT-222
	//     initialized with the serial ports

	// PC3 is TXBLUE, serial output to BT-222
	//     initialized with the serial ports

	// PC4 is unused
	__PORT_PULLUP(PORTC, PIN4_bm);

	// PC5 is unused
	__PORT_PULLUP(PORTC, PIN5_bm);

	// PC6 is RS_RX, RS484 input
	//     initialized with the serial ports

	// PC7 is RS_TX, RS484 output
	//     initialized with the serial ports

	//==============
	//  Port D
	//==============
	
	// PD0 is CAL
	// this pin is connected to the calibration switch
	// and shall trigger an interrupt if pressed.
	//
	PORT_ConfigurePins( &PORTD,
	                    PIN0_bm,
	                    false,
	                    false,
	                    PORT_OPC_TOTEM_gc,
	                    PORT_ISC_FALLING_gc );

	PORT_SetPinsAsInput( &PORTD, PIN0_bm );

	// Configure Interrupt0 to have medium interrupt level, triggered by pin 0.
	//
	PORT_ConfigureInterrupt0( &PORTD, PORT_INT0LVL_MED_gc, 0x01 );


	// PD1 is unused
	__PORT_PULLUP(PORTD, PIN1_bm);

	// PD2 is RS2_RX, RS485 input
	//     initialized with the serial ports

	// PD3 is RS2_TX, RS485 output
	//     initialized with the serial ports

	// PD4 is unused
	__PORT_PULLUP(PORTD, PIN4_bm);

	// PD5 is unused
	__PORT_PULLUP(PORTD, PIN5_bm);

	// PD6 is RXD2, serial optical input
	//     initialized with the serial ports

	// PD7 is unused
	__PORT_PULLUP(PORTD, PIN7_bm);

	//==============
	//  Port E
	//==============
	
	// PE0 is unused
	__PORT_PULLUP(PORTE, PIN0_bm);

	// PE1 is unused
	__PORT_PULLUP(PORTE, PIN1_bm);

	// PE2 is RXD1, serial optical input
	//     initialized with the serial ports

	// PE3 is unused
	__PORT_PULLUP(PORTE, PIN3_bm);
}

//-----------------------------------------------------------
// Start the boot-loader via software reset
// when the button is pressed.
//
ISR(PORTD_INT0_vect)
{
	cli();
	software_reset();
}

// EOF initializations.c
