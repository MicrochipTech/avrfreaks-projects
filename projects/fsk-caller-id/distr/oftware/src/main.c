/*! \mainpage AVR-CallerID
 *
 * \section intro_sec Introduction
 * \subsection what_in What's in this package
 * This package contains a routine that can fetch caller-ID's from a 
 * telephone line and show it on an LCD.
 * 
 * The caller-ID is the telephone number of the one who is calling to
 * you.  This number, along with the date and time of the call is sent
 * to you by your telephone company.
 * 
 * Your telephone company must use the ITU-Rec.V23bis, otherwise this
 * project won't work.  It means that your telephone company must send
 * the data to you using FSK (frequency shift keying).  Which means that
 * the digital ones and zeros of the data have their own frequencies. 
 * A one is sent using a 1300Hz sine, a zero is sent by a 2100Hz sine.
 * I use the Exar XR-2211 chip to decode these frequencies to digital 
 * zeroes and ones.
 * 
 * In many countries another modulation, namely DTMF is used.  This
 * project will NOT work with this modulation.  You will need another
 * decoding chip, an MT8870.
 *
 * This project is actually very basic.  If someone calls, his/her number,
 * including the date and time are shown on the LCD.  Then the
 * microcontroller becomes blocked.  You have to reset it to receive the
 * number of the next caller.
 *
 * So there remains still a lot of work to be done if you want to use it
 * in a real application.
 *
 * \section info Bibliography
 *
 * I read a lot more information than what is given here below.  I only
 * give you the relevant data in this section.
 *
 * <b>ITU-T Recommendation V.23</b>\n
 * 		DATA COMMUNICATION OVER THE TELEPHONE NETWORK
 * 		600/1200-BAUD MODEM STANDARDIZED FOR USE IN THE GENERAL SWITCHED 
 * 		TELEPHONE NETWORK\n
 *		http://www.nist.fss.ru/hr/doc/mstd/itu/index.htm
 *
 * <b>ETSI EN 300 659-1 V1.3.1 (2001-01)</b>\n
 *		European Standard (Telecommunications series)
 *		Access and Terminals (AT);
 *		Analogue access to the Public Switched Telephone Network (PSTN);
 *		Subscriber line protocol over the local loop for display (and 
 *		related) services;
 *		Part 1: On-hook data transmission
 *
 * <b>ETSI EN 300 659-3 V1.3.1 (2001-01)</b>\n
 *		European Standard (Telecommunications series)
 *		Access and Terminals (AT);
 *		Analogue access to the Public Switched Telephone Network (PSTN);
 *		Subscriber line protocol over the local loop for display (and 
 *		related) services;
 *		Part 3: Data link message and parameter codings
 *
 * \section datasheets Datasheets
 *
 * I have downloaded some datasheets of different manufacturers.  Some
 * contain errors.  In some datasheets they choose completely different
 * component values and the calculations are also different.  So be
 * careful when reading them.
 * 
 * NJM2211 FSK Demodulator/Tone decoder, New Japan Radio Co., Ltd.\n
 * RC2211A FSK Demodulator/Tone decoder, Fairchild semiconductor\n
 * XR-2211 FSK Demodulator/Tone decoder, Exar Corporation\n
 *
 * This application note from Exar proved most useful to me:
 *
 * TAN-009 Designing Caller Identification Delivery using XR-2211
 *		for B.T.
 */
 
/*! \file main.c
 * Main file containing state machine for controlling telephone line.
 */

#include "define.h"
#include <avr/delay.h>


/*! Definition of the states of the state machine that controls the
 * telephone line.
 */
typedef enum{
	WAITING_FOR_RING,
	WAITING_FOR_LOCK,
	WAITING_FOR_MARK_SIGNAL,
	WAITING_FOR_DATA,
	CHECK_DATA,
	PRES_LAYER,
	ALL_DONE
}RX_STATE;

/*! Struct that defines some parameters that are useful in the datalink
 *  layer level.
 *  See ETSI EN 300 659-1 V1.3.1 (2001-01), section 5.2 for a definition
 *  of these parameters.
 */
typedef struct{
	uint8_t 	messageType;//!< Type of message received
	//! Total number of bytes as passed by the datalink layer
	uint8_t 	totalBytes;
	//! Checksum passed by the datalink layer
	uint8_t 	checksum;
} DATALINKLAYER;

static DATALINKLAYER dll;//!< An "object" of the datalink structure
static DATALINKLAYER* pDll;//!< Pointer to this dll variable

//! Initialize the datalinklayer structure.
void initDatalink(void){
	pDll=&dll;
	pDll->messageType=0;
	pDll->totalBytes=254;
	pDll->checksum=0;
}//initDatalink

//! Main function containing the state machine.
int main(void){

	extern uint8_t circRXArray[RXBUFSIZE];

	uint16_t counter=0;
	RX_STATE rxState=WAITING_FOR_RING;
	extern uint8_t teller;
	uint8_t sum=0, i=0;

	DDRB=0xFF;//PortB used as output
	CLR_BIT(DDRD,LOCK_DET);	//input pin for PLL lock detect
	CLR_BIT(PORTD,LOCK_DET);	//No pull up
	CLR_BIT(DDRD,FSK_IN);		//input pin for FSK signals
	CLR_BIT(PORTD,FSK_IN);		//no pull up

	//set bit6 and bit7 of portA for output
	SET_BIT(DDRA,RS);
	SET_BIT(DDRA,E);
	//set bit5 of PORTA for input (ring detect);
	CLR_BIT(DDRA,RING_DET);
	
	//initialize display
	initDisplay();
	
	PORTB=0;
	for(;;){
		PORTB=rxState;
		switch(rxState){
			case WAITING_FOR_RING:
				//wait until ring detect comes low
				if(!(TST_BIT(PINA,RING_DET))){
					rxState=WAITING_FOR_LOCK;
					timer0_init();
					setTimer0(LOCK_TIMER,255);
				}
				break;
			case WAITING_FOR_LOCK:
				//Wait for PLL of RC2211 to lock
				if(!TST_BIT(PIND,LOCK_DET)){
					counter=0;
					rxState=WAITING_FOR_MARK_SIGNAL;
				}
				/*If it takes too long to lock, then go back and
				 *wait for a ring.
				 */
				if(timer0isElapsed(LOCK_TIMER))
					rxState=WAITING_FOR_RING;
				break;
			case WAITING_FOR_MARK_SIGNAL:
				//Waiting for MARK signal (=series of consecutive 1's)
				if(TST_BIT(PIND,FSK_IN)){
					if(counter<1000)counter++;
					else{
						//Get ready to receive data
						init_USART(1200,TRUE);
						counter=0;
						initDatalink();
						rxState=WAITING_FOR_DATA;
						teller=0;
					}
				}
				else
					counter=0;
				break;
			case WAITING_FOR_DATA:
				//Reading the data
				if(teller>1)pDll->totalBytes=circRXArray[1];
				/*Wait until all the bytes have come in.
				 *messagetype + (messageLength) totalBytes + presLayer 
				 *	+ checksum = totalBytes + 3
				 */
				if(teller>pDll->totalBytes+3)rxState=CHECK_DATA;
				break;
			case CHECK_DATA:
				//check the checksum of the data
				
				//disable USART and it's interrupts
				init_USART(0,FALSE);
				//fetch checksum
				pDll->checksum=circRXArray[pDll->totalBytes+2];
				//calculate sum of all the bytes in the PresLayer
				i=pDll->totalBytes+2;
				do{
					sum+=circRXArray[i-1];
				}while(--i);
				//convert to 2's complement
				sum=(~sum)+1;
				//compare calculated with received checksum
				if(sum==pDll->checksum)
					rxState=PRES_LAYER;
				else 
					rxState=ALL_DONE;
				break;
			case PRES_LAYER:
				//interprete the data
				i=2;
				while(i<pDll->totalBytes+2){
					presLayer(circRXArray+i,circRXArray[i+1]);
					i+=2+circRXArray[i+1];
				}
				rxState=ALL_DONE;
				break;
			case ALL_DONE:
				//Everything is done here.
				rxState=ALL_DONE;
				break;
			default:
				//impossible state
				//rxState=WAITING_FOR_LOCK;
				break;
		}//switch statemachine
	}//for	
	return 0;
}//main