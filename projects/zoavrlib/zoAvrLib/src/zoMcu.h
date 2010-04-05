//**************************************************************************************
///	File Name	: \file zoMcu.h 
///	Description	: \brief Mcu global-interrupt-enable-bit manipulation macros.
//	Created		: 21/01/2010
//	Target MCU	: AVR
//	Author		: Sissakis Giannis
//  email		: info@01mech.com
//
//  Copyright (C) 2010 Zero One Mechatronics LP
//
//  This program is free software: you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation, 
//  version 3 of the License or any later version. This program is distributed in the hope
//  that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public 
//  License for more details: <http://www.gnu.org/licenses/>
//  
/// \ingroup soft 
/// \defgroup zoMcu zoMcu.h : Global-interrupt-enable-bit manipulation macros
/// 
/// \par Overview
/// This file contains macros for manipulation of the global interrupt enable bit in the 
/// CPU status register. These macros are mainly used to protect global variables from 
/// being corrupted by ISR code. The use of the enterCritical() and exitCritical() macros 
/// is recommended as they do not disturb the previous state of the global interrupt bit .
/// These macros use the stack as storage for the CPU status register, so it is obligatory
/// to call them in pairs. ie an enterCritical() must ALWAYS be followed by an 
/// exitCritical() macro. Otherwise the stack will be corrupted with unpredictable 
/// results.
///   
/// \par Usage example 
/// \code
/// #include "zoMcu.h"
///
/// u16 MyGlobalVariable = 0;
///
/// int main(void)
/// {
/// 	enterCritical();							//protect global from ISR interference
/// 	MyGlobalVariable = MyGlobalVariable + 1;	//manipulate global variable 
/// 	exitCritical();								//allow ISRs to do their job
/// }
/// \endcode 
//****************************************************************************************
//@{

#ifndef ZO_MCU_H
#define ZO_MCU_H

///Disable global interrupts by directly reseting the global-interrupt-enable bit. 
#define cli()			__asm__ __volatile__ ("cli" ::)
///Enable global inteerupt by directly setting the global-interrupt-enable bit.
#define sei()			__asm__ __volatile__ ("sei" ::)

///Store the status register in the stack and then reset the global-interrupt-enable bit.
#define enterCritical() \
__asm__ __volatile__ ("in __tmp_reg__, __SREG__\n\t " \
					  "push __tmp_reg__\n\t" \
					  "cli" ::)

///Restore the status register from the stack, leaving interrupts at the state they were
///before the enterCritical() macro
#define exitCritical() \
__asm__ __volatile__ ("pop __tmp_reg__ \n\t" \
					  "out __SREG__, __tmp_reg__" ::)  

#endif //ZO_MCU_H
//@}