
//*****************************************************
// File Name  : cashRegisterPrinter.c
// Title      : Εκπαιδευτική ταμιακή μηχανή (Printer)
// Target MCU : Atmel ATmega8 MCU at 4MHz XTAL
//*****************************************************
/*
                         HEADER FILES 
-----------------------------------------------------------------*/
#include "CashRegisterPrinter.h"

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
    
    e = 0;
    buffer2printer();

}
//=================================================
//
//=================================================
void buffer2printer(void)
{

    PORTC |= (1<<motorEnable); // Ενεργοποίησε το μοτέρ.
    findStart();

    unsigned char a = 0;

        for(; e<bigBufferLength; e++)
        {
            a = bigBuffer[e];
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

				case 'W':
                UART_PrintfProgStr_P(letterW);
                break;

				case 'F':
                UART_PrintfProgStr_P(letterF);
                break;

				case 'J':
                UART_PrintfProgStr_P(letterJ);
                break;

                case ' ':
                UART_PrintfProgStr_P(letterSPA);
                break;

                case 'n':
                changeLine();
                break;

                case '*':
                bigBufferLength = 0;
                break;

            }

            if(c == 14)
            {
                changeLine();
                c=0;
            }
            else
                c++;                                         
        }

        findStart();
        bigBufferLength = 0;
        e = 0;
        PORTC &= ~(1<<motorEnable); // Απενεργοποίησε το μοτέρ.

// Όσο ο buffer δεν έχει δεδομένα, περίμενε εδώ μεχρι να έρθουν.
    for(;bigBufferLength == 0;)
    {
        delay_ms(50);   
    }
    buffer2printer();
}

//=================================================
//
//=================================================
void scrollTape(void)
{
        changeLine();
        changeLine();
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
                delay_ms(300);
                PORTD = 0x00;
				delay_ms(400); // 300 + 400 = 700
            }
            while ((~PINB & (1<<optoMove)));
            while (!(~PINB & (1<<optoMove)));
    }   
            while ((~PINB & (1<<optoMove))); // Αφησε 2 κενές κολόνες μεταξυ των γραμματων.
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

	// copy the received data to a local buffer
	for(i=0; i<receiveDataLength; i++)
	{
		bigBuffer[bigBufferLength++] = *receiveData++;
	}
}

//=================================================
//    FUNCTION TO GENERATE DELAY IN MILLISECONDS
//=================================================

void delay_ms(unsigned long count) // Καθηστέρηση 2.5 ms
{
    while(count--);
}

