
#include "C:\Users\Joachim Beyer\Documents\Atmel Studio\E-monitor\E-monitor\ds1820.h"
//********************************************************************
// ds1820.c
// Dallas 1-wire bus
// reads Temperature of all devices and stores bytes in temperature 
// array in orig 8bit format
// timing is determined by 2 wait routines; one for us and one for sec
// each device is separately addressed and read (polling)
//*********************************************************************

//------------------------------------------------------------------------------------------------------
// sensor IDs 64 bit
// sensors 0-19
const char ID[20][8] PROGMEM={{16,159,172,213,1,8,0,33},{16,126,140,213,1,8,0,199},{16,27,119,213,1,8,0,57},
{16,24,190,213,1,8,0,177},{16,7,238,237,1,8,0,15},{16,44,166,237,1,8,0,150},{16,195,175,213,1,8,0,60},
{16,125,185,213,1,8,0,200},{16,1,244,237,1,8,0,124},{16,142,148,213,1,8,0,247},{16,166,181,213,1,8,0,213},
{16,150,147,213,1,8,0,92},{16,217,195,237,1,8,0,204},{16,208,190,213,1,8,0,143},{16,92,190,213,1,8,0,24},
{16,182,158,213,1,8,0,6},{16,185,175,213,1,8,0,107},{16,203,140,213,1,8,0,43},{16,228,32,211,1,8,0,162},
{16,157,15,211,1,8,0,59}}; 

char store[maxdevices];  // 8bit orig format without 9th sign bit
//*******************************************************************

void write(char);
void start (void);
void read_deviceT(void);
char read_T (void);
bool resetL(void);
void match_rom(int);
bool convert_T(void);

//**************************************************************
// main read temperature routine for all devices on the bus
// returns number of devices read
// it contains the entire protocol
// and stores the entries in an array.
//**************************************************************
void read_deviceT(void)
{
    char reads,faults;     
    unsigned int a;         // device counter

    if (convert_T())
    {
        
        sprintf (display1,"\1  Reading Temp  ");
        sprintf (display2,"\2  %2u HRS %2u MIN   ",hour,min);   
		while(LCD());
		
        waitsec=2;
        while (waitsec!=0);
         
        for (a=0, faults=0; a< maxdevices;a++)
        {
            if (resetL())               //repeated reset for each device entries
            {
                match_rom(a);
                write(read_scratch);
                reads = read_T();
                if (reads==255) 
                {
                    faults++;
                    sprintf(display1,"\1  device %2d     ",a);
                    sprintf(display2,"\2     faulty!    ");  
					while(LCD());            

                    waitsec=2;
                    while (waitsec!=0); 
                    reads=0;
                }
				
				
                store[a]= reads;     // store in orig format 8 bit
            }
            else NBM=true;     
        }
		if (faults==maxdevices) singlemode=true;
		else singlemode= false;
    }
    else NBM=true;
    
}   
    

//****************************************************
// general write routine
// parameter: command (char)
// this issues commands to the devices
//****************************************************

void write(char c)  
{
    char count,y;					
    cli();
    for (count=8;count>0;count--)
    {
        start();				// outp mode low
        y=c;
        // mask byte and check bit0
        if ((y&= 1)==1)					// bit0 = 1?
        {
            DDRD &= ~(1<<7);			//inp mode
            _delay_us(58);				// transmit 1
        }
        else
        {
             _delay_us(54);			// transmit 0
            DDRD  &= ~(1<<7);		// release
			_delay_us(4);
        }
        //shift byte
        c>>=1;      // next bit
    }
	sei();
}
//***********************************************
// reset data line for all devices on bus
// called by read_deviceT
// RTC is maintained during this routine
//***********************************************

bool resetL(void)  //returns 1 if ACK
{
	bool ACK =false;
    start();					// outp mode low
    _delay_us(600);				// reset pulse
    DDRD &= ~(1<<7);			//release
     _delay_us(58);
    if (PIND & (1<<7)) ACK =false;
    else ACK = true;			// yes
	_delay_us(400);
	return ACK;
}

//---------------------------------------------------------
// addresses single device by command and 64 bit device ID
// parameter passed= device number
// called by read_deviceT
// is followed by read_T
//---------------------------------------------------------
 
void match_rom(int b)   // address device
{
    int t;
	int count;
    start();
    write(match); 
    for (count=0;count<8;count++)
    {
        t = pgm_read_byte_near(&ID[b][count]);   //transmit ID 64 bit
        write(t);
    }
}

//----------------------------------------------------
// general read temperature routine 8bit, single device
// after match Rom command
// called by read_deviceT
//----------------------------------------------------
char read_T (void) 
{
    char input;
    char count;
    
	cli();
    for (input=0,count=8;count>0;count--)   // 8 bits
    {
        start();					// set outp mode
        DDRD &= ~(1<<7);			//set high
        _delay_us(10);
        input>>=1;                  // shift a zero in  
        if (PIND & (1<<7)) input|= 0x80;  // make 1
        _delay_us(49);
    }
	sei();
    return input; 	
}

//**********************************************************************
void start (void)		// start bit 2us 
{
	DDRD |= (1<<7);		//set outp mode
	PORTD &= ~(1<<7);	//set low
	_delay_us(2);		// wait 2us
}
//**********************************************************************
// sends general convert Temperature command
// called by read_deviceT
// returns 1 if ACK received
//----------------------------------------------

bool convert_T(void)
{
    if (resetL())
    {
        write(skipR);
        write(convert);
        return true;
    }
    else 
    return false;
}

//******************************************************************
