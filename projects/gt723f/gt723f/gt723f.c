#include <alcd.h>
#include <mega16.h>
#include <delay.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
// Standard Input/Output functions
#include <stdio.h>

   
void main(void)
{
// Declare your local variables here
   int i=0;
   int UTC_hh=0,UTC_mm=0,UTC_ss=0,Latitude_Deg=0,Longitude_Deg=0;
   char str_temp2[20],str_temp3[20];
     char GPRMC[13][15];
     char lcd_buffer[10];
char *pdest,*ptemp1,*ptemp2;
float Latitude_Mnt=0.0,Longitude_Mnt=0.0;
char str[7][13];
int RMC=10;
// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTA=0x00;
DDRA=0x00;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x00;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0 output: Disconnected
TCCR0=0x00;
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0x00;
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
MCUCR=0x00;
MCUCSR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: Off
// USART Mode: Asynchronous
// USART Baud Rate: 9600
UCSRA=0x00;
UCSRB=0x10;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x33;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// ADC initialization
// ADC disabled
ADCSRA=0x00;

// SPI initialization
// SPI disabled
SPCR=0x00;

// TWI initialization
// TWI disabled
TWCR=0x00;

// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTC Bit 0
// RD - PORTC Bit 1
// EN - PORTC Bit 2
// D4 - PORTC Bit 4
// D5 - PORTC Bit 5
// D6 - PORTC Bit 6
// D7 - PORTC Bit 7
// Characters/line: 16
lcd_init(16);
lcd_puts("Loading...");
  lcd_gotoxy(0,1);
  lcd_puts("NiV");
  delay_ms(500);
while (1)
      {
      lcd_clear();
scanf("%s",str[0]);
scanf("%s",str[1]);
scanf("%s",str[2]);
scanf("%s",str[3]);
scanf("%s",str[4]);
scanf("%s",str[5]);
scanf("%s",str[6]);

for (i=0;i<7;i++)
{
 	if(!strncmp(str[i],"$GPRMC",6))
		RMC=i;
}

ptemp1=str[RMC];
for(i=0;i<13;i++)
{
	ptemp2 = strchr(ptemp1,',');
	ptemp1 = ptemp2 + 1;
	pdest = strchr(ptemp1,',');
	strncpy(GPRMC[i],ptemp1,pdest - ptemp1);

}

//UTC Time-------------------------------------------------------
//---------------------------------------------------------------
strncpy(str_temp2,GPRMC[0],2);
UTC_hh = atoi(str_temp2);
strncpy(str_temp2,GPRMC[0]+2,2);
UTC_mm = atoi(str_temp2);
strncpy(str_temp2,GPRMC[0]+4,2);
UTC_ss = atoi(str_temp2);
//Tehran Time+++++++++++++++++++++
if((UTC_mm + 30)>59)
{
	UTC_mm = UTC_mm - 30;
	UTC_hh = UTC_hh + 3 + 1;
}
else
{
 	UTC_mm = UTC_mm + 30;
	UTC_hh = UTC_hh + 3;
}

lcd_gotoxy(0,0);
sprintf(lcd_buffer,"%2d:%2d:%2d",UTC_hh,UTC_mm,UTC_ss);
lcd_puts(lcd_buffer);
         /*
//---------------------------------------------------------------
//Latitude-------------------------------------------------------
//---------------------------------------------------------------
strncpy(str_temp3,GPRMC[2],2);
Latitude_Deg = atoi(str_temp3);
strncpy(str_temp3,GPRMC[2]+2,strlen(GPRMC[2])-2);
Latitude_Mnt = atof(str_temp3);


lcd_gotoxy(0,0);
sprintf(lcd_buffer,"LD: %s%4d",GPRMC[3],Latitude_Deg);
lcd_puts(lcd_buffer);
//lcd_gotoxy(8,1);
//sprintf(lcd_buffer,"Latitude Minute : %7.4f",Latitude_Mnt);
//lcd_puts(lcd_buffer);
//---------------------------------------------------------------

   */
  /*
//---------------------------------------------------------------
//Longitude------------------------------------------------------
//---------------------------------------------------------------
strncpy(str_temp4,GPRMC[4],3);
Longitude_Deg = atoi(str_temp4);
strncpy(str_temp2,GPRMC[4]+3,strlen(GPRMC[4])-3);
Longitude_Mnt = atof(str_temp2);
lcd_gotoxy(0,10);
sprintf(lcd_buffer,"LD: %s%5d",GPRMC[5],Longitude_Deg);
lcd_putsf(lcd_buffer);
lcd_gotoxy(0,11);
sprintf(lcd_buffer,"LM: %7.4f",Longitude_Mnt);
lcd_putsf(lcd_buffer);
*/
delay_ms(1000);
}
}

