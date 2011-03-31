//**************************************************************************
//! @file $RCSfile: dali_lib.h,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains all prototypes and macros of DALI lib routines
//!        for DEVICE TYPE 0.
//!
//! @version $Revision: 1.0 $ $Name: DALI_Control_Unit $
//!
//! @todo
//! @bug
//**************************************************************************


#ifndef _DALI_LIB_H_
#define _DALI_LIB_H_

//_____ D E F I N I T I O N S ______________________________________________

// @brief choose wich PIN is used for DALI_TX and DALI_RX

#define DALI_TX_PIN			PIND3
#define DALI_TX_PORT		PORTD
#define DALI_RX_PIN			PIND4
#define DALI_RX_PORT		PORTD
#define FW              TRUE
#define BW              FALSE

//_____ M A C R O S _____(mapped on EUSART)_________________________________

//	@brief calculates prediv value for TCCRxB 
// 	@brief calculates compare match value to generate DALI Clock

#if FOSC < 500000 
	#define PREDIV				1	//prediv 1
	#define COMP_VALUE		((Uint8)(417.*FOSC/1000000.))
#elif FOSC < 4000000
	#define PREDIV 				2	//prediv 8
	#define COMP_VALUE		((Uint8)(417.*FOSC/8000000.))
#else
	#define PREDIV	 			3 //prediv 64
	#define COMP_VALUE		((Uint8)(417.*FOSC/64000000.))
#endif

//_____ P R O T O T Y P E S ________________________________________________

//--------------------------------------------------------------------------
//  @fn dali_enable_TX_timer
//!
//! DALI timer initialization. Sets the predivider value and compare match 
//!	value to generate an interrupt
//!
//! @warning none
//!
//! @param  none
//!
//! @return none
//!       
//!         
//!

extern void dali_enable_TX_timer(void);

//--------------------------------------------------------------------------
//  @fn dali_enable_RX_timer
//!
//! DALI timer initialization. Sets the predivider value and compare match 
//!	value to generate an interrupt
//!
//! @warning none
//!
//! @param  none
//!
//! @return none
//!       
//!         
//!

extern void dali_enable_RX_timer(void);

//--------------------------------------------------------------------------
//  @fn dali_send_frame
//!
//! This function sends a DALI frame, repeats it if necessary, and waits
//! the answer if needed. busy flag is set during transmission and is cleared
//! when frame is transmitted and answer received.
//! @warning none
//!
//! @param  
//!					byte1 : 1st byte to send
//!					byte2 : 2nd byte to send
//!					repeat : if set, the frame is repeated after 100ms
//!					answer : if set, the function waits the answer before clearing busy flag
//!
//! @return Status
//!         ==TRUE :  frame transmitting...
//!         ==FALSE : can't send (busy is already set)    
//!         
//!

extern Bool dali_send_frame(U8 byte1, U8 byte2, Bool repeat, Bool answer);

#endif // _DALI_LIB_H_
