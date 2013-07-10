#ifndef _SCCB_H
#define _SCCB_H

#include "avr_compiler_.h"
#include"VARIABLE.h"
#include "delay.h"

#define SCCB_DDR		PORTC_DIR
#define SCCB_PORT		PORTC_OUT
#define SCCB_PORTSET            PORTC_OUTSET
#define SCCB_PORTCLR		PORTC_OUTCLR
#define SCCB_PIN		PORTC_IN

#define SCCB_SIO_C		6
#define SCCB_SIO_D		4

#define SIO_C_SET		SCCB_PORTSET=1<<SCCB_SIO_C
#define SIO_C_CLR		SCCB_PORTCLR=1<<SCCB_SIO_C
#define SIO_D_SET		SCCB_PORTSET=1<<SCCB_SIO_D
#define SIO_D_CLR		SCCB_PORTCLR=1<<SCCB_SIO_D

#define SIO_D_IN		SCCB_DDR&=~(1<<SCCB_SIO_D)
#define SIO_D_OUT		SCCB_DDR|=1<<SCCB_SIO_D

#define SIO_D_STATE	        ((SCCB_PIN&(1<<SCCB_SIO_D))==(1<<SCCB_SIO_D))


///////////////////////////////////////////
void DelaySCCB(void);
void InitSCCB(void);
void startSCCB(void);
void stopSCCB(void);
void noAck(void);
uchar SCCBwriteByte(uchar m_data);
uchar SCCBreadByte(void);

#endif


