#ifndef ZO_ERROR_H
#define ZO_ERROR_H

#include "zoTypes.h"

//Fixed size buffer used for error storage

#define ZO_ERROR_STORAGE_SIZE 8

typedef volatile struct {
	volatile u08 	ctr;							// how many bytes are currently in the buffer
	volatile u08* 	pIn;							// where to put next incoming byte
	volatile u08*   pOut;							// from where to take next outgoing byte
	volatile u08    error[ZO_ERROR_STORAGE_SIZE];	// byte storage
}ZO_ERROR;

#define ZO_ERROR_DEFAULTS {0,0,0,{0,0,0,0,0,0,0,0}}

void zoErrorInit(ZO_ERROR *p);
u08 zoErrorGetIsr(ZO_ERROR *p);
u08 zoErrorGet(ZO_ERROR *p);
void zoErrorPutIsr(ZO_ERROR *p,const u08 error);
void zoErrorPut(ZO_ERROR *p,const u08 error);
bool zoErrorIsEmpty(ZO_ERROR *p);

#endif //ZO_ERROR_H

