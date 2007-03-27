#ifndef __D12_INT_H__
#define __D12_INT_H__

#include"D12_Operation.h"
#include"USB_Descriptor.h"
#include"CHAP_9.h"


U8 USB_Init(void);
void USB_Delay(U16 Time);
void D12Ep0IntProc(void);
void D12Ep1IntProc(void);
void D12Ep2IntProc(void);
void D12Ep3IntProc(void);
void D12Ep4IntProc(void);
void D12Ep5IntProc(void);
void D12BusRstProc(void);
void D12SuspChgProc(void);

#define	UsbIntProc(i)	\
		if(i&D12Ep0Int)\
			D12Ep0IntProc();\
		if(i&D12Ep1Int)\
			D12Ep1IntProc();\
		if(i&D12Ep2Int)\
			D12Ep2IntProc();\
		if(i&D12Ep3Int)\
			D12Ep3IntProc();\
		if(i&D12Ep4Int)\
			D12Ep4IntProc();\
		if(i&D12Ep5Int)\
			D12Ep5IntProc();\
		if(i&D12BusRst)\
			D12BusRstProc();\
		if(i&D12SuspChg)\
			D12SuspChgProc();

#endif
