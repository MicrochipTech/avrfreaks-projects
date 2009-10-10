/*

        Cashregister driver for dot matrix printers.
              
        Target: ATmega8 at 4MHz.
        Communication: I2C bus.

        Wrote it by Serasidis Vasilis at 11.02.2007
        Http://www.serasidis.gr
        info@serasidis.gr
        avrsite@yahoo.gr


                
*/



/*----------------------------------------------------------------
                         HEADER FILES 
-----------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "i2c.h"		// include i2c support

//------------- Defines --------------------
#define dot1        PD0
#define dot2        PD1
#define dot3        PD2
#define dot4        PD3
#define dot5        PD4
#define dot6        PD5
#define dot7        PD6
#define dot8        PD7
#define journal     PC0  // Ρελέ Χατροταινίας εφορίας.
#define reciept     PC1  // Ρελέ Χαρτοταινίας απόδειξης.
#define motorEnable PC2  // Μοτέρ κίνησης κεφαλής.
#define PotSwitch   PB0  // Διακόπτης αρχικής θέσης κεφαλής.
#define optoLetter  PB1  // Οπτοζεύκτης ολοκλήρωσης γράμματος ή αριθμού ή συμβόλου.
#define optoMove    PB2  // Οπτοζεύκτης ένδειξης ελάχιστης μετακίνησης κεφαλής.
#define LOCAL_ADDR	0x06 // Η διεύθυνση αυτού του I2C μικροελεγκτή σε λειτουργία Slave (printer)
#define TARGET_ADDR	0x01 // Η διεύθυνση του κεντρικού μικροελεγκτή (master)


const char num0[] PROGMEM = {0b00111110,0b01010001,0b01001001,0b01000101,0b00111110,255}; // (0)
const char num1[] PROGMEM = {0b00000000,0b01000010,0b01111111,0b01000000,0b00000000,255}; // (1)
const char num2[] PROGMEM = {0b01110010,0b01001001,0b01001001,0b01001001,0b01000110,255}; // (2)
const char num3[] PROGMEM = {0b00100010,0b01000001,0b01001001,0b01001001,0b00110110,255}; // (3)
const char num4[] PROGMEM = {0b00011000,0b00010100,0b00010010,0b01111111,0b00010000,255}; // (4)
const char num5[] PROGMEM = {0b00100111,0b01000101,0b01000101,0b01000101,0b00111001,255}; // (5)	
const char num6[] PROGMEM = {0b00111100,0b01001010,0b01001001,0b01001001,0b00110000,255}; // (6)	
const char num7[] PROGMEM = {0b00000001,0b01110001,0b00001001,0b00000101,0b00000011,255}; // (7)
const char num8[] PROGMEM = {0b00110110,0b01001001,0b01001001,0b01001001,0b00110110,255}; // (8)
const char num9[] PROGMEM = {0b00000110,0b01001001,0b01001001,0b00101001,0b00011110,255}; // (9)

const char letterS[] PROGMEM = {0b01000001,0b01100011,0b01010101,0b01001001,0b01000001,255}; // (9)
const char letterY[] PROGMEM = {0b00000111,0b00001000,0b01110000,0b00001000,0b00000111,255}; // (9)
const char letterN[] PROGMEM = {0b01111111,0b00000010,0b00000100,0b00001000,0b01111111,255}; // (9)
const char letterO[] PROGMEM = {0b00111110,0b01000001,0b01000001,0b01000001,0b00111110,255}; // (9)
const char letterL[] PROGMEM = {0b01111100,0b00000010,0b00000001,0b00000010,0b01111100,255}; // (9)
const char letterM[] PROGMEM = {0b01111111,0b00000010,0b00001100,0b00000010,0b01111111,255}; // (9)
const char letterE[] PROGMEM = {0b01111111,0b01001001,0b01001001,0b01000001,0b01000001,255}; // (9)
const char letterT[] PROGMEM = {0b00000001,0b00000001,0b01111111,0b00000001,0b00000001,255}; // (9)
const char letterR[] PROGMEM = {0b01111111,0b00001001,0b00001001,0b00001001,0b00000110,255}; // (9)
const char letterH[] PROGMEM = {0b01111111,0b00001000,0b00001000,0b00001000,0b01111111,255}; // (9)
const char letterA[] PROGMEM = {0b01111100,0b00010010,0b00010001,0b00010010,0b01111100,255}; // (9)
const char letterK[] PROGMEM = {0b01111111,0b00001000,0b00010100,0b00100010,0b01000001,255}; // (9)
const char letterP[] PROGMEM = {0b01111111,0b00000001,0b00000001,0b00000001,0b01111111,255}; // (9)
const char letterD[] PROGMEM = {0b01111100,0b01000010,0b01000001,0b01000010,0b01111100,255}; // (9)
const char letterX[] PROGMEM = {0b01100011,0b00010100,0b00001000,0b00010100,0b01100011,255}; // (9)
const char letterI[] PROGMEM = {0b00000000,0b01000001,0b01111111,0b01000001,0b00000000,255}; // (9)
const char letterSLA[] PROGMEM = {0b01100000,0b00010000,0b00001000,0b00000100,0b00000011,255}; // (/)
const char letterSUM[] PROGMEM = {0b00010100,0b00010100,0b00010100,0b00010100,0b00010100,255}; // (=)
const char letterPER[] PROGMEM = {0b01100011,0b00010011,0b00001000,0b01100100,0b01100011,255}; // (%)
const char letterDIV[] PROGMEM = {0b00000000,0b00100100,0b00100100,0b00000000,0b00000000,255}; // (:)
const char letterSUB[] PROGMEM = {0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,255}; // (-)
const char letterDOT[] PROGMEM = {0b00000000,0b00000000,0b01010000,0b00110000,0b00000000,255}; // (,)
const char letterSPA[] PROGMEM = {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,255}; // ( )


// 
unsigned int bigBufferLength = 0;
unsigned char localBuffer[50];
unsigned char localBufferLength;
unsigned char bigBuffer[] =   "================"
                                 "  EKPAIDEYTIKH  ";
/*                                 " TAMIAKH MHXANH "
                                 "================"
                                 "                "
                                 "       10,25 19%"
                                 "       12,24 19%"
                                 "SYNOLO ---------"
                                 "       22,49    "
                                 "                "
                                 "METRHTA --------"
                                 "       50,00    "
                                 "                "
                                 "RESTA ----------"
                                 "       27,51    "
                                 "                "
                                 " 11/02/07 18:20 "
                                 "*";
*/


//=================================================
//                  FUNCTIONS 
//=================================================
void delay_ms(unsigned long count);
void UART_PrintfProgStr_P(const char *data);
void changeLine(void);
void buffer2printer(void);
void i2cSlaveReceiveService(unsigned char receiveDataLength, unsigned char *receiveData);
unsigned char i2cSlaveTransmitService(unsigned char transmitDataLengthMax, unsigned char *transmitData);
void findStart(void);
//void i2cMasterSendDiag(u08 deviceAddr, u08 length, u08= data);


int main(void)
{
  
    DDRB = 0b00000000;          // PortB as input
    DDRC = 0b00000111;          // PC0,1,2 as output
    DDRD = 0b11111111;          // Port D as output

    PORTB = 0b00000000;
    PORTC = 0b00000000;
    PORTD = 0b00000000;
    
    unsigned char i = 0;
    unsigned char r = 0;

    //u08 c=0;
	// initialize i2c function library
	i2cInit();
	// set local device address and allow response to general call
	i2cSetLocalDeviceAddr(LOCAL_ADDR, TRUE);

	// set the Slave Receive Handler function
	// (this function will run whenever a master somewhere else on the bus
	//  writes data to us as a slave)
	i2cSetSlaveReceiveHandler( i2cSlaveReceiveService );

	// set the Slave Transmit Handler function
	// (this function will run whenever a master somewhere else on the bus
	//  attempts to read data from us as a slave)
    //	i2cSetSlaveTransmitHandler( i2cSlaveTransmitService );

/*
  for(;;)
    {
        PORTC |= (1<<motorEnable); // Ενεργοποίησε το μοτέρ.

        changeLine();
        buffer2printer();

        PORTC &= ~(1<<motorEnable); // Απενεργοποίησε το μοτέρ.

        for(;;) //Μείνε εδώ για πάντα.
        {
        delay_ms(1000);
        }

    }
*/    
}
//=================================================
//
//=================================================
void buffer2printer(void)
{
    unsigned char a = 0;
    unsigned int i = 0;
    unsigned char c = 0;

    for(i=0; i<bigBufferLength; i++)
    {
        a = bigBuffer[i];
        switch(a)
        {
            case '0':
            UART_PrintfProgStr_P(num0);
            break;

            case '1':
            UART_PrintfProgStr_P(num1);
            break;

            case '2':
            UART_PrintfProgStr_P(num2);
            break;

            case '3':
            UART_PrintfProgStr_P(num3);
            break;

            case '4':
            UART_PrintfProgStr_P(num4);
            break;

            case '5':
            UART_PrintfProgStr_P(num5);
            break;

            case '6':
            UART_PrintfProgStr_P(num6);
            break;

            case '7':
            UART_PrintfProgStr_P(num7);
            break;

            case '8':
            UART_PrintfProgStr_P(num8);
            break;

            case '9':
            UART_PrintfProgStr_P(num9);
            break;

            case 'S':
            UART_PrintfProgStr_P(letterS);
            break;

            case 'P':
            UART_PrintfProgStr_P(letterP);
            break;

            case 'Y':
            UART_PrintfProgStr_P(letterY);
            break;

            case 'N':
            UART_PrintfProgStr_P(letterN);
            break;

            case 'O':
            UART_PrintfProgStr_P(letterO);
            break;

            case 'L':
            UART_PrintfProgStr_P(letterL);
            break;

            case 'M':
            UART_PrintfProgStr_P(letterM);
            break;

            case 'E':
            UART_PrintfProgStr_P(letterE);
            break;

            case 'T':
            UART_PrintfProgStr_P(letterT);
            break;

            case 'R':
            UART_PrintfProgStr_P(letterR);
            break;

            case 'H':
            UART_PrintfProgStr_P(letterH);
            break;

            case 'A':
            UART_PrintfProgStr_P(letterA);
            break;

            case 'K':
            UART_PrintfProgStr_P(letterK);
            break;

            case 'D':
            UART_PrintfProgStr_P(letterD);
            break;

            case 'X':
            UART_PrintfProgStr_P(letterX);
            break;

            case 'I':
            UART_PrintfProgStr_P(letterI);
            break;

            case '/':
            UART_PrintfProgStr_P(letterSLA);
            break;

            case '=':
            UART_PrintfProgStr_P(letterSUM);
            break;

            case '%':
            UART_PrintfProgStr_P(letterPER);
            break;

            case ':':
            UART_PrintfProgStr_P(letterDIV);
            break;

            case '-':
            UART_PrintfProgStr_P(letterSUB);
            break;

            case ',':
            UART_PrintfProgStr_P(letterDOT);
            break;

            case ' ':
            UART_PrintfProgStr_P(letterSPA);
            break;
        }


        if(c == 15)
        {
            changeLine();
            c=0;
        }
        else
            c++;          
    }

    changeLine();
}

//=================================================
//
//=================================================
void changeLine(void)
{
        unsigned char steps = 0;

        PORTC |= (1<<reciept);  // 
        delay_ms(5000);
        PORTC &= ~(1<<reciept); // 

        while (!(~PINB & (1<<PotSwitch)));
        delay_ms(1000);
        for (steps=0; steps<5; steps++)
        {
            while (!(~PINB & (1<<optoLetter)));
            while ((~PINB & (1<<optoLetter)));
        }
}

//=================================================
//
//=================================================
void findStart(void)
{
        unsigned char steps = 0;

        while (!(~PINB & (1<<PotSwitch)));
        delay_ms(1000);
        for (steps=0; steps<5; steps++)
        {
            while (!(~PINB & (1<<optoLetter)));
            while ((~PINB & (1<<optoLetter)));
        }
}
//=================================================
//
//=================================================
void UART_PrintfProgStr_P(const char *data)
{

    while (pgm_read_byte(data) != 255)
    {
            if ((~PINB & (1<<optoMove)))
            {
                PORTD = pgm_read_byte(data++);
                delay_ms(1500); // Με αυτόν τον χρόνο τα γράμματα γίνονται πιο πυκνά ή αραιά (1500 @ 12V ή 700 @ 24V).
                PORTD = 0x00;
            }
            while ((~PINB & (1<<optoMove)));
            while (!(~PINB & (1<<optoMove)));
    }   
            while ((~PINB & (1<<optoMove)));        // Αφησε 2 κενές κολόνες μεταξυ των γραμματων.
            while (!(~PINB & (1<<optoMove))); 
            while ((~PINB & (1<<optoMove)));
            while (!(~PINB & (1<<optoMove)));      
}


//=================================================
//              slave operations
//=================================================

void i2cSlaveReceiveService(unsigned char receiveDataLength, unsigned char *receiveData)
{

	unsigned char i;
    
	// this function will run when a master somewhere else on the bus
	// addresses us and wishes to write data to us

	// showByte(=receiveData);
	// cbi(PORTB, PB6);

	// copy the received data to a local buffer
	for(i=0; i<receiveDataLength; i++)
	{
		localBuffer[i] = *receiveData++;
	}

    for (i=0; i<receiveDataLength; i++)
    {
        bigBuffer[i] = localBuffer[i];      
    }

	    bigBufferLength = receiveDataLength;
        PORTC |= (1<<motorEnable); // Ενεργοποίησε το μοτέρ.

        findStart();
        buffer2printer();

        PORTC &= ~(1<<motorEnable); // Απενεργοποίησε το μοτέρ.


}

//=================================================
//
//=================================================
/*
unsigned char i2cSlaveTransmitService(unsigned char transmitDataLengthMax, unsigned char *transmitData)
{
	u08 i;

	// this function will run when a master somewhere else on the bus
	// addresses us and wishes to read data from us

	// showByte(*transmitData);
	// cbi(PORTB, PB7);

	// copy the local buffer to the transmit buffer
	for(i=0; i<localBufferLength; i++)
	{
		*transmitData++ = localBuffer[i];
	}

	localBuffer[0]++;

	return localBufferLength;
}
*/


//=================================================
//    FUNCTION TO GENERATE DELAY IN MILLISECONDS
//=================================================

void delay_ms(unsigned long count) // Καθηστέρηση 2.5 ms
{
    while(count--);
}

