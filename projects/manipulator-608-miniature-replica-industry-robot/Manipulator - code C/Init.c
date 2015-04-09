/*
 * Init.c
 *
 *  Created on: 18 mar 2015
 *      Author: M.R.O.
 */

/**************************
 * --->>  Includes  <<--- *
 **************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stddef.h>
#include <avr\pgmspace.h>
#include "usart.h"
#include "Adc.h"
#include "dma_driver.h"
#include "Globals_Defs.h"

/***************************
 * --->>  Functions  <<--- *
 ***************************/

/*
* setClockTo32MHz - function sets crystal to 32MHz and run PLL.
*
* PLL multiplicator is equal 4, finally Fosc = 128 MHz. Peripherals are clocked by 32 MHz (A prescaler 4).
*/

void setClockTo32MHz(void)
{
	CCP = CCP_IOREG_gc;       /* disable register security for oscillator update */
	OSC.CTRL = OSC_RC32MEN_bm;       /* enable 32MHz oscillator */
	while (!(OSC.STATUS & OSC_RC32MRDY_bm)); /* wait for oscillator to be ready */
	CCP = CCP_IOREG_gc;            /* disable register security for clock update */
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc; /* switch to 32MHz clock */
	CCP = CCP_IOREG_gc;
	CLK.PSCTRL =  CLK_PSADIV_4_gc;
	OSC.PLLCTRL = OSC_PLLSRC_RC32M_gc | PLL128MHZ;
	OSC.CTRL = OSC_PLLEN_bm;
	while (!(OSC.STATUS & OSC_PLLRDY_bm));
	CCP = CCP_IOREG_gc;            /* disable register security for clock update */
	CLK.CTRL = CLK_SCLKSEL_PLL_gc; /* switch to 128MHz clock */
}

/*!
* TimerConfig - Initialization function
*
* Configuration of Timer base, PWM.
* 	timer E0 and F0 - PWM
* 	timer C1 - Time base
* 	timer D0 - Delay function
*/

void TimerConfing(void)
{
	/*timer0 E0*/
	TCE0.CTRLA = TC_CLKSEL_DIV8_gc;
	TCE0.CTRLB = TC_WGMODE_SS_gc | TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm
			| TC0_CCDEN_bm; /*SS mode and OCn output ABCD enable*/
	TCE0.PER = 1023; /* 10 bit PWM*/
	TCE0.CCABUF = 512; /*compare register A*/
	TCE0.CCBBUF = 512; /*compare register B*/
	TCE0.CCCBUF = 512; /*compare register C*/
	TCE0.CCDBUF = 512; /*compare register D*/

	/*timer F0*/
	TCF0.CTRLA = TC_CLKSEL_DIV8_gc;
	TCF0.CTRLB = TC_WGMODE_SS_gc | TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm;
	TCF0.PER = 1023;
	TCF0.CCABUF = 512;
	TCF0.CCBBUF = 512;
	TCF0.CCCBUF = 1000;

	/*timer C1 - Base time */
	TCC1.CTRLA = TC_CLKSEL_DIV1024_gc;
	TCC1.CTRLB = TC_WGMODE_NORMAL_gc;
	TCC1.INTCTRLA = TC_OVFINTLVL_HI_gc;
	TCC1.CNT = 65535;

	/*timer D0 - Delay function */
	TCD0.CTRLA = TC_CLKSEL_OFF_gc;
	TCD0.CTRLB = TC_WGMODE_NORMAL_gc;
	TCD0.INTCTRLA = TC_OVFINTLVL_LO_gc;
	TCD0.CNT = 33537;
}

/*!
* PortConfing - Initialization function
*
* Function is responsible for setting I/O ports and pull up/down of them.
* 	Configuration list:
* 	PWM - O
* 	Engines - O
* 	Encoders - I
* 	LimitSwitchs - I
* 	Buttons - I
* 	Buttons Led - O
* 	Adc - I
* 	Buzzer - O
* 	Lcd Screen - O
* 	RS-232 - I/O
* 	Output Socked - I/O
*/

void PortConfing(void)
{
	/* PWM PINS */

	PORTE_DIRSET = PIN0_bm; /* Pwm 1 */
	PORTE_DIRSET = PIN1_bm; /* Pwm 2 */
	PORTE_DIRSET = PIN2_bm; /* Pwm 3 */
	PORTE_DIRSET = PIN3_bm; /* Pwm 4 */
	PORTF_DIRSET = PIN0_bm; /* Pwm 5 */
	PORTF_DIRSET = PIN1_bm; /* Pwm 6 */
	PORTF_DIRSET = PIN2_bm; /* Pwm gripper */

	/* ENGINE PINS */

	PORTD_DIRSET = PIN0_bm; /* Engine 1 */
	PORTD_DIRSET = PIN1_bm;
	PORTD_PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTD_PIN1CTRL = PORT_OPC_PULLUP_gc;
	PORTD_DIRSET = PIN2_bm; /* Engine 2 */
	PORTD_DIRSET = PIN3_bm;
	PORTD_PIN2CTRL = PORT_OPC_PULLUP_gc;
	PORTD_PIN3CTRL = PORT_OPC_PULLUP_gc;
	PORTD_DIRSET = PIN4_bm; /* Engine 3 */
	PORTD_DIRSET = PIN5_bm;
	PORTD_PIN4CTRL = PORT_OPC_PULLUP_gc;
	PORTD_PIN5CTRL = PORT_OPC_PULLUP_gc;
	PORTD_DIRSET = PIN6_bm; /* Engine 4 */
	PORTD_DIRSET = PIN7_bm;
	PORTD_PIN6CTRL = PORT_OPC_PULLUP_gc;
	PORTD_PIN7CTRL = PORT_OPC_PULLUP_gc;
	PORTE_DIRSET = PIN4_bm; /* Engine 5 */
	PORTE_DIRSET = PIN5_bm;
	PORTE_PIN4CTRL = PORT_OPC_PULLUP_gc;
	PORTE_PIN5CTRL = PORT_OPC_PULLUP_gc;
	PORTE_DIRSET = PIN6_bm; /* Engine 6 */
	PORTE_DIRSET = PIN7_bm;
	PORTE_PIN6CTRL = PORT_OPC_PULLUP_gc;
	PORTE_PIN7CTRL = PORT_OPC_PULLUP_gc;
	PORTF_DIRSET = PIN3_bm; /* Gripper */
	PORTF_DIRSET = PIN4_bm;
	PORTF_PIN3CTRL = PORT_OPC_PULLUP_gc;
	PORTF_PIN4CTRL = PORT_OPC_PULLUP_gc;

	/* ENCODERS PINS */

	/* Interrupt source pins*/
	PORTR_DIRCLR = PIN0_bm; /* Interrupt 1 */
	PORTR_DIRCLR = PIN1_bm; /* Interrupt 2 */
	PORTH_DIRCLR = PIN2_bm; /* Interrupt 3 */
	PORTJ_DIRCLR = PIN5_bm; /* Interrupt 4 */
	PORTQ_DIRCLR = PIN0_bm; /* Interrupt 5 */
	PORTK_DIRCLR = PIN7_bm; /* Interrupt 6 */

	/* Sense pins*/
	PORTH_DIRCLR = PIN3_bm; /* Interrupt 1 */
	PORTH_DIRCLR = PIN5_bm; /* Interrupt 2 */
	PORTH_DIRCLR = PIN0_bm; /* Interrupt 3 */
	PORTJ_DIRCLR = PIN4_bm; /* Interrupt 4 */
	PORTJ_DIRCLR = PIN0_bm; /* Interrupt 5 */
	PORTJ_DIRCLR = PIN3_bm; /* Interrupt 6 */

	/* LIMITSWITCHES PINS */

	PORTJ_DIRCLR = PIN7_bm; /* LimitSwitch 1 */
	PORTJ_PIN7CTRL = PORT_OPC_PULLDOWN_gc;
	PORTJ.DIRCLR = PIN6_bm; /* LimitSwitch 2 */
	PORTJ.PIN6CTRL = PORT_OPC_PULLDOWN_gc;
	PORTH_DIRCLR = PIN7_bm; /* LimitSwitch 3 */
	PORTH_PIN7CTRL = PORT_OPC_PULLDOWN_gc;
	PORTH_DIRCLR = PIN6_bm; /*LimitSwitch 4 */
	PORTH_PIN6CTRL = PORT_OPC_PULLDOWN_gc;
	PORTA_DIRCLR = PIN7_bm; /* LimitSwitch 5 */
	PORTA_PIN7CTRL = PORT_OPC_PULLDOWN_gc;
	PORTF_DIRCLR = PIN5_bm; /*LimitSwitch 6 (gripper) */
	PORTF_PIN5CTRL = PORT_OPC_PULLDOWN_gc;

	/* BUTTONS PINS */

	PORTF_DIRCLR = PIN6_bm; /* P button */
	PORTF_PIN6CTRL = PORT_OPC_PULLUP_gc;
	PORTF_DIRCLR = PIN7_bm; /* L button */
	PORTF_PIN7CTRL = PORT_OPC_PULLUP_gc;
	PORTJ_DIRCLR = PIN2_bm; /* OK button */
	PORTJ_PIN2CTRL = PORT_OPC_PULLUP_gc;
	PORTK_DIRCLR = PIN6_bm; /* ESC button */
	PORTK_PIN6CTRL = PORT_OPC_PULLUP_gc;

	/* BUTTONS LIGHNTING PINS */

	PORTQ_DIRSET |=	PIN1_bm;
	PORTQ_OUTSET |= PIN1_bm; /*P button led*/
	PORTH_DIRSET |=	PIN1_bm;
	PORTH_OUTSET |= PIN1_bm; /*ESC button led*/
	PORTQ_DIRSET |=	PIN2_bm;
	PORTQ_OUTSET |= PIN2_bm; /*OK button led*/
	PORTC_DIRSET |= PIN5_bm;
	PORTC_OUTSET |= PIN5_bm; /*L button led*/

	/* ADC PINS */

	PORTA_DIRCLR = PIN0_bm; /* Adc engine 1 */
	PORTA_DIRCLR = PIN1_bm; /* Adc engine 2 */
	PORTA_DIRCLR = PIN2_bm; /* Adc engine 3 */
	PORTA_DIRCLR = PIN3_bm; /* Adc engine 4 */
	PORTA_DIRCLR = PIN4_bm; /* Adc engine 5 */
	PORTA_DIRCLR = PIN5_bm; /* Adc engine 6 */
	PORTA_DIRCLR = PIN6_bm; /* Adc gripper */

	/* BUZZER PIN & and STATUS LEDS */

	PORTQ_DIRSET = PIN3_bm; /* Buzzer */
	PORTH_DIRSET = PIN4_bm; /* Status led */
	PORTJ_DIRSET = PIN1_bm; /* Error led */

	/* OUTPUT SOCKED PINS (more info in documentation) */

	PORTB_DIRSET = PIN5_bm;
	PORTB_DIRSET = PIN4_bm;
	PORTB_DIRSET = PIN6_bm;

	/*Vodka pins*/
	PORTC_DIRSET = PIN1_bm; /* Pump ON/OFF */
	PORTC_DIRCLR = PIN3_bm; /* Sense */
	PORTC.PIN3CTRL = PORT_OPC_PULLDOWN_gc;
	PORTC_OUTSET = PIN1_bm;
	PORTD_OUTSET = PIN0_bm;
	PORTD_OUTSET = PIN1_bm;

	/* USART PINS */

	PORTC.DIRSET = PIN7_bm;	/*TxD*/
	PORTC.OUTSET = PIN7_bm;	/*TxD*/
	PORTC.DIRCLR = PIN6_bm; /*RxD*/

	/* LCD SCREEN */

	PORTK_DIRSET = PIN0_bm; /*D7*/
	PORTK_DIRSET = PIN1_bm;	/*D6*/
	PORTK_DIRSET = PIN2_bm;	/*D5*/
	PORTK_DIRSET = PIN3_bm;	/*D4*/
	PORTK_DIRSET = PIN4_bm;	/*E*/
	PORTK_DIRSET = PIN5_bm;	/*RS*/
}

/*!
* InterruptConfig - Initialization function
*
* This function configure Encoder interrupts and PMIC register.
*/

void InterruptConfing(void)
{
	PORTR_PIN1CTRL = PORT_ISC_RISING_gc; /* Configure interrupts on rising edge*/
	PORTR_PIN0CTRL = PORT_ISC_RISING_gc;
	PORTH_PIN2CTRL = PORT_ISC_RISING_gc;

	PORTQ_PIN0CTRL = PORT_ISC_RISING_gc;
	PORTK_PIN7CTRL = PORT_ISC_RISING_gc;
	PORTJ_PIN5CTRL = PORT_ISC_RISING_gc;

	PORTH_INT0MASK = 0x04; /*Setup interrupt mask*/
	PORTJ_INT0MASK = 0x20;
	PORTB_INT0MASK = 0x20;
	PORTK_INT0MASK = 0x80;
	PORTQ_INT0MASK = 0x01;
	PORTR_INT0MASK = 0x01;
	PORTR_INT1MASK = 0x02;

	PORTH_INTCTRL = PORT_INT0LVL_HI_gc; /*All encoders interrupt has a highest priority*/
	PORTJ_INTCTRL = PORT_INT0LVL_HI_gc;
	PORTK_INTCTRL = PORT_INT0LVL_HI_gc;
	PORTQ_INTCTRL = PORT_INT0LVL_HI_gc;
	PORTR_INTCTRL = PORT_INT0LVL_HI_gc | PORT_INT1LVL_HI_gc;

	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm; /*Enable all interrupt priorities */
	ADCB.CTRLA = 0x00;
}

/*!
* UsartInit - Initialization function
* Initialization of RS-232 Connection.
*
* 	Transmission parameters:
* 	Baud: 57600
* 	Stop bit: 1
* 	Transmission: 8 bit
* 	Parity: none
*/

void UsartInit(void)
{
	USARTC1.CTRLA = USART_RXCINTLVL_MED_gc; /*RX interrupt set as medium lvl*/
	USARTC1.CTRLB |= USART_TXEN_bm | USART_RXEN_bm; /*Enable RX and TX*/
	USARTC1.CTRLC = USART_CHSIZE_8BIT_gc; /*8 - bit transmission*/
	USARTC1.CTRLC |= (1 << USART_SBMODE_bp);
	usart_set_baudrate(&USARTC1, 57600, 32000000); /* Selecting USART, baud and Fosc*/
}

/*!
* AdcConfig - Initialization function
*
* Adc is configured as free running, 7 channels are used.
* DMA is responsible for collecting data. Result are kept in AdcBuf[8] array.
* AdcBuf[3] = AdcBuf[7].
*/

void AdcConfig(void)
{

	ADCA.CALL = ReadCalibrationByte( offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0)); /*Calibration bits*/
	ADCA.CALH = ReadCalibrationByte( offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1));

	ADCA.CTRLA = ADC_ENABLE_bm;
	ADCA.CTRLB = ADC_FREERUN_bm | ADC_RESOLUTION_12BIT_gc;
	ADCA.REFCTRL = ADC_REFSEL_VCC_gc;
	ADCA.EVCTRL = ADC_SWEEP_0123_gc;
	ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;

	ADCA.CH0.CTRL = ADC_CH_START_bm | ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN0_gc;

	ADCA.CH1.CTRL = ADC_CH_START_bm | ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA.CH1.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc;

	ADCA.CH2.CTRL = ADC_CH_START_bm | ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA.CH2.MUXCTRL = ADC_CH_MUXPOS_PIN2_gc;

	ADCA.CH3.CTRL = ADC_CH_START_bm | ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA.CH3.MUXCTRL = ADC_CH_MUXPOS_PIN3_gc;


}

/*!
* DmaConfig - Initialization function
*
* DMA is used for handling UART Tx and Adc.
* External Atmel library has been used.
*/

void DmaConfig(void)
{
	DMA_SetupBlock( &DMA.CH0,
					TxBuf,
					DMA_CH_SRCRELOAD_BLOCK_gc,
					DMA_CH_SRCDIR_INC_gc,
					(void *) &USARTC1.DATA,
                    DMA_CH_DESTRELOAD_NONE_gc,
                    DMA_CH_DESTDIR_FIXED_gc,
                    2,
                    DMA_CH_BURSTLEN_1BYTE_gc,
                    0,
                    false
                    );
	DMA_EnableSingleShot (&DMA.CH0);
	DMA_SetTriggerSource (&DMA.CH0, DMA_CH_TRIGSRC_USARTC1_DRE_gc);

	DMA_SetupBlock( &DMA.CH2,
					(void *)&ADCA.CH0RES,
					DMA_CH_SRCRELOAD_BLOCK_gc,
					DMA_CH_SRCDIR_INC_gc,
					&AdcBuf,
					DMA_CH_DESTRELOAD_BLOCK_gc,
					DMA_CH_DESTDIR_INC_gc,
					8,
					DMA_CH_BURSTLEN_2BYTE_gc,
					0,
					false
			);
	DMA_SetIntLevel (&DMA.CH2,
					DMA_CH_TRNINTLVL_LO_gc,
					DMA_CH_ERRINTLVL_LO_gc);
	DMA_SetTriggerSource(&DMA.CH2, DMA_CH_TRIGSRC_ADCA_CH3_gc);
	DMA_EnableChannel(&DMA.CH2);

	DMA.CH2.CTRLA |= DMA_CH_ENABLE_bm;
}


