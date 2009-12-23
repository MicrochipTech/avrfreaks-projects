
//Dibuat pada tanggal 18 Desember 2009

#include<mega16.h>
#include<delay.h>  
#define pinmotor PORTC
/*
PORTC.0 kanan1
PORTC.1 kanan2
PORTC.2 kiri1
PORTC.3 kiri2
*/
int i;
void motormati(){
pinmotor=0x00;   
}
void belokkanan(){ //motor kiri jalan
pinmotor=0x08;//0b00001000     
}
void belokkiri(){ //motor kanan jalan
pinmotor=0x02;//0b00000010
}
void motormaju(){
pinmotor=0x0A;//0b00001010
} 
void main(void){
// Port D initialization
// Func7=In Func6=In Func5=Out Func4=Out Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=0 State4=0 State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x30;
DDRC=0xff;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=FFh
// OC0 output: Disconnected
TCCR0=0x00;
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 10,800 kHz
// Mode: Ph. correct PWM top=03FFh
// OC1A output: Non-Inv.
// OC1B output: Non-Inv.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0xA3;
TCCR1B=0x05;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;
while(1){
motormaju();
OCR1A=1023;//motor Left
OCR1A=100;
delay_ms(500);
}
}
}