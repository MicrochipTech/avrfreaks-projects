/*****************************************************
Mobile Robot with Sonar Scanner

www.swarmrobotic.com

Youtube:
http://www.youtube.com/watch?v=UZ18KQiC6b4

Chip type           : ATmega16
Program type        : Application
Clock frequency     : 8.000000 MHz
Memory model        : Small
External RAM size   : 0
Data Stack size     : 256
*****************************************************/

#include <mega16.h>
#include <delay.h>

//Motor Control
#define Motor_left_En  PORTC.4
#define Motor_right_En PORTC.1

#define Motor_left_A   PORTC.3
#define Motor_left_B   PORTC.2

#define Motor_right_A  PORTC.0
#define Motor_right_B  PORTD.7

// Push BT
#define Push_BT   PINC.5

#define Dist_thr 20


#define RXB8 1
#define TXB8 0
#define UPE 2
#define OVR 3
#define FE 4
#define UDRE 5
#define RXC 7

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<OVR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

// USART Receiver buffer
#define RX_BUFFER_SIZE 4
char rx_buffer[RX_BUFFER_SIZE];

#if RX_BUFFER_SIZE<256
unsigned char rx_wr_index,rx_rd_index,rx_counter;
#else
unsigned int rx_wr_index,rx_rd_index,rx_counter;
#endif

// This flag is set on USART Receiver buffer overflow
bit rx_buffer_overflow;

// USART Receiver interrupt service routine
interrupt [USART_RXC] void usart_rx_isr(void)
{
char status,data;
status=UCSRA;
data=UDR;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer[rx_wr_index]=data;
   if (++rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
   if (++rx_counter == RX_BUFFER_SIZE)
      {
      rx_counter=0;
      rx_buffer_overflow=1;
      };
   };
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+
char getchar(void)
{
char data;
if (rx_counter==0) return 255;
while (rx_counter==0);
data=rx_buffer[rx_rd_index];
if (++rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0;
#asm("cli")
--rx_counter;
#asm("sei")
return data;
}
#pragma used-
#endif

// Standard Input/Output functions
#include <stdio.h>

int dist_counter=0;

Timer 1 overflow interrupt service routine
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
 Reinitialize Timer 1 value
 TCNT1H=0xFF;
 TCNT1L=0xFE;
 dist_counter++;
}

long  Distance (){
    int  dis;
    long i;
    i=0;
    PORTC.7=0;
    delay_ms(10);
    PORTC.7=1;
    delay_us(12);
    PORTC.7=0;
    #asm("sei");
    while (PINC.6==0);
    dist_counter=0;
    while (PINC.6==1){
        dist_counter++;
        delay_us(1);
    };
    dis=(dist_counter*2)/58;
    #asm("cli");
    return dis;           
}     


void Stop(){
    Motor_left_En=0;
    Motor_right_En=0;
}

void Motors_enable(){
    Motor_left_En=1;
    Motor_right_En=1;
}

void Forward(int delay){
    Motors_enable();
    Motor_left_A=1;
    Motor_left_B=0;
    Motor_right_A=1;
    Motor_right_B=0;
    if (delay!=0){
     delay_ms(delay);
     Stop();
    }
}    

void Backward(int delay){
    Motors_enable();
    Motor_left_A=0;
    Motor_left_B=1;
    Motor_right_A=0;
    Motor_right_B=1;
    if (delay!=0){
     delay_ms(delay);
     Stop();
    }
}    

void Turn_left(int delay){
    Motors_enable();
    Motor_left_A=1;
    Motor_left_B=0;
    Motor_right_A=0;
    Motor_right_B=1;
    delay_ms(delay);
    Stop();
}    

void Turn_right(int delay){
    Motors_enable();
    Motor_left_A=0;
    Motor_left_B=1;
    Motor_right_A=1;
    Motor_right_B=0;
    delay_ms(delay);
    Stop();
}    


void Sonar_callibration(){
    #asm("cli");
    PORTA=0;
    while (Push_BT){
        PORTA.3=1;
        PORTA.4=1;
        delay_ms(200);
        PORTA.3=0;
        PORTA.4=0;
        delay_ms(200);
    }
    #asm("sei");
}

int dist[5]={100,100,100,100,100};

void Display(){
    int i;
    PORTA=0;
    if (dist[0]<Dist_thr) { PORTA.0=1; }
    if (dist[1]<Dist_thr) { PORTA.1=1; PORTA.2=1;}
    if (dist[2]<Dist_thr) { PORTA.3=1; PORTA.4=1;}
    if (dist[3]<Dist_thr) { PORTA.5=1; PORTA.6=1;}
    if (dist[4]<Dist_thr) { PORTA.7=1; }
}

void Control_motor(){
       
     if (dist[0]<Dist_thr || dist[1]<Dist_thr) {Turn_left(300); return;}
     if (dist[4]<Dist_thr || dist[3]<Dist_thr) {Turn_right(300); return;}
     if (dist[2]<Dist_thr ) {Backward(600); Turn_right(300); return;}
     Forward(0);
}

char Sampling(){
      char step[4]={14,13,11,7};
      char ch;
      int i,j,c;
//      #asm("cli");
      Display();
      PORTB=step[3];
      for (j=0;j<13;j++)
          for (i=0;i<4;i++){
              PORTB=step[i];
              delay_ms(10);
          }
      c=0;
      
      for (j=0;j<=24;j++){
          if ( (j%6)==0){
            Display();
            switch (c){
                case 0 : PORTA.0=1; break;
                case 1 : PORTA.1=1; PORTA.2=1; break;
                case 2 : PORTA.3=1; PORTA.4=1; break;
                case 3 : PORTA.5=1; PORTA.6=1; break;
                case 4 : PORTA.7=1; break;
            }
            dist[c++]=Distance();
            if (dist[c-1]<15) Stop();
          }
          for (i=3;i>=0;i--){
              PORTB=step[i];
              delay_ms(10);
          }
      }
      Display();
      for (j=0;j<12;j++)
          for (i=0;i<4;i++){
              PORTB=step[i];
              delay_ms(10);
          }
      PORTB=0;
    if (Push_BT==0) {
        while (Push_BT==0);
        return 0;
    }
    return 1;
}

void Light_dance(){
    int i,j;
    char dsp=0x80;
    for (i=0;i<8;i++){
        PORTA=dsp;
        dsp/=2;
        delay_ms(100);
    }
    dsp=1;
    for (i=0;i<8;i++){
        PORTA=dsp;
        dsp*=2;
        delay_ms(100);
    }
    PORTA=255;
    delay_ms(500);
    PORTA=0;
}

void Manual_Control(){
    char ch,i;
    Light_dance();
    printf("%c%c%c%c",13,10,13,10);
    delay_ms(100);
    printf("%c%c* Control Robot Manually * %c%c",13,10,13,10);
    delay_ms(100);
    printf("%c%c Active Keys:",13,10);
    delay_ms(100);
    printf("%c%c (I) Forward",13,10);
    delay_ms(100);
    printf("%c%c (K) Backward",13,10);
    delay_ms(100);
    printf("%c%c (L) Right",13,10);
    delay_ms(100);
    printf("%c%c (J) Left %c%c",13,10,13,10);
    delay_ms(100);
    printf("%c%c (S) Sampling %c%c",13,10,13,10);
    delay_ms(100);
    
    while (1){
        ch=getchar();
        if (ch==255) continue;
        switch (ch){
            case 'i' : 
            case 'I' : PORTA=0x3c; Forward(700);  break;
            case 'k' :
            case 'K' : PORTA=0x18; Backward(700); break;
            case 'j' :
            case 'J' : PORTA=0x03; Turn_right(300); break;
            case 'l' :
            case 'L' : PORTA=0xc0; Turn_left(300); break;
            case 's' :
            case 'S' : Sampling();
                       for (i=0;i<5;i++)
                           printf(" %d ", dist[i]);
                       printf("%c%c",13,10);
                    
                        break;
        }        
        
    
        
    
    }    

}

void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port A initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTA=0x00;
DDRA=0xFF;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=T State6=T State5=T State4=T State3=0 State2=0 State1=0 State0=0 
PORTB=0x00;
DDRB=0x0F;

// Port C initialization
// Func7=Out Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=0 State6=T State5=1 State4=T State3=T State2=T State1=T State0=T 
PORTC=0x20;
DDRC=0x9F;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0xF0;

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
// Clock value: Timer 1 Stopped
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: Off
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
// Clock value: Timer 2 Stopped
// Mode: Normal top=FFh
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
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 9600
UCSRA=0x00;
UCSRB=0x98;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x33;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// Global enable interrupts
#asm("sei")

while (1)
      {
      char ch;
      if (Push_BT==0){
        while (Push_BT=0);
        Manual_Control();
      }  
      
      Sonar_callibration();
        printf(" * ");
      while (1){
          if (Sampling()==0) Sonar_callibration();
          PORTB=0;
          for (ch=0;ch<5;ch++)
            printf(" %d ",dist[ch]);
          printf("%c%c",13,10);
          
          Control_motor();          
      }

      };
}
