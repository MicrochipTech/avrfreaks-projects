
/****************************************************************************
 Title	:   Header file for the DELAY FUNCTIONS library (delay.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:	    26/Oct/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/


#ifndef DELAY_H
#define DELAY_H	1

/*######################################################################################################*/
/*                             START OF CONFIGURATION BLOCK                                             */
/*######################################################################################################*/
#ifndef  F_CPU
#define  F_CPU		              3686400   /* The cpu clock frequency in Hertz */
#endif
#define  USE_DELAY_MACROS             1         /* 1=enable macros, 0=no macros are defined */
#define  USE_DELAY_FUNCTIONS          1         /* 1=enable functions, 0=no functions are included */

/*######################################################################################################*/
/*                              END OF CONFIGURATION BLOCK                                              */
/*######################################################################################################*/

#if  USE_DELAY_MACROS == 1 

#define TIME_L1_MS    ( 1*(F_CPU/6000) )    /* MUST USE A LONG FOR COUNTING TO BE ACCURATE  */
#define TIME_S1_MS    ( 1*(F_CPU/4000) ) 

#define DELAY_MS(ms)  DELAY_LONG((TIME_L1_MS*ms))
#define delay_ms(ms)  delay_long((TIME_L1_MS*ms))

#define DELAY_US(us)  DELAY_SHORT( ((TIME_S1_MS*us)/1000) )
#define delay_us(us)  delay_short( ((TIME_S1_MS*us)/1000) )


/* 
   IF YOU USE THE ACCURATE DELAY MACROS MAKE SURE THAT THE VALUE PASSED IS NOT ZERO
   OTHERWISE BAD THINGS WILL HAPPEN TO YOU. 
*/

/* 3 cpu cycles per loop when a constant is passed.*/
#define DELAY_C_ACCURATE(x)      ({  __asm__ volatile (  "ldi r25,%0 \n\t"           \
                                                         "L_%=:      \n\t"           \
                                                         "dec r25    \n\t"           \
                                                         "brne L_%=  \n\t"           \
                                                         : /* NO OUTPUT */           \
                                                         : "g" ((unsigned char)(x))  \
		                                         : "r25"	             \
           	                                       );                            \
                                  })                                                     

/* 4 cpu cycles per loop + 1 overhead when a constant is passed. */
#define DELAY_S_ACCURATE(x)  ({ unsigned short number_of_loops=(unsigned short)x; \
                              	__asm__ volatile ( "L_%=:           \n\t"         \
                                                   "sbiw %A0,1      \n\t"         \
                                                   "brne L_%=       \n\t"         \
                                                   : /* NO OUTPUT */              \
                                                   : "w" (number_of_loops)        \
                                                 );                               \
                             })                                     

/* 6 cpu cycles per loop + 3 overhead when a constant is passed. */
#define DELAY_L_ACCURATE(x)  ({ unsigned long number_of_loops=(unsigned long)x;   \
                                __asm__ volatile ( "L_%=: \n\t"                   \
                                                   "subi %A0,lo8(-(-1)) \n\t"     \
                                                   "sbci %B0,hi8(-(-1)) \n\t"     \
                                                   "sbci %C0,hlo8(-(-1)) \n\t"    \
                                                   "sbci %D0,hhi8(-(-1)) \n\t"    \
                                                   "brne L_%= \n\t"               \
                                                   : /* NO OUTPUT */              \
                                                   : "w" (number_of_loops)        \
                                                 );                               \
                             })                                        



/* 3 cpu cycles per loop  + 2 overhead when a constant is passed. */
#define DELAY_CHAR(x)     ({  unsigned char number_of_loops=(unsigned char)x;    \
                              __asm__ volatile ( "cp  %A0,__zero_reg__ \n\t"     \
                                                 "breq L_EXIT_%=       \n\t"     \
                                                 "L_%=:                \n\t"     \
                                                 "dec %A0              \n\t"     \
                                                 "brne L_%=            \n\t"     \
                                                 "L_EXIT_%=:           \n\t"     \
                                                 : /* NO OUTPUT */               \
                                                 : "r" (number_of_loops)         \
		                               );                                \
                          })                                        

/* 4 cpu cycles per loop +4 overhead when a constant is passed.  */
#define DELAY_SHORT(x)    ({  unsigned short number_of_loops=(unsigned short)x;  \
                              __asm__ volatile ( "cp  %A0,__zero_reg__ \n\t"     \
                                                 "cpc %B0,__zero_reg__ \n\t"     \
                                                 "breq L_EXIT_%=       \n\t"     \
                                                 "L_%=:                \n\t"     \
                                                 "sbiw r24,1           \n\t"     \
                                                 "brne L_%=            \n\t"     \
                                                 "L_EXIT_%=:           \n\t"     \
                                                 : /* NO OUTPUT */               \
                                                 : "w" (number_of_loops)         \
                                               );                                \
                          })                                     


/* 6 cpu cycles per loop +12 overhead when a constant is passed.  */
#define DELAY_LONG(x)     ({ unsigned long number_of_loops=(unsigned long)x;     \
                              __asm__ volatile ( "cp  %A0,__zero_reg__ \n\t"     \
                                                 "cpc %B0,__zero_reg__ \n\t"     \
                                                 "cpc %C0,__zero_reg__ \n\t"     \
                                                 "cpc %D0,__zero_reg__ \n\t"     \
                                                 "breq L_EXIT_%=       \n\t"     \
                                                 "L_%=:                \n\t"     \
                                                 "subi %A0,lo8(-(-1))  \n\t"     \
                                                 "sbci %B0,hi8(-(-1))  \n\t"     \
                                                 "sbci %C0,hlo8(-(-1)) \n\t"     \
                                                 "sbci %D0,hhi8(-(-1)) \n\t"     \
                                                 "brne L_%=            \n\t"     \
                                                 "L_EXIT_%=:           \n\t"     \
                                                 : /* NO OUTPUT */               \
                                                 : "w" (number_of_loops)         \
                                               );                                \
                          })                                        


#endif  /* #if  USE_DELAY_MACROS == 1  */

#if  USE_DELAY_FUNCTIONS == 1
void delay_char(unsigned char number_of_loops) 
{
/* 3 cpu cycles per loop + 10 cycles overhead when a constant is passed. */
  __asm__ volatile ( "cp  %A0,__zero_reg__ \n\t"  \
                     "breq L_EXIT_%=       \n\t"  \
                     "L_%=:                \n\t"  \
                     "dec %A0              \n\t"  \
                     "brne L_%=            \n\t"  \
                     "L_EXIT_%=:           \n\t"  \
                     : /* NO OUTPUT */            \
                     : "r" (number_of_loops)      \
                   );                            
                                            
return;
}

 
void delay_short(unsigned short number_of_loops) 
{
/* 4 cpu cycles per loop + 12 cycles overhead when a constant is passed. */
  __asm__ volatile ( "cp  %A0,__zero_reg__ \n\t"  \
                     "cpc %B0,__zero_reg__ \n\t"  \
                     "breq L_EXIT_%=       \n\t"  \
                     "L_%=:                \n\t"  \
                     "sbiw r24,1           \n\t"  \
                     "brne L_%=            \n\t"  \
                     "L_EXIT_%=:           \n\t"  \
                     : /* NO OUTPUT */            \
                     : "w" (number_of_loops)      \
                   );                            

                                            
return;
}

void delay_long(unsigned long number_of_loops) 
{
/* 6 cpu cycles per loop + 20 cycles overhead when a constant is passed. */
  __asm__ volatile ( "cp  %A0,__zero_reg__ \n\t"  \
                     "cpc %B0,__zero_reg__ \n\t"  \
                     "cpc %C0,__zero_reg__ \n\t"  \
                     "cpc %D0,__zero_reg__ \n\t"  \
                     "breq L_EXIT_%=       \n\t"  \
                     "L_%=:                \n\t"  \
                     "subi %A0,lo8(-(-1))  \n\t"  \
                     "sbci %B0,hi8(-(-1))  \n\t"  \
                     "sbci %C0,hlo8(-(-1)) \n\t"  \
                     "sbci %D0,hhi8(-(-1)) \n\t"  \
                     "brne L_%=            \n\t"  \
                     "L_EXIT_%=:           \n\t"  \
                     : /* NO OUTPUT */            \
                     : "w" (number_of_loops)      \
                   );                             \
                                              
return;
}
#endif  /* #if  USE_DELAY_FUNCTIONS == 1 */

#endif



 
