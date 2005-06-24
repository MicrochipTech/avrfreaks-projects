








/*****************************************************
This program was produced by the
CodeWizardAVR V1.24.6 Standard
Automatic Program Generator
© Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com
e-mail:office@hpinfotech.com

Project : UR Zero
Version : 
Date    : 11-06-2005
Author  : Lars Chr Fergo                  
Company : Denmark                         
Comments: 


Chip type           : ATmega16
Program type        : Application
Clock frequency     : 8,000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 256
*****************************************************/

#include <mega16.h>

// Alphanumeric LCD Module functions
#asm
   .equ __lcd_port=0x15 ;PORTC
#endasm
#include <lcd.h>      

#include <delay.h>     
#include <math.h>   


#define		B1				PINA.0  
#define		B2				PINA.1
#define		B3				PINA.2
#define		B4				PINA.3       
#define		LED				DDRB.0             
#define		BUZZ1				PORTB.4		//BUZZ OUTPUT
#define		BUZZ2				PORTB.5		//BUZZ OUTPUT  
#define		LYS				PORTB.6		//BUZZ OUTPUT
#define		ON				1
#define     	OFF				0    



int t,u,h,ms,sen,sti,men,mti,ten,tti,menu,pen,pti; 
int Omen,Omti,Oten,Otti,Nmen,Nmti,Nten,Ntti;           // LYS tænd, LYS sluk
unsigned long  Dx,D,Den,Dti,D_ON,D_UP;                                  // LYS dæmpnings periode        
unsigned long v,v2;


void LCD_TIME(void)
{     
  lcd_gotoxy(8,0);
  lcd_putsf("     ");  
  lcd_gotoxy(7,0);
  lcd_putchar(48+sen);  
  lcd_gotoxy(6,0);
  lcd_putchar(48+sti);
  lcd_gotoxy(5,0);
  lcd_putsf(":");  
  
  lcd_gotoxy(4,0);
  lcd_putchar(48+men);  
  lcd_gotoxy(3,0);
  lcd_putchar(48+mti);
  lcd_gotoxy(2,0);
  lcd_putsf(":");     
  
  lcd_gotoxy(1,0);
  lcd_putchar(48+ten);  
  lcd_gotoxy(0,0);
  lcd_putchar(48+tti);     

}          


void LCD_OTIME(void)
{   
  lcd_gotoxy(5,0);
  lcd_putsf("       ");  
  
  lcd_gotoxy(4,0);
  lcd_putchar(48+Omen);  
  lcd_gotoxy(3,0);
  lcd_putchar(48+Omti);
  lcd_gotoxy(2,0);
  lcd_putsf(":");     
  
  lcd_gotoxy(1,0);
  lcd_putchar(48+Oten);  
  lcd_gotoxy(0,0);
  lcd_putchar(48+Otti);     

}      

void LCD_NTIME(void)
{
  
  lcd_gotoxy(5,0);
  lcd_putsf("       ");  
  
  lcd_gotoxy(4,0);
  lcd_putchar(48+Nmen);  
  lcd_gotoxy(3,0);
  lcd_putchar(48+Nmti);
  lcd_gotoxy(2,0);
  lcd_putsf(":");     
  
  lcd_gotoxy(1,0);
  lcd_putchar(48+Nten);  
  lcd_gotoxy(0,0);
  lcd_putchar(48+Ntti);     

}

// External Interrupt 2 service routine
interrupt [EXT_INT2] void ext_int2_isr(void)
{    
 if(D_ON==1)
{   
 LYS =0; 
 ++Dx; 
 v2=(1500/D);
 v=v+v2;       
 
  for(h=0;h<v;)
 {        
     #asm("nop");      
    ++h;
 }  
 h=0;
 LYS = 1;    
 
    

 
 if(Dx==D) 
 {LYS=0;
  D_ON=0;
  Dx=0;
 }
 }   

     

++ms; 
if(ms==100)
{       
ms=0;
++sen;
if(sen==10) 
{sen=0;  
++sti;
}
if(sti==6) 
{sti=0;  
++men;
}
if(men==10) 
{men=0;  
++mti;
}
if(mti==6) 
{mti=0;  
++ten;
}
if(ten==10) 
{ten=0;  
++tti;      
}
if(tti==2 && ten==4) 
{ten=0;  
tti=0;
}   
//Dx=Dx + (7700/(D*60));
}

}       



// Declare your global variables here

void main(void)
{
// Declare your local variables here

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
DDRB=0x40;

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
// INT2: On
// INT2 Mode: Rising Edge
GICR|=0x20;
MCUCR=0x00;
MCUCSR=0x40;
GIFR=0x20;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// LCD module initialization
lcd_init(16);

// Global enable interrupts
#asm("sei")    
delay_ms(100);

while (1)
      {    
 
 // v=1500;        // LYSSTYRKE 7700 = min, 1 = max
  if(B1==0)
  { 
  ++menu;
  while(B1==0)
  delay_ms(10);
  }     
  

  
  if(menu==6) menu=0;         
  
              //-----------------------menu 1 start
  
  if(menu==1) 
  {    
  
   if(B2==0)
  {      
  ++men;
  sen=0;
  sti=0;     
  while(B2==0)
  delay_ms(10);
  }  
  if(men==10) 
  {men=0;  
  ++mti;
  if(mti==6) mti=0;  
  }           
  
   if(B3==0)
  {      
  ++ten;   
  sen=0;
  sti=0;
  while(B3==0)
  delay_ms(10);
  }  

  if(ten==10) 
  {ten=0;  
  ++tti; 
  }
  if(tti==2 && ten==4) 
  {ten=0;  
   tti=0;  
   }
  LCD_TIME();
  lcd_gotoxy(0,1);
  lcd_putsf("SET TIME");      
  }
               //-----------------------menu 1 slut
              //-----------------------menu 2 start
  
  if(menu==2) 
  {    
   if(B2==0)
  {      
  ++Omen;
  while(B2==0)
  delay_ms(10);
  }  
  if(Omen==10) 
  {Omen=0;  
  ++Omti;
  if(Omti==6) Omti=0;  
  }           
  
   if(B3==0)
  {      
  ++Oten;   
  while(B3==0)
  delay_ms(10);
  }  

  if(Oten==10) 
  {Oten=0;  
  ++Otti; 
  }
  if(Otti==2 && Oten==4) 
  {Oten=0;  
   Otti=0;  
   }  

 
  LCD_OTIME();
  lcd_gotoxy(0,1);
  lcd_putsf("LIGHT ON");      
  }
               //-----------------------menu 2 slut 
  
              //-----------------------menu 3 start
  
  if(menu==3) 
  {    
    if(B2==0)
  {      
  ++Nmen;
  while(B2==0)
  delay_ms(10);
  }  
  if(Nmen==10) 
  {Nmen=0;  
  ++Nmti;
  if(Omti==6) Omti=0;  
  }           
  
   if(B3==0)
  {      
  ++Nten;   
  while(B3==0)
  delay_ms(10);
  }  

  if(Nten==10) 
  {Nten=0;  
  ++Ntti; 
  }
  if(Ntti==2 && Nten==4) 
  {Nten=0;  
   Ntti=0;  
   }   

  LCD_NTIME();
  lcd_gotoxy(0,1);
  lcd_putsf("LIGHT OFF");      
  }
               //-----------------------menu 3 slut 
                //-----------------------menu 4 start
  
  if(menu==4) 
  {    
   if(B2==0)
  {      
  ++Den;
  while(B2==0)
  delay_ms(10);
  }  
  if(Den==10) 
  {Den=0;  
  ++Dti;  
  }
  if(Dti==10) Dti=0;  
      
   if(B3==0)
  {  
  Dx=0;    
  if (D_ON==0) { D_ON=1;
    }
 else  {   D_ON=0; 
    };

  while(B3==0)
  delay_ms(10);
  }   
  

  
  lcd_gotoxy(1,0);
  lcd_putchar(48+Den);  
  lcd_gotoxy(0,0);
  lcd_putchar(48+Dti);      
    
  if(D_ON==1)
  {   
  lcd_gotoxy(2,0);
  lcd_putsf(" min    ON ");    
  }            
  
    if(D_ON==0)
  {   
  lcd_gotoxy(2,0);
  lcd_putsf(" min    OFF");    
  }
  D=(Dti*10+Den)*6000;  
//  D=(7700/(D*6))/1000;

  lcd_gotoxy(0,1);
  lcd_putsf("DIMMER TIME");      
  }
               //-----------------------menu 4 slut         
              //-----------------------menu 5 start
  
  if(menu==5) 
  {    
  
   if(B2==0)
  {      
++v;  

  }           
  
   if(B3==0)
  {      
 --v;
 
  }  


  LCD_TIME();
  lcd_gotoxy(0,1);
  lcd_putsf("DIMMER        ");      
  }
               //-----------------------menu 5 slut               
               //-----------------------menu 0 start
  
  if(menu==0) 
  {           
  LCD_TIME();
  lcd_gotoxy(0,1);
  lcd_putsf("RUN         ");      
  }
               //-----------------------menu 0 slut 
  
  


  
      };
}



