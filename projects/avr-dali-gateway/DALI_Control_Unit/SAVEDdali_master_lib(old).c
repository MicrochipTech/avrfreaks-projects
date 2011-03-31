//**************************************************************************
//! @file $RCSfile: dali_lib.c,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains DALI lib routines available for DEVICE TYPE 0.
//!
//! This template file can be parsed by Doxygen for automatic documentation
//! generation.
//! Put here the functional description of this file within the software
//! architecture of your program.
//!
//! @version $Revision: 1.0 $ $Name: DALI_Control_Unit$
//!
//! @todo
//! @bug
//**************************************************************************

//_____ I N C L U D E S ____________________________________________________

#include "config.h"
#include "dali_master_lib.h"

//_____ M A C R O S ________________________________________________________

#define dali_disable_timer()		(TIMSK0 = 0x00)	

#define DALI_TX(x)		(x==0 ? (DALI_TX_PORT |= (1<<DALI_TX_PIN)) : (DALI_TX_PORT &= ~(1<<DALI_TX_PIN)))
#define DALI_RX				((DALI_RX_PORT & (1<<DALI_RX_PIN)) == 0 ? 1 : 0)	
#define LED(x)        (x==0 ? (PORTA &= ~(1<<PINA4)) : (PORTA |= (1<<PINA4)) )

//_____ D E F I N I T I O N S ______________________________________________

volatile U8 postscale = 0;
U8 daliBitN = 0;
Bool daliBusy = 0;
U8 daliRxBuffer = 0;
U16 daliTxBuffer = 0;
U16 daliTxMask = 0x8000;
Bool daliRepeat = FALSE;
Bool daliAnswer = FALSE;
Bool daliMode = FW; // indicates if device is transmitting a FW frame or receiving a BW frame

//interrupt RX :
  //daliBitN = 0;
  //daliMode == BW;
  //set timer OF to 1/2Te (dali enable RX Timer)

#pragma vector = TIMER0_COMPA_vect
__interrupt void dali_tick(void);

__interrupt void dali_tick(void)
{
  daliBitN ++;	
  
  if (daliMode == FW){
    switch (daliBitN)
    {
      case 1:{       //start bit : 1st half
        DALI_TX(0);
        LED(0);
        break;
      }
      case 2:{       //start bit : 2nd half
        DALI_TX(1);
        LED(1);
        break;
      }
      case 35:{      //1st stop bit
        DALI_TX(1);
        LED(1);
        break; 
      }
      case 36:{      //2nd stop bit
        DALI_TX(1);
        LED(1);
        break;
      }
      case 37:{      //3rd stop bit
        DALI_TX(1);
        LED(1);
        break;  
      }
      case 38:{      //last stop bit
        DALI_TX(1);
        LED(1);
        break;
      }
      case 44:{      //stop + 6 Te (answer frame can be received)
        if (daliAnswer == 1){
        //activate INT
        }
        break;
      }
      case 62:{      //stop + 23 Te (minimum time between 2 FW frames)
        DALI_TX(1);
        LED(1);
        if (daliRepeat == 0 && daliAnswer == 0){  //a single FW frame is sent. Busy is cleared after FWframe + 23Te
          daliBusy = 0;
          dali_disable_timer();
        }
        if (daliRepeat == 1 && daliAnswer == 0){  //repeats the FW frame (needed for some control frames)
          daliRepeat = 0;                         //clears Repeat flag (next it won't be repeated)
          daliBitN = 0;                           //sets bit index to 0
          daliTxMask = 0x8000;                    //reset mask to 1st bit.
        }
        break;
      }
      case 67:{    //if no BW frame has been received within 28Te after stop, the answer is "no", end of transmission
        if (daliMode == FW && daliAnswer == 1){
        daliRxBuffer = 0x00;
        daliAnswer = 0;
        daliBusy = 0;
        dali_disable_timer();
        //disable INT
        }
      }
      
      default:{
        if (daliBitN < 35){       //data bits to send (half bits for manchester encoding)
          if (daliBitN & 0x01){               //odd half bits
            if (daliTxBuffer & daliTxMask){	
              DALI_TX(0);
              LED(0);
            }
            else{
              DALI_TX(1);
              LED(1);
            }
          }
          else{                              //even half bits
            if (daliTxBuffer & daliTxMask){	
              DALI_TX(1);
              LED(1);
            }
            else{
              DALI_TX(0);
              LED(0);
            }
          daliTxMask = daliTxMask >> 1;
          }
        }
        else{
          DALI_TX(1);             //half bits >37 && <61 : waiting before a new Frame
          LED(1);
        }
        break;
      }//end default:
    }//end switch
  }//end if
  else{       //DALI is in BW mode
    switch (daliBitN){
      case 1:{       //start bit : middle of 1st half (1/4Te)
        GTCCR = 0x00; //resets timer prescaler
        OCR0A = COMP_VALUE; //sets timer to overflow after Te
        break;
      }
      case 46:{    //BW stop + 24 Te : end of transmission
        daliAnswer = 0;
        daliBusy = 0;
        dali_disable_timer();
        break;
      }
      default:{
        if (daliBitN>=4 && !daliBitN&0x01 && daliBitN <= 18){ //not start bit and only for 2nd halves (even) , for 8 bits
          if (DALI_RX){
            daliRxBuffer |= (1<<(8-(daliBitN>>1 - 1)));  //if RX pin is high, it sets the corresponding bit in RX buffer
          }
          else{
            daliRxBuffer &= ~(1<<(8-(daliBitN>>1 - 1)));  //if RX pin is low, the corresponding bit is cleared
          }
        }
      }//end default
    }//end switch
  }//end else
  
  return;
}

Bool dali_send_frame(U8 byte1, U8 byte2, Bool repeat, Bool answer)
{
	if (daliBusy) //&& DALI_RX_PIN LOW (=power OK && line idle)
		return FALSE;						//"sorry I can't send"
	else
	{
		daliBusy = TRUE;	
		daliRxBuffer = 0;				//clear RxBuffer
		daliBitN = 0;						//sets bit index at 0
		daliTxMask = 0x8000;
		daliTxBuffer = byte1<<8;		//2 bytes to send
		daliTxBuffer += byte2;
		daliRepeat = repeat;		//repeat frame ?
		daliAnswer = answer;		//wait for answer ?
    daliMode = FW;
		dali_enable_TX_timer();		//start sending
		
		return TRUE;						//"OK I'm sending"
	}
}

void dali_enable_TX_timer(void)
{
	// ****** Inits timer 0 to generate an interrupt every 416탎 (1/2 dali bit=Te), used for FW Frames
	
	//reset prescaler GTCCR
		//0 : a 1 stops the prescaler to synchronise timers
		//0 : input capture pin selection (not used)
		//00000 : reserved
		//1 : resets the prescaler
	
	GTCCR = 0x00;
	
	//init	TCCR0A : 
		//00 : no connection with output compare pin
		//00 : no connection with output compare pin
		//00 : reserved
		//10 :	clear timer on compare match
	
	TCCR0A = 0x02;
	
	//init	TCCR0B :
		//00 : no force output compare
		//00 : reserved
		//0 : clear timer on compare match
		//010 : internal clock, Prescaler : PREDIV macro
	
	TCCR0B = 0x00 + PREDIV;
	//TCCR0B = 0x05;
	//set compare value OCR0A
		//OCR0A = 0x34 (52d --> 416탎 period)	COMP_VALUE macro
		//OCR0B = 0xFF (not used)
	
	OCR0A = COMP_VALUE;
	OCR0B = 0xFF;
	
  //clear interrupt flags (OCF0B, OCF0A, TOV0)
  
 	TIFR0 |= 0x02;
  
	//activate interrupts TIMSK0
		//00000 : reserved
		//010 : interrupt on match with OCR0A

	TIMSK0 |= 0x02;
  return;
}	

void dali_enable_RX_timer(void)
{
	// ****** Inits timer 0 to generate an interrupt after 208탎 (0.5Te).
  //This samples the 1st half of the start byte. The timer will then overflow
  //after Te for 33 periods to sample received bits.
	
	//reset prescaler GTCCR
		//0 : a 1 stops the prescaler to synchronise timers
		//0 : input capture pin selection (not used)
		//00000 : reserved
		//1 : resets the prescaler
	
	GTCCR = 0x01;
	
	//init	TCCR0A : 
		//00 : no connection with output compare pin
		//00 : no connection with output compare pin
		//00 : reserved
		//10 :	clear timer on compare match
	
	TCCR0A = 0x02;
	
	//init	TCCR0B :
		//00 : no force output compare
		//00 : reserved
		//0 : clear timer on compare match
		//010 : internal clock, Prescaler : PREDIV macro
	
	TCCR0B = 0x00 + PREDIV;
	//TCCR0B = 0x05;
	//set compare value OCR0A
		//OCR0A = 0x34 (52d --> 416탎 period)	COMP_VALUE macro
		//OCR0B = 0xFF (not used)
	
	OCR0A = COMP_VALUE>>1;  //first interrupt on 1/4Te. 
	OCR0B = 0xFF;
	
  //clear interrupt flags (OCF0B, OCF0A, TOV0)
  
 	TIFR0 |= 0x02;
  
	//activate interrupts TIMSK0
		//00000 : reserved
		//010 : interrupt on match with OCR0A

	TIMSK0 |= 0x02;
  return;
}	
