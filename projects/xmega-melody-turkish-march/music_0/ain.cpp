// Mozart-Turkish march
// Written by OZHAN KD
// Output: PC7

#include <ioavr.h>
#include <intrinsics.h>
#define mi1 0
#define fa1 1
#define fa1_ 2
#define sol1 3
#define sol1_ 4
#define la1 5
#define la1_ 6
#define si1 7
#define do2 8
#define do2_ 9
#define re2 10
#define re2_ 11
#define mi2 12
#define fa2 13
#define fa2_ 14
#define sol2 15
#define sol2_ 16
#define la2 17
#define la2_ 18
#define si2 19
#define do3 20
#define do3_ 21
#define re3 22
#define re3_ 23
#define mi3 24
#define fa3 25
#define fa3_ 26
#define sol3 27
#define sol3_ 28
#define sil 29
#define d1 1
#define d2 2
#define d3 3
#define d4 4 
#define d6 6
#define d7 7
#define d8 8
__flash unsigned char notes[]={si1,d1,la1,d1,sol1_,d1,la1,d1,do2,d2,sil,d2,re2,d1,do2,d1,si1,d1,do2,d1,mi2,d2,
sil,d2,fa2,d1,mi2,d1,re2_,d1,mi2,d1,si2,d1,la2,d1,sol2_,d1,la2,d1,si2,d1,la2,d1,sol2_,d1,la2,d1,do3,d4,la2,d1,
sil,d1,do3,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,sol2,d1,sil,d1,la2,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,
sol2,d1,sil,d1,la2,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,sol2,d1,sil,d1,fa2_,d1,sil,d1,mi2,d4,

si1,d1,la1,d1,sol1_,d1,la1,d1,do2,d2,sil,d2,re2,d1,do2,d1,si1,d1,do2,d1,mi2,d2,
sil,d2,fa2,d1,mi2,d1,re2_,d1,mi2,d1,si2,d1,la2,d1,sol2_,d1,la2,d1,si2,d1,la2,d1,sol2_,d1,la2,d1,do3,d4,la2,d1,
sil,d1,do3,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,sol2,d1,sil,d1,la2,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,
sol2,d1,sil,d1,la2,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,sol2,d1,sil,d1,fa2_,d1,sil,d1,mi2,d4,

mi2,d1,sil,d1,fa2,d1,sil,d1,sol2,d1,sil,d1,sol2,d1,sil,d1,la2,d1,sol2,d1,fa2,d1,mi2,d1,re2,d2,sol1,d2,
mi2,d1,sil,d1,fa2,d1,sil,d1,sol2,d1,sil,d1,sol2,d1,sil,d1,la2,d1,sol2,d1,fa2,d1,mi2,d1,re2,d4,
do2,d1,sil,d1,re2,d1,sil,d1,mi2,d1,sil,d1,mi2,d1,sil,d1,fa2,d1,mi2,d1,re2,d1,do2,d1,si1,d2,mi1,d2,
do2,d1,sil,d1,re2,d1,sil,d1,mi2,d1,sil,d1,mi2,d1,sil,d1,fa2,d1,mi2,d1,re2,d1,do2,d1,si1,d2,si1,d1,sil,d1,

si1,d1,la1,d1,sol1_,d1,la1,d1,do2,d3,sil,d1,
re2,d1,do2,d1,si1,d1,do2,d1,mi2,d2,sil,d2,
fa2,d1,mi2,d1,re2_,d1,mi2,d1,si2,d1,la2,d1,sol2_,d1,la2,d1,
si2,d1,la2,d1,sol2_,d1,la2,d1,do3,d4,la2,d1,sil,d1,si2,d1,
sil,d1,do3,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,sol2_,d1,
sil,d1,la2,d1,sil,d1,mi2,d1,sil,d1,fa2,d1,sil,d1,re2,d1,
sil,d1,do2,d4,si1,d4,la1,d2,la1,d1,sil,d1,

la2,d1,sil,d1,si2,d1,sil,d1,do3_,d4,la2,d1,sil,d1,si2,d1,
sil,d1,do3_,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,sol2_,d1,
sil,d1,fa2_,d1,sil,d1,sol2_,d1,sil,d1,la2,d1,sil,d1,si2,d1,
sil,d1,sol2_,d1,sil,d1,mi2,d1,sil,d1,la2,d1,sil,d1,si2,d1,
sil,d1,do3_,d4,la2,d1,sil,d1,si2,d1,sil,d1,do3_,d1,sil,d1,
si2,d1,sil,d1,la2,d1,sil,d1,sol2_,d1,sil,d1,fa2_,d1,sil,d1,
si2,d1,sil,d1,sol2_,d1,sil,d1,mi2,d1,sil,d1,la2,d3,sil,d1,

la2,d1,sil,d1,si2,d1,sil,d1,do3_,d4,la2,d1,sil,d1,si2,d1,
sil,d1,do3_,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,sol2_,d1,
sil,d1,fa2_,d1,sil,d1,sol2_,d1,sil,d1,la2,d1,sil,d1,si2,d1,
sil,d1,sol2_,d1,sil,d1,mi2,d1,sil,d1,la2,d1,sil,d1,si2,d1,
sil,d1,do3_,d4,la2,d1,sil,d1,si2,d1,sil,d1,do3_,d1,sil,d1,
si2,d1,sil,d1,la2,d1,sil,d1,sol2_,d1,sil,d1,fa2_,d1,sil,d1,
si2,d1,sil,d1,sol2_,d1,sil,d1,mi2,d1,sil,d1,la2,d3,sil,d1,

do3_,d1,re3,d1,do3_,d1,si2,d1,la2,d1,si2,d1,
la2,d1,sol2_,d1,fa2_,d1,la2,d1,sol2_,d1,fa2_,d1,
fa2,d1,fa2_,d1,sol2_,d1,fa2,d1,do2_,d1,re2_,d1,
fa2,d1,do2_,d1,fa2_,d1,fa2,d1,fa2_,d1,sol2_,d1,
la2,d1,sol2_,d1,la2,d1,si2,d1,do3_,d1,do3,d1,
do3_,d1,do3,d1,do3_,d1,re3,d1,do3_,d1,si2,d1,la2,d1,si2,d1,
la2,d1,sol2_,d1,fa2_,d1,la2,d1,sol2_,d1,fa2_,d1,mi2,d1,fa2_,d1,
sol2_,d1,mi2,d1,do2_,d1,re2_,d1,mi2,d1,do2_,d1,re2_,d1,mi2,d1,
fa2_,d1,re2_,d1,do2,d1,do2_,d1,re2_,d1,do2,d1,do2_,d3,do2_,d1,

do3_,d1,re3,d1,do3_,d1,si2,d1,la2,d1,si2,d1,
la2,d1,sol2_,d1,fa2_,d1,la2,d1,sol2_,d1,fa2_,d1,
fa2,d1,fa2_,d1,sol2_,d1,fa2,d1,do2_,d1,re2_,d1,
fa2,d1,do2_,d1,fa2_,d1,fa2,d1,fa2_,d1,sol2_,d1,
la2,d1,sol2_,d1,la2,d1,si2,d1,do3_,d1,do3,d1,
do3_,d1,do3,d1,do3_,d1,re3,d1,do3_,d1,si2,d1,la2,d1,si2,d1,
la2,d1,sol2_,d1,fa2_,d1,la2,d1,sol2_,d1,fa2_,d1,mi2,d1,fa2_,d1,
sol2_,d1,mi2,d1,do2_,d1,re2_,d1,mi2,d1,do2_,d1,re2_,d1,mi2,d1,
fa2_,d1,re2_,d1,do2,d1,do2_,d1,re2_,d1,do2,d1,do2_,d3,do2_,d1,

mi2,d1,re2,d1,do2_,d1,si1,d1,la1,d1,si1,d1,do2_,d1,re2,d1,
mi2,d1,fa2_,d1,sol2_,d1,la2,d1,la2,d1,sol2_,d1,fa2_,d1,mi2,d1,
mi2,d1,re2,d1,do2_,d1,si1,d1,la1,d1,si1,d1,do2_,d1,re2,d1,
mi2,d1,fa2_,d1,sol2_,d1,la2,d1,la2_,d2,si2,d1,sil,d1,mi2,d1,
re2,d1,do2_,d1,si1,d1,la1,d1,si1,d1,do2_,d1,re2,d1,mi2,d1,
fa2_,d1,sol2_,d1,la2,d1,la2,d1,sol2_,d1,fa2_,d1,mi2,d1,mi2,d1,
re2,d1,do2_,d1,si1,d1,do2_,d1,mi2,d1,la1,d1,do2_,d1,si1,d1, 
re2,d1,sol1_,d1,si1,d1,la1,d4,do3_,d1,re3,d1,do3_,d1,si2,d1,
la2,d1,si2,d1,la2,d1,sol2_,d1,fa2_,d1,la2,d1,sol2_,d1,fa2_,d1,
fa2,d1,fa2_,d1,sol2_,d1,fa2,d1,do2_,d1,re2_,d1,fa2,d1,do2_,d1,
fa2_,d1,fa2,d1,fa2_,d1,sol2_,d1,la2,d1,sol2_,d1,la2,d1,si2,d1,
do3_,d1,do3,d1,do3_,d1,do3,d1,do3_,d1,do3,d1,do3_,d1,la2_,d1,
re3,d1,do3_,d1,re3,d1,do3_,d1,re3,d1,do3_,d1,re3,d1,do3_,d1,
re3,d1,do3_,d1,si2,d1,la2,d1,sol2_,d1,la2,d1,si2,d1,sol2_,d1,
la2,d1,si2,d1,do3_,d1,fa2_,d1,fa2,d1,fa2_,d1,sol2_,d1,fa2,d1,
fa2_,d3,fa2_,d1,

la2,d1,sil,d1,si2,d1,sil,d1,do3_,d4,la2,d1,sil,d1,si2,d1,
sil,d1,do3_,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,sol2_,d1,
sil,d1,fa2_,d1,sil,d1,sol2_,d1,sil,d1,la2,d1,sil,d1,si2,d1,
sil,d1,sol2_,d1,sil,d1,mi2,d1,sil,d1,la2,d1,sil,d1,si2,d1,
sil,d1,do3_,d4,la2,d1,sil,d1,si2,d1,sil,d1,do3_,d1,sil,d1,
si2,d1,sil,d1,la2,d1,sil,d1,sol2_,d1,sil,d1,fa2_,d1,sil,d1,
si2,d1,sil,d1,sol2_,d1,sil,d1,mi2,d1,sil,d1,la2,d3,sil,d1,

si1,d1,la1,d1,sol1_,d1,la1,d1,do2,d2,sil,d2,re2,d1,do2,d1,si1,d1,do2,d1,mi2,d2,
sil,d2,fa2,d1,mi2,d1,re2_,d1,mi2,d1,si2,d1,la2,d1,sol2_,d1,la2,d1,si2,d1,la2,d1,sol2_,d1,la2,d1,do3,d4,la2,d1,
sil,d1,do3,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,sol2,d1,sil,d1,la2,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,
sol2,d1,sil,d1,la2,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,sol2,d1,sil,d1,fa2_,d1,sil,d1,mi2,d4,
//
si1,d1,la1,d1,sol1_,d1,la1,d1,do2,d2,sil,d2,re2,d1,do2,d1,si1,d1,do2,d1,mi2,d2,
sil,d2,fa2,d1,mi2,d1,re2_,d1,mi2,d1,si2,d1,la2,d1,sol2_,d1,la2,d1,si2,d1,la2,d1,sol2_,d1,la2,d1,do3,d4,la2,d1,
sil,d1,do3,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,sol2,d1,sil,d1,la2,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,
sol2,d1,sil,d1,la2,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,sol2,d1,sil,d1,fa2_,d1,sil,d1,mi2,d4,
//
mi2,d1,sil,d1,fa2,d1,sil,d1,sol2,d1,sil,d1,sol2,d1,sil,d1,la2,d1,sol2,d1,fa2,d1,mi2,d1,re2,d2,sol1,d2,
mi2,d1,sil,d1,fa2,d1,sil,d1,sol2,d1,sil,d1,sol2,d1,sil,d1,la2,d1,sol2,d1,fa2,d1,mi2,d1,re2,d4,
do2,d1,sil,d1,re2,d1,sil,d1,mi2,d1,sil,d1,mi2,d1,sil,d1,fa2,d1,mi2,d1,re2,d1,do2,d1,si1,d2,mi1,d2,
do2,d1,sil,d1,re2,d1,sil,d1,mi2,d1,sil,d1,mi2,d1,sil,d1,fa2,d1,mi2,d1,re2,d1,do2,d1,si1,d2,si1,d1,sil,d1,
//
si1,d1,la1,d1,sol1_,d1,la1,d1,do2,d3,sil,d1,
re2,d1,do2,d1,si1,d1,do2,d1,mi2,d2,sil,d2,
fa2,d1,mi2,d1,re2_,d1,mi2,d1,si2,d1,la2,d1,sol2_,d1,la2,d1,
si2,d1,la2,d1,sol2_,d1,la2,d1,do3,d4,la2,d1,sil,d1,si2,d1,
sil,d1,do3,d1,sil,d1,si2,d1,sil,d1,la2,d1,sil,d1,sol2_,d1,
sil,d1,la2,d1,sil,d1,mi2,d1,sil,d1,fa2,d1,sil,d1,re2,d1,
sil,d1,do2,d4,si1,d4,la1,d2,la1,d1,sil,d1,
};
volatile unsigned char duration;
__flash unsigned int TCC1_values[]={1804,1703,1607,1517,1432,1351,1276,1204,1136,1073,1012,955,901,851,803,758,
715,676,638,602,568,536,506,478,451,426,402,379,358}; //..
volatile bool note_f=false;

void main()
{
 unsigned int i; 
 PORTC.DIR=PIN7_bm;
 PORTC.OUTSET=PIN7_bm;
 TCC0_PER=192;
 TCC1_PER=1204; // si1
 TCC0_INTCTRLA=TC0_OVFINTLVL0_bm;
 TCC1_INTCTRLA=TC0_OVFINTLVL0_bm;
 TCC0_CTRLA=TC_CLKSEL_DIV1024_gc;
 TCC1_CTRLA=TC_CLKSEL_DIV1_gc;
 PMIC.CTRL=PMIC_LOLVLEN_bm;
 __enable_interrupt();
 
 for(i=0;i<sizeof (notes);i+=2)
 { 
  if (notes[i]!=sil)
  {    
   note_f=true;      
   duration=notes[i+1];           
   TCC1.PERBUF=TCC1_values[notes[i]]-1;         
  }
   
  else
  {        
   note_f=false;
   duration=notes[i+1];          
  }    
  while(duration);
 }
 while(1);
} 

#pragma vector= TCC0_OVF_vect
__interrupt void TCC0_isr(void)
{
 if (duration!=0) duration--;
}

#pragma vector = TCC1_OVF_vect
__interrupt void TCC1_isr(void)
{
 if (note_f==true)
 {
  PORTC.OUTTGL=PIN7_bm;
 }
}
