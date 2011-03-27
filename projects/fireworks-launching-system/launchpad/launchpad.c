#define OSCSPEED	8000000		/* in Hz */
#define __AVR_ATmega644P__ 1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h> 
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define sbi(a, b) (a) |= (1 << (b))//set bit,  sbi(PORTA, 1);cbi(PORTA,1);
#define cbi(a, b) (a) &= ~(1 << (b)) 


void WriteLn(char Text[80]);
void Write(char Text[80]);
unsigned char Length(char Temp[80]);
void UartTransmit(unsigned char Data);
unsigned char UartReceive(void);
void UartInit(uint32_t Baud);
void Init(void);
void LedMode (unsigned char Temp);
void BlinkLed(void);
double getNumber(void);
void menu(void);
void EnablePWM(void);
void DisablePWM(void);
void setFuse(unsigned char fuseNumber);
void setDutyCycle(uint32_t number);
void setFrequency(uint32_t number);

char text[30], text2[30];
unsigned char Latch1 = 4, Latch2 = 5, oeLatch1 = 3, oeLatch2 = 4, TCCR1A_User = 0xA2, TCCR1B_User = 0x1A;
uint32_t fuseNumber = 0, writeDisplay = 0, rx = 0;
uint16_t delay = 0, userNumber = 0, DC = 0, Frequency = 0;

uint16_t temp16_1 = 0;
uint32_t temp32_1 = 0, temp32_2 = 0, temp32_3 = 0;

int main(void) {

	Init();
	DisablePWM();
	UartInit(19200U);
	
	setFuse(1);
	setFrequency(2000U);
	setDutyCycle(20U);
	delay = 500U;

	while( 1 ) {
		menu();
		userNumber = getNumber();
		WriteLn("");

		if( userNumber == 1 ) {
			strcpy(text, "Enter Fuse (1-32): ");
			Write(text);
			userNumber = getNumber();
			WriteLn("");
			if( !(userNumber >= 0 && userNumber <= 31) ) {
				strcpy(text, "Invalid Fuse Number!");
				WriteLn(text);
			} else {
				setFuse(userNumber);
			}

		} else if( userNumber == 2 ) {
			strcpy(text, "Enter Duty Cycle Register (0-100): ");
			Write(text);
			userNumber = getNumber();
			WriteLn("");
			if( !(userNumber >= 0 && userNumber <= 100) ) {
				strcpy(text, "Invalid Duty Cycle!");
				WriteLn(text);
			} else {
				setDutyCycle(userNumber);
				
			}

		} else if( userNumber == 3 ) {
			strcpy(text, "Enter ICR1 Value (0-65535): ");
			Write(text);
			userNumber = getNumber();
			WriteLn("");
			if( !(userNumber >= 0 && userNumber <= 65535) ) {
				strcpy(text, "Invalid Frequency!");
				WriteLn(text);
			} else {
				setFrequency(userNumber);
				setDutyCycle(DC);//recalc

			}

		} else if(  userNumber == 4 ) {
			strcpy(text, "Enter Duration 0-20000ms: ");
			Write(text);
			userNumber = getNumber();
			WriteLn("");
			if( !(userNumber >= 0 && userNumber <= 20000) ) {
				strcpy(text, "Invalid Duration!");
				WriteLn(text);
			} else {
				delay = userNumber;

			}

		} else if( userNumber == 5 && delay > 0 ) {
			strcpy(text, "Running...");
			Write(text);

			setFrequency(Frequency);//I think it changes my ICR1 when an 'event' occurs..

			EnablePWM();

			//Delay for ..the duration
			_delay_ms(delay);

			DisablePWM();

			strcpy(text, "Stopped");
			WriteLn(text);
		}

	}
}

void setFrequency(uint32_t number) {
	Frequency = number;
	ICR1 = Frequency;
}
void setDutyCycle(uint32_t number) {
		//Set Output Compare Register 0
			//0-255  where x/255 = DC, DC:DutyCycle therefore x = (DC/100)*255
			//10% would be 25.5 where .5 would get truncated.
			//which is actually 9.8% duty cycle for .2% error

		//255 is the frequency combined with the prescaler
		/*
		Prescaler will choose the frequency
		1		8MHz:		256/8=32 micro-seconds		1
		8		8/8MHz:		256 micro-seconds			2
		64		8/64:		256/.125 = 2048 us			3
		256		8/256:		256/that = 8192 us			4
		1024	8/1024		256/that = 32.768 ms		5

		//ICR 2^16
		256     8/256		65536/that = 2,097,152  about 2 seconds

		OCR0 A&B choose the duty cycle
		Switching the TOP away from 0xFF changes frequency also, see ICR
		*/

		//DC = ICR1 - (userNumber/100)*ICR1;  basically

	DC = 100 - number;
	temp32_1 = DC;
	temp32_2 = ICR1;
	temp32_3 = temp32_1 * temp32_2;
	temp32_1 = temp32_3 / 100U;
	temp16_1 = temp32_1;
	
	OCR1A = temp16_1;//16 bit registers, A and B are 2 independant output compares
	OCR1B = temp16_1;
}

void setFuse(unsigned char fuseNumber) {
	//Set, no light
	fuseNumber--;

	if( fuseNumber < 16 ) {
		//Ensure outputs are low on latches, so power lines are disabled
		sbi(PORTD, oeLatch1);
		sbi(PORTD, oeLatch2);

		//Raise enable latch line on latche to accept new data
		sbi(PORTA, Latch1);

		//Apply data to lines, first clear, then set
		PORTA &= 0xF0;
		PORTA |= (0x0F & fuseNumber);

		//Latch this latch for the fuses we want, low latches input
		cbi(PORTA, Latch1);

	} else if( fuseNumber >= 16 && fuseNumber < 33 ) {
		fuseNumber -= 16;//0-15 still for decoding

		//Ensure outputs are low on latches, so power lines are disabled
		sbi(PORTD, oeLatch1);
		sbi(PORTD, oeLatch2);

		//Raise enable latch line on latche to accept new data
		sbi(PORTA, Latch2);

		//Apply data to lines, first clear, then set
		PORTA &= 0xF0;
		PORTA |= (0x0F & fuseNumber);

		//Latch this latch for the fuses we want, low latches input
		cbi(PORTA, Latch2);

	}
}
void EnablePWM() {
	//Set WGM01 & WGM02 for fast pwm(last 2 bits)
	//also COM0A1&2 and COM0B1&2 for output (first 4 bits)
	TCCR1A = TCCR1A_User;

																
	//Set pwm clock scaler, pre-scaler are last 3 bits :00:00:0:001
	TCCR1B = TCCR1B_User;//prescaler: 0 stopped, 1 noscale, 2 8, 3 64, 4 256, 5 1024
	//the 4th bit sets whether it's comparing OCR0/1, or just top=0xff or something..
	//TCCR0B = 0x0C or 0x04..depending on bit 4
}
void DisablePWM() {
	//Set WGM01 & WGM02 for fast pwm(last 2 bits)
	//also COM0A1&2 and COM0B1&2 for output (first 4 bits)
	TCCR1A = 0x00;

																
	//Set pwm clock scaler, pre-scaler are last 3 bits :00:00:0:001
	TCCR1B = 0x00;//prescaler /8
	//the 4th bit sets whether it's comparing OCR0/1, or just top=0xff
}
void menu() {
	WriteLn("");
	strcpy(text, "1) Set Fuse");	
	WriteLn(text);
	strcpy(text, "2) Set Duty Cycle");	
	WriteLn(text);
	strcpy(text, "3) Set Frequency");	
	WriteLn(text);
	strcpy(text, "4) Set Duration");	
	WriteLn(text);
	strcpy(text, "5) Go!");	
	WriteLn(text);
	strcpy(text, "Choose: ");
	Write(text);
}


//This holds up till the number is gotten
double getNumber () 
{
	unsigned long number = 0, test = 0, done = 0;

	while( !done ) {
		rx = UartReceive();
		test = rx - 48;//convert to number
		if (test >= 0 && test <= 9)
		{
			//send back for user display
			UartTransmit(rx);
		
			//shift over the decimal and add the 0-9 to the right
			number = number*10 + test;
		} else if( rx == '\r' ) {
			done = 1;
		}
	}
	return number;
}


void Init(void)
{
	//A6&A7=0xC0 - Drive Outputs high so the outputs of the latches go low, so the explosives don't go off on boot up.
	PORTA = 0xC0;
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x30;//Drive oeLatch1&2 high, they are active low

	//set the output enables for address lines and enable-latch lines and the output enable latches.
	DDRA = 0xFF;
	DDRB = 0x01;//led
	DDRC = 0x00;
	DDRD = 0x30;//Make sure we're driving oeLatch1&2 low  (PWM)
}

void LedMode (unsigned char Temp)
{
	switch (Temp)
	{
	// 1 - LED ON
	case 1:
		PORTB = PORTB & 0b11111110;
		break;
	// 2 - Led OFF
	case 2:
		PORTB = PORTB | 0b00000001;
		break;
	// 3 - Toggle Led
	case 3:
		if (PINB & 0b00000001)
			PORTB = PORTB & 0b11111110;
		else
			PORTB = PORTB | 0b00000001;
		break;
	}
}

void UartInit(uint32_t Baud)
{
	int BaudRate = OSCSPEED / (16 * Baud) - 1;

	UBRR0H = (unsigned char) BaudRate>>8;
	UBRR0L = (unsigned char) BaudRate;
	//set BaudRate

	UCSR0B = UCSR0B | (0b00011000); 
	// RXEN & TXEN enable (Bits 4, 3 = 1)

	UCSR0C = (UCSR0C | (0b10000110)); 
	// USART Register Select (Bit 7 = 1)
	// 8 data bits per frame (Bit 2, 1 = 1)

	UCSR0C = UCSR0C & 0b11110111;
	// 1 Stop bit (Bit 3 = 0)	
}

unsigned char UartReceive(void)
{
	if (UCSR0A & 0b10000000)
	//if RXC(Bit 7) = 1 
		return UDR0;
	else
		return 0;
}

void UartTransmit(unsigned char Data)
{
	//Wait untill the transmitter is ready
	while(!(UCSR0A & (1<<UDRE0))){
		LedMode(3);//flash led
	} 
	UDR0 = Data;
}
unsigned char Length(char Temp[80])
{
	unsigned char L=0;
	while (Temp[L]) L++;
	return L;
}

void Write(char Text[80])
{
	unsigned char Len, i, T;
	Len = Length(Text);
	//UartTransmit(Len);
	for (i=0; i<Len; i++) 
	{
		T = Text[i];
		UartTransmit(T);
	}
	strcpy(Text, "");
}
void WriteLn(char Text[80])
{
	unsigned char Len, i, T;
	Len = Length(Text);
	//UartTransmit(Len);
	for (i=0; i<Len; i++) 
	{
		T = Text[i];
		UartTransmit(T);
	}
	strcpy(Text, "");
	UartTransmit(10);
	UartTransmit(13);
}
