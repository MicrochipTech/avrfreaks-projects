#ifndef _SCI_H
#define _SCI_H 1

/*************************************************************//**
* \file sci.h
*
* SCI/UART interface
*
* \author Stefan Filipek
* \date March 2009
*
******************************************************************/

//! Globally accessable. If not defined, here, defaults to 9600.
#define __B_RATE 57600

void SCI_Init(void);

#endif
