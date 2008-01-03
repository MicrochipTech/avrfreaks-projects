//*****************************************************************************
// File Name	: main.c
// 
// Title		: SoE AVR card Software
// Revision		: 1.0
// Notes		:	
// Target MCU	: AVR 32 series
// Editor Tabs	: 4
// 
// Revision History:
// When			Who			Description of change
// -----------	-----------	-----------------------
// 20-Sep-2007	HsK		Created the program
// 24-Oct-2007  HsK		New parameters added
// 30-Oct-2007  HsK		Freeze Code ver 1.0
// 11-dec-2007  HsK     Code ver 2.0
// 19-Dec-2007  HsK		Final Single COntroller Software
//*****************************************************************************


//----- Include Files ---------------------------------------------------------
#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include <avr/signal.h>		// include "signal" names (interrupt names)
#include <avr/interrupt.h>	// include interrupt support
#include <avr/eeprom.h>

#include "global.h"		// include global settings
#include "a2d.h"	
#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "timer.h"		// include timer function library

/*
Timer Specifications 256 
In timer.h prescale divider is	
At crystal of 11.0592 MHz

*/
//--------------------------


#define MAF1_LENGTH 8
unsigned char initMAF1(unsigned int dPoint);
unsigned int MAF1(unsigned int dPoint);
unsigned int MAF1_Buffer[MAF1_LENGTH],MAF1_cntr,MAF1_ptr,MAF1_output;
unsigned char MAF1_var;


#define MAF2_LENGTH 8
#define BUFF 40
unsigned char initMAF2(unsigned int dPoint);
unsigned int MAF2(unsigned int dPoint);
unsigned int MAF2_Buffer[MAF2_LENGTH],MAF2_cntr,MAF2_ptr,MAF2_output;
unsigned char MAF2_var;

#define MAF3_LENGTH 8
unsigned char initMAF3(unsigned int dPoint);
unsigned int MAF3(unsigned int dPoint);
unsigned int MAF3_Buffer[MAF3_LENGTH],MAF3_cntr,MAF3_ptr,MAF3_output;
unsigned char MAF3_var;

#define MAF4_LENGTH 8
unsigned char initMAF4(unsigned int dPoint);
unsigned int MAF4(unsigned int dPoint);
unsigned int MAF4_Buffer[MAF4_LENGTH],MAF4_cntr,MAF4_ptr,MAF4_output;
unsigned char MAF4_var;

//--------------------------


void initAI(void);
void updateAI(void);


#define ENQ 0x05
#define ETX 0x06

#define RX_PACKET_LENGTH 64
unsigned char rxFile[RX_PACKET_LENGTH];



struct PARAMETER_FILE
	{
	unsigned int aiThreshold[4];
	unsigned char AI_ChnStatus[4];
	
	unsigned char diThreshold[16];
	unsigned char DI_ChnStatus[16];
		
	unsigned int dBuffer_Threshold;
		
	unsigned int diFilter_Coff;
	unsigned int aiFilter_Coff;
	
	unsigned char sysAddr;
	//struct IPaddr IP1;
	};
struct PARAMETER_FILE pf;

struct TIME_STAMP
	{
	unsigned int msec;
	unsigned char sec;
	unsigned char min;
	unsigned char hour;
	unsigned char dow;
	unsigned char dom;
	unsigned char month;
	unsigned char year;
	};
struct TIME_STAMP ts,tsp;	



struct DATA_BUFFER_DI
	{
	unsigned char cnum;
	unsigned char diData;	//with channel info
	unsigned char yrStamp;
	unsigned char monthStamp;
	unsigned char dtStamp;
	unsigned char hrStamp;
	unsigned char minStamp;
	unsigned char secStamp;
	unsigned int msStamp;
	};
struct DATA_BUFFER_DI diDump[BUFF];
unsigned char diCntr,diStorage;

struct DATA_BUFFER_AI 
	{
	unsigned char cnum;
	unsigned int aiData;	//with channel info
	unsigned char yrStamp;
	unsigned char monthStamp;
	unsigned char dtStamp;
	unsigned char hrStamp;
	unsigned char minStamp;
	unsigned char secStamp;
	unsigned int msStamp;
	};
struct DATA_BUFFER_AI aiDump[BUFF];
unsigned char aiCntr,aiStorage;

// global variables
volatile u16 Int0Count,Tmr0OverFlowCount,Tmr1OverFlowCount;

unsigned char flag,packet_flag,rxByte,rxByteCounter,aflag,cFlag,timeFlag,aiFill,diFill;
unsigned char txByte=0;
unsigned int return_count;
unsigned char acq,waste;
unsigned char wst,scn;

void myOverflow0Function(void);
void myOverflow1Function(void);
void rx(unsigned char r);
unsigned char unpack(void);


void storePF(void);
void loadPF(void);
void load_default(void);
unsigned char *eep_byte;
unsigned int *eep_int;

void delayx(unsigned long d);

void initDI(void);
void updateDI(void);

void tx_packet_ACK_NAK(unsigned char tp);
void tx_packet_RTC(void);
void tx_packet_DI(unsigned char cnum,unsigned char scan);
void tx_packet_AI(unsigned char cnum);
void tx_packet_FLUSH(void);
void tx_packet_RETURN(unsigned int cd);

void time_calibration(void);

volatile u16 AI[4],prevAI[4],baseAI[4];
volatile u08 DI[16],prevDI[16];

int main(void)
{
unsigned char ma,mt,mc;
unsigned int i,lti;
unsigned int lck,uck;


MCUCSR=MCUCSR&0xf7;//clearing bit, if last was a watchdog reset

packet_flag=0;flag=1;
rxByteCounter=0;
acq=1;aiFill=0;diFill=0;
scn=0;

aflag=0;timeFlag=0;
cFlag=0;

diCntr=0;aiCntr=0;diStorage=1;aiStorage=1;


ts.year=7;ts.month=12;ts.dom=19;ts.hour=21;ts.min=5;ts.sec=16;

load_default();
//storePF();
//loadPF();

uartInit();uartSetBaudRate(9600);uartSetRxHandler(rx);
rprintfInit(uartSendByte);
	
timerInit();
timerAttach(TIMER0OVERFLOW_INT,myOverflow0Function);//timerDetach(TIMER0OVERFLOW_INT);
//timerAttach(TIMER1OVERFLOW_INT,myOverflow1Function);//timerDetach(TIMER0OVERFLOW_INT);

a2dInit();DDRA = 0x00;PORTA = 0x00;
a2dSetPrescaler(ADC_PRESCALE_DIV128);a2dSetReference(ADC_REFERENCE_256V);

timerPause(500);

initAI();
updateAI();updateAI();updateAI();updateAI();updateAI();updateAI();
updateAI();updateAI();updateAI();updateAI();updateAI();updateAI();
updateAI();updateAI();updateAI();updateAI();updateAI();updateAI();
updateAI();updateAI();updateAI();updateAI();updateAI();updateAI();
for(i=0;i<=3;i++)
		{
		prevAI[i]=AI[i];	
		baseAI[i]=AI[i];
		}	
		
		
initDI();
updateDI();
for(i=0;i<=15;i++)
		{prevDI[i]=DI[i];}
		
/*
while(1)
	{
	timerPause(800);		
	updateAI();
	rprintf("[0]:%d[1]:%d[2]:%d[3]:%d",AI[0],AI[1],AI[2],AI[3]);
	}
*/

//uartSendByte('S');uartSendByte('o');uartSendByte('E');uartSendByte('2');uartSendByte('5');uartSendByte('5');


/*	
while(1)
	{
	timerPause(1000);
	rprintf("StartYr:%d,Mn:%d,Dt:%d,Hr:%d,Min:%d,Sec:%dEnd",tsp.year,tsp.month,tsp.dom,tsp.hour,tsp.min,tsp.sec);		
	}
*/

rprintf("SoE255");  
	
while(1)
	{
	
	
	if(aflag==1)
		{tx_packet_AI(0);tx_packet_DI(0,0);aflag=0;}
		
	if(cFlag==1)
		{rprintf("SoE255");cFlag=0;}
			
		
	//------------------------------------------	
	updateDI();scn++;
	for(i=0;i<=15;i++)
		{
OTD:		
		
		if(DI[i]!=prevDI[i])
			{
			if(acq==1 && pf.DI_ChnStatus[i]==1)
				{
				
				mt=DI[i];
				timerPause(5);
				if(DI[i]!=mt){goto OTD;}
				
				if(diCntr>=39){diCntr=0;diStorage=0;}
				else{diCntr++;}
				diFill=1;
				//store di packet
				diDump[diCntr].cnum=i;
				diDump[diCntr].diData=DI[i];
				
				diDump[diCntr].yrStamp=tsp.year;
				diDump[diCntr].monthStamp=tsp.month;
				diDump[diCntr].dtStamp=tsp.dom;
				diDump[diCntr].hrStamp=tsp.hour;
				diDump[diCntr].minStamp=tsp.min;
				diDump[diCntr].secStamp=tsp.sec;
				diDump[diCntr].msStamp=tsp.msec;
				
				//rprintf("StORAGEYr:%d,Mn:%d,Dt:%d,Hr:%d,Min:%d,Sec:%dmsec:%dEnd",tsp.year,tsp.month,tsp.dom,tsp.hour,tsp.min,tsp.sec,diDump[diCntr]);	
				
				}
			}	
		prevDI[i]=DI[i];	
		}	
		

//------------------------------------------
	updateAI();
	
	for(i=0;i<=3;i++)
		{
		
		if(baseAI[i]>=pf.aiThreshold[i])
			{lck=baseAI[i]-pf.aiThreshold[i];}	
		else
			{lck=0;}
		if(baseAI[i]<=(4095-pf.aiThreshold[i]))
			{uck=baseAI[i]+pf.aiThreshold[i];}
		else
			{uck=4095;}
		
		if(AI[i]<lck || AI[i]>uck)
			{
			if(acq==1 && pf.AI_ChnStatus[i]==1)
				{
				
				
				if(aiCntr>=39){aiCntr=0;aiStorage=0;}
				else{aiCntr++;}
				aiFill=1;
				
				baseAI[i]=AI[i];
				//store ai packet
				
				aiDump[aiCntr].cnum=i;
				aiDump[aiCntr].aiData=AI[i];
				
				aiDump[aiCntr].yrStamp=tsp.year;
				aiDump[aiCntr].monthStamp=tsp.month;
				aiDump[aiCntr].dtStamp=tsp.dom;
				aiDump[aiCntr].hrStamp=tsp.hour;
				aiDump[aiCntr].minStamp=tsp.min;
				aiDump[aiCntr].secStamp=tsp.sec;
				aiDump[aiCntr].msStamp=tsp.msec;
				
				
				//rprintf("Store:%d",aiDump[aiCntr].aiData);
				}
			}	
			
		}
//------------------------------------------

		
	if(packet_flag==1)
		{
		ma=unpack();
		
		packet_flag=0;
		rxByteCounter=0;	//to init reception again	
		//uartSendByte(rxByte);
		}	
		
	}
return 0;
}




void delayx(unsigned long w)
{
unsigned long e=0;
while(w!=0)
	{e++;w--;}
}


//--------------------------------------



void initDI(void)
{
unsigned char la;

DDRB=0x00;PORTB=0xff;
DDRC=0x00;PORTC=0xff;

for(la=0;la<=15;la++)
	{DI[la]=0;}
	
}

void updateDI1(void)
{

if(bit_is_set(PINB,2)==1){DI[2]=1;}
else{DI[2]=0;}

if(bit_is_set(PINB,3)==1){DI[3]=1;}
else{DI[3]=0;}




if(bit_is_set(PINB,0)==1){DI[0]=1;}
else{DI[0]=0;}

if(bit_is_set(PINB,1)==1){DI[1]=1;}
else{DI[1]=0;}





if(bit_is_set(PINB,4)==1){DI[4]=1;}
else{DI[4]=0;}

if(bit_is_set(PINB,5)==1){DI[5]=1;}
else{DI[5]=0;}

if(bit_is_set(PINB,6)==1){DI[6]=1;}
else{DI[6]=0;}

if(bit_is_set(PINB,7)==1){DI[7]=1;}
else{DI[7]=0;}


if(bit_is_set(PINC,0)==1){DI[8]=1;}
else{DI[8]=0;}

if(bit_is_set(PINC,1)==1){DI[9]=1;}
else{DI[9]=0;}

if(bit_is_set(PINC,2)==1){DI[10]=1;}
else{DI[10]=0;}

if(bit_is_set(PINC,3)==1){DI[11]=1;}
else{DI[11]=0;}

if(bit_is_set(PINC,4)==1){DI[12]=1;}
else{DI[12]=0;}

if(bit_is_set(PINC,5)==1){DI[13]=1;}
else{DI[13]=0;}

if(bit_is_set(PINC,6)==1){DI[14]=1;}
else{DI[14]=0;}

if(bit_is_set(PINC,7)==1){DI[15]=1;}
else{DI[15]=0;}
}



void updateDI(void)
{
unsigned char lb,ld;
ld=PINB;lb=PINC;


if((lb&0x01)==0x01){DI[0]=1;}
else{DI[0]=0;}

lb=lb>>1;
if((lb&0x01)==0x01){DI[1]=1;}
else{DI[1]=0;}

lb=lb>>1;
if((lb&0x01)==0x01){DI[2]=1;}
else{DI[2]=0;}

lb=lb>>1;
if((lb&0x01)==0x01){DI[3]=1;}
else{DI[3]=0;}

lb=lb>>1;
if((lb&0x01)==0x01){DI[4]=1;}
else{DI[4]=0;}

lb=lb>>1;
if((lb&0x01)==0x01){DI[5]=1;}
else{DI[5]=0;}

lb=lb>>1;
if((lb&0x01)==0x01){DI[6]=1;}
else{DI[6]=0;}

lb=lb>>1;
if((lb&0x01)==0x01){DI[7]=1;}
else{DI[7]=0;}



if((ld&0x01)==0x01){DI[8]=1;}
else{DI[8]=0;}

ld=ld>>1;
if((ld&0x01)==0x01){DI[9]=1;}
else{DI[9]=0;}

ld=ld>>1;
if((ld&0x01)==0x01){DI[10]=1;}
else{DI[10]=0;}

ld=ld>>1;
if((ld&0x01)==0x01){DI[11]=1;}
else{DI[11]=0;}

ld=ld>>1;
if((ld&0x01)==0x01){DI[12]=1;}
else{DI[12]=0;}

ld=ld>>1;
if((ld&0x01)==0x01){DI[13]=1;}
else{DI[13]=0;}

ld=ld>>1;
if((ld&0x01)==0x01){DI[14]=1;}
else{DI[14]=0;}

ld=ld>>1;
if((ld&0x01)==0x01){DI[15]=1;}
else{DI[15]=0;}
}



void rx(unsigned char r)
{

if(rxByteCounter<RX_PACKET_LENGTH)
	{
	if(rxByteCounter==0 && r=='A')
		{aflag=1;goto GF;}
		
	if(rxByteCounter==0 && r=='C')
		{
		cFlag=1;goto GF;
		}
		
	if(rxByteCounter==0 && r=='X')
		{
		WDTCR|=(1<<WDE);while(1){}//watchdog on, waiting for reset
		}
	
	rxFile[rxByteCounter]=r;
	rxByteCounter++;
	}
else					//packet ready to be analysed
	{
	if(packet_flag==1){}
	else{packet_flag=1;}
	}

GF:
wst=0;
}


unsigned char unpack(void)
{
unsigned char la,lb,lByte,hByte;
unsigned int li,lj,lk;

la=0;

if(rxFile[la]!=ENQ){tx_packet_ACK_NAK(0);return(1);}	//error code - packet header mismatch
la++;	// increment for command type

switch(rxFile[la])	//checking command type
	{
	case 0x01:								//start acq command
		{
		if(rxFile[63]!=ETX){tx_packet_ACK_NAK(0);return(2);}	//Footer Mismatch
		else{acq=1;return(8);}//start acq command
		}	
	break;

	case 0x02:								// stop acq command
		{
		
		if(rxFile[63]!=ETX){tx_packet_ACK_NAK(0);return(2);}	//Footer Mismatch
		else{acq=0;return(9);}//stop acq command
		}	
	break;
	
	case 0x03:					//parameter file
		{			
		la++;					//increment for byte count
		lb=rxFile[la];la++;	//byte count
		
		hByte=rxFile[la];la++;	
		lByte=rxFile[la];la++;
		li=hByte;lj=lByte;
		lk=(li<<8)|lj;
		pf.aiThreshold[0]=lk;

		hByte=rxFile[la];la++;	
		lByte=rxFile[la];la++;
		li=hByte;lj=lByte;
		lk=(li<<8)|lj;
		pf.aiThreshold[1]=lk;

		hByte=rxFile[la];la++;	
		lByte=rxFile[la];la++;
		li=hByte;lj=lByte;
		lk=(li<<8)|lj;
		pf.aiThreshold[2]=lk;

		hByte=rxFile[la];la++;	
		lByte=rxFile[la];la++;
		li=hByte;lj=lByte;
		lk=(li<<8)|lj;
		pf.aiThreshold[3]=lk;


		lb=rxFile[la];la++;	
		pf.AI_ChnStatus[0]=lb;
		
		lb=rxFile[la];la++;	
		pf.AI_ChnStatus[1]=lb;
		
		lb=rxFile[la];la++;	
		pf.AI_ChnStatus[2]=lb;
		
		lb=rxFile[la];la++;	
		pf.AI_ChnStatus[3]=lb;
		
	
		lb=rxFile[la];la++;	
		pf.diThreshold[0]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[1]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[2]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[3]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[4]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[5]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[6]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[7]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[8]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[9]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[10]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[11]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[12]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[13]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[14]=lb;
		
		lb=rxFile[la];la++;	
		pf.diThreshold[15]=lb;
		

		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[0]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[1]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[2]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[3]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[4]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[5]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[6]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[7]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[8]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[9]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[10]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[11]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[12]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[13]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[14]=lb;
		
		lb=rxFile[la];la++;	
		pf.DI_ChnStatus[15]=lb;	
		
		
		hByte=rxFile[la];la++;	
		lByte=rxFile[la];la++;
		li=hByte;lj=lByte;
		lk=(li<<8)|lj;
		pf.dBuffer_Threshold=lk;
		//dBuffer_Threshold=1;
		
		
		hByte=rxFile[la];la++;	
		lByte=rxFile[la];la++;
		li=hByte;lj=lByte;
		lk=(li<<8)|lj;
		pf.diFilter_Coff=lk;
		//diFilter_Coff=1;
		
		hByte=rxFile[la];la++;	
		lByte=rxFile[la];la++;
		li=hByte;lj=lByte;
		lk=(li<<8)|lj;
		pf.aiFilter_Coff=lk;
		//aiFilter_Coff=1;
		
		lb=rxFile[la];la++;	
		pf.sysAddr=lb;
		
		
		if(rxFile[63]!=ETX){tx_packet_ACK_NAK(0);return(2);}	//Footer Mismatch
		else{tx_packet_ACK_NAK(1);return(10);}	//parameter file update
		}	
	break;


	case 0x04:
		{
		la++;//increment for data length byte
		lb=rxFile[la];la++;	//byte count
		
		hByte=rxFile[la];la++;	
		lByte=rxFile[la];la++;
		li=hByte;lj=lByte;
		lk=(li<<8)|lj;
		ts.msec=lk;
		
		lb=rxFile[la];la++;
		ts.sec=lb;
		
		lb=rxFile[la];la++;	
		ts.min=lb;
		
		lb=rxFile[la];la++;	
		ts.hour=lb;
				
		lb=rxFile[la];la++;	
		ts.dow=lb;
		
		lb=rxFile[la];la++;	
		ts.dom=lb;
		
		lb=rxFile[la];la++;	
		ts.month=lb;
		
		lb=rxFile[la];la++;	
		ts.year=lb;
		
		if(rxFile[63]!=ETX){tx_packet_ACK_NAK(0);return(2);}	//Footer Mismatch
		else{tx_packet_ACK_NAK(1);return(11);}	//rtc update
		
		}
	break;	
	
	
		case 0x07:			//return command
		{
		la++;//increment for data length byte
		lb=rxFile[la];la++;	//byte count
		
		hByte=rxFile[la];la++;	
		lByte=rxFile[la];la++;
		li=hByte;lj=lByte;
		lk=(li<<8)|lj;
		return_count=lk;
		
		if(rxFile[63]!=ETX){tx_packet_ACK_NAK(0);return(2);}	//Footer Mismatch
		else{tx_packet_RETURN(return_count);return(12);}	//return command
		
		}
	break;	

	
	
	}
	
return(0);//unknown command
}



void load_default(void)
{
pf.aiThreshold[0]=100;
pf.aiThreshold[1]=100;
pf.aiThreshold[2]=200;
pf.aiThreshold[3]=100;

pf.AI_ChnStatus[0]=0;
pf.AI_ChnStatus[1]=0;
pf.AI_ChnStatus[2]=1;
pf.AI_ChnStatus[3]=0;

pf.diThreshold[0]=1;
pf.diThreshold[1]=1;
pf.diThreshold[2]=1;
pf.diThreshold[3]=1;
pf.diThreshold[4]=1;
pf.diThreshold[5]=1;
pf.diThreshold[6]=1;
pf.diThreshold[7]=1;
pf.diThreshold[8]=1;
pf.diThreshold[9]=1;
pf.diThreshold[10]=1;
pf.diThreshold[11]=1;
pf.diThreshold[12]=1;
pf.diThreshold[13]=1;
pf.diThreshold[14]=1;
pf.diThreshold[15]=1;


pf.DI_ChnStatus[0]=1;
pf.DI_ChnStatus[1]=1;
pf.DI_ChnStatus[2]=1;
pf.DI_ChnStatus[3]=1;
pf.DI_ChnStatus[4]=1;
pf.DI_ChnStatus[5]=1;
pf.DI_ChnStatus[6]=1;
pf.DI_ChnStatus[7]=1;
pf.DI_ChnStatus[8]=1;
pf.DI_ChnStatus[9]=1;
pf.DI_ChnStatus[10]=1;
pf.DI_ChnStatus[11]=1;
pf.DI_ChnStatus[12]=1;
pf.DI_ChnStatus[13]=1;
pf.DI_ChnStatus[14]=1;
pf.DI_ChnStatus[15]=1;

pf.dBuffer_Threshold=1;

pf.diFilter_Coff=1;
pf.aiFilter_Coff=1;
pf.sysAddr=1;


}


void tx_packet_ACK_NAK(unsigned char tp)	//only 3 byte packet
{
uartSendByte(ENQ);
if(tp==1){uartSendByte(0x01);}	//for ack
else{uartSendByte(0x00);}		//for nak
uartSendByte(ETX);
}

void tx_packet_RTC(void)
{
unsigned char la;

uartSendByte(ENQ);
uartSendByte(0x02);		//for RTC update

uartSendByte(ts.year);
uartSendByte(ts.month);
uartSendByte(ts.dom);
uartSendByte(ts.dow);
uartSendByte(ts.hour);
uartSendByte(ts.min);
uartSendByte(ts.sec);

la=(unsigned char)(ts.msec&0x00ff);	//lower byte
uartSendByte(la);

la=(unsigned char)(ts.msec<<8);		//higher byte
uartSendByte(la);

uartSendByte(ETX);
}

/*
void tx_packet_DI(unsigned char cnum,unsigned char scan)
{

if(diStorage<diCntr)
	{
	
	
	uartSendByte(ENQ);
	uartSendByte(0x03);		//for DI activity
	
	uartSendByte(diDump[diStorage].cnum);	//for chn number
	uartSendByte(diDump[diStorage].diData);//for signal value

	uartSendByte(diDump[diStorage].yrStamp);
	uartSendByte(diDump[diStorage].monthStamp);
	uartSendByte(diDump[diStorage].dtStamp);
	uartSendByte(diDump[diStorage].hrStamp);
	uartSendByte(diDump[diStorage].minStamp);
	uartSendByte(diDump[diStorage].secStamp);
	uartSendByte((diDump[diStorage].msStamp)>>8);	//higher byte
	uartSendByte((diDump[diStorage].msStamp)&0x00ff);	//lower byte
	
	uartSendByte(ETX);
	
	
	
	diStorage++;
	}


}


void tx_packet_AI(unsigned char cnum)	
{
unsigned char la;

if(aiStorage<diCntr)
	{
	
	
	uartSendByte(ENQ);
	uartSendByte(0x04);		//for AI activity
	
	uartSendByte(aiDump[diStorage].cnum);	//for chn number
	uartSendByte((aiDump[diStorage].aiData)>>8);//for signal value High Byte
	uartSendByte((aiDump[diStorage].aiData)&0x00ff);//for signal value low Byte
	
	uartSendByte(aiDump[aiStorage].yrStamp);
	uartSendByte(aiDump[aiStorage].monthStamp);
	uartSendByte(aiDump[aiStorage].dtStamp);
	uartSendByte(aiDump[aiStorage].hrStamp);
	uartSendByte(aiDump[aiStorage].minStamp);
	uartSendByte(aiDump[aiStorage].secStamp);
	uartSendByte((aiDump[aiStorage].msStamp)>>8);	//higher byte
	uartSendByte((aiDump[aiStorage].msStamp)&0x00ff);	//lower byte
	
	uartSendByte(ETX);
	
	aiStorage++;
	}
}
*/


void tx_packet_FLUSH(void)	//only 3 byte packet
{
uartSendByte(ENQ);
uartSendByte(0x05);		//for FLUSH command

uartSendByte(ETX);
}

void tx_packet_RETURN(unsigned int cd)	//only 3 byte packet
{
//unsigned char la;

uartSendByte(ENQ);
uartSendByte(0x07);		//for RETURN command

//uartSendByte(pf.sysAddr);		//system address return
/*
la=(unsigned char)(cd&0x00ff);	//lower byte
uartSendByte(la);timerPause(5);

la=(unsigned char)(cd<<8);		//higher byte
uartSendByte(la);timerPause(5);
*/

uartSendByte(ETX);
}



void initAI(void)
{

MAF1_ptr=0;
while(initMAF1(a2dConvert10bit(0))!=1){}

MAF2_ptr=0;
while(initMAF2(a2dConvert10bit(1))!=1){}

MAF3_ptr=0;
while(initMAF3(a2dConvert10bit(2))!=1){}

MAF4_ptr=0;
while(initMAF4(a2dConvert10bit(3))!=1){}
}


void updateAI(void)
{
unsigned int li;

li=a2dConvert10bit(0);//if(li!=0){AI[0]=li;goto O1;}else{goto O1;}
if(li>0 && li<404){if(li>404){AI[0]=404;}else{AI[0]=MAF1(li);}}else{goto O1;}
if(AI[0]>=80)
	{
	AI[0]=(AI[0]-80)*126;
	AI[0]=AI[0]/10;
	}
else
	{
	AI[0]=0;
	}
O1:


li=a2dConvert10bit(1);//if(li!=0){AI[1]=li;goto O2;}else{goto O2;}
if(li>0 && li<404){if(li>404){AI[1]=404;}else{AI[1]=MAF2(li);}}else{goto O2;}
if(AI[1]>=80)
	{
	AI[1]=(AI[1]-80)*126;
	AI[1]=AI[1]/10;
	}
else
	{
	AI[1]=0;
	}
O2:


li=a2dConvert10bit(2);//if(li!=0){AI[2]=li;goto O3;}else{goto O3;}
if(li>0 && li<404){if(li>404){AI[2]=404;}else{AI[2]=MAF3(li);}}else{goto O3;}
if(AI[2]>=80)
	{
	AI[2]=(AI[2]-80)*126;
	AI[2]=AI[2]/10;
	}
else
	{
	AI[2]=0;
	}
O3:

li=a2dConvert10bit(3);//if(li!=0){AI[3]=li;goto O4;}else{goto O4;}
if(li>0 && li<404){if(li>404){AI[3]=404;}else{AI[3]=MAF4(li);}}else{goto O4;}
if(AI[3]>=80)
	{
	AI[3]=(AI[3]-80)*126;
	AI[3]=AI[3]/10;
	}
else
	{
	AI[3]=0;
	}
O4:

waste=0;
}




/*
void updateAI(void)
{
unsigned int li;

li=a2dConvert10bit(0);	AI[0]=li;goto O1;
if(li>0 && li<790){if(li>790){AI[0]=791;}else{AI[0]=MAF1(li);}}else{goto O1;}
if(AI[0]>=160)
	{
	AI[0]=(AI[0]-160)*64;
	AI[0]=AI[0]/10;
	}
else
	{
	AI[0]=0;
	}
O1:


li=a2dConvert10bit(1);	AI[1]=li;goto O2;
if(li>0 && li<770){if(li>770){AI[1]=770;}else{AI[1]=MAF2(li);}}else{goto O2;}
if(AI[1]>=152)
	{
	AI[1]=(AI[1]-152)*66;
	AI[1]=AI[1]/10;
	}
else
	{
	AI[1]=0;
	}
O2:


li=a2dConvert10bit(2);	AI[2]=li;goto O3;
if(li>0 && li<840){if(li>840){AI[2]=840;}else{AI[2]=MAF3(li);}}else{goto O3;}
if(AI[2]>=166)
	{
	AI[2]=(AI[2]-166)*60;
	AI[2]=AI[2]/10;
	}
else
	{
	AI[2]=0;
	}
O3:

li=a2dConvert10bit(3);	AI[3]=li;goto O4;
if(li>0 && li<840){if(li>840){AI[3]=840;}else{AI[3]=MAF4(li);}}else{goto O4;}
if(AI[3]>=166)
	{
	AI[3]=(AI[3]-166)*60;
	AI[3]=AI[3]/10;
	}
else
	{
	AI[3]=0;
	}
O4:

waste=0;
}
*/


/*
void updateAI(void)
{
unsigned int li;
//AI[0]=MAF1(a2dConvert10bit(4));

li=a2dConvert10bit(4);
//if(li>0 && li<790){AI[0]=li;}else{goto O1;}
if(li>0 && li<790){AI[0]=MAF1(li);}else{goto O1;}
if(AI[0]>=160)
	{
	AI[0]=(AI[0]-160)*65;
	AI[0]=AI[0]/10;
	}
else
	{
	AI[0]=0;
	}
O1:


AI[1]=MAF2(a2dConvert10bit(6));
if(AI[1]>=180)
	{
	AI[1]=(AI[1]-180)*57;
	AI[1]=AI[1]/10;
	}
else
	{
	AI[1]=0;
	}

AI[2]=MAF3(a2dConvert10bit(5));
if(AI[2]>=180)
	{
	AI[2]=(AI[2]-180)*57;
	AI[2]=AI[2]/10;
	}
else
	{
	AI[2]=0;
	}

AI[3]=MAF4(a2dConvert10bit(3));
if(AI[3]>=180)
	{
	AI[3]=(AI[3]-180)*57;
	AI[3]=AI[3]/10;
	}
else
	{
	AI[3]=0;
	}
}
*/

/*
--------------------------------MAF1-------------------------------------------------------------
Uses MAF1_LENGTH Macro,unsigned int MAF1_Buffer[LENGTH], MAF1_cntr,MAF1_var,MAF1_ptr,MAF1_output
To init check output for 1(marks success), till then loop in while() and feed new value to dPoint
Set MAF1_ptr to zero before executing initMAF1
*/
unsigned char initMAF1(unsigned int dPoint)  
{
if(MAF1_ptr<=(MAF1_LENGTH-1))
	{
	MAF1_Buffer[MAF1_ptr]=dPoint;
	MAF1_ptr++;
	return(0);
	}
else
	{
	return(1);
	}	
}


unsigned int MAF1(unsigned int dPoint)
{
	for(MAF1_cntr=0;MAF1_cntr<=(MAF1_LENGTH-1)-1;MAF1_cntr++)
		{MAF1_Buffer[MAF1_cntr]=MAF1_Buffer[MAF1_cntr+1];}
		
	MAF1_Buffer[MAF1_LENGTH-1]=dPoint;

	
	MAF1_output=0;
	for(MAF1_cntr=0;MAF1_cntr<=(MAF1_LENGTH-1);MAF1_cntr++)
		{MAF1_output=MAF1_output+MAF1_Buffer[MAF1_cntr];}
	MAF1_output=MAF1_output/MAF1_LENGTH;					
return(MAF1_output);
}



/*
--------------------------------MAF2-------------------------------------------------------------
Uses MAF2_LENGTH Macro,unsigned int MAF2_Buffer[MAF2_LENGTH], MAF2_cntr,MAF2_var,MAF2_ptr,MAF2_output
To init check output for 1(marks success), till then loop in while() and feed new value to dPoint
Set MAF2_ptr to zero before executing initMAF2
*/
unsigned char initMAF2(unsigned int dPoint)  
{
if(MAF2_ptr<=(MAF2_LENGTH-1))
	{
	MAF2_Buffer[MAF2_ptr]=dPoint;
	MAF2_ptr++;
	return(0);
	}
else
	{
	return(1);
	}	
}


unsigned int MAF2(unsigned int dPoint)
{
	for(MAF2_cntr=0;MAF2_cntr<=(MAF2_LENGTH-1)-1;MAF2_cntr++)
		{MAF2_Buffer[MAF2_cntr]=MAF2_Buffer[MAF2_cntr+1];}
		
	MAF2_Buffer[MAF2_LENGTH-1]=dPoint;

	
	MAF2_output=0;
	for(MAF2_cntr=0;MAF2_cntr<=(MAF2_LENGTH-1);MAF2_cntr++)
		{MAF2_output=MAF2_output+MAF2_Buffer[MAF2_cntr];}
	MAF2_output=MAF2_output/MAF2_LENGTH;					
return(MAF2_output);
}




/*
--------------------------------MAF3-------------------------------------------------------------
Uses MAF3_LENGTH Macro,unsigned int MAF3_Buffer[MAF3_LENGTH], MAF3_cntr,MAF3_var,MAF3_ptr,MAF3_output
To init check output for 1(marks success), till then loop in while() and feed new value to dPoint
Set MAF3_ptr to zero before executing initMAF3
*/
unsigned char initMAF3(unsigned int dPoint)  
{
if(MAF3_ptr<=(MAF3_LENGTH-1))
	{
	MAF3_Buffer[MAF3_ptr]=dPoint;
	MAF3_ptr++;
	return(0);
	}
else
	{
	return(1);
	}	
}


unsigned int MAF3(unsigned int dPoint)
{
	for(MAF3_cntr=0;MAF3_cntr<=(MAF3_LENGTH-1)-1;MAF3_cntr++)
		{MAF3_Buffer[MAF3_cntr]=MAF3_Buffer[MAF3_cntr+1];}
		
	MAF3_Buffer[MAF3_LENGTH-1]=dPoint;

	
	MAF3_output=0;
	for(MAF3_cntr=0;MAF3_cntr<=(MAF3_LENGTH-1);MAF3_cntr++)
		{MAF3_output=MAF3_output+MAF3_Buffer[MAF3_cntr];}
	MAF3_output=MAF3_output/MAF3_LENGTH;					
return(MAF3_output);
}




/*
--------------------------------MAF4-------------------------------------------------------------
Uses MAF4_LENGTH Macro,unsigned int MAF4_Buffer[MAF4_LENGTH], MAF4_cntr,MAF4_var,MAF4_ptr,MAF4_output
To init check output for 1(marks success), till then loop in while() and feed new value to dPoint
Set MAF4_ptr to zero before executing initMAF4
*/
unsigned char initMAF4(unsigned int dPoint)  
{
if(MAF4_ptr<=(MAF4_LENGTH-1))
	{
	MAF4_Buffer[MAF4_ptr]=dPoint;
	MAF4_ptr++;
	return(0);
	}
else
	{
	return(1);
	}	
}


unsigned int MAF4(unsigned int dPoint)
{
	for(MAF4_cntr=0;MAF4_cntr<=(MAF4_LENGTH-1)-1;MAF4_cntr++)
		{MAF4_Buffer[MAF4_cntr]=MAF4_Buffer[MAF4_cntr+1];}
		
	MAF4_Buffer[MAF4_LENGTH-1]=dPoint;

	
	MAF4_output=0;
	for(MAF4_cntr=0;MAF4_cntr<=(MAF4_LENGTH-1);MAF4_cntr++)
		{MAF4_output=MAF4_output+MAF4_Buffer[MAF4_cntr];}
	MAF4_output=MAF4_output/MAF4_LENGTH;					
return(MAF4_output);
}


//-----------------




/*
void storePF(void)
{
unsigned char la;

eeprom_write_word(0,pf.aiThreshold[0]);
eeprom_write_word(4,pf.aiThreshold[1]);
eeprom_write_word(8,pf.aiThreshold[2]);
eeprom_write_word(12,pf.aiThreshold[3]);

eeprom_write_byte(15,pf.AI_ChnStatus[0]);
eeprom_write_byte(16,pf.AI_ChnStatus[1]);
eeprom_write_byte(17,pf.AI_ChnStatus[2]);
eeprom_write_byte(18,pf.AI_ChnStatus[3]);

eeprom_write_byte(20,pf.diThreshold[0]);
eeprom_write_byte(21,pf.diThreshold[1]);
eeprom_write_byte(22,pf.diThreshold[2]);
eeprom_write_byte(23,pf.diThreshold[3]);
eeprom_write_byte(24,pf.diThreshold[4]);
eeprom_write_byte(25,pf.diThreshold[5]);
eeprom_write_byte(26,pf.diThreshold[6]);
eeprom_write_byte(27,pf.diThreshold[7]);
eeprom_write_byte(28,pf.diThreshold[8]);
eeprom_write_byte(29,pf.diThreshold[9]);
eeprom_write_byte(30,pf.diThreshold[10]);
eeprom_write_byte(31,pf.diThreshold[11]);
eeprom_write_byte(32,pf.diThreshold[12]);
eeprom_write_byte(33,pf.diThreshold[13]);
eeprom_write_byte(34,pf.diThreshold[14]);
eeprom_write_byte(35,pf.diThreshold[15]);


eeprom_write_byte(40,pf.DI_ChnStatus[0]);
eeprom_write_byte(41,pf.DI_ChnStatus[1]);
eeprom_write_byte(42,pf.DI_ChnStatus[2]);
eeprom_write_byte(43,pf.DI_ChnStatus[3]);
eeprom_write_byte(44,pf.DI_ChnStatus[4]);
eeprom_write_byte(45,pf.DI_ChnStatus[5]);
eeprom_write_byte(46,pf.DI_ChnStatus[6]);
eeprom_write_byte(47,pf.DI_ChnStatus[7]);
eeprom_write_byte(48,pf.DI_ChnStatus[8]);
eeprom_write_byte(49,pf.DI_ChnStatus[9]);
eeprom_write_byte(50,pf.DI_ChnStatus[10]);
eeprom_write_byte(51,pf.DI_ChnStatus[11]);
eeprom_write_byte(52,pf.DI_ChnStatus[12]);
eeprom_write_byte(53,pf.DI_ChnStatus[13]);
eeprom_write_byte(54,pf.DI_ChnStatus[14]);
eeprom_write_byte(55,pf.DI_ChnStatus[15]);

eeprom_write_word(60,pf.dBuffer_Threshold);

eeprom_write_byte(65,pf.diFilter_Coff);
eeprom_write_byte(66,pf.aiFilter_Coff);
eeprom_write_byte(70,pf.sysAddr);

}



void loadPF(void)
{
//eeprom_read_byte ();
//eeprom_read_word ();


pf.aiThreshold[0]=eeprom_read_word(0);
pf.aiThreshold[1]=eeprom_read_word(4);
pf.aiThreshold[2]=eeprom_read_word(8);
pf.aiThreshold[3]=eeprom_read_word(12);

pf.AI_ChnStatus[0]=eeprom_read_byte(15);
pf.AI_ChnStatus[1]=eeprom_read_byte(16);
pf.AI_ChnStatus[2]=eeprom_read_byte(17);
pf.AI_ChnStatus[3]=eeprom_read_byte(18);

pf.diThreshold[0]=eeprom_read_byte(20);
pf.diThreshold[1]=eeprom_read_byte(21);
pf.diThreshold[2]=eeprom_read_byte(22);
pf.diThreshold[3]=eeprom_read_byte(23);
pf.diThreshold[4]=eeprom_read_byte(24);
pf.diThreshold[5]=eeprom_read_byte(25);
pf.diThreshold[6]=eeprom_read_byte(26);
pf.diThreshold[7]=eeprom_read_byte(27);
pf.diThreshold[8]=eeprom_read_byte(28);
pf.diThreshold[9]=eeprom_read_byte(29);
pf.diThreshold[10]=eeprom_read_byte(30);
pf.diThreshold[11]=eeprom_read_byte(31);
pf.diThreshold[12]=eeprom_read_byte(32);
pf.diThreshold[13]=eeprom_read_byte(33);
pf.diThreshold[14]=eeprom_read_byte(34);
pf.diThreshold[15]=eeprom_read_byte(35);


pf.DI_ChnStatus[0]=eeprom_read_byte(40);
pf.DI_ChnStatus[1]=eeprom_read_byte(41);
pf.DI_ChnStatus[2]=eeprom_read_byte(42);
pf.DI_ChnStatus[3]=eeprom_read_byte(43);
pf.DI_ChnStatus[4]=eeprom_read_byte(44);
pf.DI_ChnStatus[5]=eeprom_read_byte(45);
pf.DI_ChnStatus[6]=eeprom_read_byte(46);
pf.DI_ChnStatus[7]=eeprom_read_byte(47);
pf.DI_ChnStatus[8]=eeprom_read_byte(48);
pf.DI_ChnStatus[9]=eeprom_read_byte(49);
pf.DI_ChnStatus[10]=eeprom_read_byte(50);
pf.DI_ChnStatus[11]=eeprom_read_byte(51);
pf.DI_ChnStatus[12]=eeprom_read_byte(52);
pf.DI_ChnStatus[13]=eeprom_read_byte(53);
pf.DI_ChnStatus[14]=eeprom_read_byte(54);
pf.DI_ChnStatus[15]=eeprom_read_byte(55);

pf.dBuffer_Threshold=eeprom_read_word(60);

pf.diFilter_Coff=eeprom_read_byte(65);
pf.aiFilter_Coff=eeprom_read_byte(66);
pf.sysAddr=eeprom_read_byte(70);



}

*/

/*//9872659666
				uartSendByte('B');timerPause(5);
				lti=AI[i];
				mc=lti>>8;//high
				uartSendByte(i);timerPause(5);
				uartSendByte(mc);timerPause(5);
				mt=lti&0x00ff;//low
				uartSendByte(mt);timerPause(5);
				*/
				
//rprintf("DI%d=%d__",i,DI[i]);	


void myOverflow0Function(void)
{
unsigned char lwd;

timer0ClearOverflowCount();

if(Tmr0OverFlowCount>=169)//41
	{
	//rprintf("_%d",tsp.msec);
	
		tsp.year=ts.year;
		tsp.month=ts.month;
		tsp.dom=ts.dom;
		tsp.hour=ts.hour;
		tsp.min=ts.min;
		tsp.sec=ts.sec;
		
		
	Tmr0OverFlowCount=0;
	timeFlag=1;
	ts.msec=0;tsp.msec=ts.msec;
	if(ts.sec>=59)
		{
		ts.sec=0;
		if(ts.min>=59)
			{
			ts.min=0;
			if(ts.hour>=23)
				{
				ts.hour=0;
				
				if(ts.month==2){lwd=29;}
				else
					{
					if(ts.month==1 ||ts.month==3 ||ts.month==5 ||ts.month==7 ||ts.month==8 ||ts.month==10||ts.month==12)
						{lwd=31;}
					else{lwd=30;}	
					}
					
					
				if(ts.dom>=lwd)
					{
					ts.dom=1;
					if(ts.month>=12)
						{
						ts.month=1;
						ts.year++;
						}
					else{ts.month++;}	
					}
				else{ts.dom++;}
				
				
				}
			else{ts.hour++;}	
			}
		else{ts.min++;}	
		}
	else{ts.sec++;}	
	
	}
else
	{
	Tmr0OverFlowCount++;	
	ts.msec=Tmr0OverFlowCount*6;//23
	tsp.msec=ts.msec;
	
	}
	
}





void tx_packet_DI(unsigned char cnum,unsigned char scan)
{
unsigned int li;
unsigned char la;

if(diStorage<=diCntr && diFill==1)
	{
	/*
	rprintf("Start Chn:%d,Val:%d",diDump[diStorage].cnum,diDump[diStorage].diData);
	rprintf("Yr:%d,Mn:%d,Dt:%d,Hr:%d,Min:%d,Sec:%dmsec:%d",diDump[diStorage].yrStamp,diDump[diStorage].monthStamp,diDump[diStorage].dtStamp,diDump[diStorage].hrStamp,diDump[diStorage].minStamp,diDump[diStorage].secStamp,diDump[diStorage].msStamp);	
	rprintf("End");
	*/
	
	
	uartSendByte(ENQ);
	uartSendByte(0x03);		//for DI activity
	
	uartSendByte(diDump[diStorage].cnum);	//for chn number
	uartSendByte(diDump[diStorage].diData);//for signal value

	uartSendByte(diDump[diStorage].yrStamp);
	uartSendByte(diDump[diStorage].monthStamp);
	uartSendByte(diDump[diStorage].dtStamp);
	uartSendByte(diDump[diStorage].hrStamp);
	uartSendByte(diDump[diStorage].minStamp);
	uartSendByte(diDump[diStorage].secStamp);
	
	li=diDump[diStorage].msStamp;li=li>>8;la=(unsigned char)li;uartSendByte(la);	//higher byte
	li=diDump[diStorage].msStamp;li=li&0x00ff;la=(unsigned char)li;uartSendByte(la);	//lower byte
	
	uartSendByte(ETX);
	
	diStorage++;
	}


}


void tx_packet_AI(unsigned char cnum)	
{
unsigned char la;
unsigned int li;

if(aiStorage<=aiCntr && aiFill==1)
	{
	//rprintf("Send:%d",aiDump[aiStorage].aiData);
	
	uartSendByte(ENQ);
	uartSendByte(0x04);		//for AI activity
	
	uartSendByte(aiDump[aiStorage].cnum);	//for chn number
	
	li=aiDump[aiStorage].aiData;li=li>>8;la=(unsigned char)li;uartSendByte(la);//higher byte
	li=aiDump[aiStorage].aiData;li=li&0x00ff;la=(unsigned char)li;uartSendByte(la);//lower byte
	
	
	uartSendByte(aiDump[aiStorage].yrStamp);
	uartSendByte(aiDump[aiStorage].monthStamp);
	uartSendByte(aiDump[aiStorage].dtStamp);
	uartSendByte(aiDump[aiStorage].hrStamp);
	uartSendByte(aiDump[aiStorage].minStamp);
	uartSendByte(aiDump[aiStorage].secStamp);
	
	li=aiDump[aiStorage].msStamp;li=li>>8;la=(unsigned char)li;uartSendByte(la);	//higher byte
	li=aiDump[aiStorage].msStamp;li=li&0x00ff;la=(unsigned char)li;uartSendByte(la);	//lower byte
		
	uartSendByte(ETX);
	
	aiStorage++;
	}
}



