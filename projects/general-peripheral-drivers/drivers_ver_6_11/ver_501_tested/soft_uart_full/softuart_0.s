/****************************************************************************
 Title  :   S file for the SOFTWARE UART FUNCTIONS library (softuart.s)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      10/Nov/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.


*****************************************************************************/

/*################### DO NOT CHANGE ANYTHING BELOW THIS LINE !!! #####################################*/
/*############## ALL CONFIGURATION IS DONE IN "softuart.cfg" file !!! ################################*/

#ifndef _IO_REG_MACRO_MODE_
#define _IO_REG_MACRO_MODE_  1     /* In case you have the new assignment mode io headers */
#endif

#ifndef  _SFR_ASM_COMPAT
#define  _SFR_ASM_COMPAT     1     /* This is for GCC 3.2 */
#endif  

#ifndef AVRGCC_VERSION
#define AVRGCC_VERSION       330        /* AVRGCC version for including the correct files  */
#endif

#ifndef _VECTOR                    
#define _VECTOR(N) __vector_ ## N  /* This is for GCC 3.2 and up */
#endif


#if   AVRGCC_VERSION == 330
#include <avr/io.h>
#elif AVRGCC_VERSION == 320
#include <io.h>
#else
#include <io-avr.h>
#endif

#include "softuart_0.cfg"



/*------------------------------------------------------------------------------------------------------*/
/* Warning messages */
#if SUART_0_TIMEOUT_MODE_CHANGE == 1
#warning " NO TIMEOUT IN INTERRUPT MODE "
#endif

#if    SUART_0_STOP_BIT_DETECTION > 2
#error " SUART_0_STOP_BIT_DETECTION CAN BE 0,1 OR 2 "
#endif

/*------------------------------------------------------------------------------------------------------*/

#if   SUART_0_WILL_USE_A_TIMER == 0 && SUART_0_DYNAMIC_BAUD_CHANGE == 0

#if    SUART_0_DELAY1 <=2 || SUART_0_DELAY2 <= 3 || SUART_0_DELAY3 <= 3 
#error " BAUDRATE TOO HIGH "
#elif  SUART_0_DELAY1 > 255 || SUART_0_DELAY2 > 255 || SUART_0_DELAY3 > 255
#error " BAUDRATE TOO LOW USE A TIMER "
#endif

#elif SUART_0_WILL_USE_A_TIMER == 1  && SUART_0_DYNAMIC_BAUD_CHANGE == 0

#if   SUART_0_TIME_HALF_BIT <= 0 || SUART_0_TIME_1_BIT <= 0 || SUART_0_BAUDRATE_DELAY < (SUART_0_CORRECTION_FACTOR*2)
#error " BAUDRATE TOO HIGH "
#endif

#endif /* #elif SUART_0_WILL_USE_A_TIMER == 1  && SUART_0_DYNAMIC_BAUD_CHANGE == 0 */

/*------------------------------------------------------------------------------------------------------*/

#if SUART_0_DYNAMIC_BAUD_CHANGE == 0

#if   (SUART_0_AVAILABLE_CPU_CYCLES < (RX_PROCESSING_CODE_SIZE*2)) 

#warning " NOT ENOUGH TIME TO PROCESS RX DATA "

#if    SUART_0_STOP_BIT_DETECTION == 0 || SUART_0_STOP_BIT_DETECTION == 1
#warning " REDUCE BAUDRATE OR INCREASE # OF STOP BITS "
#elif  SUART_0_STOP_BIT_DETECTION == 2
#warning " REDUCE BAUDRATE "
#endif

#endif /* #if   (SUART_0_AVAILABLE_CPU_CYCLES < (RX_PROCESSING_CODE_SIZE*2))  */

#endif /* #if SUART_0_DYNAMIC_BAUD_CHANGE == 0 */

/*------------------------------------------------------------------------------------------------------*/

#if defined(__AVR_ATmega103__)

#undef  DDRC 
#undef  DDRF 
#undef  PINC 
#undef  PORTF

#define DDRC   0
#define DDRF   0
#define PINC   0
#define PORTF  0

#if SUART_0_RX_PIN_REG == 0 
#error " THE SUART RX PIN IS OUTPUT ONLY "
#endif

#if SUART_0_TX_OUT_REG == 0 
#error " THE SUART TX PIN IS INPUT ONLY "
#endif

#if SUART_0_RX_PIN_REG == PINF
#undef  SUART_0_RX_PORT_IS_IO
#define SUART_0_RX_PORT_IS_IO    0
#endif

#if SUART_0_TX_OUT_REG == PORTC
#undef  SUART_0_TX_PORT_IS_IO
#define SUART_0_TX_PORT_IS_IO    0
#endif

#endif


/*------------------------------------------------------------------------------------------------------*/

/* CALCULATE THE I/O REGISTER ADDRESSES */
#if SREG > 0X3F 

#define S0_TX_DDR_REG  SUART_0_TX_DDR_REG   - 0X20
#define S0_TX_PORT     SUART_0_TX_OUT_REG   - 0X20
#define S0_RX_DDR_REG  SUART_0_RX_DDR_REG   - 0X20
#define S0_RX_PIN_REG  SUART_0_RX_PIN_REG   - 0X20
#define S0_INT_REG     SUART_0_INT_REG      - 0X20
#define S0_TCNT        SUART_0_TCNT         - 0X20
#define S0_TCCR        SUART_0_TCCR         - 0X20
#define S0_SREG        SREG                 - 0X20
#define S0_EIFR        SUART_0_INT_FLAG_REG - 0X20

#elif SREG <= 0X3F  

#define S0_TX_DDR_REG  SUART_0_TX_DDR_REG 
#define S0_TX_PORT     SUART_0_TX_OUT_REG    
#define S0_RX_DDR_REG  SUART_0_RX_DDR_REG 
#define S0_RX_PIN_REG  SUART_0_RX_PIN_REG 
#define S0_INT_REG     SUART_0_INT_REG 
#define S0_TCNT        SUART_0_TCNT 
#define S0_TCCR        SUART_0_TCCR       
#define S0_SREG        SREG 
#define S0_EIFR        SUART_0_INT_FLAG_REG 

#endif  

/*------------------------------------------------------------------------------------------------------*/

/* REGISTER DEFINITIONS */
#define          s0_z_high     r31 
#define          s0_z_low      r30          
#define          s0_index      r27
#define          s0_temp2      r27          
#define          s0_temp1      r26 
#define          s0_sreg       r25
#define          s0_Rxbyte     r24 
#define          s0_Txbyte     r24          
#define          s0_bitcnt     r23


/*#######################################################################################################*/

                 .global SUART_0_PUTCHAR
                 .func SUART_0_PUTCHAR

SUART_0_PUTCHAR:   

#if SUART_0_DYNAMIC_IO_SETTINGS == 1
                   clr      s0_z_high                    /* cbi using variables */
                   lds      s0_z_low,  suart_0_port
#endif
              
                   ldi      s0_bitcnt,9+SUART_0_STOP_BITS
                   com      s0_Txbyte                  /* INVERT THE BYTE TO BE TRANSMITED */
                   sec                                /* SET CARRY (CARRY=1) */

#if    SUART_0_USE_RS232_LOGIC == 0 
suart_0_putchar0:  brcc     suart_0_putchar1            /* IF CARRY IS CLEAR SET THE TX PIN TO 1 */
#elif  SUART_0_USE_RS232_LOGIC == 1
suart_0_putchar0:  brcs     suart_0_putchar1            /* IF CARRY IS SET SET THE TX PIN TO 1 */
#endif 

#if SUART_0_DYNAMIC_IO_SETTINGS == 1
                   ld       s0_temp1, z
                   lds      s0_temp2, suart_0_tx_pin       
                   com      s0_temp2 
                   and      s0_temp1, s0_temp2
                   st       z, s0_temp1                     
                   rjmp     suart_0_putchar2          
                                                    /* sbi using variables */
suart_0_putchar1:  ld       s0_temp1, z
                   lds      s0_temp2, suart_0_tx_pin 
                   or       s0_temp1, s0_temp2
                   st       z, s0_temp1           
                   nop
                   nop

#elif SUART_0_DYNAMIC_IO_SETTINGS == 0
                   cbi      S0_TX_PORT,SUART_0_TX_PIN    /* ELSE SET THE TX PIN TO 1 */
                   rjmp     suart_0_putchar2          

suart_0_putchar1:  sbi      S0_TX_PORT,SUART_0_TX_PIN         
                   nop
#endif
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if SUART_0_WILL_USE_A_TIMER == 0  &&  SUART_0_DYNAMIC_BAUD_CHANGE == 1                
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

suart_0_putchar2:  lds      s0_temp1, suart_0_delay2     /* 1/2 bit delay loop */
                   subi     s0_temp1, 1                /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */ 
                   nop                                /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */ 
suart_0_tx_delay1: dec      s0_temp1
                   brne     suart_0_tx_delay1
                 
                   lds      s0_temp1, suart_0_delay3     /* 1/2 bit delay loop. total delay 1 bit*/
                   subi     s0_temp1, 1                /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */  
                   nop                                /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */ 
suart_0_tx_delay2: dec      s0_temp1
                   brne     suart_0_tx_delay2

/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif SUART_0_WILL_USE_A_TIMER == 0  &&  SUART_0_DYNAMIC_BAUD_CHANGE == 0 
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

suart_0_putchar2:  ldi      s0_temp1, SUART_0_DELAY2     /* 1/2 bit delay loop */       
suart_0_tx_delay1: dec      s0_temp1                   /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */
                   brne     suart_0_tx_delay1

                   ldi      s0_temp1, SUART_0_DELAY3     /* 1/2 bit delay loop */ 
suart_0_tx_delay2: dec      s0_temp1                   /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */
                   brne     suart_0_tx_delay2           

/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif SUART_0_WILL_USE_A_TIMER == 1  
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

suart_0_putchar2:  clr      s0_temp1                   /* 1 bit delay loop using a timer */
                   out      S0_TCNT, s0_temp1
                                                  
#if   SUART_0_DYNAMIC_BAUD_CHANGE == 0
                   ldi     s0_temp2, SUART_0_TIME_1_BIT
#elif SUART_0_DYNAMIC_BAUD_CHANGE == 1
                   lds     s0_temp2, suart_0_time_1_bit
#endif 
                                 
suart_0_tx_delay1: in       s0_temp1, S0_TCNT
                   cp       s0_temp1, s0_temp2
                   brlo     suart_0_tx_delay1

/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

                   lsr      s0_Txbyte                  /* SHIFT LEFT TX BYTE THROUGH CARRY */
                   dec      s0_bitcnt                  /* DECREMENT BITCOUNT */
                   brne     suart_0_putchar0            /* TRANSMIT NEXT BIT  */
                                      
                   ret                             


.endfunc
                
/*####################################################################################################*/
/*####################################################################################################*/
#if SUART_0_DYNAMIC_IO_SETTINGS == 0
/*####################################################################################################*/
/*####################################################################################################*/

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if   SUART_0_WILL_USE_INTERRUPT  == 1   /* INTERRUPT MODE FOR NON DYNAMIC PORT-PIN RECEIVING */
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/


                 .global SUART_0_RX_FUNCTION
                 .func   SUART_0_RX_FUNCTION

SUART_0_RX_FUNCTION: 

/*#############    It took at least 5 cycles to get here ###############*/

                   push    s0_sreg                     /* SAVE THE SREG REGISTER FIRST */
                   in      s0_sreg, S0_SREG

/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if SUART_0_WILL_USE_A_TIMER == 0  &&  SUART_0_DYNAMIC_BAUD_CHANGE == 1                    
/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

                   push    s0_temp1                    /* SAVE THE REGISTERS TO BE USED */
                   push    s0_temp2
                   push    s0_z_low
                   push    s0_z_high
                   push    s0_Rxbyte                    
                   push    s0_bitcnt 

                   ldi     s0_bitcnt, 8                /* STOP BIT IS EATEN AND USED FOR RX DATA PROCESSING */

/********************************* TIMING  CODE BLOCK START ***********************************************/
              
suart_0_getchar1:  lds     s0_temp1, suart_0_delay1      /* 1/2 bit delay loop to be counted only once */
suart_0_rx_delay1: dec     s0_temp1                     
                   brne    suart_0_rx_delay1                    
                                                
suart_0_getchar2:  lds     s0_temp1, suart_0_delay2      /* 1/2 bit delay loop */
                   subi    s0_temp1, 1                 /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */   
                   nop                                /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */   
suart_0_rx_delay2: dec     s0_temp1                     
                   brne    suart_0_rx_delay2
                 
                   lds     s0_temp1, suart_0_delay3      /* 1/2 bit delay loop */
                   subi    s0_temp1, 1                 /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */  
                   nop                                /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */   
suart_0_rx_delay3: dec     s0_temp1                     
                   brne    suart_0_rx_delay3                           

/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/ 
#elif SUART_0_WILL_USE_A_TIMER == 0  &&  SUART_0_DYNAMIC_BAUD_CHANGE == 0
/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

                   push    s0_temp1 
                   push    s0_temp2                       
                   push    s0_z_low                     
                   push    s0_z_high                    
                   push    s0_Rxbyte                    
                   push    s0_bitcnt 

                   ldi     s0_bitcnt, 8                /* STOP BIT IS EATEN AND USED FOR RX DATA PROCESSING */
               
suart_0_getchar1:  ldi     s0_temp1, SUART_0_DELAY1      /* 1/2 bit delay loop to be counted only once */
suart_0_rx_delay1: dec     s0_temp1                    /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */ 
                   brne    suart_0_rx_delay1                    
                                                
suart_0_getchar2:  ldi     s0_temp1, SUART_0_DELAY2      /* 1/2 bit delay loop */
suart_0_rx_delay2: dec     s0_temp1                    /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */ 
                   brne    suart_0_rx_delay2
     
                   ldi     s0_temp1, SUART_0_DELAY3      /* 1/2 bit delay loop */   
suart_0_rx_delay3: dec     s0_temp1                    /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */ 
                   brne    suart_0_rx_delay3                                 

/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/                                                        
#elif SUART_0_WILL_USE_A_TIMER == 1  
/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

                   push    s0_temp1                               
                   clr     s0_temp1                    /* 1/2 bit delay loop to be counted only once */
                   out     S0_TCNT, s0_temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */
                   push    s0_temp2
                   push    s0_z_low
                   push    s0_z_high
                   push    s0_Rxbyte
                   push    s0_bitcnt 

                   ldi     s0_bitcnt, 8                /* STOP BIT IS EATEN AND USED FOR RX DATA PROCESSING */   
#if   SUART_0_DYNAMIC_BAUD_CHANGE == 0
                   ldi     s0_temp2, SUART_0_TIME_HALF_BIT
#elif SUART_0_DYNAMIC_BAUD_CHANGE == 1
                   lds     s0_temp2, suart_0_time_half_bit
#endif

/***************   Dead time end   ************/
suart_0_rx_delay1: in      s0_temp1, S0_TCNT
                   cp      s0_temp1, s0_temp2
                   brlo    suart_0_rx_delay1
            
suart_0_getchar2:  clr      s0_temp1                   /* 1 bit delay loop using a timer */
                   out      S0_TCNT, s0_temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */ 
#if   SUART_0_DYNAMIC_BAUD_CHANGE == 0
                   ldi     s0_temp2, SUART_0_TIME_1_BIT
#elif SUART_0_DYNAMIC_BAUD_CHANGE == 1
                   lds     s0_temp2, suart_0_time_1_bit
#endif 
/***************   Dead time end   ************/                              
suart_0_rx_delay2: in       s0_temp1, S0_TCNT
                   cp       s0_temp1, s0_temp2
                   brlo     suart_0_rx_delay2

/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/                                                
#endif  /* #if SUART_0_WILL_USE_A_TIMER == 0  &&  SUART_0_DYNAMIC_BAUD_CHANGE == 1  */
/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

/********************************* TIMING  CODE BLOCK END *************************************************/

/**************************  MAIN DATA HADLING CODE BLOCK START *******************************************/
                   clc                               /* CLEAR CARRY */                

#if    SUART_0_USE_RS232_LOGIC == 0                  
                   sbic    S0_RX_PIN_REG, SUART_0_RX_PIN /* IF RX PIN IS LOW CLEAR CARRY */             
#elif  SUART_0_USE_RS232_LOGIC == 1
                   sbis    S0_RX_PIN_REG, SUART_0_RX_PIN /* IF RX PIN IS HIGH CLEAR CARRY */             
#endif


                   sec
suart_0_rotate:    ror     s0_Rxbyte                  /* ROTATE RIGHT THE RX BYTE THROUGH CARRY */      
                   dec     s0_bitcnt                  /* DECREMENT BITCOUNT */
                   breq    suart_0_getchar3            /* IF THIS WAS THE LAST BIT WRITE BYTE TO BUFFER */
                                                   /* ELSE */                                               
                   rjmp    suart_0_getchar2            /* GO GET NEXT BIT */ 

suart_0_getchar3:

/**************************  MAIN DATA HADLING CODE BLOCK END *******************************************/

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if SUART_0_WILL_USE_A_TIMER == 0 && SUART_0_STOP_BIT_DETECTION == 2 
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#if    SUART_0_DYNAMIC_BAUD_CHANGE == 0

                   ldi     s0_bitcnt, SUART_0_STOP_BITS    /* load the number of stop bits */
              
suart_0_getchar4:  ldi     s0_temp1, SUART_0_DELAY2        /* 1 bit delay */
suart_0_rx_delay4: dec     s0_temp1
                   brne    suart_0_rx_delay4

                   ldi     s0_temp1, SUART_0_DELAY3      
suart_0_rx_delay5: dec     s0_temp1
                   brne    suart_0_rx_delay5

                   dec     s0_bitcnt                    /* If this was the last stop bit continue */
                   brne    suart_0_getchar4                  /* else apply 1 more bit delay             */

#elif    SUART_0_DYNAMIC_BAUD_CHANGE == 1

                   ldi     s0_bitcnt, SUART_0_STOP_BITS    /* load the number of stop bits */
              
suart_0_getchar4:  lds     s0_temp1, suart_0_delay2        /* 1/2 bit delay */
                   dec     s0_temp1
                   nop
suart_0_rx_delay4: dec     s0_temp1
                   brne    suart_0_rx_delay4

                   lds     s0_temp1, suart_0_delay3        /* 1 more 1/2 bit delay total 1 bit */
                   dec     s0_temp1
                   nop
suart_0_rx_delay5: dec     s0_temp1
                   brne    suart_0_rx_delay5

                   dec     s0_bitcnt                    /* If this was the last stop bit continue */
                   brne    suart_0_getchar4                  /* else apply 1 more bit delay             */

#endif

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif SUART_0_WILL_USE_A_TIMER == 1 && SUART_0_STOP_BIT_DETECTION == 2  
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

                   ldi     s0_bitcnt, SUART_0_STOP_BITS
            
suart_0_getchar4:  clr     s0_temp1                     /* 1 bit delay loop using a timer */
                   out     S0_TCNT, s0_temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */ 
#if    SUART_0_DYNAMIC_BAUD_CHANGE == 0
                   ldi     s0_temp2, SUART_0_TIME_1_BIT 
#elif  SUART_0_DYNAMIC_BAUD_CHANGE == 1
                   lds     s0_temp2, suart_0_time_1_bit
#endif
/***************   Dead time end   ************/ 
suart_0_rx_delay4: in      s0_temp1, S0_TCNT
                   cp      s0_temp1, s0_temp2
                   brlo    suart_0_rx_delay4

                   dec     s0_bitcnt
                   brne    suart_0_getchar4

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/



/* ######### RECEIVING IS COMPLETE SO LETS START WRITTING THE BUFFER ############## */
/* ######### RX DATA PROCESSING FOR NON DYNAMIC PORT-PIN MODE        ############## */
suart_0_wr_buf_0:  sts     rx_c, r24                           
                   lds     s0_index, suart_0_rx_count

                   lds     s0_temp1, suart_0_receive_string
                   cpi     s0_temp1, 1
                   brne    suart_0_wr_buf_1             
                

                   cpi     s0_Rxbyte, SUART_0_RX_TERMINATION_CHAR          
                   breq    suart_0_term_recv0     

suart_0_wr_buf_1:  cpi     s0_index, SUART_0_BUF_SIZE     
                   breq    suart_0_buf_reset                         

suart_0_wr_buf_2:  ldi     s0_z_low, lo8(suart_0_rx_buffer)
                   ldi     s0_temp1, hi8(suart_0_rx_buffer)         
                   add     s0_z_low, s0_index              
                   adc     s0_z_high,s0_temp1                       
suart_0_wr_buf_3:  st      Z+, s0_Rxbyte                          
                   inc     s0_index                      
                   clr     s0_temp1                              
                   st      Z, s0_temp1                            
                              
exit_suart_0_0:    lds     s0_temp1, suart_0_set_mark
                   tst     s0_temp1
                   dec     s0_temp1
                   breq    suart_0_term_recv1
                   sts     suart_0_set_mark, s0_temp1


exit_suart_0_1:    sts     suart_0_rx_count, s0_index
#if         SUART_0_INT_EDGE_TRIGERING == 1 
                   ldi     s0_temp1, (1<<SUART_0_INT_FLAG)
                   out     S0_EIFR, s0_temp1
                   ;sts    (S0_EIFR+0x20), s0_temp1
#endif
                   pop     s0_bitcnt
                   pop     s0_Rxbyte                               
                   pop     s0_z_high      
                   pop     s0_z_low
                   pop     s0_temp2
                   pop     s0_temp1
                   out     S0_SREG, s0_sreg
                   pop     s0_sreg

#if SUART_0_STOP_BIT_DETECTION == 0 || SUART_0_STOP_BIT_DETECTION == 1
#if    SUART_0_USE_RS232_LOGIC == 0 
exit_suart_0_2:    sbis    S0_RX_PIN_REG, SUART_0_RX_PIN
                   rjmp    exit_suart_0_2                        
#elif  SUART_0_USE_RS232_LOGIC == 1 
exit_suart_0_2:    sbic    S0_RX_PIN_REG, SUART_0_RX_PIN
                   rjmp    exit_suart_0_2         
#endif
#endif
                   reti                                        

suart_0_term_recv0:inc     s0_index
suart_0_term_recv1:in      s0_temp1, S0_INT_REG                       
                   andi    s0_temp1, (~(1<<SUART_0_INT_BIT))                   
                   out     S0_INT_REG, s0_temp1                     
                   clr     s0_temp1
                   sts     suart_0_set_mark, s0_temp1
                   sts     suart_0_string_received, s0_index  
                   rjmp    exit_suart_0_1

suart_0_buf_reset: ldi     s0_index, 0xFF
                   sts     suart_0_set_mark, s0_index
                   clr     s0_index                     
                   ldi     s0_z_low,  lo8(suart_0_rx_buffer)        
                   ldi     s0_z_high, hi8(suart_0_rx_buffer)        
                   rjmp    suart_0_wr_buf_3 



.endfunc


/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif SUART_0_WILL_USE_INTERRUPT  == 0  /* NON INTERRUPT MODE FOR NON DYNAMIC PORT-PIN RECEIVING */
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

                 .global SUART_0_RX_FUNCTION
                 .func   SUART_0_RX_FUNCTION

SUART_0_RX_FUNCTION: 

/************************** WATCH FOR A PROPER START BIT CODE BLOCK START ********************************/

#if    SUART_0_TIMEOUT_ENABLED == 1
                   ldi  r24, lo8(SUART_0_TIMEOUT_LOOPS)
                   ldi  r25, hi8(SUART_0_TIMEOUT_LOOPS)
                   ldi  r26, hlo8(SUART_0_TIMEOUT_LOOPS)
                   ldi  r27, hhi8(SUART_0_TIMEOUT_LOOPS)
#endif

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if    SUART_0_USE_RS232_LOGIC == 0  
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#if SUART_0_STOP_BIT_DETECTION == 0 
suart_0_wait_high: sbis    S0_RX_PIN_REG, SUART_0_RX_PIN /* WAIT FOR A HIGH TO LOW TRANSITION OF THE RX PIN */
                   rjmp    suart_0_wait_high 
#endif
           

suart_0_wait_low:
#if    SUART_0_TIMEOUT_ENABLED == 1
                   subi    r24, lo8(-(-1))
                   sbci    r25, hi8(-(-1))  
                   sbci    r26, hlo8(-(-1)) 
                   sbci    r27, hhi8(-(-1)) 
                   in      r30, S0_SREG
                   sbrc    r30, 1
                   rjmp    suart_0_timeout1
#endif
                   sbic    S0_RX_PIN_REG, SUART_0_RX_PIN 
                   rjmp    suart_0_wait_low            

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  SUART_0_USE_RS232_LOGIC == 1  /* #if    SUART_0_USE_RS232_LOGIC == 0  */
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#if SUART_0_STOP_BIT_DETECTION == 0
suart_0_wait_low:  sbic    S0_RX_PIN_REG, SUART_0_RX_PIN /* WAIT FOR A LOW TO HIGH TRANSITION OF THE RX PIN */
                   rjmp    suart_0_wait_low            
#endif

suart_0_wait_high: 
#if    SUART_0_TIMEOUT_ENABLED == 1                 
                   subi    r24, lo8(-(-1))
                   sbci    r25, hi8(-(-1))  
                   sbci    r26, hlo8(-(-1)) 
                   sbci    r27, hhi8(-(-1)) 
                   in      r30, S0_SREG
                   sbrc    r30, 1
                   rjmp    suart_0_timeout1
#endif
                   sbis    S0_RX_PIN_REG, SUART_0_RX_PIN          
                   rjmp    suart_0_wait_high            

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif  /* #elif  SUART_0_USE_RS232_LOGIC == 1 */
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

/************************** WATCH FOR A PROPER START BIT CODE BLOCK END *********************************/


/*********************************** TIMING  CODE BLOCK START *******************************************/
 
                   ldi     s0_bitcnt, 8                /* STOP BIT IS EATEN AND USED FOR RX DATA PROCESSING */                                                                                          

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if SUART_0_WILL_USE_A_TIMER == 0                     
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#if  SUART_0_DYNAMIC_BAUD_CHANGE == 1 
              
                   lds     s0_temp1, suart_0_delay1      /* 1/2 bit delay loop to be counted only once */
suart_0_rx_delay1: dec     s0_temp1                    /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */ 
                   brne    suart_0_rx_delay1                    
                                                
                
suart_0_getchar2:  lds     s0_temp1, suart_0_delay2      /* 1 bit delay loop */
                   subi    s0_temp1, 1                 /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */  
                   nop                                /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */   
suart_0_rx_delay2: dec     s0_temp1                     
                   brne    suart_0_rx_delay2                       

                   lds     s0_temp1, suart_0_delay3      /* 1/2 bit delay loop */
                   subi    s0_temp1, 1                 /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */  
                   nop                                /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */   
suart_0_rx_delay3: dec     s0_temp1                     
                   brne    suart_0_rx_delay3                 

#elif  SUART_0_DYNAMIC_BAUD_CHANGE == 0

                   ldi     s0_temp1, SUART_0_DELAY1      /* 1/2 bit delay loop to be counted only once */
suart_0_rx_delay1: dec     s0_temp1                    /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */
                   brne    suart_0_rx_delay1                    
                                                
suart_0_getchar2:  ldi     s0_temp1, SUART_0_DELAY2      /* 1/2 bit delay loop */
suart_0_rx_delay2: dec     s0_temp1                    /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */ 
                   brne    suart_0_rx_delay2
              
                   ldi     s0_temp1, SUART_0_DELAY3      /* 1/2 bit delay loop */   
suart_0_rx_delay3: dec     s0_temp1                    /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */ 
                   brne    suart_0_rx_delay3                       

#endif

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif SUART_0_WILL_USE_A_TIMER == 1  
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

                   clr     s0_temp2                    /* 1/2 bit delay loop to be counted only once */
                   out     S0_TCNT, s0_temp2
/* Dead time start(TIMER IS ALREADY COUNTING). */                        
#if    SUART_0_DYNAMIC_BAUD_CHANGE == 0
                   ldi     s0_temp2, SUART_0_TIME_HALF_BIT 
#elif  SUART_0_DYNAMIC_BAUD_CHANGE == 1
                   lds     s0_temp2, suart_0_time_half_bit
#endif    
/***************   Dead time end   ************/                                                       
suart_0_rx_delay1: in      s0_temp1, S0_TCNT
                   cp      s0_temp1, s0_temp2
                   brlo    suart_0_rx_delay1
            
suart_0_getchar2:  clr     s0_temp1                    /* 1 bit delay loop using a timer */
                   out     S0_TCNT, s0_temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */ 
#if    SUART_0_DYNAMIC_BAUD_CHANGE == 0
                   ldi     s0_temp2, SUART_0_TIME_1_BIT 
#elif  SUART_0_DYNAMIC_BAUD_CHANGE == 1
                   lds     s0_temp2, suart_0_time_1_bit
#endif
/***************   Dead time end   ************/                             
suart_0_rx_delay2: in      s0_temp1, S0_TCNT
                   cp      s0_temp1, s0_temp2
                   brlo    suart_0_rx_delay2

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

/*********************************** TIMING CODE BLOCK END ************************************************/
    
/**************************  MAIN DATA HADLING CODE BLOCK START *******************************************/
                   clc                               /* CLEAR CARRY */                

#if    SUART_0_USE_RS232_LOGIC == 0                  
                   sbic    S0_RX_PIN_REG, SUART_0_RX_PIN /* IF RX PIN IS LOW CLEAR CARRY */             
#elif  SUART_0_USE_RS232_LOGIC == 1
                   sbis    S0_RX_PIN_REG, SUART_0_RX_PIN /* IF RX PIN IS HIGH CLEAR CARRY */             
#endif


                   sec
suart_0_rotate:    ror     s0_Rxbyte                  /* ROTATE RIGHT THE RX BYTE THROUGH CARRY */      
                   dec     s0_bitcnt                  /* DECREMENT BITCOUNT */
                   breq    suart_0_getchar3            /* IF THIS WAS THE LAST BIT WRITE BYTE TO BUFFER */
                                                   /* ELSE */                                               
                   rjmp    suart_0_getchar2            /* GO GET NEXT BIT */ 

suart_0_getchar3:
/**************************  MAIN DATA HADLING CODE BLOCK END  *******************************************/

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if SUART_0_WILL_USE_A_TIMER == 0 && SUART_0_STOP_BIT_DETECTION == 2 
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#if    SUART_0_DYNAMIC_BAUD_CHANGE == 0

                   ldi     s0_bitcnt, SUART_0_STOP_BITS    /* load the number of stop bits */
              
suart_0_getchar4:  ldi     s0_temp1, SUART_0_DELAY2        /* 1 bit delay */
suart_0_rx_delay4: dec     s0_temp1
                   brne    suart_0_rx_delay4

                   ldi     s0_temp1, SUART_0_DELAY3      
suart_0_rx_delay5: dec     s0_temp1
                   brne    suart_0_rx_delay5

                   dec     s0_bitcnt                    /* If this was the last stop bit continue */
                   brne    suart_0_getchar4                  /* else apply 1 more bit delay             */

#elif    SUART_0_DYNAMIC_BAUD_CHANGE == 1

                   ldi     s0_bitcnt, SUART_0_STOP_BITS    /* load the number of stop bits */
              
suart_0_getchar4:  lds     s0_temp1, suart_0_delay2        /* 1/2 bit delay */
                   dec     s0_temp1
                   nop
suart_0_rx_delay4: dec     s0_temp1
                   brne    suart_0_rx_delay4

                   lds     s0_temp1, suart_0_delay3        /* 1 more 1/2 bit delay total 1 bit */
                   dec     s0_temp1
                   nop
suart_0_rx_delay5: dec     s0_temp1
                   brne    suart_0_rx_delay5

                   dec     s0_bitcnt                    /* If this was the last stop bit continue */
                   brne    suart_0_getchar4                  /* else apply 1 more bit delay             */

#endif

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif SUART_0_WILL_USE_A_TIMER == 1 && SUART_0_STOP_BIT_DETECTION == 2  
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

                   ldi     s0_bitcnt, SUART_0_STOP_BITS
            
suart_0_getchar4:  clr     s0_temp1                     /* 1 bit delay loop using a timer */
                   out     S0_TCNT, s0_temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */ 
#if    SUART_0_DYNAMIC_BAUD_CHANGE == 0
                   ldi     s0_temp2, SUART_0_TIME_1_BIT 
#elif  SUART_0_DYNAMIC_BAUD_CHANGE == 1
                   lds     s0_temp2, suart_0_time_1_bit
#endif
/***************   Dead time end   ************/ 
suart_0_rx_delay4: in      s0_temp1, S0_TCNT
                   cp      s0_temp1, s0_temp2
                   brlo    suart_0_rx_delay4

                   dec     s0_bitcnt
                   brne    suart_0_getchar4

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

/* ######### RECEIVING IS COMPLETE SO LETS START WRITTING THE BUFFER ############## */
/* ######### RX DATA PROCESSING FOR NON DYNAMIC PORT-PIN MODE        ############## */

suart_0_wr_buf_0:  sts     rx_c, r24                           
                   lds     s0_index, suart_0_rx_count
                             
                   lds     s0_temp1, suart_0_receive_string
                   cpi     s0_temp1, 1
                   brne    suart_0_wr_buf_1             

                   cpi     s0_Rxbyte, SUART_0_RX_TERMINATION_CHAR          
                   breq    suart_0_term_recv0     

suart_0_wr_buf_1:  cpi     s0_index, SUART_0_BUF_SIZE     
                   breq    suart_0_buf_reset                         

suart_0_wr_buf_2:  ldi     s0_z_low, lo8(suart_0_rx_buffer)
                   ldi     s0_temp1, hi8(suart_0_rx_buffer)         
                   add     s0_z_low, s0_index              
                   adc     s0_z_high,s0_temp1                       
suart_0_wr_buf_3:  st      Z+, s0_Rxbyte                          
                   inc     s0_index                      
                   clr     s0_temp1                              
                   st      Z, s0_temp1                            
                 
exit_suart_0_0:    sts     suart_0_rx_count, s0_index 

#if   SUART_0_STOP_BIT_DETECTION == 1 && SUART_0_USE_RS232_LOGIC == 0 
exit_suart_0_1:    sbis    S0_RX_PIN_REG, SUART_0_RX_PIN           /* Wait for high level            */
                   rjmp    exit_suart_0_1                         /* which should be the stop bit   */
#elif SUART_0_STOP_BIT_DETECTION == 1 && SUART_0_USE_RS232_LOGIC == 1
exit_suart_0_1:    sbic    S0_RX_PIN_REG, SUART_0_RX_PIN           /* Wait for low level             */
                   rjmp    exit_suart_0_1                         /* which should be the stop bit   */
#endif
                   ret                                         

#if    SUART_0_TIMEOUT_ENABLED == 1
suart_0_timeout1:  ldi     s0_Rxbyte, SUART_0_RX_TERMINATION_CHAR
                   sts     rx_c, s0_Rxbyte 
                   lds     s0_index, suart_0_rx_count 
                   tst     s0_index
                   brne    suart_0_timeout2
                   ldi     s0_index, 0xFF
suart_0_timeout2:  sts     suart_0_string_received, s0_index 
                   sts     suart_0_rx_count, s0_index
                   ldi     s0_index, 1
                   sts     suart_0_timeout_flag, s0_index
                   ret
#endif

suart_0_term_recv0:inc     s0_index
                   sts     suart_0_string_received, s0_index           
                   sts     suart_0_rx_count, s0_index
                   ret

suart_0_buf_reset: clr     s0_index                     
                   ldi     s0_z_low,  lo8(suart_0_rx_buffer)        
                   ldi     s0_z_high, hi8(suart_0_rx_buffer)        
                   rjmp    suart_0_wr_buf_3 

.endfunc

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/



/*####################################################################################################*/
/*####################################################################################################*/
#elif SUART_0_DYNAMIC_IO_SETTINGS == 1   /* NON INTERRUPT DYNAMIC PORT-PIN MODE */
/*####################################################################################################*/
/*####################################################################################################*/

                 .global SUART_0_RX_FUNCTION
                 .func   SUART_0_RX_FUNCTION

SUART_0_RX_FUNCTION: 

#if    SUART_0_TIMEOUT_ENABLED == 1
                   ldi  r22, lo8(SUART_0_TIMEOUT_LOOPS)
                   ldi  r23, hi8(SUART_0_TIMEOUT_LOOPS)
                   ldi  r24, hlo8(SUART_0_TIMEOUT_LOOPS)
                   ldi  r25, hhi8(SUART_0_TIMEOUT_LOOPS)
#endif

                   clr      s0_z_high
                   lds      s0_z_low,  suart_0_port
                   subi     s0_z_low, 2
                   lds      s0_temp2, suart_0_rx_pin 

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if    SUART_0_USE_RS232_LOGIC == 0  
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#if SUART_0_STOP_BIT_DETECTION == 0 

suart_0_wait_high: ld       s0_temp1, z
                   and      s0_temp1, s0_temp2
                   tst      s0_temp1                
                   breq     suart_0_wait_high            
#endif


suart_0_wait_low:
#if    SUART_0_TIMEOUT_ENABLED == 1               
                   subi r22, lo8(-(-1))
                   sbci r23, hi8(-(-1))  
                   sbci r24, hlo8(-(-1)) 
                   sbci r25, hhi8(-(-1)) 
                   in   s0_temp1, S0_SREG
                   sbrc s0_temp1, 1
                   rjmp suart_0_timeout1 
#endif
                   ld       s0_temp1, z
                   and      s0_temp1, s0_temp2
                   tst      s0_temp1                
                   brne     suart_0_wait_low 

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif  SUART_0_USE_RS232_LOGIC == 1
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#if SUART_0_STOP_BIT_DETECTION == 0 

suart_0_wait_low:  ld       s0_temp1, z
                   and      s0_temp1, s0_temp2
                   tst      s0_temp1                
                   brne     suart_0_wait_low 

#endif

suart_0_wait_high:
#if    SUART_0_TIMEOUT_ENABLED == 1
                   subi r22, lo8(-(-1))
                   sbci r23, hi8(-(-1))  
                   sbci r24, hlo8(-(-1)) 
                   sbci r25, hhi8(-(-1)) 
                   in   s0_temp1, S0_SREG
                   sbrc s0_temp1, 1
                   rjmp suart_0_timeout1 

#endif               
                   ld       s0_temp1, z
                   and      s0_temp1, s0_temp2
                   tst      s0_temp1                
                   breq     suart_0_wait_high                

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif  /* #elif  SUART_0_USE_RS232_LOGIC == 1 */
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
                  
                   ldi     s0_bitcnt, 8                /* STOP BIT IS EATEN AND USED FOR RX DATA PROCESSING */                                                                                          

#if SUART_0_WILL_USE_A_TIMER == 0  

#if   SUART_0_DYNAMIC_BAUD_CHANGE == 1                    
              
                   lds     s0_temp1, suart_0_delay1      /* 1/2 bit delay loop to be counted only once */
suart_0_rx_delay1: dec     s0_temp1                    /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */ 
                   brne    suart_0_rx_delay1                    
                                                
                
suart_0_getchar2:  lds     s0_temp1, suart_0_delay2      /* 1 bit delay loop */
                   subi    s0_temp1, 1                 /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */  
                   nop                                /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */   
suart_0_rx_delay2: dec     s0_temp1                     
                   brne    suart_0_rx_delay2                       

                   lds     s0_temp1, suart_0_delay3      /* 1/2 bit delay loop */
                   subi    s0_temp1, 1                 /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */  
                   nop                                /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */   
suart_0_rx_delay3: dec     s0_temp1                     
                   brne    suart_0_rx_delay3                 

#elif SUART_0_DYNAMIC_BAUD_CHANGE == 0

                   ldi     s0_temp1, SUART_0_DELAY1      /* 1/2 bit delay loop to be counted only once */
suart_0_rx_delay1: dec     s0_temp1                    /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */
                   brne    suart_0_rx_delay1                    
                                                
suart_0_getchar2:  ldi     s0_temp1, SUART_0_DELAY2      /* 1/2 bit delay loop */
suart_0_rx_delay2: dec     s0_temp1                    /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */ 
                   brne    suart_0_rx_delay2
              
                   ldi     s0_temp1, SUART_0_DELAY3      /* 1/2 bit delay loop */   
suart_0_rx_delay3: dec     s0_temp1                    /* TOTAL DELAY = (suart_0_delayX*3)*(1/F_CPU) */ 
                   brne    suart_0_rx_delay3                       

#endif /* #elif SUART_0_DYNAMIC_BAUD_CHANGE == 0 */

#elif SUART_0_WILL_USE_A_TIMER == 1  

                   clr     s0_temp2                    /* 1/2 bit delay loop to be counted only once */
                   out     S0_TCNT, s0_temp2
/* Dead time start(TIMER IS ALREADY COUNTING). */                        
#if    SUART_0_DYNAMIC_BAUD_CHANGE == 0
                   ldi     s0_temp2, SUART_0_TIME_HALF_BIT 
#elif  SUART_0_DYNAMIC_BAUD_CHANGE == 1
                   lds     s0_temp2, suart_0_time_half_bit
#endif    
/***************   Dead time end   ************/                                                       
suart_0_rx_delay1: in      s0_temp1, S0_TCNT
                   cp      s0_temp1, s0_temp2
                   brlo    suart_0_rx_delay1
            
suart_0_getchar2:  clr     s0_temp1                    /* 1 bit delay loop using a timer */
                   out     S0_TCNT, s0_temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */ 
#if    SUART_0_DYNAMIC_BAUD_CHANGE == 0
                   ldi     s0_temp2, SUART_0_TIME_1_BIT 
#elif  SUART_0_DYNAMIC_BAUD_CHANGE == 1
                   lds     s0_temp2, suart_0_time_1_bit
#endif
/***************   Dead time end   ************/                             
suart_0_rx_delay2: in      s0_temp1, S0_TCNT
                   cp      s0_temp1, s0_temp2
                   brlo    suart_0_rx_delay2

#endif   /* #elif SUART_0_WILL_USE_A_TIMER == 1  &&  SUART_0_DYNAMIC_BAUD_CHANGE == 0 */

                   clc                               /* CLEAR CARRY */                

#if    SUART_0_USE_RS232_LOGIC == 0                  

                   nop
                   ld       s0_temp1, z                                      
                   lds      s0_temp2, suart_0_rx_pin 
                   and      s0_temp1, s0_temp2
                   tst      s0_temp1                
                   breq     suart_0_rotate
        
#elif  SUART_0_USE_RS232_LOGIC == 1
                   nop
                   ld       s0_temp1, z
                   lds      s0_temp2, suart_0_rx_pin         
                   and      s0_temp1, s0_temp2
                   tst      s0_temp1                
                   brne     suart_0_rotate            /* IF RX PIN IS HIGH CLEAR CARRY */             
#endif

                   sec                               /* SET CARRY IF NOT SKIPPED BY SBI-CBI ABOVE */
suart_0_rotate:    ror     s0_Rxbyte                  /* ROTATE RIGHT THE RX BYTE THROUGH CARRY */      
                   dec     s0_bitcnt                  /* DECREMENT BITCOUNT */
                   breq    suart_0_getchar3            /* IF THIS WAS THE LAST BIT WRITE BYTE TO BUFFER */
                                                   /* ELSE */                                               
                   rjmp    suart_0_getchar2            /* GO GET NEXT BIT */ 

suart_0_getchar3:
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if SUART_0_WILL_USE_A_TIMER == 0 && SUART_0_STOP_BIT_DETECTION == 2 
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#if    SUART_0_DYNAMIC_BAUD_CHANGE == 0

                   ldi     s0_bitcnt, SUART_0_STOP_BITS    /* load the number of stop bits */
              
suart_0_getchar4:  ldi     s0_temp1, SUART_0_DELAY2        /* 1 bit delay */
suart_0_rx_delay4: dec     s0_temp1
                   brne    suart_0_rx_delay4

                   ldi     s0_temp1, SUART_0_DELAY3      
suart_0_rx_delay5: dec     s0_temp1
                   brne    suart_0_rx_delay5

                   dec     s0_bitcnt                    /* If this was the last stop bit continue */
                   brne    suart_0_getchar4                  /* else apply 1 more bit delay             */

#elif    SUART_0_DYNAMIC_BAUD_CHANGE == 1

                   ldi     s0_bitcnt, SUART_0_STOP_BITS    /* load the number of stop bits */
              
suart_0_getchar4:  lds     s0_temp1, suart_0_delay2        /* 1/2 bit delay */
                   dec     s0_temp1
                   nop
suart_0_rx_delay4: dec     s0_temp1
                   brne    suart_0_rx_delay4

                   lds     s0_temp1, suart_0_delay3        /* 1 more 1/2 bit delay total 1 bit */
                   dec     s0_temp1
                   nop
suart_0_rx_delay5: dec     s0_temp1
                   brne    suart_0_rx_delay5

                   dec     s0_bitcnt                    /* If this was the last stop bit continue */
                   brne    suart_0_getchar4                  /* else apply 1 more bit delay             */

#endif

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif SUART_0_WILL_USE_A_TIMER == 1 && SUART_0_STOP_BIT_DETECTION == 2  
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

                   ldi     s0_bitcnt, SUART_0_STOP_BITS
            
suart_0_getchar4:  clr     s0_temp1                     /* 1 bit delay loop using a timer */
                   out     S0_TCNT, s0_temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */ 
#if    SUART_0_DYNAMIC_BAUD_CHANGE == 0
                   ldi     s0_temp2, SUART_0_TIME_1_BIT 
#elif  SUART_0_DYNAMIC_BAUD_CHANGE == 1
                   lds     s0_temp2, suart_0_time_1_bit
#endif
/***************   Dead time end   ************/ 
suart_0_rx_delay4: in      s0_temp1, S0_TCNT
                   cp      s0_temp1, s0_temp2
                   brlo    suart_0_rx_delay4

                   dec     s0_bitcnt
                   brne    suart_0_getchar4

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif  /* #if SUART_0_WILL_USE_A_TIMER == 0 && SUART_0_STOP_BIT_DETECTION == 2 */
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

/* ######### RECEIVING IS COMPLETE SO LETS START WRITTING THE BUFFER ############## */
/* ######### RX DATA PROCESSING FOR DYNAMIC PORT-PIN MODE            ############## */
                 
suart_0_wr_buf_0:  sts     rx_c, r24                           
                   lds     s0_index, suart_0_rx_count

                   lds     s0_temp1, suart_0_receive_string
                   cpi     s0_temp1, 1
                   brne    suart_0_wr_buf_1        

                   cpi     s0_Rxbyte, SUART_0_RX_TERMINATION_CHAR          
                   breq    suart_0_term_recv0     

suart_0_wr_buf_1:  cpi     s0_index, SUART_0_BUF_SIZE     
                   breq    suart_0_buf_reset                         

              

suart_0_wr_buf_2:  ldi     s0_z_low, lo8(suart_0_rx_buffer)
                   ldi     s0_temp1, hi8(suart_0_rx_buffer)         
                   add     s0_z_low, s0_index              
                   adc     s0_z_high,s0_temp1                       
suart_0_wr_buf_3:  st      Z+, s0_Rxbyte                          
                   inc     s0_index                      
                   clr     s0_temp1                              
                   st      Z, s0_temp1                            
                 
exit_suart_0_0:    sts     suart_0_rx_count, s0_index 

#if    SUART_0_STOP_BIT_DETECTION == 1 && SUART_0_USE_RS232_LOGIC == 0 

                   clr      s0_z_high
                   lds      s0_z_low,  suart_0_port
                   subi     s0_z_low, 2
                   lds      s0_temp2, suart_0_rx_pin  
suart_0_wait_high: ld       s0_temp1, z
                   and      s0_temp1, s0_temp2
                   tst      s0_temp1                
                   breq     suart_0_wait_high            

#elif  SUART_0_STOP_BIT_DETECTION == 1 && SUART_0_USE_RS232_LOGIC == 1 

                   clr      s0_z_high
                   lds      s0_z_low,  suart_0_port
                   subi     s0_z_low, 2
                   lds      s0_temp2, suart_0_rx_pin 
suart_0_wait_low:  ld       s0_temp1, z
                   and      s0_temp1, s0_temp2
                   tst      s0_temp1                
                   brne     suart_0_wait_low 

#endif
                   ret                                         

suart_0_timeout1:  ldi     s0_Rxbyte, SUART_0_RX_TERMINATION_CHAR
                   sts     rx_c, s0_Rxbyte 
                   lds     s0_index, suart_0_rx_count 
                   tst     s0_index
                   brne    suart_0_timeout2
                   ldi     s0_index, 0xFF
suart_0_timeout2:  sts     suart_0_string_received, s0_index 
                   sts     suart_0_rx_count, s0_index
                   ldi     s0_index, 1
#if  SUART_0_TIMEOUT_ENABLED == 1 
                   sts     suart_0_timeout_flag, s0_index
#endif
                   ret

suart_0_term_recv0:inc     s0_index
                   sts     suart_0_string_received, s0_index           
                   sts     suart_0_rx_count, s0_index
                   ret

suart_0_buf_reset: clr     s0_index                     
                   ldi     s0_z_low,  lo8(suart_0_rx_buffer)        
                   ldi     s0_z_high, hi8(suart_0_rx_buffer)        
                   rjmp    suart_0_wr_buf_3 



.endfunc

/*####################################################################################################*/
/*####################################################################################################*/
#endif   /* #elif SUART_0_DYNAMIC_IO_SETTINGS == 1  */
/*####################################################################################################*/
/*####################################################################################################*/



                                      /* THANK GOD IS OVER!!! */
/*########################################################################################################*/
/*                                     T   H   E      E   N   D                                           */
/*########################################################################################################*/                

