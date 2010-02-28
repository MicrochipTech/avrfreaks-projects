/*****************************************************
Project                 : Time Machine
Version                 : 0.0
Date                    : 15/04/2007
Author                  : Ehab Anwar                            
Company                 : BDR Electronics
                        : flik_80@yahoo.com
                        : +20107697744
///////////////////////////////////////////////////                           
Comments                : There is no god but Allah
///////////////////////////////////////////////////
Chip type               : ATmega8L
Clock frequency         : 8.000000 MHz
*****************************************************/

#include <mega8.h>
#include <delay.h>
#include <bcd.h>

#define SCLK  PORTB.6
#define CE    PORTD.5 

#define  prog_sw  PIND.6
#define  incr_sw  PIND.7


unsigned char sec_=0x25,min_=0x55,hr_=0x10;


char code[10]={
               0xfc,//0
               0xc0,//1
               0xb6,//2
               0xe6,//3
               0xca,//4
               0x6e,//5
               0x7e,//6
               0xcc,//7
               0xfe,//8
               0xee //9
              };

bit hold_prog_sw,hold_incr_sw;

/////////////////////////
void ds1302_reset()    //
 {                     //
 SCLK = 0;             //
 CE = 0;               // 
 delay_us(1);          //
 CE = 1;               // 
 delay_us(1);          //
 }                     //
/////////////////////////

//////////////////////////////////////////////
void ds1302_wbyte(char data)	            //
{                                           //
char a;                                     //
DDRB|=0x80;//IO=output                      //
for(a=0;a<8;++a)                            //
 {                                          //
 PORTB.7=(0x01&(data>>a));                  //
 SCLK = 0;                                  // 
 delay_us(1);                               //
 SCLK = 1;                                  // 
 delay_us(1);                               //
 }                                          //
}                                           //
//////////////////////////////////////////////

//////////////////////////////////////////////
/* --- read one byte from the device --- */ //
char ds1302_rbyte(void)	                    //
{                                           //
char a;                                     //
char data=0;                                //
DDRB&=0x7f;//IO=input                       //
for(a=0;a<8;a++)                            //
 {                                          //
 SCLK = 1;                                  // 
 delay_us(1);                               //
 SCLK = 0;                                  // 
 delay_us(1);                               //
 if(PINB.7)                                 //
  data|=((char)1)<<a;                       //
 }                                          //
return data;                                //
}                                           //
//////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/* --- write one byte using values entered by user --- */  //
void ds1302_writebyte(char ClkAdd,char ClkData)	           //
{                                                          //
ds1302_reset();                                            //
ds1302_wbyte(ClkAdd);                                      //
ds1302_wbyte(ClkData);                                     //
ds1302_reset();                                            //
}                                                          //
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/* --- initialize time & date from user entries ---  */        //
void write_clk_regs(char sec,char min,char hr)                 //
{                                                              //
ds1302_reset();                                                //
ds1302_writebyte(0x8e,0); /* control register */               //
ds1302_reset();                                                //
ds1302_writebyte(0x90,0xab);/* trickle charger register */     //
ds1302_reset();                                                //
ds1302_wbyte(0xbe); /* clock burst write (eight registers) */  //
ds1302_wbyte(sec);                                             //
ds1302_wbyte(min);                                             //
ds1302_wbyte(hr);                                              //
ds1302_wbyte(1);                                               //
ds1302_wbyte(1);                                               //
ds1302_wbyte(1);                                               //
ds1302_wbyte(07);                                              //
ds1302_wbyte(0x80);//must write control register in burst mode //
ds1302_reset();                                                //
}                                                              //
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////
void read_clk_regs(char *sec,char *min,char *hr) //
 {                                               //
 ds1302_reset();                                 //
 ds1302_wbyte(0xBF);	/* clock burst */        //
 *sec=ds1302_rbyte();                            //
 *min=ds1302_rbyte();                            //
 *hr=ds1302_rbyte();                             //
 ds1302_reset();                                 //
 }                                               //
 //////////////////////////////////////////////////
 
///////////////////////////////
 void set_data(char data)    //
  {                          //
  PORTC.0=(data>>7)&1;//a    //
  PORTC.2=(data>>6)&1;//b    //
  PORTD.0=(data>>5)&1;//c    //
  PORTD.1=(data>>4)&1;//d    //
  PORTC.3=(data>>3)&1;//e    //
  PORTC.1=(data>>2)&1;//f    //
  PORTC.5=(data>>1)&1;//g    //
  PORTC.4=(data>>0)&1;//dot  //
  }                          //
 //////////////////////////////
 
 //////////////////////////////////////////ok
void show_time(int a,int b,int c,char d) //
{                                        //
char x;                                  //
int y;                                   //
for(x=0;x<d;x++)                         //
 {                                       //
 set_data(code[sec_&0x0f]);              //
 PORTB.4=1;                              //
 for(y=0;y<a;y++)                        //
  {delay_us(1);}                         //
 PORTB.4=0;                              //
                                         //
 set_data(code[(sec_&0xf0)>>4]);         //
 PORTB.3=1;                              //
 for(y=0;y<a;y++)                        //
  {delay_us(1);}                         //
 PORTB.3=0;                              //
                                         //
 set_data(code[min_&0x0f]);              //
 PORTB.1=1;                              //
 for(y=0;y<b;y++)                        //
  {delay_us(1);}                         //
 PORTB.1=0;                              //
                                         //
 set_data(code[(min_&0xf0)>>4]);         //
 PORTB.0=1;                              //
 for(y=0;y<b;y++)                        //
  {delay_us(1);}                         //
 PORTB.0=0;                              // 
                                         //
 set_data(code[hr_&0x0f]);               //
 PORTB.2=1;                              //
 for(y=0;y<c;y++)                        //
  {delay_us(1);}                         //
 PORTB.2=0;                              //
                                         //
 set_data(code[(hr_&0xf0)>>4]);          //
 PORTB.5=1;                              //
 for(y=0;y<c;y++)                        //
  {delay_us(1);}                         //
 PORTB.5=0;                              //
 }                                       //
                                         //
}                                        //
/////////////////////////////////////////// 

/////////////////////////////////////// 
void set_time(void)                  //
{                                    //
char time[3],a;                      //
char time_max[3]={12,59,59};         //
char time_min[3]={1,0,0};            //
int time_illumnas[7]={               //
                      900,50         //
                     ,50,800         //
                     ,50,50          //
                     ,900            //
                    };               //
read_clk_regs(                       //
              &time[2]               //
             ,&time[1]               //
             ,&time[0]               //
             );                      //
time[2]&=0x7f;                       //
time[1]&=0x7f;                       //
time[0]&=0x1f;                       //
                                     //
time[2]=bcd2bin(time[2]);            //
time[1]=bcd2bin(time[1]);            //
time[0]=bcd2bin(time[0]);            //
                                     //
for(a=0;a<3;a++)                     //
 {                                   //
 while(1)                            //
  {                                  //
///////////////////////////////////////
  hold_prog_sw=prog_sw;              //
  hold_incr_sw=incr_sw;              //
  sec_=0x7f&bin2bcd(time[2]);             //
  min_=0x7f&bin2bcd(time[1]);             //
  hr_=0x1f&bin2bcd(time[0]);              //
  show_time(
             time_illumnas[(char)(a*2)+2]
            ,time_illumnas[(char)(a*2)+1]
            ,time_illumnas[(char)(a*2)]
            ,25
           );                        //
///////////////////////////////////////
  if(prog_sw!=hold_prog_sw)          //
   {                                 //
   hold_prog_sw=prog_sw;             //
   if(hold_prog_sw==0)               //
    break;                           //
   }                                 //
/////////////////////////////////////// 
  if(incr_sw!=hold_incr_sw)          //
   {                                 //
   hold_incr_sw=incr_sw;             //
   while(incr_sw==0)                 //
    {                                //
    hold_incr_sw=incr_sw;            //
    if(time[a]>=time_max[a])         //
     time[a]=time_min[a];            //
    else                             //
     ++time[a];                      //
///////////////////////////////////////
    sec_=0x7f&bin2bcd(time[2]);             //
    min_=0x7f&bin2bcd(time[1]);             //
    hr_=0x1f&bin2bcd(time[0]);              //
    show_time(
               time_illumnas[(char)(a*2)+2]
              ,time_illumnas[(char)(a*2)+1]
              ,time_illumnas[(char)(a*2)]
              ,25
             );                        //
///////////////////////////////////////
    }                                //
   }                                 //
  }                                  //
 }                                   //
write_clk_regs(bin2bcd(time[2])      //
              ,bin2bcd(time[1])      //
              ,0x80|bin2bcd(time[0]) //
              );                     //
}                                    //
///////////////////////////////////////

//Timer 1 output compare A interrupt service routine
interrupt [TIM1_COMPA] void timer1_compa_isr(void)
{
// Place your code here

}

// Declare your global variables here

void main(void)
{
// Declare your local variables here

 unsigned char _sec=60;


// Input/Output Ports initialization
// Port B initialization
// Func7=In Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=P State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTB=0x80;
DDRB=0x7F;

// Port C initialization
// Func6=In Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State6=T State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTC=0x80;
DDRC=0x3F;

// Port D initialization
// Func7=in Func6=in Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTD=0xc0;
DDRD=0x3f;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
TCCR0=0x00;
TCNT0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 7.813 kHz
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: On
// Compare B Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x05;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x1E;
OCR1AL=0x85;
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
MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x10;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// Global enable interrupts
#asm("sei") 
//write_clk_regs(0,0x18,06);
while (1)
      {
       hold_prog_sw=prog_sw;
      /////////////////////////////////////////////////
      read_clk_regs(&sec_,&min_,&hr_);
      _sec =sec_;
      sec_&=0x7f;
      min_&=0x7f;
      hr_&=0x1f;
      show_time(80,80,80,10);
      /////////////////////////////////////////////////
      if(hold_prog_sw!=prog_sw)
       {
       delay_ms(1);
       if(prog_sw==0)
        set_time();
       }
      };
}
