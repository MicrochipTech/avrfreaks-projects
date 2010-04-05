#ifndef ZO_BUFFER_H
#define ZO_BUFFER_H

#include "zoTypes.h"

//Types___________________________________________________________________________________
typedef volatile struct{
	volatile u08 size;	// Size of the ring buffer
	volatile u08 ctr;	// how many bytes are currently in the buffer
	u08* pIn;			// where to put next incoming byte
	u08* pOut;			// from where to take next outgoing byte
	u08 *data;			// byte storage
} ZO_BUFFER;

#define ZO_BUFFER_DEFAULTS {0,0,0,0,0}

//High level functionality________________________________________________________________
bool zoBufferInit(ZO_BUFFER *p,u08 size);
u08  zoBufferGet(ZO_BUFFER *p);
void zoBufferPut(ZO_BUFFER *p,const u08 data);
u08  zoBufferRead(ZO_BUFFER *p,u08 index);
bool zoBufferIsEmpty(ZO_BUFFER *p);
bool zoBufferIsFull(ZO_BUFFER *p);
void zoBufferFlush(ZO_BUFFER *p);

//Unprotected inline versions for use in ISRs_____________________________________________
void zoBufferPutIsr(ZO_BUFFER *p,const u08 data);
u08  zoBufferGetIsr(ZO_BUFFER *p);
u08	 zoBufferReadIsr(ZO_BUFFER *p,u08 index);

#endif //ZO_BUFFER_H