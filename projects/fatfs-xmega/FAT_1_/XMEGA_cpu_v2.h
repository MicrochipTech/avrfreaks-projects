#ifndef XM_CPU_V2
#define XM_CPU_V2
#include "avr_compiler.h"

#define F_CPU_      32000000UL        

#define	XMEGA_Ax    1
/*
 0:A4 
 1:A3 
 2:A1 
*/	


#define	SPI_MODULE  1
/*
 0:SPIC 
 1:SPID 
 2:SPIE 
 3:SPIF 
*/				
/*---------------------------------------
  SPI
----------------------------------------*/
													
#if SPI_MODULE==0
#define   	SPI_INTERRUPT   SPIC_INT_vect
#define   	SPI_PORT_INIT   PORTC_OUTSET=PIN7_bm;PORTC_DIRSET=PIN7_bm|PIN5_bm|PIN4_bm;\
				PORTC_PIN6CTRL=PORT_OPC_PULLUP_gc;   // set SCK,MOSI,SS as output SCK=1
#define   	SPI_DATA        SPIC_DATA
#define   	SPI_STATUS      SPIC_STATUS
#define   	SPI_CTRL        SPIC_CTRL
#define   	SPI_INTCTRL     SPIC_INTCTRL
#define   	mmc_port_init 	PORTC_DIRSET=PIN4_bm
#define   	CS_LOW()	PORTC_OUTCLR=PIN4_bm
#define	  	CS_HIGH()	PORTC_OUTSET=PIN4_bm

#elif SPI_MODULE==1
#define   	SPI_INTERRUPT   SPID_INT_vect
#define   	SPI_PORT_INIT   PORTD_OUTSET=PIN7_bm;PORTD_DIRSET=PIN7_bm|PIN5_bm|PIN4_bm;\
				PORTD_PIN6CTRL=PORT_OPC_PULLUP_gc;   // set SCK,MOSI,SS as output SCK=1
#define   	SPI_DATA        SPID_DATA
#define   	SPI_STATUS      SPID_STATUS
#define   	SPI_CTRL        SPID_CTRL
#define   	SPI_INTCTRL     SPID_INTCTRL
#define   	mmc_port_init 	PORTD_DIRSET=PIN4_bm
#define   	CS_LOW()	PORTD_OUTCLR=PIN4_bm
#define	  	CS_HIGH()	PORTD_OUTSET=PIN4_bm

#elif SPI_MODULE==2
#if   XMEGA_Ax>0

#define   	SPI_INTERRUPT   SPIE_INT_vect
#define   	SPI_PORT_INIT   PORTE_OUTSET=PIN7_bm;PORTE_DIRSET=PIN7_bm|PIN5_bm|PIN4_bm;\
				PORTE_PIN6CTRL=PORT_OPC_PULLUP_gc;   // set SCK,MOSI,SS as output SCK=1
#define   	SPI_DATA        SPIE_DATA
#define   	SPI_STATUS      SPIE_STATUS
#define   	SPI_CTRL        SPIE_CTRL
#define   	SPI_INTCTRL     SPIE_INTCTRL
#define   	mmc_port_init 	PORTE_DIRSET=PIN4_bm
#define   	CS_LOW()	PORTE_OUTCLR=PIN4_bm
#define	  	CS_HIGH()	PORTE_OUTSET=PIN4_bm
#else
#error	Not supported
#endif

#elif SPI_MODULE==3
#if   XMEGA_Ax==2

#define   	SPI_INTERRUPT   SPIF_INT_vect
#define   	SPI_PORT_INIT   PORTF_OUTSET=PIN7_bm;PORTF_DIRSET=PIN7_bm|PIN5_bm|PIN4_bm;\
				PORTF_PIN6CTRL=PORT_OPC_PULLUP_gc   // set SCK,MOSI,SS as output SCK=1
#define   	SPI_DATA        SPIF_DATA
#define   	SPI_STATUS      SPIF_STATUS
#define   	SPI_CTRL        SPIF_CTRL
#define   	SPI_INTCTRL     SPIF_INTCTRL
#define   	mmc_port_init 	PORTF_DIRSET=PIN4_bm
#define   	CS_LOW()	PORTF_OUTCLR=PIN4_bm
#define	  	CS_HIGH()	PORTF_OUTSET=PIN4_bm
#else
#error	Not supported
#endif

#else
#error	Not supported
#endif

#if F_CPU_ >= 16000000UL
#define   SPI_CTRL_SLOW     SPI_CTRL=SPI_ENABLE_bm|SPI_MASTER_bm|SPI_MODE_0_gc|SPI_PRESCALER_DIV128_gc

#elif F_CPU_ >= 4000000UL	
#define   SPI_CTRL_SLOW     SPI_CTRL=SPI_ENABLE_bm|SPI_MASTER_bm|SPI_MODE_0_gc|SPI_PRESCALER_DIV64_gc

#else
#define   SPI_CTRL_SLOW     SPI_CTRL=SPI_ENABLE_bm|SPI_MASTER_bm|SPI_MODE_0_gc|SPI_PRESCALER_DIV16_gc
#endif

#define   SPI_CTRL_FAST     SPI_CTRL=SPI_CLK2X_bm|SPI_ENABLE_bm|SPI_MASTER_bm|SPI_MODE_0_gc|SPI_PRESCALER_DIV4_gc


/*---------------------------------------
  TIMERS
----------------------------------------*/
#define   TC_PER              TCC0_PER
#define   TC_OVF_INTERRUPT    TCC0_OVF_vect
#define   TC_INTCTRLA         TCC0_INTCTRLA
#define   TC_CTRLA            TCC0_CTRLA
#endif