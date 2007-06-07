
/****************************************************************************
 Title  :   C  file for the SRF04 FUNCTIONS library (srf04.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      7/Jan/2003
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/
/* Although i could had written it in the new style, i prefer the old one for compatibility sake. */

#ifndef _IO_REG_MACRO_MODE_
#define _IO_REG_MACRO_MODE_  1     /* In case you have the new assignment mode io headers */
#endif

#ifndef  _SFR_ASM_COMPAT
#define  _SFR_ASM_COMPAT     1     /* This is for GCC 3.2 */
#endif

#ifndef AVRGCC_VERSION
#define AVRGCC_VERSION       330        /* AVRGCC version for including the correct files  */
#endif

#if AVRGCC_VERSION == 330

#include <avr/io.h>
#include <avr/signal.h>

#else

#include <io.h>
#include <sig-avr.h>

#endif

#include "srf04.h"














#if !defined(outp)
#define outp(value,port)  outb(port,value)
#endif

#if !defined(inp)
#define inp(port)  inb(port)
#endif

#if defined(__AVR_ATmega103__)
#define DDRC   0
#define DDRF   0
#endif

#ifndef CONCAT1
#define CONCAT1(a, b) CONCAT2(a, b)
#endif

#ifndef CONCAT2
#define CONCAT2(a, b) a ## b
#endif


#define SRF04_UNIT_0_OUT_REG       CONCAT1(PORT, SRF04_UNIT_0_PORT)
#define SRF04_UNIT_0_DDR_REG       CONCAT1(DDR,  SRF04_UNIT_0_PORT)
#define SRF04_UNIT_0_PIN_REG       CONCAT1(PIN,  SRF04_UNIT_0_PORT)

#define SRF04_WILL_USE_ONE_PORT    1

#if SRF04_UNITS_USED >= 2

#ifndef IO_TO_MEM_OFFSET
#if SREG > 0X3F
#define  IO_TO_MEM_OFFSET  0x00
#else
#define  IO_TO_MEM_OFFSET  0x20
#endif
#endif


#define SRF04_UNIT_1_OUT_REG       CONCAT1(PORT, SRF04_UNIT_1_PORT)
#define SRF04_UNIT_1_DDR_REG       CONCAT1(DDR, SRF04_UNIT_1_PORT)
#define SRF04_UNIT_1_PIN_REG       CONCAT1(PIN, SRF04_UNIT_1_PORT)

#if SRF04_UNIT_1_OUT_REG != SRF04_UNIT_0_OUT_REG
#undef  SRF04_WILL_USE_ONE_PORT
#define SRF04_WILL_USE_ONE_PORT    0
#endif

#endif

#if SRF04_UNITS_USED >= 3

#define SRF04_UNIT_2_OUT_REG       CONCAT1(PORT, SRF04_UNIT_2_PORT)
#define SRF04_UNIT_2_DDR_REG       CONCAT1(DDR, SRF04_UNIT_2_PORT)
#define SRF04_UNIT_2_PIN_REG       CONCAT1(PIN, SRF04_UNIT_2_PORT)

#if ( (SRF04_UNIT_2_OUT_REG != SRF04_UNIT_1_OUT_REG) && SRF04_WILL_USE_ONE_PORT == 1 )
#undef  SRF04_WILL_USE_ONE_PORT
#define SRF04_WILL_USE_ONE_PORT    0
#endif

#endif

#if SRF04_UNITS_USED >= 4

#define SRF04_UNIT_3_OUT_REG       CONCAT1(PORT, SRF04_UNIT_3_PORT)
#define SRF04_UNIT_3_DDR_REG       CONCAT1(DDR, SRF04_UNIT_3_PORT)
#define SRF04_UNIT_3_PIN_REG       CONCAT1(PIN, SRF04_UNIT_3_PORT)

#if ( (SRF04_UNIT_3_OUT_REG != SRF04_UNIT_2_OUT_REG) && SRF04_WILL_USE_ONE_PORT == 1 )
#undef  SRF04_WILL_USE_ONE_PORT
#define SRF04_WILL_USE_ONE_PORT    0
#endif

#endif

/* Auto Settings */

#if SRF04_WILL_USE_ONE_PORT == 1

#undef IO_TO_MEM_OFFSET
#undef SRF04_UNIT_0_OUT_REG
#undef SRF04_UNIT_0_DDR_REG
#undef SRF04_UNIT_0_PIN_REG

#if SRF04_UNITS_USED >= 2
#undef SRF04_UNIT_1_OUT_REG
#undef SRF04_UNIT_1_DDR_REG
#undef SRF04_UNIT_1_PIN_REG
#endif

#if SRF04_UNITS_USED >= 3
#undef SRF04_UNIT_2_OUT_REG
#undef SRF04_UNIT_2_DDR_REG
#undef SRF04_UNIT_2_PIN_REG
#endif

#if SRF04_UNITS_USED >= 4
#undef SRF04_UNIT_3_OUT_REG
#undef SRF04_UNIT_3_DDR_REG
#undef SRF04_UNIT_3_PIN_REG
#endif

#define SRF04_OUT_REG      CONCAT1(PORT, SRF04_UNIT_0_PORT)
#define SRF04_DDR_REG      CONCAT1(DDR,  SRF04_UNIT_0_PORT)
#define SRF04_PIN_REG      CONCAT1(PIN,  SRF04_UNIT_0_PORT)


#define ECHO_MASK_1  (1<<SRF04_UNIT_0_ECHO_PIN)
#define PING_MASK_1  (1<<SRF04_UNIT_0_PING_PIN)


#define ECHO_MASK_2  (1<<SRF04_UNIT_1_ECHO_PIN)
#define PING_MASK_2  (1<<SRF04_UNIT_1_PING_PIN)

#define ECHO_MASK_3  (1<<SRF04_UNIT_2_ECHO_PIN)
#define PING_MASK_3  (1<<SRF04_UNIT_2_PING_PIN)

#define ECHO_MASK_4  (1<<SRF04_UNIT_3_ECHO_PIN)
#define PING_MASK_4  (1<<SRF04_UNIT_3_PING_PIN)



#if   SRF04_UNITS_USED == 1

#define ECHO_MASK    (1<<SRF04_UNIT_0_ECHO_PIN)
#define PING_MASK    (1<<SRF04_UNIT_0_PING_PIN)

#elif SRF04_UNITS_USED == 2

#define ECHO_MASK    ((1<<SRF04_UNIT_0_ECHO_PIN)|(1<<SRF04_UNIT_1_ECHO_PIN))
#define PING_MASK    ((1<<SRF04_UNIT_0_PING_PIN)|(1<<SRF04_UNIT_1_PING_PIN))

#elif SRF04_UNITS_USED == 3

#define ECHO_MASK    ( (1<<SRF04_UNIT_0_ECHO_PIN)|(1<<SRF04_UNIT_1_ECHO_PIN)|(1<<SRF04_UNIT_2_ECHO_PIN) )
#define PING_MASK    ( (1<<SRF04_UNIT_0_PING_PIN)|(1<<SRF04_UNIT_1_PING_PIN)|(1<<SRF04_UNIT_2_PING_PIN) )
 
#elif SRF04_UNITS_USED == 4

#define ECHO_MASK    ( (1<<SRF04_UNIT_0_ECHO_PIN)|(1<<SRF04_UNIT_1_ECHO_PIN)| \
                       (1<<SRF04_UNIT_2_ECHO_PIN)|(1<<SRF04_UNIT_3_ECHO_PIN) )

#define PING_MASK    ( (1<<SRF04_UNIT_0_PING_PIN)|(1<<SRF04_UNIT_1_PING_PIN)| \
                       (1<<SRF04_UNIT_2_PING_PIN)|(1<<SRF04_UNIT_3_PING_PIN) )


#endif                        

#endif  /* #if SRF04_WILL_USE_ONE_PORT == 1 */

/*******************************************************************************************************/
/* SOME ERROR CHECKING WHEN USING MEGA 103 */
#if SRF04_WILL_USE_ONE_PORT == 1

#if defined(__AVR_ATmega103__)
#if SRF04_DDR_REG == 0
#error "SORRY THE SRF04 PORT MUST! BE BIDIRECTIONAL"
#endif
#endif /* #if defined(__AVR_ATmega103__) */

#elif SRF04_WILL_USE_ONE_PORT == 0

#if defined(__AVR_ATmega103__)

#if SRF04_UNIT_0_DDR_REG == 0  
#error "SORRY THE SRF04 UNIT 0 PORT MUST! BE BIDIRECTIONAL"
#endif

#if   SRF04_UNITS_USED >= 2
#if SRF04_UNIT_1_DDR_REG == 0  
#error "SORRY THE SRF04 UNIT 1 PORT MUST! BE BIDIRECTIONAL"
#endif
#endif

#if   SRF04_UNITS_USED >= 3
#if SRF04_UNIT_2_DDR_REG == 0  
#error "SORRY THE SRF04 UNIT 2 PORT MUST! BE BIDIRECTIONAL"
#endif
#endif

#if   SRF04_UNITS_USED >= 4
#if SRF04_UNIT_3_DDR_REG == 0  
#error "SORRY THE SRF04 UNIT 3 PORT MUST! BE BIDIRECTIONAL"
#endif
#endif

#endif /* #if defined(__AVR_ATmega103__) */

#endif /* #elif SRF04_WILL_USE_ONE_PORT == 0 */
/*******************************************************************************************************/


#define DELAY_50_MS         ((50000*(F_CPU/60000))/100)-3
#define DELAY_30_MS         ((30000*(F_CPU/60000))/100)-3
#define DELAY_1_MS          ((1000*(F_CPU/60000))/100)-3
#define DELAY_50_US         ((50*(F_CPU/60000))/100)-3

#if DELAY_50_US < 0
#undef DELAY_50_US
#define DELAY_50_US   0
#endif

#define TIMER0_PRESCALER      8
#define SET_PRESCALER         (1<<CS01)

#define CONVERSION_FACTOR     ( ((TIMER0_PRESCALER*SPEED_OF_SOUND*100)/F_CPU) )




/* global variables */
volatile union word2byte {
                             volatile unsigned char byte[2];
                             volatile unsigned int  word;
                         }timer0;

                     


/*######################################################################################################*/
static void delay(unsigned long us)
{
/* 
   6 cpu cycles per loop + 17 cycles overhead(18 are pre-subtracted in the definition of DELAY).
   (4 to load the us variable, 4 to call the delay function, 5 to test for us == 0 and 4 to return. 
*/
   while ( us ) { us--; }  

return; 
}
/*######################################################################################################*/
#if SRF04_UNITS_USED == 1
#warning " ONE SRF04 UNIT IS USED "
void srf04_init(void)
{

   /* CONFIGURE SRF04_PING_PIN AS OUTPUT AND SRF04_ECHO_PIN AS INPUT */
   sbi(SRF04_DDR_REG, SRF04_UNIT_0_PING_PIN);
   cbi(SRF04_DDR_REG, SRF04_UNIT_0_ECHO_PIN);

   /* RESET SRF04_PING_PIN (SRF04_PING_PIN = 0) */
   cbi(SRF04_OUT_REG, SRF04_UNIT_0_PING_PIN);

   /* PULLUP RESISTOR ACTIVATED */
   sbi(SRF04_OUT_REG, SRF04_UNIT_0_ECHO_PIN);

   /* ENABLE TIMER 0 OVERFLOW INTERRUPT */
   outp((1<<TOIE0),TIMSK);
   asm("sei");

   

   /* DELAY IN CASE THE SRF04 PINGED ACCIDENTALLY */
   delay(DELAY_50_MS);
   

return;
}
/*######################################################################################################*/

unsigned int srf04_ping(void)
{
unsigned int  range=0;

/* STOP AND RESET TIMER 0 */
outp(0,TCCR0);
outp(0,TCNT0);
timer0.word=0;

/* GIVE A PULSE TO START RANGING */
sbi(SRF04_OUT_REG, SRF04_UNIT_0_PING_PIN);
delay(DELAY_50_US);
cbi(SRF04_OUT_REG, SRF04_UNIT_0_PING_PIN);

/* WAIT UNTIL THE RISING EDGE IN THE OUTPUT PIN IS DETECTED  */
loop_until_bit_is_set(SRF04_PIN_REG, SRF04_UNIT_0_ECHO_PIN);

/* START TIMER 0 PRESCALER SET TO 64 */
outp(SET_PRESCALER, TCCR0);

/* WAIT UNTIL THE FALLING EDGE IN THE OUTPUT PIN IS DETECTED */
loop_until_bit_is_clear(SRF04_PIN_REG, SRF04_UNIT_0_ECHO_PIN);

/* STOP TIMER 0 */
outp(0,TCCR0);

timer0.byte[0]=inp(TCNT0);
/* For F_CPU = 3686400, SET_PRESCALER = 64, SPEED_OF_SOUND = 347,26 m/sec & Temperature = 26 C : 
   Each count is 17,36 us and sound will travel 6,03 mm.
   since the sound traveled twice the required to measure distance :
   dinstance(cm) = ((counts/2) * 6,03)/10 ~= ((counts/2) * 60)/100   
*/

/* DELAY IN ORDER FOR THE SRF04 TO RECHARGE ITS SUPPLY CAPACITORS AND AVOID LONG ECHOS */
delay(DELAY_30_MS);

range=( ((((unsigned long)timer0.word/2)*CONVERSION_FACTOR)/1000) );
if(range>300) { return(300); } 

return(range);
}


#elif SRF04_UNITS_USED > 1

#warning " MULTIPLE SRF04 UNITS ARE USED "

#if SRF04_WILL_USE_ONE_PORT == 0

#warning " MULTIPLE SRF04 UNITS ON VARIUS PORTS "

unsigned char srf04_ddr_reg  = SRF04_UNIT_0_DDR_REG;
unsigned char srf04_pin_reg  = SRF04_UNIT_0_PIN_REG;
unsigned char srf04_out_reg = SRF04_UNIT_0_OUT_REG;
unsigned char srf04_ping_pin = SRF04_UNIT_0_PING_PIN;
unsigned char srf04_echo_pin = SRF04_UNIT_0_ECHO_PIN;


#define inpvar(addr)               (*((unsigned char*)(addr+IO_TO_MEM_OFFSET)))

#define outpvar(value,addr)        { (*((unsigned char*)(addr+IO_TO_MEM_OFFSET))) = value; }

#define sbivar(addr,pin)           { (*((unsigned char*)(addr+IO_TO_MEM_OFFSET))) |= (1<<pin); }

#define cbivar(addr,pin)           { (*((unsigned char*)(addr+IO_TO_MEM_OFFSET))) &= (~(1<<pin)); }

#define bit_is_set_var(addr,pin)   ( *((unsigned char*)(addr+IO_TO_MEM_OFFSET)) & (1<<pin) )

#define bit_is_clear_var(addr,pin) ( !(*((unsigned char*)(addr+IO_TO_MEM_OFFSET)) & (1<<pin)) )


/*####################################################################################################*/

static void srf04_select(unsigned char unit)
{

if(unit == SRF04_0) 
 {
     srf04_ddr_reg  = SRF04_UNIT_0_DDR_REG;
     srf04_pin_reg  = SRF04_UNIT_0_PIN_REG;
     srf04_out_reg  = SRF04_UNIT_0_OUT_REG;
     srf04_ping_pin = SRF04_UNIT_0_PING_PIN;
     srf04_echo_pin = SRF04_UNIT_0_ECHO_PIN;
 }
else if(unit == SRF04_1)    
      {
          srf04_ddr_reg  = SRF04_UNIT_1_DDR_REG;
          srf04_pin_reg  = SRF04_UNIT_1_PIN_REG;
          srf04_out_reg  = SRF04_UNIT_1_OUT_REG;
          srf04_ping_pin = SRF04_UNIT_1_PING_PIN;
          srf04_echo_pin = SRF04_UNIT_1_ECHO_PIN;
      } 
#if SRF04_UNITS_USED > 2
else if(unit == SRF04_2)    
      {
          srf04_ddr_reg  = SRF04_UNIT_2_DDR_REG;
          srf04_pin_reg  = SRF04_UNIT_2_PIN_REG;
          srf04_out_reg  = SRF04_UNIT_2_OUT_REG;
          srf04_ping_pin = SRF04_UNIT_2_PING_PIN;
          srf04_echo_pin = SRF04_UNIT_2_ECHO_PIN;
      } 
#endif
#if SRF04_UNITS_USED > 3
else if(unit == SRF04_3)    
      {
          srf04_ddr_reg  = SRF04_UNIT_3_DDR_REG;
          srf04_pin_reg  = SRF04_UNIT_3_PIN_REG;
          srf04_out_reg  = SRF04_UNIT_3_OUT_REG;
          srf04_ping_pin = SRF04_UNIT_3_PING_PIN;
          srf04_echo_pin = SRF04_UNIT_3_ECHO_PIN;
      } 
#endif

return;
}
/*####################################################################################################*/

void srf04_init(void)
{
unsigned char unit=0;

for(unit=1; unit <= SRF04_UNITS_USED; unit++)  
  {
     srf04_select(unit); 
     /* CONFIGURE SRF04_PING_PIN AS OUTPUT AND SRF04_ECHO_PIN AS INPUT */
     sbivar(srf04_ddr_reg, srf04_ping_pin);
     cbivar(srf04_ddr_reg, srf04_echo_pin);

     /* RESET SRF04_PING_PIN (SRF04_PING_PIN = 0) */
     cbivar(srf04_out_reg, srf04_ping_pin);

     /* PULLUP RESISTOR ACTIVATED */
     sbivar(srf04_out_reg, srf04_echo_pin);


     /* ENABLE TIMER 0 OVERFLOW INTERRUPT */
     outp((1<<TOIE0),TIMSK);
     asm("sei");


     /* DELAY IN CASE THE SRF04 PINGED ACCIDENTALLY */
     delay(DELAY_50_MS);
  } 

return;
}
/*######################################################################################################*/

unsigned int srf04_ping(unsigned char srf04_unit)
{
unsigned int range=0;

/* select the desired srf04 unit */
srf04_select(srf04_unit);

/* STOP AND RESET TIMER 0 */
outp(0,TCCR0);
outp(0,TCNT0);
timer0.word=0;



/* GIVE A PULSE TO START RANGING */
sbivar(srf04_out_reg, srf04_ping_pin);
delay(DELAY_50_US);
cbivar(srf04_out_reg, srf04_ping_pin);

/* START TIMER 0 PRESCALER SET TO 64  */
outp(SET_PRESCALER, TCCR0);

/* WAIT UNTIL THE RISING EDGE IN THE OUTPUT PIN IS DETECTED  */
while(bit_is_clear_var(srf04_pin_reg, srf04_echo_pin));

/* WAIT UNTIL THE FALLING EDGE IN THE OUTPUT PIN IS DETECTED  */
while(bit_is_set_var(srf04_pin_reg, srf04_echo_pin));

/* STOP TIMER 0 */
outp(0,TCCR0);

timer0.byte[0]=inp(TCNT0);
/* For F_CPU = 3686400, SET_PRESCALER = 64, SPEED_OF_SOUND = 347,26 m/sec & Temperature = 26 C : 
   Each count is 17,36 us and sound will travel 6,03 mm.
   since the sound traveled twice the required to measure distance :
   dinstance(cm) = ((counts/2) * 6,03)/10 ~= ((counts/2) * 60)/100   
*/


/* DELAY IN ORDER FOR THE SRF04 TO RECHARGE ITS SUPPLY CAPACITORS AND AVOID LONG ECHOS */
delay(DELAY_30_MS);


range=( ((((unsigned long)timer0.word/2)*CONVERSION_FACTOR)/1000) );
if(range>300) { return(300); }

return(range);
}

/*######################################################################################################*/
#elif SRF04_WILL_USE_ONE_PORT == 1
#warning " MULTIPLE SRF04 UNITS ON THE SAME PORT "
/*######################################################################################################*/

void srf04_init(void)
{
  
/* CONFIGURE THE PING PINS AS OUTPUTS AND THE ECHO PINS AS INPUTS WITH PULL UP RESISTORS ACTIVATED */

   outp((inp(SRF04_DDR_REG)|PING_MASK), SRF04_DDR_REG );
   outp((inp(SRF04_OUT_REG)|ECHO_MASK),  SRF04_OUT_REG);

   /* ENABLE TIMER 0 OVERFLOW INTERRUPT */
   outp((1<<TOIE0),TIMSK);
   asm("sei");

   /* DELAY IN CASE THE SRF04 PINGED ACCIDENTALLY */
   delay(DELAY_50_MS);
   

return;
}
/*######################################################################################################*/

unsigned int srf04_ping(unsigned char srf04_unit)
{
unsigned int  range=0;
unsigned char echo_mask=0;
unsigned char ping_mask=0;

if(srf04_unit == SRF04_ALL)    { echo_mask=ECHO_MASK;    ping_mask=PING_MASK;   }
else if(srf04_unit == SRF04_0) { echo_mask=ECHO_MASK_1;  ping_mask=PING_MASK_1; }
else if(srf04_unit == SRF04_1) { echo_mask=ECHO_MASK_2;  ping_mask=PING_MASK_2; }
else if(srf04_unit == SRF04_2) { echo_mask=ECHO_MASK_3;  ping_mask=PING_MASK_3; }
else if(srf04_unit == SRF04_3) { echo_mask=ECHO_MASK_4;  ping_mask=PING_MASK_4; }


/* STOP AND RESET TIMER 0 */
outp(0,TCCR0);
outp(0,TCNT0);
timer0.word=0;

/* GIVE A HIGH-LOW PULSE TO THE PING PINS */
/* SWITCH ON THE PING PINS LEAVING ECHO PINS PULLED HIGH */
outp((inp(SRF04_OUT_REG)|ping_mask), SRF04_OUT_REG );
delay(DELAY_50_US);
/* SWITCH OFF THE PING PINS LEAVING ECHO PINS PULLED HIGH */
outp((inp(SRF04_OUT_REG)&(~(ping_mask))), SRF04_OUT_REG );


/* WAIT UNTIL THE FIRST RISING EDGE IN THE OUTPUT PIN OF THE SRF04 UNIT(S) IS DETECTED  */
while( (inp(SRF04_PIN_REG) & echo_mask) != echo_mask );

/* START TIMER 0 PRESCALER SET TO 64 */
outp(SET_PRESCALER, TCCR0);

/* WAIT UNTIL THE FIRST! FALLING EDGE OF ANY SRF04 UNIT IS DETECTED (SHORTEST DISTANCE) */
while( (inp(SRF04_PIN_REG) & echo_mask) == echo_mask );

/* STOP TIMER 0 */
outp(0,TCCR0);

timer0.byte[0]=inp(TCNT0);
/* For F_CPU = 3686400, SET_PRESCALER = 64, SPEED_OF_SOUND = 347,26 m/sec & Temperature = 26 C : 
   Each count is 17,36 us and sound will travel 6,03 mm.
   since the sound traveled twice the required to measure distance :
   dinstance(cm) = ((counts/2) * 6,03)/10 ~= ((counts/2) * 60)/100   
*/


/* DELAY IN ORDER FOR THE SRF04 TO RECHARGE ITS SUPPLY CAPACITORS AND AVOID LONG ECHOS */
delay(DELAY_30_MS);
range=( ((((unsigned long)timer0.word/2)*CONVERSION_FACTOR)/1000) );
if(range>300) { return(300); }

return(range);
}


#endif  /* #if SRF04_UNITS_USED == 1 -> #elif SRF04_UNITS_USED > 1 */

/*######################################################################################################*/
#endif /* #if SRF04_WILL_USE_ONE_PORT == 0 -> #elif SRF04_WILL_USE_ONE_PORT == 1 */
/*######################################################################################################*/


/* INTERRUPT SERVICE ROUTINE */
SIGNAL(SIG_OVERFLOW0)
{
timer0.byte[1]++;

return;
}


/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

       

