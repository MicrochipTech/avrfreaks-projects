
#include "C:\Users\Joachim Beyer\Documents\Atmel Studio\E-monitor\E-monitor\readID.h"

// File: readID.c
// Bus with multiple devices is to be disconnected!!!!
// reads ID and displays ID of a single device inserted in front socket
// activated after reset if any PB is pressed
// waits for 20 sec to display ID and continues with monitor program
//------------------------------------------------------------------

//-----------------------------------------------------
char IDX[8];       // storage of ID new device
//-------------------------------------------------------
char read_rom (void);
void ds_read_id(void);
void readSdeviceT(void);
void match_rom1(void);
void read_RdeviceT(void);


//----------------------------------------------------

//------------------------------------------------------
// reads ID of a single only device on bus
//------------------------------------------------------

void ds_read_id(void)
{
	unsigned int t;
	unsigned int count;
	
	t=sprintf( display1,"\1  Reading ID    ");
	//sprintf( display2,"\2 single device  ");
	
	sprintf( display2,"\2  %d         ",t);
	while(LCD());
	
	waitsec=2;
	while (waitsec!=0);
	
	if (resetL())
	{
		write (readRom);
		
		t=read_rom();                       //read ID
		sprintf( display1,"\1 Read bytes  %d  ",t);     //shows failures
		sprintf( display2,"\2                ");     //shows failures
		while(LCD());
		
		waitsec=2;
		while (waitsec!=0);
			
			for (count=0;count<8;count++)
			{
				sprintf(display1,"\1 %u        D %u ", IDX[count],count);
				count++;
				sprintf(display2,"\2 %u        D %u ", IDX[count],count);
				while(LCD());
				waitsec=4;
				while (waitsec!=0);
			}	
		read_RdeviceT();		
	}
	else
	{
		sprintf ( display1,"\1 resetL  failure ");
		sprintf ( display2,"\2                 ");
		while(LCD());
	}	
	waitsec=3;
	while (waitsec!=0);
	
}


//--------------------------------------------------
// reads ROM ID
// returns number of bytes read (8)
// called by ds_read_id();
//--------------------------------------------------

char read_rom (void)
{
	unsigned int count;
	char input;     // read input
	unsigned int bnum;      // byte number
	
	for (bnum =0;bnum<8;bnum++)					// 8 bytes ID
	{
		for (input=0,count=8;count>0;count--)	//8 bits
		{
			start();
			DDRD &= ~(1<<7);					// set high
			_delay_us(10);	;
			input>>=1;							// shift zero in
			if (PIND & (1<<7)) input|= 0x80;	// input 1
			_delay_us(49);	
		}
		IDX[bnum]=input;						//store byte
		if (input==255)
		{
			sprintf(display1,"\1  read fault    ");
			sprintf(display2,"\2  byte %d      ",bnum);
			while(LCD());
			waitsec=2;
			while (waitsec!=0);
		}
	}
	resetL();
	return bnum;
}

//****************************************************************
// reads temperature of a single device without ROM addressing
// called by main on singlemode 
//*******************************************************************

void readSdeviceT(void)
{
	char reads,dP;
	unsigned int a, t;
	
	if(convert_T())
	{	
		sprintf (display1,"\1  Single Mode   ");
		sprintf (display2,"\2   Reading     ");
		while(LCD());	
		waitsec=2;
		while (waitsec!=0);
		
			resetL();
			write(skipR);	
			write(read_scratch);
			reads = read_T();
			resetL();		
			if (reads==255)
			{
				
				sprintf(display1,"\1     device     ");
				sprintf(display2,"\2     faulty!    ");
				while(LCD());

				waitsec=2;
				while (waitsec!=0);
				reads = 0;
			}
		
			else
			{
				store[0]= reads;
				t=reads;
				for(a=1;a<maxdevices;a++)	// fill up all device temp
				{
					t+=2;					//add temperature increase 1°
					store[a]= t;
				}
			}	
		
			if (reads>0)
			{
				sprintf(display1,"\1  Device Temp   ");

				if (reads&1) dP = 5;
				else dP=0;
				reads>>=1;
				
				sprintf(display2,"\2     %d.%1d C     ",reads,dP);
				while(LCD());
				waitsec=2;
				while (waitsec!=0);
			}
			
	}		
	else
	{
		sprintf (display1,"\1  Device fault ");
		sprintf (display2,"\2    no ACK     ");
		while(LCD());
		singlemode=false;
	}
}	

//************************************************************
// reads temperature with ROM addressing
// called by ds_read_id(void)
//************************************************************ 
void read_RdeviceT(void)
{
	char reads,dP;

	if (convert_T())
	{	
		sprintf (display1,"\1  Reading Temp  ");
		while(LCD());
		sprintf (display2,"\2 addressing Dev ");
		while(LCD());
		
		waitsec=2;
		while (waitsec!=0);
		
		if (resetL())              
		{
			match_rom1();
			write(read_scratch);
			reads = read_T();
			if (reads==255)
			{
				sprintf(display1,"\1      device    ");
				sprintf(display2,"\2     faulty!    ");
				while(LCD());

				waitsec=2;
				while (waitsec!=0);
				reads=0;
			}
			else
			{
				sprintf(display1,"\1  Device Temp   ");

				if (reads&1) dP = 5;
				else dP=0;
				reads>>=1;
				
				sprintf(display2,"\2     %d.%1d C     ",reads,dP);
				while(LCD());
			}
		}		
	}					
	else
	{
		sprintf(display1,"\1     No         ");
		sprintf(display2,"\2     ACK        ");
		while(LCD());
	}
	
}
			

void match_rom1(void)   // address device
{
	char t;
	unsigned int count;
	
	cli();
	start();
	write(match);
	for (count=0;count<8;count++)
	{
		t = IDX[count];   //transmit ID 64 bit
		write(t);
	}
	sei();
}
