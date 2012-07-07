/*
 * PWM.c
 *
 * Created: 5/14/2012 10:26:20 PM
 *  Author: Ignacio
 *
 */ 

#define F_CPU 1000000
#define LED 1

#define BAUDRATE 4800 //valor usual

#include <avr/io.h>
//#include <avr/interrupt.h>
#include <util/delay.h>
//#include <string.h>
//#include <stdlib.h>
#include <inttypes.h>


void PWM_Init(){
		 
	 DDRD |= 0xFF;
	 TCCR1A |= 1<<WGM11  |              // Fast PWM
	           1<<COM1A1 | 1<<COM1B1;	// PWM1 e PWM2 non-inverted
	 
	 TCCR1B |= 1<<WGM12  | 1<<WGM13 |   // Fast PWM
	           1<<CS10;					// 1-prescaler
	 
	 ICR1 = 19999;						// Sets the period, using Timer 1
}
	 
void PWM2_Init(){
	
		
	 DDRB |= 1<<PB3;                    // Uses PB3 as OC0
	 
	 TCCR0 |= 1<<WGM00 | 1<<WGM01 |		// Fast PWM
	          1<<COM00 | 1<<COM01 |		// Non inverted
			  1<<CS00  | 1<<CS01 ;		// 64- prescaler

	 
	 DDRD |= 1<<PD7;                    // Use PD7 as OC2
	 
	 TCCR2 |= 1<<WGM20 | 1<<WGM21 |		// Fast PWM
	          1<<COM20 | 1<<COM21 |		// Non inverted   1<<COM20
			  1<<CS22;					// Clock Externo
	   

}
/*
 *    Frequency equation                         on Timer 1:  F_CPU/(prescaler(1+ICR1))
 *                                               on Timer 0:  F_CPU/(prescaler*510)
 */

//PWM input is the percentage that you need 
void PWM_setDutyA(double x){
	//Timer 1 function that sets the width of the pulse
	OCR1A = x*ICR1/100; //50Hz
}
void PWM_setDutyB(double x){
	//Timer 1 function that sets the width of the pulse
	OCR1B = x*ICR1/100;
	
}	
void PWM_setDutyC(double x){
	//Timer 0 function that sets the width of the inverted pulse
	OCR0 = (100-x)*256/100;
}
void PWM_setDutyD(double x){
	//Timer 2 function that sets the width of the inverted pulse
	OCR2 = (100-x)*256/100;
}

/*
 * Defining UBRR = F_CPU/(16*BAUDRATE) - 1
 */

void USARTInit(uint16_t ubrr_value) 
{
	DDRD |= 1<<PIND0; //Set PD0 as input, because of Rx
	//Set Baud rate

	UBRRL = ubrr_value;
	UBRRH = (ubrr_value>>8); 

   /*Set Frame Format
    *> Asynchronous mode
    *> No Parity
    *> 1 StopBit
    *> char size 8
	*/

    UCSRC=(1<<URSEL)|(3<<UCSZ0);

    //Enable The receiver and transmitter
    UCSRB=(1<<RXEN)|(1<<TXEN);
   
}   


char USARTReadChar()
{
   //Wait untill a data is available

   while(!(UCSRA & (1<<RXC)))
   {
      //Do nothing
   }

   //Now USART has got data from host
   //and is available is buffer

   return UDR;
}

void USARTWriteChar(char data)
{
   //Wait until the transmitter is ready

   while(!(UCSRA & (1<<UDRE)))
   {
      //Do nothing
   }

   //Now write the data to USART buffer

   UDR=data;
}


//Braco 1 = Left Hand , Braco 2 = Right Hand ,
//Abrir = open , Fechar = close ,
//Centro = move to center, Horario = turn clockwise, AntiHorario = turn anti-clockwise 
void Braco1Abrir()
{
	PWM_setDutyC(13); //OC0 , 60Hz, Timer 0
}

void Braco1Fechar()
{
	PWM_setDutyC(3.5); //OC0, 60Hz, Timer 0
}

void Braco1Centro()
{
	PWM_setDutyA(6.55);
}

void Braco1Horario()
{
	PWM_setDutyA(11);
}

void Braco1AntiHorario()
{
	PWM_setDutyA(2.06);
}

void Braco2Fechar()	
{
	PWM_setDutyD(13); //OC2 , 60Hz, Timer 2
}

void Braco2Abrir()
{
	PWM_setDutyD(5.5); //OC2, 60Hz, Timer 2
}

void Braco2Centro()
{
	PWM_setDutyB(6.75);
}

void Braco2Horario()
{
	PWM_setDutyB(11.3);
}

void Braco2AntiHorario()
{
	PWM_setDutyB(2.2);
}



void PIinit()
{
		PWM_setDutyC(7);
		PWM_setDutyD(7);
		Braco1Centro();
		Braco2Centro();
}

void GirarFaceB2h()
{	
	Braco2Horario();
	_delay_ms(1000);
	Braco2Abrir();
	_delay_ms(1000);
	Braco2Centro();
	_delay_ms(1000);
	Braco2Fechar();
	_delay_ms(1000);
}

void GirarCuboB2h()
{
	Braco1Centro();
	Braco1Abrir();	
	_delay_ms(1000);
	Braco2Horario();
	_delay_ms(1000);
	Braco1Fechar();
	_delay_ms(1000);
	Braco2Abrir();
	_delay_ms(1000);
	Braco2Centro();
	_delay_ms(1000);
	Braco2Fechar();
	_delay_ms(1000);
					
}

void GirarFaceB2a()
{
	Braco2AntiHorario();
	_delay_ms(1000);
	Braco2Abrir();
	_delay_ms(1000);
	Braco2Centro();
	_delay_ms(1000);
	Braco2Fechar();
	_delay_ms(1000);
}

void GirarCuboB2a()
{
	Braco1Centro();
	Braco1Abrir();	
	_delay_ms(1000);
	Braco2AntiHorario();
	_delay_ms(1000);
	Braco1Fechar();
	_delay_ms(1000);
	Braco2Abrir();
	_delay_ms(1000);
	Braco2Centro();
	_delay_ms(1000);
	Braco2Fechar();
	_delay_ms(1000);
}

void GirarFaceB1h()
{
	Braco1Horario();
	_delay_ms(1000);
	Braco1Abrir();
	_delay_ms(1000);
	Braco1Centro();
	_delay_ms(1000);
	Braco1Fechar();
	_delay_ms(1000);
}

void GirarCuboB1h()
{
	Braco2Centro();
	Braco2Abrir();	
	_delay_ms(1000);
	Braco1Horario();
	_delay_ms(1000);
	Braco2Fechar();
	_delay_ms(1000);
	Braco1Abrir();
	_delay_ms(1000);
	Braco1Centro();
	_delay_ms(1000);
	Braco1Fechar();
	_delay_ms(1000);
					
}

void GirarFaceB1a()
{
	Braco1AntiHorario();
	_delay_ms(1000);
	Braco1Abrir();
	_delay_ms(1000);
	Braco1Centro();
	_delay_ms(1000);
	Braco1Fechar();
	_delay_ms(1000);
}

void GirarCuboB1a()
{
	Braco2Centro();
	Braco2Abrir();	
	_delay_ms(1000);
	Braco1AntiHorario();
	_delay_ms(1000);
	Braco2Fechar();
	_delay_ms(1000);
	Braco1Abrir();
	_delay_ms(1000);
	Braco1Centro();
	_delay_ms(1000);
	Braco1Fechar();
	_delay_ms(1000);	
}

int main(void)
{
	DDRA = 0xFF;
	double i ;
	char data,data_aux;
	USARTInit(12);// 51 para BaudRate = 9600 e 25 para Baud = 19200 e 12 para Baud = 4800 (1MHz)
	PWM_Init();
	PWM2_Init();
	data = 'i';
  	while(1) 
	{	
		//data = USARTReadChar();
		switch (data) {
			case 'a':
				GirarFaceB1a();
				break;
			case 'b':
				GirarFaceB1h();
				break;
			case 'c':
				GirarCuboB1a();
				break;
			case 'd':
				GirarCuboB1h();
				break;
			case 'e':
				GirarFaceB2a();
				break;
			case 'f':
				GirarFaceB2h();
				break;
			case 'g':
				GirarCuboB2a();
				break;
			case 'h':
				GirarCuboB2h();
				break;
			case 'i':
				PIinit();
				data_aux = USARTReadChar();
				Braco1Fechar();
				Braco2Fechar();
				Braco2Centro();
				Braco1Centro();
				_delay_ms(1000);
				USARTWriteChar(data_aux);
			break;
		}				
		USARTWriteChar(data);
		data = USARTReadChar();
	}

}
