/**************************************************************************************************
---------------------------------------------------------------------------------------------------
	Copyright (c) 2007, Jonathan Bagg
	All rights reserved.

	 Redistribution and use in source and binary forms, with or without modification, are permitted 
	 provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of 
	  conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of 
	  conditions and the following disclaimer in the documentation and/or other materials provided 
	  with the distribution.
    * Neither the name of Jonathan Bagg nor the names of its contributors may be used to 
	  endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
  POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------------------------------
   Project name : Infidigm AVR Drivers
   Processor	: AT90CAN32/64/128
   File name    : can.c
---------------------------------------------------------------------------------------------------
   Modifications: 

   Revision 1.0  2007/05/19 	Bagg
   - Initial Release
   
---------------------------------------------------------------------------------------------------
   Created      : 10 March 2007                Author(s) : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   CAN bus Interrupt Based Driver 
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <avr/io.h> 
#include <avr/interrupt.h>
#include "can.h"

/**************************************************************************************************
*   CAN Baud Rate
**************************************************************************************************/
#define	CAN_BAUD_RATE		125000

/**************************************************************************************************
*   CAN PHY driver delays and LINE delay (unit = nS)
**************************************************************************************************/
#define	TX_DELAY		50			//bus driver delay
#define	RX_DELAY		30			//receiver delay
#define	LINE_DELAY		110			//bus line delay

/**************************************************************************************************
*   Acceptance Filtering
**************************************************************************************************/
#define ACCPT_MASK_ID	0
#define ACCPT_TAG_ID	0
#define ACCPT_MASK_RTR	0
#define ACCPT_TAG_RTR	0
#define ACCPT_MASK_IDE	0
#define ACCPT_TAG_RB0	0
#define ACCPT_TAG_RB1	0			//RB1 in extended (CAN 2.0B) Only

/**************************************************************************************************
*   TX and RX frame buffer sizes 
* 	- 1 can frame ~ 11 bytes for CAN2A and 13 bytes for CAN2B
* 	- RX_SIZE * 11 or 13 is aprox rx buffer memory size.
**************************************************************************************************/
#define TX_SIZE			8			//Transmit Buffer Size, can be 2^n where n=0 to 6
#define RX_SIZE			8			//Receiver Buffer Size, can be 2^n where n=0 to 6

/**************************************************************************************************
*   Pre-Processor ONLY! - Do not edit!
**************************************************************************************************/
#define BIT_TIME_NS			(1000000000/CAN_BAUD_RATE)
#define TCSC				(BIT_TIME_NS/16)
#define TPRS				(2*(TX_DELAY+RX_DELAY+LINE_DELAY))
#define	PROPAG_CALC			((TPRS+(TCSC/2-1))/TCSC)			//round at 0.5 instead of 1
#if PROPAG_CALC>7
	#define	PROP_SEG		PROPAG_CALC
#else
	#define	PROP_SEG		7
#endif
#define	REMAINING			(16-1-(PROP_SEG-1)-1)
#define	PHASE_SEG_1			(REMAINING/2)
#if (REMAINING & 1)											//check if odd or even
	#define	PHASE_SEG_2		(PHASE_SEG_1-1)						//phase2 <= phase1
#else
	#define	PHASE_SEG_2		PHASE_SEG_1
#endif

#define	SJW_VALUE			1
#define	BRP_VALUE			(F_CPU/CAN_BAUD_RATE/16)

#if (BRP_VALUE > 0x3F)
	#warning CAN Baud Rate too LOW
#endif
#if (PHASE_SEG_2 < 2)
	#warning Propagation time is too long!
#endif
#if (F_CPU%CAN_BAUD_RATE)
	#warning Clock (F_CPU) is not a multipe of CAN Baud rate!
#endif
#if !(TX_SIZE==64 || TX_SIZE==32 || TX_SIZE==16 || TX_SIZE==8 \
	|| TX_SIZE==4 || TX_SIZE==2 || TX_SIZE==1)
	#warning:Wrong CAN TX Buffer Size
#endif
#if !(RX_SIZE==64 || RX_SIZE==32 || RX_SIZE==16 || RX_SIZE==8 \
	|| RX_SIZE==4 || RX_SIZE==2 || RX_SIZE==1)
	#warning:Wrong CAN RX Buffer Size
#endif

#define RX_ABS_MASK		0x7F
#define TX_ABS_MASK		0x7F

/**************************************************************************************************
*   Internal Variables
**************************************************************************************************/
union can_frame tx_frames[TX_SIZE];
union can_frame rx_frames[RX_SIZE];
static unsigned char tx_off;
static unsigned char tx_on;
static unsigned char tx_busy;
static unsigned char rx_off;
static unsigned char rx_on;

/**************************************************************************************************
*   init_can(); - See 'can.h' Header file for Description
**************************************************************************************************/
void init_can(void)
{
 CANGCON = _BV(SWRES);							//Reset CAN controller
 
 CANBT1 = (BRP_VALUE - 1) << 1;
 CANBT2 = ((SJW_VALUE - 1) << 5) | ((PROP_SEG - 1) << 1);
 CANBT3 = ((PHASE_SEG_2 - 1) << 4) | ((PHASE_SEG_1 - 1) << 1) | 1;
 
 CANTIM = 0;
 CANTTC = 0;

 CANHPMOB = 0;
 CANTCON = 0;
 
 CANPAGE = 0 << 4;								//Switch to Mob 0 access
 CANSTMOB = 0;
 CANPAGE = 1 << 4;								//Switch to Mob 1 access
 CANSTMOB = 0;
 #if CAN_V==CAN2A
 CANIDM4 = ACCPT_MASK_RTR << 2 | ACCPT_MASK_IDE;
 CANIDM2 = (ACCPT_MASK_ID << 5) & 0xFF;
 CANIDM1 = (ACCPT_MASK_ID >> 3) & 0xFF;
 CANIDT4 = ACCPT_TAG_RTR << 2 | ACCPT_TAG_RB0;
 CANIDT2 = (ACCPT_TAG_ID << 5) & 0xFF;
 CANIDT1 = (ACCPT_TAG_ID >> 3) & 0xFF;
 #elif CAN_V==CAN2B
 CANIDM4 = ((ACCPT_MASK_ID << 3) & 0xFF) | ACCPT_MASK_RTR << 2 | ACCPT_MASK_IDE;
 CANIDM3 = (ACCPT_MASK_ID >> 5) & 0xFF;
 CANIDM2 = (ACCPT_MASK_ID >> 13) & 0xFF;
 CANIDM1 = ((long)ACCPT_MASK_ID >> 21) & 0xFF;
 CANIDT4 = ((ACCPT_TAG_ID << 3) & 0xFF) | ACCPT_TAG_RTR << 2 | ACCPT_TAG_RB1 << 1 | ACCPT_TAG_RB0;
 CANIDT3 = (ACCPT_TAG_ID >> 5) & 0xFF;
 CANIDT2 = (ACCPT_TAG_ID >> 13) & 0xFF;
 CANIDT1 = ((long)ACCPT_TAG_ID >> 21) & 0xFF;
 #endif

 CANCDMOB = _BV(CONMOB1) | CAN_IDE;			//Set Mob 1 as RX and IDE
 
 CANEN2 = _BV(ENMOB1) | _BV(ENMOB0);			//Enable Mob 0 and 1
 CANIE2 = _BV(ENMOB1) | _BV(ENMOB0);			//Enable Mob 0 and 1 Interrupt
 CANGIE = _BV(ENIT) | _BV(ENRX) | _BV(ENTX);	//Enable TX and RX interrupt
 CANGCON = _BV(ENASTB);							//Enable CAN controller
}

/**************************************************************************************************
*   CAN ISR - See 'can.h' Header file for Description
**************************************************************************************************/
SIGNAL(SIG_CAN_INTERRUPT1)
{
 unsigned char can_irq;
 volatile unsigned char canstmod;
 
 can_irq = CANSIT2;
 
 // TX
 if (can_irq & _BV(SIT0) && CANIE2 & _BV(ENMOB0)) {
 	//Select TX Mob (=Mob0)
	CANPAGE = 0 << 4;
	canstmod = CANSTMOB; 
	CANSTMOB&= ~_BV(TXOK);		//clear MB1, TX interrupt
	if (tx_on != tx_off) {
 		unsigned char pos;
 		pos = tx_off & (TX_SIZE-1);
 		//set ID
		#if CAN_V==CAN2A
		CANIDT4 = tx_frames[pos].array[0]; 
		CANIDT2 = tx_frames[pos].array[0];
		CANIDT1 = tx_frames[pos].array[1];
		#elif CAN_V==CAN2B
		CANIDT4 = tx_frames[pos].array[0]; 
		CANIDT3 = tx_frames[pos].array[1];
		CANIDT2 = tx_frames[pos].array[2];
		CANIDT1 = tx_frames[pos].array[3];
		#endif
		//program data registers - auto increment CANMSG
		CANMSG = tx_frames[pos].data[0];
		CANMSG = tx_frames[pos].data[1];
		CANMSG = tx_frames[pos].data[2];
		CANMSG = tx_frames[pos].data[3];
		CANMSG = tx_frames[pos].data[4];
		CANMSG = tx_frames[pos].data[5];
		CANMSG = tx_frames[pos].data[6];
		CANMSG = tx_frames[pos].data[7];
		//set length and request send
		CANCDMOB = _BV(CONMOB0) | CAN_IDE | tx_frames[pos].length;
		tx_off++;
 	}
 	else {
		tx_busy = 0;
	}
 }
 // RX
 else if (can_irq & _BV(SIT1) && CANIE2 & _BV(ENMOB1)) {
 	//Select RX Mob (=Mob1)
	CANPAGE = 1 << 4;							//Switch to Mob 1 access
	if (((rx_on - rx_off) & RX_ABS_MASK) < RX_SIZE) {
		unsigned char pos;
 		pos = rx_on & (RX_SIZE-1);
 		//Read length
		rx_frames[pos].length = CANCDMOB & 0x0F;
		//Read ID
		#if CAN_V==CAN2A
		rx_frames[pos].array[0] = (CANIDT2 & 0xE0) | (CANIDT4 & 0x07);
		rx_frames[pos].array[1] = CANIDT1;
		#elif CAN_V==CAN2B
		rx_frames[pos].array[0] = CANIDT4; 
		rx_frames[pos].array[1] = CANIDT3;
		rx_frames[pos].array[2] = CANIDT2;
		rx_frames[pos].array[3] = CANIDT1;
		#endif
		//read data registers - auto increment CANMSG
		rx_frames[pos].data[0] = CANMSG;
		rx_frames[pos].data[1] = CANMSG;
		rx_frames[pos].data[2] = CANMSG;
		rx_frames[pos].data[3] = CANMSG;
		rx_frames[pos].data[4] = CANMSG;
		rx_frames[pos].data[5] = CANMSG;
		rx_frames[pos].data[6] = CANMSG;
		rx_frames[pos].data[7] = CANMSG;
 		rx_on++;	
 	}
	//clear irq
	canstmod = CANSTMOB;
	CANSTMOB&= ~_BV(RXOK);
	CANCDMOB = _BV(CONMOB1) | CAN_IDE;			//Set Mob 1 as RX and IDE
 }
}

/**************************************************************************************************
*   send_can_frame(); - See 'can.h' Header file for Description
**************************************************************************************************/
unsigned char send_can_frame(union can_frame *frame)
{
 unsigned char result;
 
 result = 0;
 CANGIE&= ~_BV(ENIT);
 if (!tx_busy) {
	CANPAGE = 0 << 4;							//Switch to Mob 0 access
	//set ID
	#if CAN_V==CAN2A
	CANIDT4 = frame->array[0]; 
	CANIDT2 = frame->array[0];
	CANIDT1 = frame->array[1];
	#elif CAN_V==CAN2B
	CANIDT4 = frame->array[0]; 
	CANIDT3 = frame->array[1];
	CANIDT2 = frame->array[2];
	CANIDT1 = frame->array[3];
	#endif
	//program data registers - auto increment
	CANMSG = frame->data[0];
	CANMSG = frame->data[1];
	CANMSG = frame->data[2];
	CANMSG = frame->data[3];
	CANMSG = frame->data[4];
	CANMSG = frame->data[5];
	CANMSG = frame->data[6];
	CANMSG = frame->data[7];
	result = 1;
	tx_busy = 1;
	//set length, start send which restarts tx interrupt	
	CANCDMOB = _BV(CONMOB0) | CAN_IDE | frame->length;
 }
 else if (TX_SIZE - ((tx_on - tx_off) & TX_ABS_MASK)) {
 	result = 1;
	unsigned char i;
 	for (i=0;i<CAN_FRAME_SIZE;i++) {
		tx_frames[tx_on & (TX_SIZE-1)].array[i] = frame->array[i];
 	}
	tx_on++;
 	result = 1;	
 }
 CANGIE|= _BV(ENIT);
 return result;
}

/**************************************************************************************************
*   read_can_frame(); - See 'can.h' Header file for Description
**************************************************************************************************/
union can_frame *read_can_frame(void)
{
 return &rx_frames[(rx_off & (RX_SIZE-1))];	
}

/**************************************************************************************************
*   next_can_frame(); - See 'can.h' Header file for Description
**************************************************************************************************/
void next_can_frame(void)
{
 if (rx_on != rx_off) {
 	rx_off++;
 }
}

/**************************************************************************************************
*   new_can_frame(); - See 'can.h' Header file for Description
**************************************************************************************************/
unsigned char new_can_frame(void)
{
 unsigned char result;
 
 if (rx_on != rx_off)							//check for new data on rx pdu buffer
	result = 1;
 else
	result = 0;
 return result;		
}
