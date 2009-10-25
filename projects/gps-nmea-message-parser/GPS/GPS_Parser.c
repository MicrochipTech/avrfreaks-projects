


#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\iomx8.h>
#include <stdbool.h> 
#include "Init.h"
#include "nmea.h"
#define outp(a, b) b = a



void USART_Transmit( unsigned char data );
void Term_Send_Value_as_Digits( uint16_t value );
void Term_Send_Value_as_Digits8( uint8_t value );
void PrintNMEABuffer(char* packet);
volatile uint8_t status=0;
volatile uint8_t BuffIndex=0;
char USARTBuffer[80];
char NMEAPacket[80];
char GPGGA[80];//working buffer
//GPGGA data
char   Lat[9];
char Long[10];
char GPSTime[6];
char NS[1];
char EW[1];
char FixQuality;
char Altitude[7];

// GPRMC data
char Speed[5];//in knots
char Heading[5];
char GPSDate[6];

uint16_t Average(uint16_t);




int main(void) {
Init_all();
sei();
USART_Transmit('S');
USART_Transmit('T');
USART_Transmit('A');
USART_Transmit('R');
USART_Transmit('T');
USART_Transmit(' ');

while (1) {//loop forever

if ((!status))
{
//PrintNMEABuffer(NMEAPacket);
nmeaProcess();
USART_Transmit('T');
USART_Transmit('I');
USART_Transmit('M');
USART_Transmit('E');
USART_Transmit(' ');
for (int i=0;i<6;i++)
{USART_Transmit(GPSTime[i]);
}
USART_Transmit(' ');
USART_Transmit('L');
USART_Transmit('A');
USART_Transmit('T');
USART_Transmit(' ');
for (int i=0;i<9;i++)
{USART_Transmit(Lat[i]);
}
USART_Transmit(' ');
USART_Transmit(NS[0]);
USART_Transmit(' ');
USART_Transmit('L');
USART_Transmit('O');
USART_Transmit('N');
USART_Transmit('G');
USART_Transmit(' ');
for (int i=0;i<10;i++)
{USART_Transmit(Long[i]);
}
USART_Transmit(' ');
USART_Transmit(EW[0]);
USART_Transmit(' ');
USART_Transmit('A');
USART_Transmit('L');
USART_Transmit('T');
USART_Transmit(' ');
for (int i=0;i<7;i++)
{USART_Transmit(Altitude[i]);
}
USART_Transmit(' ');
USART_Transmit('S');
USART_Transmit('P');
USART_Transmit('E');
USART_Transmit('E');
USART_Transmit('D');
USART_Transmit(' ');
for (int i=0;i<5;i++)
{USART_Transmit(Speed[i]);
}
USART_Transmit(' ');
USART_Transmit('D');
USART_Transmit('I');
USART_Transmit('R');
USART_Transmit(' ');
for (int i=0;i<5;i++)
{USART_Transmit(Heading[i]);
}
USART_Transmit(' ');
USART_Transmit('D');
USART_Transmit('A');
USART_Transmit('T');
USART_Transmit('E');
USART_Transmit(' ');
for (int i=0;i<6;i++)
{USART_Transmit(GPSDate[i]);
}

USART_Transmit('\r');
USART_Transmit('\n');

}
} 
}

void PrintNMEABuffer(char* packet)
{
int i=0;
while(packet[i]!='*')
{
USART_Transmit(packet[i]);
i++;
}

USART_Transmit(0xa);
USART_Transmit(0xd);


}




void USART_Transmit( unsigned char data )
{
/* Wait for empty transmit buffer */
while ( !( UCSR0A & (1<<UDRE0)) )
;
/* Put data into buffer, sends the data */
UDR0 = data;
}
//***************************
// Convert byte to 3 ASCII digits and send
//***************************
void Term_Send_Value_as_Digits( uint16_t value )
{
    unsigned char digit;
	
	
    
    digit = '0';
    while( value >= 100 )                // Still larger than 100 ?
    {
        digit++;                         // Increment first digit
        value -= 100;
    }
    
    USART_Transmit( digit );                  // Send first digit
    
    digit = '0';
    while( value >= 10 )                 // Still larger than 10 ?
    {
        digit++;                         // Increment second digit
        value -= 10;
    }
    
    USART_Transmit( digit );                  // Send second digit
    
    USART_Transmit( '0' + value );            // Send third digit
}







void Term_Send_Value_as_Digits8( uint8_t value )
{
    unsigned char digit;
	
	
    
    digit = '0';
    while( value >= 100 )                // Still larger than 100 ?
    {
        digit++;                         // Increment first digit
        value -= 100;
    }
    
    USART_Transmit( digit );                  // Send first digit
    
    digit = '0';
    while( value >= 10 )                 // Still larger than 10 ?
    {
        digit++;                         // Increment second digit
        value -= 10;
    }
    
    USART_Transmit( digit );                  // Send second digit
    
    USART_Transmit( '0' + value );            // Send third digit
}

