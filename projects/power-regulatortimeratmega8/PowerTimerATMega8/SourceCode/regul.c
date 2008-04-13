 /*
    Copyright (c) 2005  Gediminas Labutis                   
   
  Permission is hereby granted, free of charge, to any       
 person obtaining a copy of this software and associated     
 documentation files (the "Software"), to deal in the        
 Software without restriction, including without             
 limitation the rights to use, copy, modify, merge, publish, 
 distribute, sublicense, and/or sell copies of the Software, 
 and to permit persons to whom the Software is furnished to  
 do so, subject to the following conditions:                 
  
   The above copyright notice and this permission notice     
 shall be included in all copies or substantial portions of  
 the Software.                                               
      
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF       
 ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED     
 TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF         
 CONTRACT, TORT OR OTHERWISE, ARISING FROM,                  
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR     
 OTHER DEALINGS IN THE SOFTWARE.   

*/                          


/*
             TIMER/POWER REGULATOR     V1.0  
                 
              Compiler - WinAVR 20050214

   LED indicator pin assignment and code
   for symbols
 
  PB7 PB6 PB5 PB4 PB3 PB2 PB1 PB0         pins               
  DP   G   F   E   D   C   B   A          segments          
   1   1   0   0   0   0   0   0  0xC0      0                
   1   1   1   1   1   0   0   1  0xF9      1               
   1   0   1   0   0   1   0   0  0xA4      2               
   1   0   1   1   0   0   0   0  0xB0      3                
   1   0   0   1   1   0   0   1  0x99      4                
   1   0   0   1   0   0   1   0  0x92      5                
   1   0   0   0   0   0   1   0  0x82      6                
   1   1   1   1   1   0   0   0  0xF8      7                
   1   0   0   0   0   0   0   0  0x80      8                
   1   0   0   1   0   0   0   0  0x90      9               
*/



#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/eeprom.h>  


#define bit_get(p,m) ((p) & (m)) 
#define bit_flip(p,m) ((p) ^= (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m)) 
#define BIT(x) (0x01 << (x)) 

 // IO pins
#define TRIAC 6            // PD6
#define Q1    5            // PD5
#define Q2    4            // PD4
#define Q3    3            // PD3
#define DP    7            // PB7 - decimal point
#define BUTTON_PLUS   2    // PD2
#define BUTTON_MINUS  1    // PD1 
#define BUTTON_ON_OFF 0    // PD0
#define LED_ON_OFF 2       // PC2
#define LED_TMR    1       // PC1
#define LED_PWR    0       // PC0



   // Button status
#define NOPUSH   0         //  Button not pushed
#define PUSHED   1         //  Button pushed 
#define SHORT    2         //  Short push
#define LONG     3         //  Long push
#define HANDLED  4         //  Button status handled


  // Buttons port status
#define NOT_READED  0      // Buttons port readed
#define READED      1      // Buttons port not readed

#define SHORT_PUSH_COUNTER      15     
#define LONG_PUSH_COUNTER       200    
#define AUTO_COUNTER            15    //  auto increment/decrement counter   
#define NEW_VALUE               1
#define OLD_VALUE               0   

#define AUTO_SEC_INC_DEC_CNT         55
#define AUTO_SEC_TENS_INC_DEC_CNT    25
#define AUTO_MINS_INC_DEC_CNT        45          

#define OFF        0
#define ON         1

#define TMR        0 // mode - timer
#define PWR        1 // mode - power control
#define TMR_PWR    2 // mode - timer and power control

#define SHOW_PWR   0 // show time
#define SHOW_TMR   1 // show power percent

#define TIME_INT_FREQ  5   // time_sec counting interrupt frequency Hz
#define MAX_TIME 59940     // 59940 seconds -> 999 minutes



uint8_t const numbers[11] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xFF} ;
                            //  0    1    2    3     4    5    6    7    8    9    OFF
uint8_t  volatile   buttons_port ;        // copy PINx here 
uint8_t  volatile   buttons_port_status ; // readed/not readed
uint8_t  volatile   percent_value;        // NEW/OLD
uint16_t volatile   time_sec ;            // variable for seconds
uint8_t  volatile   time_tmp ;            // temporary variable for time counting

uint8_t   mode ;    // TMR, PWR, TMR_PWR
uint8_t   percent ; // power percent for triac  0-100%
uint8_t   status ;  // device status - ON/OFF

// button status -> NOPUSH,PUSHED,SHORT,LONG,HANDLED
uint8_t   button_plus_status ;    
uint8_t   button_minus_status ;    
uint8_t   button_on_off_status ; 

uint8_t   dp_position; // decimal point position

uint8_t   display;    // SHOW_PWR,SHOW_TMR

// digits to display on LED indicator
uint8_t   first_digit  ;
uint8_t   second_digit ;
uint8_t   third_digit  ;

uint8_t   volatile sanity_check ; // for wdr();



// initialize IO pins
 void port_init(void)  
{
 DDRB =  (1<<DDB7) | (1<<DDB6) | (1<<DDB5) | (1<<DDB4) | (1<<DDB3) 
 | (1<<DDB2) | (1<<DDB1) | (1<<DDB0) ;   // All outpout
 PORTB = (1<<PB7) | (1<<PB6) | (1<<PB5) | (1<<PB4) | (1<<PB3) | (1<<PB2) 
 | (1<<PB1) | (1<<PB0) ;                 // Output "1"
 
 DDRC =  (1<<PC0) | (1<<PC1) | (1<<PC2) ;// PC0, PC1, PC2 - output
 PORTC = (1<<PC3) | (1<<PC4) | (1<<PC5) ; //Inputs - Pull Up, output - "0"
 
  DDRD  = (1<<PD3) | (1<<PD4) | (1<<PD5) | (1<<PD6)  ;
 // PD3,PD4,PD5,PD6 - Output ; PD0,PD1,PD2,PD7 - Input ;
 PORTD = (1<<PD0) | (1<<PD1) | (1<<PD2) ;
 // Outputs - Low ; Inputs - Pull Up ( PD7 - no pull-up); 
}



//Comparator initialize
// bandgeap reference voltage on, interrupt on rising output edge
void comparator_init(void) 
{
  ACSR = ACSR & 0xF7; //ensure interrupt is off before changing
  ACSR = (1<<ACBG) | (1<<ACIE) | (1<<ACIS1) | (1<<ACIS0);
}

//Watchdog initialize
// prescale: 2048K
void watchdog_init(void)
{
 wdt_reset() ; //this prevents a timout on enabling
 WDTCR |= (1<<WDCE) | (1<<WDE) ;
 WDTCR = (1<<WDE) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0); //WATCHDOG ENABLED - dont forget to issue WDRs
}


//TIMER0 initialize - prescale:256
// desired value: 200Hz
// actual value: 205,592Hz (2,7%)
void timer0_init(void) // +
{
 TCCR0 = 0x00; //stop
 TCNT0 = 0xED; //set count
 TCCR0 = 0x04; //start timer
}


//TIMER1 initialize - prescale:256
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 5Hz @ 1MHz
// actual value:  5,002Hz (0,0%)
void timer1_init(void) // +
{
 TCCR1B = 0x00; //stop
 TCNT1H = 0xFC; //setup
 TCNT1L = 0xF3;
 OCR1AH = 0x03;
 OCR1AL = 0x0D;
 OCR1BH = 0x03;
 OCR1BL = 0x0D;
 ICR1H  = 0x03;
 ICR1L  = 0x0D;
 TCCR1A = 0x00;
 TCCR1B = 0x04; //start Timer
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 int16_t temp0,temp1;

 cli(); //disable all interrupts
 port_init();
 
// some time for start-up ( to avoid comparator interrupt at power-on) 
  for(temp0=0;temp0<10000;temp0++)   
    {
        for(temp1=0;temp1<1000;temp1++);
    }
 comparator_init();	
 watchdog_init();
 timer0_init();
 timer1_init();
 

 MCUCR = 0x00;
 GICR  = 0x00;
 TIMSK = (1<<TOIE1) | (1<<TOIE0) ; //timer interrupt sources
 //all peripherals are now initialized
 time_tmp = TIME_INT_FREQ;
 
 sei(); // enable interrupts 
}



// TIMER0 overflow interrupt
// read buttons port, output numbers to LED indicator
SIGNAL(SIG_OVERFLOW0)             
{
  uint8_t static digit_position ; // 0 - leftmost, 2 - rightmost
    
  TCNT0 = 0xED;                    // reload counter value
  buttons_port = PIND ;            // read buttons
  buttons_port_status = READED ;   // mark 
  
  bit_set(sanity_check,BIT(0));
 
 if (digit_position > 2)         
   digit_position = 0 ;

 
 switch(digit_position)
  {
  case 0 :
  bit_clear(PORTD, BIT(Q2) | BIT(Q3));  // Q2,Q3 OFF
  PORTB = numbers[first_digit] ;
   if (dp_position == 1)
    bit_clear(PORTB,BIT(DP));
  bit_set(PORTD,BIT(Q1));               // Q1 ON
  digit_position++;
  break;
  
  case 1 :
  bit_clear(PORTD, BIT(Q1) | BIT(Q3)); // Q1,Q3 OFF
  PORTB = numbers[second_digit] ;
  if (dp_position == 2)
    bit_clear(PORTB,BIT(DP));
  bit_set(PORTD,BIT(Q2));              // Q2 ON
  digit_position++;
  break;
  
  case 2 :
  bit_clear(PORTD, BIT(Q1) | BIT(Q2)); // Q1,Q2 OFF
  PORTB = numbers[third_digit] ;
  if (dp_position == 3)
   bit_clear(PORTB,BIT(DP));
  bit_set(PORTD,BIT(Q3));              // Q3 ON
  digit_position++;
  break;
  
  default:                            // should never go here
  digit_position=0;
  bit_clear(PORTB,BIT(DP));
  status = OFF ;
  
  }
  
 
 switch (mode)
  {
   case TMR :
   bit_set(PORTC,BIT(LED_TMR));
   bit_clear(PORTC,BIT(LED_PWR));
   break;
	 
   case PWR :
   bit_clear(PORTC,BIT(LED_TMR));
   bit_set(PORTC,BIT(LED_PWR));
   break;
	 
   case TMR_PWR :
   bit_set(PORTC,BIT(LED_TMR));
   bit_set(PORTC,BIT(LED_PWR));
   break;
	 
   default:        // should never go here
   status = OFF ;
  }	 
		 
  
 if (status == ON)
   bit_set(PORTC,BIT(LED_ON_OFF));
  else
   bit_clear(PORTC,BIT(LED_ON_OFF));	
   
}


// TIMER1 overflow interrupt
// time counting, triac control
SIGNAL(SIG_OVERFLOW1) 
{
 int16_t static i,e ;
  
 TCNT1H = 0xFC; //reload counter high value
 TCNT1L = 0xF3; //reload counter low value
 
 bit_set(sanity_check,BIT(2));
   
 // time counting
 
 if ((mode == TMR || mode == TMR_PWR) && (status == ON))
   {
     if (time_sec > 0)
      {
       time_tmp++;
	   if(time_tmp>=TIME_INT_FREQ)
	   {
	    time_tmp = 0;
		time_sec--;
		if(time_sec==0)
		status = OFF ; // if time out , turn off triac  
	   }
      }   
     else
	 status = OFF ; // if time out , turn off triac 
   } 
 
 // Triac control
 
  if (!(status == ON))
    bit_clear(PORTD, BIT(TRIAC));                  
  else
   if (mode == TMR)
    bit_set(PORTD, BIT(TRIAC));
   else if ((mode == PWR) || (mode == TMR_PWR))  
    {
 
//  Bresenham Algorithm
    
      if ((i>99) || (percent_value == NEW_VALUE)) // if cycle finished or new value - start again
       {
        i=0;
	    e=2*percent - 100;
	    percent_value = OLD_VALUE;
       }
	
	  i++;
	  if (e>=0)
	   {
	     e=e+2*(percent - 100);
	     bit_set(PORTD, BIT(TRIAC));
	    }
	 
	  else 
	   {
	     e=e+2*percent ;
	     bit_clear(PORTD, BIT(TRIAC));
	   }
	}
    
    else
      status = OFF ; // should never go here   
  

}



// save percent,time and mode to EEPROM on power fail
SIGNAL(SIG_COMPARATOR)
 {    
      uint8_t temp ;
      uint16_t temp1; 
	    
	  // save some power
	  bit_clear(PORTD, BIT(TRIAC));
	  bit_clear(PORTD, BIT(Q1) | BIT(Q2) | BIT(Q3));
	    
	  temp = eeprom_read_byte((uint8_t*)0x00);
	  if (!(temp == percent))  
	  eeprom_write_byte((uint8_t*)0x00,percent);
	 	 
      temp = eeprom_read_byte((uint8_t*)0x01);
	  if (!(temp == mode))
	  eeprom_write_byte((uint8_t*)0x01,mode);
	  
	  temp1 = eeprom_read_word((uint16_t*)0x02);
	  if (!(temp1 == time_sec))
	  eeprom_write_word((uint16_t*)0x02,time_sec);
	
  }	  
  
// check if buttons pushed
void check_buttons ( uint8_t buttons_port)
{
 
 uint16_t static button_plus_counter ;
 uint16_t static button_minus_counter ;
 uint16_t static button_on_off_counter ;
 
 bit_set(sanity_check,BIT(3));
 
 // button +
if ((bit_get(buttons_port,BIT(BUTTON_PLUS))) == 0)   // button is down ....
           {                                   
              if (button_plus_status == NOPUSH)  // is this a new push ?
	           {                                
                cli();
				button_plus_counter = 0;        // yes - start the debounce timer
                sei();
				button_plus_status = PUSHED;   // note that it's down
               }
   
              if (button_plus_status == PUSHED) // button is down - have we already noted that ?
	            {         
                   if (button_plus_counter< 1000) // overflow protection
				   {
				   cli();
				   button_plus_counter++;      // yes - inc the debounce timer
                   sei();
                   }             
				   if (button_plus_counter >= LONG_PUSH_COUNTER)
				       {   // long enough for a LONG push ?
                         button_plus_status = LONG;      // yes - note a LONG push
                       }
				   
				   
                }

          }
    
	
	else
	  {
          if (button_plus_status == PUSHED)
		      {         // button is up, was it pushed before ?
                  if (button_plus_counter > SHORT_PUSH_COUNTER) 
				   {   // yes, was it long enough for a short push ?
                      button_plus_status = SHORT;   //   past bounce timeout, was a short push
                   } 
		          else  button_plus_status = NOPUSH;  // no, too short - was a bounce
              }
      
	  if ((button_plus_status == HANDLED) || (button_plus_status == LONG) )
	    {         // button is up, did we handle it ?
             button_plus_status = NOPUSH; // yes - reset back to waiting for push
        }                  
     }
	 
	 
	 
	 
	 
	 // button -
if ((bit_get(buttons_port,BIT(BUTTON_MINUS)))==0) // button is down ....
           {                                   
              if (button_minus_status == NOPUSH)  // is this a new push ?
	           {     
                cli();           
                button_minus_counter = 0;        // yes - start the debounce timer
                sei();
				button_minus_status = PUSHED;   // note that it's down
               }
   
              if (button_minus_status == PUSHED)  // button is down - have we already noted that ?
	            {  
                   if (button_minus_counter< 1000)   // overflow protection    
                   {
				   cli();
				   button_minus_counter++;         // yes - inc the debounce timer    
				   sei();
				   }
				   if (button_minus_counter >= LONG_PUSH_COUNTER)
				       {   // long enough for a LONG push ?
                         button_minus_status = LONG;     // yes - note a LONG push
                       }
				   
				   
                }

          }
    
	
	else
	  {
          if (button_minus_status == PUSHED)
		      {         // button is up, was it pushed before ?
                  if (button_minus_counter > SHORT_PUSH_COUNTER) 
				   {   // yes, was it long enough for a short push ?
                      button_minus_status = SHORT;  //   past bounce timeout, was a short push
                   } 
		          else  button_minus_status = NOPUSH;  // no, too short - was a bounce
              }
      
	  if ((button_minus_status == HANDLED) || (button_minus_status == LONG) )
	    {         // button is up, did we handle it ?
             button_minus_status = NOPUSH; // yes - reset back to waiting for push
        }                  
     }
	 
	 
	
	 // button ON/OFF
if ((bit_get(buttons_port,BIT(BUTTON_ON_OFF)))==0) // button is down ....
           {                                   
              if (button_on_off_status == NOPUSH)  // is this a new push ?
	           {                                
                cli();
				button_on_off_counter = 0;        // yes - start the debounce timer
                sei();
				button_on_off_status = PUSHED;   // note that it's down
               }
   
              if (button_on_off_status == PUSHED) // button is down - have we already noted that ?
	            {         
                   if (button_on_off_counter< 1000) // overflow protection 
				   {
				   cli();
				   button_on_off_counter++;  // yes - inc the debounce timer 
                   sei();  
                   }          
				}
          }
    
	
	else
	  {
          if (button_on_off_status == PUSHED)
		      {         // button is up, was it pushed before ?
                  if (button_on_off_counter > SHORT_PUSH_COUNTER) 
				   {   // yes, was it long enough for a short push ?
                      button_on_off_status = SHORT;  //   past bounce timeout, was a short push
                   } 
		          else  button_on_off_status = NOPUSH; // no, too short - was a bounce
              }
      
	  if (button_on_off_status == HANDLED)
	    {         // button is up, did we handle it ?
             button_on_off_status = NOPUSH; // yes - reset back to waiting for push
        }                 
     }
   
  	 
}	 



// handle buttons 
 void status_handling(void)
 {
 
   uint16_t static auto_inc_dec_ct;
   uint8_t static auto_sec_inc_cnt, auto_sec_tens_inc_cnt,auto_mins_inc_cnt,
   auto_sec_dec_cnt, auto_sec_tens_dec_cnt,auto_mins_dec_cnt;
   
   bit_set(sanity_check,BIT(6));
   
   
  // increment percent value if:
  // button PLUS short push AND button MINUS not pushed AND display == SHOW_PWR
   if ((button_plus_status == SHORT) && (button_minus_status == NOPUSH) && ( display == SHOW_PWR) && (!(mode == TMR_PWR)))
	  {
	    if (percent < 100 )
		{
		cli();
		percent++;
		percent_value = NEW_VALUE;
		sei();
		}
		button_plus_status = HANDLED ;
	  }
	  
  // increment timer value if:
  // button PLUs short push AND button MINUS not pushed AND display == SHOW_TMR
   if ((button_plus_status == SHORT) && (button_minus_status == NOPUSH) && ( display == SHOW_TMR) && (!(mode == TMR_PWR)))
	  {
	    if (time_sec < MAX_TIME )  // MAX_TIME seconds = 999 minutes
		{
		cli();
		 if (time_sec < 600)   // 600 seconds = 10 minutes
		  time_sec++ ;         // add 1 second
		   else if (time_sec < 5999) // 5999 secconds = 99 minutes 59 seconds
		    time_sec=time_sec+10; // add 10 seconds
			 else         // if time_sec >= 100 minutes
			  time_sec=time_sec+60; // add 60 seconds
		sei();
		}
		button_plus_status = HANDLED ;
	  }  
	  
	  
 
  // decrement percent value if:
  // button MINUS short push AND button PLUS not pushed AND display == SHOW_PWR
   if ((button_minus_status == SHORT) && (button_plus_status == NOPUSH) && ( display == SHOW_PWR) && (!(mode == TMR_PWR)))
	  {
	    if (percent > 0 )
		{
		cli();
		percent--;
		percent_value = NEW_VALUE;
		sei();
		}
		button_minus_status = HANDLED ;
	  }
 
 
  // decrement time_sec value if:
  // button MINUS short push AND button PLUS not pushed AND display == SHOW_PWR
  if ((button_minus_status == SHORT) && (button_plus_status == NOPUSH) && ( display == SHOW_TMR) && (!(mode == TMR_PWR)))
	 {
	 if (time_sec > 0 )  // 
		{
		cli();
		 if (time_sec < 600)   // 600 seconds = 10 minutes
		  time_sec-- ;         // subtract 1 second
		   else if (time_sec < 5999) // 5999 secconds = 99 minutes 59 seconds
		    time_sec=time_sec-10; // subtract 10 seconds
			 else         // if time_sec >= 100 minutes
			  time_sec=time_sec-60; // subtract 60 seconds
		sei();
		}
		button_minus_status = HANDLED ;
	  }  
 
 
 
 
   
   // auto incremet percent value if:
  // button PLUS long push AND button MINUS not pushed AND display == SHOW_PWR
   
   if ((button_plus_status == LONG) && (button_minus_status == NOPUSH) && ( display == SHOW_PWR) && (!(mode == TMR_PWR)))
	 {
	  if (auto_inc_dec_ct < 1000) // overflow protection
	  {
	  cli();
	  auto_inc_dec_ct ++;
	  sei();
	  }
	  if (auto_inc_dec_ct  > AUTO_COUNTER )
	    {
		  cli();
		  auto_inc_dec_ct  = 0;
		  sei();
		  if (percent < 100 )
		  {
		  cli();
		  percent++;
		  percent_value = NEW_VALUE;
		  sei();
		  }
		}
     }
 
   // auto incremet timer value if:
  // button PLUS long push AND button MINUS not pushed AND display == SHOW_TMR
   
   if ((button_plus_status == LONG) && (button_minus_status == NOPUSH) && ( display == SHOW_TMR) && (!(mode == TMR_PWR)))
	 {
	    if (auto_inc_dec_ct < 1000) // overflow protection
	     {
	      cli();
	      auto_inc_dec_ct ++;
	      sei();
	     }
	  
	  if (auto_inc_dec_ct  > AUTO_COUNTER )
	    {
		  cli();
		  auto_inc_dec_ct  = 0;
		  sei();
		  
		   if (time_sec < MAX_TIME )  // MAX_TIME seconds = 999 minutes
		     {
		       cli();
		       if (time_sec < 600)   // 600 seconds = 10 minutes
			    {
				  if (auto_sec_inc_cnt<255)
				  auto_sec_inc_cnt++;
				  if (auto_sec_inc_cnt < AUTO_SEC_INC_DEC_CNT)
				  time_sec++ ;         // add 1 second
			      else
				  time_sec=time_sec+10;
			   }
		       else if (time_sec < 5999) // 5999 seconds = 99 minutes 59 seconds
			   {
			   if (auto_sec_tens_inc_cnt<255)
			   auto_sec_tens_inc_cnt++;
			   if (auto_sec_tens_inc_cnt < AUTO_SEC_TENS_INC_DEC_CNT)
		       time_sec=time_sec+10; // add 10 seconds
			   else
			   time_sec=time_sec+60;
			   }
			   else         // if time_sec >= 100 minutes
			   {
			   if(auto_mins_inc_cnt < 255)
			   auto_mins_inc_cnt++;
			   if (auto_mins_inc_cnt < AUTO_MINS_INC_DEC_CNT)
			   {
			   if (time_sec<59880) // max time_sec - 999 min (MAX_TIME sec) 
			   time_sec=time_sec+60;   // add 60 seconds
			   else
			   time_sec=MAX_TIME ;  //999 min
			   }
			   else
			   {
			   if (time_sec<59340)
			   time_sec=time_sec+600; 
			   else
			   time_sec=MAX_TIME;
		       }
			   }
			   sei();
		     }
		}  
	 } 
     else
	   {
	   auto_sec_inc_cnt=0;
	   auto_sec_tens_inc_cnt=0;
	   auto_mins_inc_cnt=0;
	   }
 
 
 
 
  // auto decremet percent value if:
  // button MINUS long push AND button PLUS not pushed AND regulator mode ON
  
   if ((button_minus_status == LONG) && (button_plus_status == NOPUSH) && ( display == SHOW_PWR) && (!(mode == TMR_PWR)) )
	 {
	  if (auto_inc_dec_ct < 1000)  // overflow protection
	  {
	  cli();
	  auto_inc_dec_ct ++;
	  sei();
	  }
	  if (auto_inc_dec_ct  > AUTO_COUNTER )
	    {
		  cli();
		  auto_inc_dec_ct  = 0;
		  sei();
		  if (percent > 0 )
		  {
		  cli();
		  percent--;
		  percent_value = NEW_VALUE;
		  sei();
		  }
		}
     }
 
  
    // auto decrement timer value if:
  // button MINUS long push AND button PLUS not pushed AND isplay == SHOW_TMR
   
   if ((button_minus_status == LONG) && (button_plus_status == NOPUSH) && ( display == SHOW_TMR) && (!(mode == TMR_PWR)))
	 {
	    if (auto_inc_dec_ct < 1000) // overflow protection
	     {
	      cli();
	      auto_inc_dec_ct ++;
	      sei();
	     }
	  
	  if (auto_inc_dec_ct  > AUTO_COUNTER )
	    {
		  cli();
		  auto_inc_dec_ct  = 0;
		  sei();
		  
		   if (time_sec > 0 )  // MAX_TIME seconds = 999 minutes
		     {
		       cli();
		       if (time_sec < 600)   // 600 seconds = 10 minutes
			    {
				  if (auto_sec_dec_cnt<255)
				  auto_sec_dec_cnt++;
				  if (auto_sec_dec_cnt < AUTO_SEC_INC_DEC_CNT)
				  time_sec-- ;         // 
			      else
				  {
				  if (time_sec>10)
				  time_sec=time_sec-10;
				  else
				  time_sec=0; // stop at 0 seconds
				  }
			   }
		       else if (time_sec < 5999) // 5999 seconds = 99 minutes 59 seconds
			   {
			   if (auto_sec_tens_dec_cnt<255)
			   auto_sec_tens_dec_cnt++;
			   if (auto_sec_tens_dec_cnt < AUTO_SEC_TENS_INC_DEC_CNT)
		       time_sec=time_sec-10; // 
			   else
			   time_sec=time_sec-60;
			   }
			   else         // if time_sec >= 100 minutes
			   {
			   if(auto_mins_dec_cnt < 255)
			   auto_mins_dec_cnt++;
			   if (auto_mins_dec_cnt < AUTO_MINS_INC_DEC_CNT)
			   time_sec=time_sec-60;   // 
			   else
			   time_sec=time_sec-600; 
			    }
			   }
			   sei();
		    }  
	 }  
   else
   {
      auto_sec_dec_cnt=0;
	  auto_sec_tens_dec_cnt=0;
	  auto_mins_dec_cnt=0;
   }
 
  // change mode (TMR,PWR or TMR_PWR) is both + and - short push
 if ((button_minus_status == SHORT) && (button_plus_status == SHORT))
     {
     mode++;
	 button_minus_status = HANDLED ;
	 button_plus_status = HANDLED ;
	 }
 if (mode>2)
    mode = 0 ;


  // select display format
 if (mode == TMR)
    display = SHOW_TMR ;

 if (mode == PWR)
    display = SHOW_PWR ;

 if ((mode == TMR_PWR) && (button_plus_status == SHORT) && (button_minus_status == NOPUSH))
     {
	 display = SHOW_TMR ;
	 button_plus_status = HANDLED ;
     }


 if ((mode == TMR_PWR) && (button_minus_status == SHORT) && (button_plus_status == NOPUSH))
     {
	 display = SHOW_PWR ;   	 
     button_minus_status = HANDLED ;
     }
 
  // If  ON and ON_OFF button pushed, mode changes to OFF
  if (( status == ON) && (button_on_off_status == SHORT))
    {
	   status = OFF ;
	   button_on_off_status = HANDLED ;
	}   
  
  // If  OFF and ON_OFF button pushed, mode changes to ON
   if (( status == OFF) && (button_on_off_status == SHORT))
    {
	   status = ON ;
	   button_on_off_status = HANDLED ;
	   percent_value = NEW_VALUE;
	}   
   
 
 }


int main(void)
{

 uint16_t number = 0;
 uint16_t minutes,seconds ;
 uint16_t temp ;
 uint8_t  first_digit_tmp ;
 uint8_t  second_digit_tmp ;
 uint8_t  third_digit_tmp ;
 
 
 init_devices();
  
 // at power-on, read saved values
 percent_value = NEW_VALUE ; 
 percent = eeprom_read_byte((uint8_t*)0x00);
 if(percent> 100) 
 percent = 100;
 
 mode = eeprom_read_byte((uint8_t*)0x01);
 if (mode>2)
 mode = TMR ;
 
 time_sec = eeprom_read_word((uint16_t*)0x02);
 if (time_sec > MAX_TIME)
 time_sec = 0;
   
 for( ; ; )
   {
      if (buttons_port_status == READED )
        {
	       check_buttons (buttons_port) ;
           status_handling();
           buttons_port_status = NOT_READED ;
		   
        }

  // calculate digits and decimal point position for 7 segment LED indicator
  
  if (display == SHOW_PWR) // if  we need to indicate power percents on display
      {
      number = percent ; // percent value directly gives number to display
	  dp_position = 0  ; // dont need to display decimal point in PWR mode 
	  }
 
 else if (display == SHOW_TMR) // if  we need to indicate minutes and seconds on display 
     {
	 
	 minutes  = time_sec / 60 ;
	 seconds = time_sec % 60 ;
	 
	 if (minutes >= 100)
	  {
	  dp_position = 3 ;
	  number = minutes ;
	  }
	  
	 else if ( (10<= minutes) && (minutes < 100))
	  {
	  dp_position = 2;
	  number = (minutes*10) + (seconds/10);
	  }
	  
	 else if (minutes < 10)
	  {
	  dp_position = 1 ;
	  number = (minutes*100) + seconds;
	  }
	 	 	 
	 }
	 
	else				/* should never go here */
     {
		status = OFF ;
     } 
  
   third_digit_tmp= number % 10; 
   temp= number / 10;
   second_digit_tmp= temp % 10;
   first_digit_tmp= temp / 10;
	
 
 if (display == SHOW_PWR)  // dont need to display leading zeros in PWR mode
  {  
   if (percent<100)
   first_digit_tmp = 10 ; // all segments off
	 
   if (percent<10)
   second_digit_tmp = 10 ;// all segments off 
  }
 
 // final result of calculations for interrrup
 cli() ;
 first_digit = first_digit_tmp;
 second_digit = second_digit_tmp;
 third_digit = third_digit_tmp;
 sei();
 
 // watchdog

 if (sanity_check == 0x4D)
       {
	   wdt_reset() ;
	   sanity_check = 0 ;
	   }

   }

  return (0);
}

