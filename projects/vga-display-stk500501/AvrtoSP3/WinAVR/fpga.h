/*******************************************************************************
** fpga.h
**
** drivers for everything in the xilinx bus interface
**
** created Oct 10, 2007
** Jim Patchell
**
*******************************************************************************/

#ifndef FPGA__H
#define FPGA__H

//******************************************************************
// register defines
//******************************************************************

#define FPGA_LED				*((volatile unsigned char *)0xf000)


#define FPGA_CONTROLREG			*((volatile unsigned char *)0xf020)


////////////////////////////////////////////////////////////////////////////////
// function prototypes
////////////////////////////////////////////////////////////////////////////////

#ifndef BIT
#define BIT(x)	(1 << (x))
#endif

#endif
