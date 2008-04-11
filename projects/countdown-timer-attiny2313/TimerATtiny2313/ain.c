 /*
    Copyright (c) 2008  Gediminas Labutis                   
   
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
      Simple timer with ATtiny2313     V1.0  
                 
   IAR Embedded Workbench for Atmel AVR KickStart 5.10A

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

// Include I/O definition file
#include <ioavr.h>
#include <intrinsics.h>

#define bit_get(p,m) ((p) & (m)) 
#define bit_flip(p,m) ((p) ^= (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m)) 
#define BIT(x) (0x01 << (x)) 

 // IO pins
#define OUT_1 3            // PD3 - output 
#define BUTTON_ON_OFF 0    // PD0 - on/off button
#define BUTTON_PLUS   1    // PD1 - + button(increase time)
#define BUTTON_MINUS  2    // PD2 - - button(decrease time)
#define CA1    6            // PD6  common anode(first digit)
#define CA2    5            // PD5  common anode(second digit)
#define CA3    4            // PD4  common anode(third digit)
#define DP    7            // PB7 - decimal point

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

#define AUTO_SEC_INC_DEC_CNT         55
#define AUTO_SEC_TENS_INC_DEC_CNT    25
#define AUTO_MINS_INC_DEC_CNT        45          

#define OFF        0
#define ON         1

#define TIME_INT_FREQ  5   // time_sec counting interrupt frequency Hz
#define MAX_TIME 59940     // 59940 seconds -> 999 minutes

unsigned char const numbers[11] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xFF} ;
                            //  0    1    2    3     4    5    6    7    8    9    OFF
unsigned char  volatile   buttons_port ;        // copy PINx here 
unsigned char  volatile   buttons_port_status ; // readed/not readed
unsigned int volatile   time_sec ;            // variable for seconds
unsigned char  volatile   time_tmp ;            // temporary variable for time counting
__eeprom unsigned int savedTime ;     // time saved in EEPROM

unsigned char   status ;  // device status - ON/OFF

// button status -> NOPUSH,PUSHED,SHORT,LONG,HANDLED
unsigned char   button_plus_status ;    
unsigned char   button_minus_status ;    
unsigned char   button_on_off_status ; 

unsigned char   dp_position; // decimal point position

// digits to display on LED indicator
unsigned char   first_digit  ;
unsigned char   second_digit ;
unsigned char   third_digit  ;

unsigned char   volatile sanity_check ; // for wdr();

void port_init(void)
{
 DDRA  = (0<<DDA2) | (0<<DDA1) | (0<<DDA0) ; // all in
 PORTA = (1<<PA2) | (1<<PA1) | (1<<PA0) ;    // pull up
 
 DDRB  = (1<<DDB7) | (1<<DDB6) | (1<<DDB5) | (1<<DDB4) | (1<<DDB3) 
 | (1<<DDB2) | (1<<DDB1) | (1<<DDB0) ;  // all out
 PORTB = (1<<PB7) | (1<<PB6) | (1<<PB5) | (1<<PB4) | (1<<PB3) | (1<<PB2) 
 | (1<<PB1) | (1<<PB0) ; // output "1"
  
 DDRD  = (1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (1<<DDD3) 
 | (0<<DDD2) | (0<<DDD1) | (0<<DDD0) ;
 // DDD6,DDD5,DDD4,DD3 - output ; DDD2,DDD1,DDD0 - input
 PORTD = (0<<PD6) | (0<<PD5) | (0<<PD4) | (0<<PD3) | (1<<PD2) 
 | (1<<PD1) | (1<<PD0) ; // output - "0", input - pull-up
}


//Watchdog initialize
// prescale: 128K (131072) cycles 1.0 s
void watchdog_init(void)
{
 __watchdog_reset() ; //this prevents a timeout on enabling
 WDTCR |= (1<<WDCE) | (1<<WDE);
 WDTCR = (1<<WDE) | (1<<WDP2) | (1<<WDP1);
 //WATCHDOG ENABLED - dont forget WDR
}


//TIMER0 initialize - prescale:256
// WGM: Normal
// desired value: 200Hz
// actual value: 200,321Hz (0,2%)
void timer0_init(void)
{
 TCCR0B = 0x00; //stop timer
 TCNT0 = 0xB2; //set count
 OCR0A = 0x4E; //set count
 OCR0B = 0x4E; //set count
 TCCR0A = 0x00; //start timer
 TCCR0B = 0x04; //start timer
}


// TIMER0 overflow interrupt
// read buttons port, output numbers to LED indicator
#pragma vector=TIMER0_OVF0_vect 
__interrupt void Timer0_overflow( void ) 
{
 unsigned char static digit_position ; // 0 - leftmost, 2 - rightmost

 TCNT0 = 0xB2; //reload counter value

 bit_set(sanity_check,BIT(0)); // for WDT

 buttons_port = PIND ;            // read buttons
 buttons_port_status = READED ;   // mark 
  
 if (digit_position > 2)         
   digit_position = 0 ;
  
  switch(digit_position)
  {
  case 0 :
  bit_clear(PORTD, BIT(CA2) | BIT(CA3));  // Q2,Q3 OFF
  PORTB = numbers[first_digit] ;
   if (dp_position == 1)
    bit_clear(PORTB,BIT(DP));
  bit_set(PORTD,BIT(CA1));               // Q1 ON
  break;
  
  case 1 :
  bit_clear(PORTD, BIT(CA1) | BIT(CA3)); // Q1,Q3 OFF
  PORTB = numbers[second_digit] ;
  if (dp_position == 2)
    bit_clear(PORTB,BIT(DP));
  bit_set(PORTD,BIT(CA2));              // Q2 ON
  break;
  
  case 2 :
  bit_clear(PORTD, BIT(CA1) | BIT(CA2)); // Q1,Q2 OFF
  PORTB = numbers[third_digit] ;
  if (dp_position == 3)
   bit_clear(PORTB,BIT(DP));
  bit_set(PORTD,BIT(CA3));              // Q3 ON
  break;
  
  default:                            // should never go here
  digit_position=0;
  bit_clear(PORTB,BIT(DP));
  status = OFF ;
  }

  digit_position++;
}



//TIMER1 initialize - prescale:256
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 5Hz
// actual value:  5,000Hz (0,0%)
void timer1_init(void)
{
 TCCR1B = 0x00; //stop timer
 TCNT1H = 0xF3; //set count value
 TCNT1L = 0xCB;
 OCR1AH  = 0x0C; //set compare value
 OCR1AL  = 0x35;
 OCR1BH  = 0x0C; //set compare value
 OCR1BL  = 0x35;
 TCCR1A = 0x00;
 TCCR1B = 0x04; //start Timer
}

// TIMER1 overflow interrupt
// time counting, output control
#pragma vector=TIMER1_OVF1_vect 
__interrupt void Timer1_overflow( void ) 
{
  
 TCNT1H = 0xF3; //reload counter high value
 TCNT1L = 0xCB; //reload counter low value

 bit_set(sanity_check,BIT(2));
   
 // time counting
 
 if (status == ON)
   {
     if (time_sec > 0)
      {
       time_tmp++;
	   if(time_tmp>=TIME_INT_FREQ)
	   {
	    time_tmp = 0;
		time_sec--;
		if(time_sec==0)
		status = OFF ; // if time out , turn off output  
	   }
      }   
     else
	 status = OFF ; // if time out , turn off output
   } 
 
 // Output control
 
  if (status == ON)
    bit_set(PORTD, BIT(OUT_1));
  else
    bit_clear(PORTD, BIT(OUT_1));
}


//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
  __disable_interrupt();
 port_init();
 watchdog_init();
 timer0_init();
 timer1_init();

 MCUCR = 0x00;
 GIMSK = 0x00;
 TIMSK = (1<<TOIE1) | (1<<TOIE0) ; //timer interrupt sources
   __enable_interrupt();
 //re-enable interrupts
 //all peripherals are now initialized
}


// check if buttons pushed
void check_buttons ( unsigned char buttons_port)
{
 
 unsigned int static button_plus_counter ;
 unsigned int static button_minus_counter ;
 unsigned int static button_on_off_counter ;
 
 bit_set(sanity_check,BIT(3));
 
 // button +
if ((bit_get(buttons_port,BIT(BUTTON_PLUS))) == 0)   // button is down ....
           {                                   
              if (button_plus_status == NOPUSH)  // is this a new push ?
	           {                                
                __disable_interrupt();
				button_plus_counter = 0;        // yes - start the debounce timer
                __enable_interrupt();
				button_plus_status = PUSHED;   // note that it's down
               }
   
              if (button_plus_status == PUSHED) // button is down - have we already noted that ?
	            {         
                   if (button_plus_counter< 1000) // overflow protection
				   {
				   __disable_interrupt();
				   button_plus_counter++;      // yes - inc the debounce timer
                   __enable_interrupt();
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
                __disable_interrupt();           
                button_minus_counter = 0;        // yes - start the debounce timer
                __enable_interrupt();
				button_minus_status = PUSHED;   // note that it's down
               }
   
              if (button_minus_status == PUSHED)  // button is down - have we already noted that ?
	            {  
                   if (button_minus_counter< 1000)   // overflow protection    
                   {
				   __disable_interrupt();
				   button_minus_counter++;         // yes - inc the debounce timer    
				   __enable_interrupt();
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
                __disable_interrupt();
				button_on_off_counter = 0;        // yes - start the debounce timer
                __enable_interrupt();
				button_on_off_status = PUSHED;   // note that it's down
               }
   
              if (button_on_off_status == PUSHED) // button is down - have we already noted that ?
	            {         
                   if (button_on_off_counter< 1000) // overflow protection 
				   {
				   __disable_interrupt();
				   button_on_off_counter++;  // yes - inc the debounce timer 
                   __enable_interrupt();  
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
  // unsigned int temp1;
   unsigned int static auto_inc_dec_ct;
   unsigned char static auto_sec_inc_cnt, auto_sec_tens_inc_cnt,auto_mins_inc_cnt,
   auto_sec_dec_cnt, auto_sec_tens_dec_cnt,auto_mins_dec_cnt;
   
   bit_set(sanity_check,BIT(6));
   
   
  // increment timer value if:
  // button PLUs short push AND button MINUS not pushed 
   if ((button_plus_status == SHORT) && (button_minus_status == NOPUSH))
	  {
	    if (time_sec < MAX_TIME )  // MAX_TIME seconds = 999 minutes
		{
		__disable_interrupt();
		 if (time_sec < 600)   // 600 seconds = 10 minutes
		  time_sec++ ;         // add 1 second
		   else if (time_sec < 5999) // 5999 secconds = 99 minutes 59 seconds
		    time_sec=time_sec+10; // add 10 seconds
			 else         // if time_sec >= 100 minutes
			  time_sec=time_sec+60; // add 60 seconds
		__enable_interrupt();
		}
		button_plus_status = HANDLED ;
	  }  
	  
	  
  // decrement time_sec value if:
  // button MINUS short push AND button PLUS not pushed 
  if ((button_minus_status == SHORT) && (button_plus_status == NOPUSH))
	 {
	 if (time_sec > 0 )  // 
		{
		__disable_interrupt();
		 if (time_sec < 600)   // 600 seconds = 10 minutes
		  time_sec-- ;         // subtract 1 second
		   else if (time_sec < 5999) // 5999 secconds = 99 minutes 59 seconds
		    time_sec=time_sec-10; // subtract 10 seconds
			 else         // if time_sec >= 100 minutes
			  time_sec=time_sec-60; // subtract 60 seconds
		__enable_interrupt();
		}
		button_minus_status = HANDLED ;
	  }  
 

   // auto incremet timer value if:
  // button PLUS long push AND button MINUS not pushed 
     
   if ((button_plus_status == LONG) && (button_minus_status == NOPUSH))
	 {
	    if (auto_inc_dec_ct < 1000) // overflow protection
	     {
	      __disable_interrupt();
	      auto_inc_dec_ct ++;
	      __enable_interrupt();
	     }
	  
	  if (auto_inc_dec_ct  > AUTO_COUNTER )
	    {
		  __disable_interrupt();
		  auto_inc_dec_ct  = 0;
		  __enable_interrupt();
		  
		   if (time_sec < MAX_TIME )  // MAX_TIME seconds = 999 minutes
		     {
		       __disable_interrupt();
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
			   __enable_interrupt();
		     }
		}  
	 } 
     else
	   {
	   auto_sec_inc_cnt=0;
	   auto_sec_tens_inc_cnt=0;
	   auto_mins_inc_cnt=0;
	   }
 
     
  // auto decrement timer value if:
  // button MINUS long push AND button PLUS not pushed 
     
   if ((button_minus_status == LONG) && (button_plus_status == NOPUSH))
	 {
	    if (auto_inc_dec_ct < 1000) // overflow protection
	     {
	      __disable_interrupt();
	      auto_inc_dec_ct ++;
	      __enable_interrupt();
	     }
	  
	  if (auto_inc_dec_ct  > AUTO_COUNTER )
	    {
		  __disable_interrupt();
		  auto_inc_dec_ct  = 0;
		  __enable_interrupt();
		  
		   if (time_sec > 0 )  // MAX_TIME seconds = 999 minutes
		     {
		       __disable_interrupt();
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
			   __enable_interrupt();
		    }  
	 }  
   else
   {
      auto_sec_dec_cnt=0;
	  auto_sec_tens_dec_cnt=0;
	  auto_mins_dec_cnt=0;
   }
 
 
  
  // write time to eeprom is both + and - short push 
 
   if ((button_minus_status == SHORT) && (button_plus_status == SHORT))
     {    
          __disable_interrupt();
          savedTime =  time_sec;
          __enable_interrupt();
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
	}   
   
 
 }



int main(void)
{
 unsigned int number = 0;
 unsigned int minutes,seconds ;
 unsigned int temp ;
 unsigned char  first_digit_tmp ;
 unsigned char  second_digit_tmp ;
 unsigned char  third_digit_tmp ;

 init_devices();
  
  // at power-on, read saved time from EEPROM
 time_sec = savedTime;
 if (time_sec > MAX_TIME)
 time_sec = 0;
 status = OFF ;
 
  while (1)
  {
    if (buttons_port_status == READED )
     {
      check_buttons (buttons_port) ;
      status_handling();
      buttons_port_status = NOT_READED ;
     }

// calculate digits and decimal point position for 7 segment LED indicator
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
	 	 	 
	 
   third_digit_tmp= number % 10; 
   temp= number / 10;
   second_digit_tmp= temp % 10;
   first_digit_tmp= temp / 10; 

    // final result of calculations for interrupt
  __disable_interrupt();
 first_digit = first_digit_tmp;
 second_digit = second_digit_tmp;
 third_digit = third_digit_tmp;
 __enable_interrupt();
 
 // watchdog

 if (sanity_check == 0x4D)
       {
	__watchdog_reset() ;
	sanity_check = 0 ;
       }


  }
}
