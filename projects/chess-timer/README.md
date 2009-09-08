# Chess Timer

Uploaded by admin on 2009-09-08 13:23:00 (rating 0 out of 5)

## Summary

This project has been done using an ATmega16. Players can set the time in mins and secs before starting using 3 buttons(interrupts) and they can see the time and players turn on a 2x16 LCD. once a player's time is up, the winner will be declared on LCD.


here is the code:


/*****************************************************  

Chip type : ATmega16  

Program type : Application  

Clock frequency : 1.000000 MHz  

*****************************************************/


#include 


// Alphanumeric LCD Module functions  

#asm  

 .equ \_\_lcd\_port=0x1B ;PORTA  

#endasm  

#include   

#include   

#include  


// Declare your global variables here  

int turn=0,wtime=0,btime=0,min=10,sec=5,start=0;


// External Interrupt 0 service routine  

interrupt [EXT\_INT0] void ext\_int0\_isr(void)  

{  

// Place your code here  

#asm("cli");  

delay\_ms(200); 


if(start==1)  

{  

 turn=1;  

 wtime+=sec; 


 GICR|=0x80;  

 MCUCR=0x00;  

 MCUCSR=0x00;  

 GIFR=0x80;  

}  

else  

{  

 if(min<30)  

 min+=5;  

 else  

 min=5;


 GICR|=0xE0;  

 MCUCR=0x00;  

 MCUCSR=0x00;  

 GIFR=0xE0;  

} 


#asm("sei")  

}


// External Interrupt 1 service routine  

interrupt [EXT\_INT1] void ext\_int1\_isr(void)  

{  

// Place your code here  

#asm("cli");  

delay\_ms(200); 


if(start==1)  

{  

 turn=0;  

 btime+=sec; 


 GICR|=0x40;  

 MCUCR=0x00;  

 MCUCSR=0x00;  

 GIFR=0x40;  

}  

else  

{  

 if(sec<20)  

 sec+=5;  

 else  

 sec=0; 


 GICR|=0xE0;  

 MCUCR=0x00;  

 MCUCSR=0x00;  

 GIFR=0xE0;  

}


#asm("sei")  

}


// External Interrupt 2 service routine  

interrupt [EXT\_INT2] void ext\_int2\_isr(void)  

{  

// Place your code here  

#asm("cli");  

delay\_ms(200); 


start=1;  

btime=wtime=min*60;


GICR|=0xA0;  

MCUCR=0x00;  

MCUCSR=0x00;  

GIFR=0xA0;  

#asm("sei")  

} 


void finish(int u)  

{  

char s[16]="";  

if(u==1)  

 sprintf(s," White Won! ");  

else  

 sprintf(s," Black Won! ");  

lcd\_clear();  

lcd\_gotoxy(0,0);  

lcd\_putsf("****************");  

lcd\_gotoxy(0,1);  

lcd\_putsf("****************");  

delay\_ms(300);  

lcd\_gotoxy(0,0);  

lcd\_putsf("******* *******");  

lcd\_gotoxy(0,1);  

lcd\_putsf("******* *******");  

delay\_ms(300);  

lcd\_gotoxy(0,0);  

lcd\_putsf("****** ******");  

lcd\_gotoxy(0,1);  

lcd\_putsf("****** ******");  

delay\_ms(300);  

lcd\_gotoxy(0,0);  

lcd\_putsf("***** *****");  

lcd\_gotoxy(0,1);  

lcd\_putsf("***** *****");  

delay\_ms(300);  

lcd\_gotoxy(0,0);  

lcd\_putsf("**** ****");  

lcd\_gotoxy(0,1);  

lcd\_putsf("**** ****");  

delay\_ms(300);  

lcd\_gotoxy(0,0);  

lcd\_putsf("*** ***");  

lcd\_gotoxy(0,1);  

lcd\_putsf("*** ***");  

delay\_ms(300);  

lcd\_gotoxy(0,0);  

lcd\_putsf("** **");  

lcd\_gotoxy(0,1);  

lcd\_putsf("** **");  

delay\_ms(300);  

lcd\_gotoxy(0,0);  

lcd\_putsf("* *");  

lcd\_gotoxy(0,1);  

lcd\_putsf("* *");  

delay\_ms(300);  

lcd\_gotoxy(0,0);  

lcd\_puts(s);  

lcd\_gotoxy(0,1);  

lcd\_putsf(" ");  

}


void main(void)  

{  

// Declare your local variables here  

char s1[16]="",s2[16]="";  

int u=2;  

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

// INT0: On  

// INT0 Mode: Low level  

// INT1: On  

// INT1 Mode: Low level  

// INT2: On  

// INT2 Mode: Falling Edge  

GICR|=0xE0;  

MCUCR=0x00;  

MCUCSR=0x00;  

GIFR=0xE0;


// Timer(s)/Counter(s) Interrupt(s) initialization  

TIMSK=0x00;


// Analog Comparator initialization  

// Analog Comparator: Off  

// Analog Comparator Input Capture by Timer/Counter 1: Off  

// Analog Comparator Output: Off  

ACSR=0x80;  

SFIOR=0x00;


// LCD module initialization  

lcd\_init(16); 


//WatchDogTimer: OSC/2048k  

WDTCR=0x1F;  

WDTCR=0x0F;


// Global enable interrupts  

#asm("sei")


while (1)  

{  

 // Place your code here  

 if(u==2)  

 {  

 if(start==0)  

 {  

 sprintf(s1," %2d %2d ",min,sec);  

 lcd\_gotoxy(0,0);  

 lcd\_putsf("Minutes Seconds");  

 lcd\_gotoxy(0,1);  

 lcd\_puts(s1);  

 }  

 else  

 {  

 delay\_ms(1000);  

 if(turn==1)  

 {  

 btime--;  

 if(btime==0)  

 u=1;  

 }  

 else  

 {  

 wtime--;  

 if(wtime==0)  

 u=0;  

 }  

 sprintf(s2," %2d:%2d %2d:%2d ",wtime/60,wtime%60,btime/60,btime%60);  

 lcd\_gotoxy(0,0);  

 lcd\_putsf(" White Black ");  

 lcd\_gotoxy(0,1);  

 lcd\_puts(s2);  

 }  

 }  

 else  

 break;  

 #asm("wdr");  

};  

finish(u);  

#asm("wdr");  

}

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
