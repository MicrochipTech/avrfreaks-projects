//! @file $RCSfile: config.h,v $
//!
//! Copyright (c) 2004 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the low level definitions for the megaballsat Reference Design use
//!
//! @version $Revision: 1.7 $ $Name: megaballast-refd-0_2_9 $
//!
//! @todo
//! @bug

#ifndef CONFIG_H
#define CONFIG_H

//_____  I N C L U D E S ___________________________________________________

#include "common/lib_mcu/compiler.h"
#include "common/lib_mcu/mcu.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________

#define FOSC		          8000000  //!< Oscillator Frequency : 8MHz

//Interruption sense.
//0 : low level, 1: any edge, 2: falling edge, 3: rising edge
#define INT0_SNS          3 
#define INT1_SNS          3 
#define INT2_SNS          3 
#define INT3_SNS          1 

#endif  // CONFIG_H
