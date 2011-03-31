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

#define DALI_TX_PIN			PIND4
#define DALI_TX_PORT		PORTD
#define DALI_RX_PIN			PIND3
#define DALI_RX_PORT		PIND       

#define END_DELAY       23  //(delay of 23 Te, see DALI spec.)
#define MIN_FW_BW_DELAY 6   //min delay between FW and BW frame
#define MAX_FW_BW_DELAY 23    

//_____ M A C R O S _____(mapped on EUSART)_________________________________

//	@brief calculates prediv value for TCCRxB 
// 	@brief calculates compare match value to generate DALI Clock

#define PREDIV        3 //prediv 64

#if FOSC == 8000000
  #define TE_PERIOD   52
  #define MS_PERIOD   125
#elif FOSC == 16000000
  #define TE_PERIOD   104
  #define MS_PERIOD   250
#else 
  #error FOSC must be defined at 8000000 or 16000000
#endif


//_____ P R O T O T Y P E S ________________________________________________

extern U8 dali_send_command(U8 byte1, U8 byte2, Bool repeat, Bool answer, U8 *rxByte, U8 repeat_delay);
extern bit dali_send(U8 txByte1, U8 txByte2);
extern bit dali_receive(U8 *rxByte);
extern void dali_start_timer_0(U8 period);
extern void dali_stop_timer_0(void);

extern void wait_te(U8 nbTe);
#endif // _DALI_LIB_H_
