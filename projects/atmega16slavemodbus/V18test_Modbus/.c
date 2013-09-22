/*
 * V17test_main.c
 *
 *  Created on: 02 Ott 12
 *      Author: Mirko Bugli
 *  Revision on: 09 Giu 13
 */
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "JG_Modbus.c"
#include "JG_Modbus.h"
#include "p_defines.h"
#include "Modbus_uart.c"
#include "Modbus_uart.h"


volatile uint32_t sysSec = 0;
tMBChangedBit changedBit = {99, 0};


volatile uint16_t actProfileData[MAX_WordAddress];
volatile uint8_t systemBits[MaxActionBytes];

volatile uint8_t UART_RxBuf[UART_RX_BUFFER_SIZE];
volatile uint8_t UART_RxHead;



volatile uart_status_t uart0_status = {
		RS_Wait, // status
		0 // bitTics
};

void performBitActions();


inline uint8_t setBitValForSysBit(uint8_t b, uint8_t v){
	if (v) {
		systemBits[getByteForSysBit(b)] |= (1 << getBitForSysBit(b)); //Set bit
	} else {
		systemBits[getByteForSysBit(b)] &= ~(1 << getBitForSysBit(b)); //Clear bit
	}
	return ((systemBits[getByteForSysBit(b)] & (1 << getBitForSysBit(b))) ? 1 : 0);
}


ISR (TIMER2_COMP_vect) {
	
	if (uart0_status.status != RS_Wait) {
		uart0_status.bitTics++;
		// this value is reset each time
		// we receive a new byte on UART
		if (uart0_status.bitTics >= FRAME_TIMEOUT_TICS) {
			// here we are if bitTics is more than 3.5 chars
			// that means frame complete or timeout for frame
			uart0_status.status = RS_FrameComplete;
		}
		if ((uart0_status.bitTics <= FRAME_TIMEOUT_TICS)
				&& (uart0_status.bitTics >= BYTE_TIMEOUT_TICS)) {
			// here we are if bitTics exceeds 1.5 chars
			// that means frame is useless
			// anyway we give it to higher instance for check
			// and exception handling
			uart0_status.status = RS_FrameComplete;
			
		}
	}

}

void initInterrupts(void) {


	// TC2 used for Modbus Timeout and sysSec Counter

	TCCR2 |= ((1<<CS21));//Prescaler set clkt2s/8
	OCR2 = 32; //32; Original value
	TIMSK |= (1<<OCIE2);//Enable interrupt from Timer/Counter2 Compare Match
}

void initPorts(void) {

	RS485_DirPort	= 0xFE; //Set Port as OUTPUT, but not RX
	RS485_Port		= 0xF9; //OFF Led Pin enable Rx SN75176) and Enable Pull-Up resistor on Rx pin
	Led_DirPort	= 0xFF; //Set Port as OUTPUT, for LED's debug
	Led_Port	= LedOFF;	//OFF all LED
}

void initSysVars(void) {
	uint8_t i8;

	for (i8 = 0; i8 < MaxActionBytes; i8++) {
		systemBits[i8] = 0; //Reset Byte Modbus in RAM
	}
	for (i8 = 0; i8 < MAX_WordAddress; i8++) {
		actProfileData[i8] = 0;//Reset Word Modbus in RAM
	}
}

int main(void) {

	uint8_t oldStatus = 0;

	initSysVars();
	initPorts();

	// we use RS485 here, to go with RS232 just have a look at Modbus_uart.c/.h
	// and use the rs232 init function
	rs485_init(UART_BAUD_SELECT(38400,F_CPU));
	initInterrupts();


	actProfileData[IDX_DW_Debug01] = 'C';
	actProfileData[IDX_DW_Debug02] = 'i';
	actProfileData[IDX_DW_Debug03] = 'a';
	actProfileData[IDX_DW_Debug04] = 'o'; 
	actProfileData[IDX_DW_Debug05] = '_';
	actProfileData[IDX_DW_Debug06] = '!'; 
	
	sei();
	

	for (;;) {
		
		RS485_Port |= (1<<RS485_LRx); // Led Rx OFF for debug (MiBu)
		if (oldStatus != uart0_status.status) {
			oldStatus = uart0_status.status;
			
			if (uart0_status.status == RS_FrameComplete) {
				if (UART_RxHead) {
					RS485_Port &= ~(1<<RS485_LRx); // Led Rx ON for debug (MiBu)
					modbus_processSlaveFrame((uint8_t*) UART_RxBuf, UART_RxHead);
				}
				uart_flush();
				uart0_status.status = RS_Wait;
			}
		}
		if (changedBit.cBit != 99) {
			//performBitActions();
		}
	}
}

void switchUSART (uint8_t how){

	// Enable/Disable USART receiver and receive complete interrupt
	// to ensure we don't get disturbed by another incoming stuff

	if (how) {
		UART0_CONTROL |= ((1<<RXCIE0) | (1 << RXEN0));   //Set bit "Rx Complete Interrupt Enable" & "Receiver Enable"
		RS485_Port &= ~((1 << RS485_Send)); //Clear PIN DE+RE SN75176 for enable RX from serial line
	} else {
		UART0_CONTROL &= ~((1<<RXCIE0) | (1 << RXEN0));  //Clear bit "Rx Complete Interrupt Enable" & "Receiver Enable"
		RS485_Port &= ~((1 << RS485_Send)); //Clear PIN DE+RE SN75176 for enable RX from serial line
	}

}

void performBitActions(){

	// switching the UART off and later on again is just
	// because here I had a really com-hungry HMI connected

	switchUSART(OFF);

	switch (changedBit.cBit) {
	case IDX_PB_Dir:
		if (changedBit.cVal) {
			// action on Bit is 1
			// do some here
		} else {
			// action on Bit is 0
			// do some here
		}
		break;
	}
		changedBit.cBit = 99;
		changedBit.cVal = 0;
		switchUSART(ON);

}