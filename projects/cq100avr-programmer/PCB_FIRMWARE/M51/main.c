
#include <AT89X51.H>

#undef	MONITOR51				  /* not using monitor debuger                */

#define		MOSI		P0_4
#define		MISO		P0_5
#define		SCK			P0_6
#define		RESET		P0_7

#define		LED_RUN		P2_0
#define		LED_PROG	P2_1

#define		HIGH		0xFF
#define		LOW			0x00

#define		FRM_START	0x55
#define		FRM_STOP	0xAA

#define		OK			1
#define		ERROR		0

#define		CBR_57600	0x03		//	FF
#define		CBR_28800	0x02		// 	FE
#define		CBR_19200	0x01		//  FC
#define		CBR_9600	0x00		//  F9

#define		BYTE		unsigned char

// global variables
static BYTE INSTRUCTION[4] = {0};
static BYTE SIGNATURE[4] = {'$','c','q','$'};
static BYTE AUTOBAUDRATE[4] = {'$','$','$','$'};
// functions prototype
void	AutoBaudrate();
void	ResetDevice();
BYTE	IsResetDevice();
// communication functions
void	InitComm(BYTE baud);
BYTE 	Receive();
BYTE 	Transmit(BYTE byte);

BYTE	GetInstruction();
void	SetInstruction();

// line controlling interface

BYTE	SPI_Byte(BYTE val);

////////////////////////////////////////////////////////////////////////////////

void main()
{

	InitComm(CBR_19200);
	ResetDevice();
	
	Transmit('C');
	Transmit('Q');
	Transmit('1');
	Transmit('0');
	Transmit('0');
	Transmit('\r');
   	Transmit('\n');

  	while (1) 
  	{
		if (GetInstruction()==OK)
		{
			if(IsResetDevice()==OK)	ResetDevice();
			else	SetInstruction();
		}
  	}

}

/***********************************************************************************/

void InitComm(BYTE baud)
{
	if (baud == CBR_57600)	
	{ TH1   = 0xFF;   PCON  = 0x80;}
	if (baud == CBR_28800)
	{ TH1   = 0xFD;   PCON  = 0x80;}
	if (baud == CBR_19200)
	{ TH1   = 0xFD;   PCON  = 0x80;}
	if (baud == CBR_9600)
	{ TH1   = 0xFD;   PCON  = 0x00;}

	SCON  = 0x50;		        /* SCON: mode 1, 8-bit UART, enable rcvr      */
    TMOD  = 0x20;               /* TMOD: timer 1, mode 1, 3 auto-reload       */
	TCON  = 0x41;				/* run counter 1 and set edge trig ints		  */		

	TL1   = 0xFF;				/* TL1:  current value for 57600 baud  11MHz  */
    TR1   = 1;                  /* TR1:  timer 1 run                          */
    TI    = 1;                  /* TI:   set TI to send first char of UART    */
	EA	  = 0;					/* Disable all interrupts                     */
}

void AutoBaudrate()
{
	BYTE rate = CBR_57600 + 1;			/* ************************/
	unsigned int i = 0;					/* index counter          */
	while (1)							/**************************/
	{
		BYTE recv = 0;LED_PROG = 1;
		// reduce baudrate
		rate = rate - 1;
		InitComm(rate);
		if (rate == CBR_9600) 
			rate = CBR_57600 + 1;
		for (i=0;i<20000;i++);
		// send sync frame
		Transmit(FRM_START);
		for (i=0 ;i<4; i++)
			Transmit(AUTOBAUDRATE[i]);
		Transmit(FRM_STOP);
		for (i=0;i<20000;i++);
		LED_PROG = 0; recv = 0;
 		// get sync charactor
		for (i=0;i<50000;i++)
		{
	  		if (!RI){recv = SBUF; RI = 0;}
			if (recv=='#') break;
		}
		if (i < 50000)	break;
	}
	// baudrate established

}

void ResetDevice()
{
	BYTE i= 0;
	long l = 0;	
	LED_PROG = 0;
	RESET = 0;MOSI  = 0;
	SCK   = 0;MISO  = 1;	

	SCK   = 0;for(i = 0;i<255;i++);
	SCK   = 0;for(i = 0;i<255;i++);

	RESET = 1;for(l = 0; l < 50000; l++);
	RESET = 0;for(l = 0; l < 10000; l++);

	Transmit(FRM_START);
	for (i=0; i<4; i++)		
		Transmit(SIGNATURE[i]);
		
	Transmit(FRM_STOP);

	LED_RUN = 1;LED_PROG = 1;
}

BYTE GetInstruction()
{
	BYTE i = 0;LED_PROG = 1;

	while (Receive()!=FRM_START);
	for (i=0;i<4;i++)
		INSTRUCTION[i] = Receive();
	if (Receive()==FRM_STOP)	return OK;
	return ERROR;	// return with error
}
void SetInstruction()
{
	BYTE i = 0;LED_PROG = 0;

	Transmit(FRM_START);
	for (i=0; i<4; i++)	
		Transmit(SPI_Byte(INSTRUCTION[i]));	
	Transmit(FRM_STOP);
}

BYTE IsResetDevice()
{
	BYTE i = 0;
	if (INSTRUCTION[0] == '?' &&
		INSTRUCTION[1] == '?' &&
		INSTRUCTION[2] == '?' &&
		INSTRUCTION[3] == '?' )
	{		
		return OK;
	}
	return ERROR;
}

/*********************************************************************************
/* SPI functions
/*********************************************************************************/

BYTE SPI_Byte(BYTE val)
{
	BYTE	bit_mask = 0x00;
	BYTE	i=0,res  = 0x00;

	for (bit_mask = 128; bit_mask >0; bit_mask = (bit_mask>>1))
	{		
		if (MISO)	res |= bit_mask;

		MOSI = (val&bit_mask)?1:0;
		for(i = 0;i<10;i++);		
		SCK  = 1;for(i = 0;i<10;i++);
		SCK  = 0;for(i = 0;i<10;i++);
	}
	return (BYTE)res;
}

/***********************************************************************************/

BYTE Transmit(BYTE byte)  
{	
  	while (!TI);
  	TI = 0;
  	return (SBUF = byte);
}

BYTE Receive()  
{
  	BYTE byte;
  	while (!RI);
  	byte = SBUF;
  	RI = 0;
  	return (byte);
}



