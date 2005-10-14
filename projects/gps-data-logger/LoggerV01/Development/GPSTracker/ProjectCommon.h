

#ifndef INCLUDE_ProjectCommon
#define INCLUDE_ProjectCommon
// Priorities

#define TSKPRI_taskLogData       2  //highr
#define TSKPRI_taskUserInterface 3  //lower

#include <inttypes.h>

#include "PCSerialIO.h"

//each error/status message is a 16 bit number of the structure

//0x<code><LED><t1 part><t0 part>
//0x  0    0         0		0

// <code> allows status codes to be uniquely id'd (for msg to PC ), does not
// effect the flashing.

//flash sequence is on/off for t0 cycles, then on/off for t1 cycles, repeated 5 times. 
//This allows the led to flash or stay on for a specified time.
//the number t0 or t1 is not a mask, but a counter, eg t0=3 indicates its
//on for the first 3 t0 cycles, the off for the remaining 2 cycles.

// cycle            1  2  3  4  5  6  7  8  9 10
// flash sequence  t0 t1 t0 t1 t0 t1 t0 t1 t0 t1
//          --------------------------------------
//eg value         1  0  1  0  1  0  0  0  0  0    (ie 3 flashes, value of 0x03)
//eg value         1  1  1  1  1  1  1  1  1  1    (ie full on, value of   0x55)
//eg value         1  1  1  1  1  0  0  0  0  0    (ie on 1/2 time, value of 0x23

//ie value is the number of times the led is on from the begining of the 
//flashing sequence. eg 0x23 indicates on for the first 3 t0's cycles and first 
//2 t1's cycles

//( sequence runs left right)

//			  t0  t1  t0  t1  t0  t1  t0  t1  t0  t1 
//ie, 0x23    on  on  on  on  on  off off off off off 		(on for 50% - ie longish flash)
//ie  0x03    on  off on  off on  off off off off off       (3 quck flashes)


//flash codes ( & error clases)

//										    //<-- flash code
//                                        xxVV  	
#define GPSStatus_NoFixMask				0x0001	//1 flash
#define GPSStatus_CommsFailMask			0x0002	//2 flash
#define GPSStatus_HWFailMask			0x0003	//3 flash
#define GPSStatus_HWFatalMask			0x0055	//on

//values            status code->||<- LED #
//                               || 
//                               ||        //<-- flash code
//                               VVxx    xxVV  	
//        															//status codes	PC code
#define GPSStatus_GotGPS	  (0x0100| 0x0001)						//L1 : 1 flash		a
#define GPSStatus_Commit 	  (0x1100| 0x0055)						//L1 : full on		b

#define GPSStatus_NoFixOrMode (0x0200| GPSStatus_NoFixMask)			//L2 : 1 flash		A
#define GPSStatus_NoFix 	  (0x1200| GPSStatus_NoFixMask)			//L2 : 1 flash		B
#define GPSStatus_NoMode 	  (0x2200| GPSStatus_NoFixMask)			//L2 : 1 flash		C
#define GPSStatus_TimeSlip 	  (0x3200| GPSStatus_CommsFailMask)		//L2 : 2 flashes	D
#define GPSStatus_CommsOvflow (0x4200| GPSStatus_CommsFailMask)		//L2 : 2 flashes	E
#define GPSStatus_CommsCorrpt (0x5200| GPSStatus_CommsFailMask)		//L2 : 2 flashes	F
#define GPSStatus_CommsCRC    (0x6200| GPSStatus_CommsFailMask)		//L2 : 2 flashes	G
#define GPSStatus_FlashErr    (0x7200| GPSStatus_HWFailMask)		//L2 : 3 flashes	H
#define GPSStatus_LowBatt     (0x8200| 0x0005)						//L2 : 5 flashes	I
#define GPSStatus_NoFlash     (0x9200| GPSStatus_HWFatalMask)		//L2 : 3 flashes	J

#define GPSStatus_CommsOK	  0x0001	//status only

#define LEDMaskID    0x0F00
#define LEDStatus1   0x0100
#define LEDStatus2   0x0200
#define LEDFlashMask 0x00FF

#define M_ExtractStatusCode(x) (((x) & 0xF000) >> 12)		//get code number form status msg

#define bool  uint8_t
#define true  1
#define false 0


void GPSMsgStatus(uint16_t code);

extern uint8_t gReceiverWatchdog;
extern bool    gForceReset;


#ifdef DBGPRINT 
	#define DBGPutStr_P(s)   	  PCSIOPutStr_P(s)
	#define DBGPutStrNum_P(s,n)   PCSIOPutStrNum_P(s,n)
	#define DBGPutStr(s)     	  PCSIOPutStr(s)
	#define DBGPutc(a)		 	  PCSIOPutChar(a)
#else
	#define DBGPutStr_P(s)   
	#define DBGPutStr(s)    
	#define DBGPutc(a)		
#endif


#ifdef CPU8MHZ
#define CPUCLK   8000000L   // CPU xtal
#define PC UBRR_INIT ??		//PC uart 56K baud (to PC)

#endif

#ifdef CPU14MHZ				//14.745 CPU xtal
#define CPUCLK   14745600L  //CPU xtal
#define PC_UBRR_INIT 15		//PC uart 115.2K baud (to PC)  note:U2X=1
							
#endif

#ifdef CPU16MHZ
#define CPUCLK   16000000L   //CPU xtal
#define UPC_UBRR_INIT 16	 //PC uart 115.2K baud (to PC) note:U2X=1
#endif

#define TICKRATE 100 		 // AvrX timer queue Tick rate & preemtive task swapper
							 // NOTE: this effects the resolution of the AVRX timers.

// flash stuff
#ifdef DFPageSize528
#define FRW_FLASHPAGES    4096					// number of pages used in flash device
#else
#define FRW_FLASHPAGES    1000					// number of pages used in flash device
#endif
#define FRW_NUMGPS_PAGES  (FRW_FLASHPAGES-1)	//0..FRW_NUMGPS_PAGES-1

#define FRW_FIRSTGPS_PAGE 0
#define FRW_LASTGPS_PAGE  FRW_NUMGPS_PAGES

#define FRW_OUTBUF 1			//flash device buffer 1 for writing
#define FRW_INBUF  2			//flash device buffer 2 for reading

#define MStoTickRate(ms) ((uint16_t)((long)(ms)*TICKRATE/1000L))



#if !defined(BV)
#  define BV(A) (1<<A)
#endif

#  define outp(A,B) B = (A)
#  define inp(A) A
#  define cbi(P, B) P &= ~BV(B)
#  define sbi(P, B) P |= BV(B)

#endif //INCLUDE_ProjectCommon
