 /*
    Copyright (c) 2009  Gediminas Labutis                   
   
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
             1-Wire Temperature Alarm     V1.0  
                 
              Compiler - WinAVR 20081205

  1-Wire code from Atmel appnote
  AVR318: Dallas 1-Wire master 
              
*/

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>
#include <util/delay.h>
#include "OWI.h" 


#define BUSES   (OWI_PIN_0) //!< Buses to search.

#define HighLimitPort PIND   // High limit switch port 
#define LowLimitPort  PINB   // Low limit switch
#define SignPort      PINC   // Sing 
#define LLSignPin     PIN5   // Low level sign pin
#define HLSignPin     PIN4   // High level sign pin
#define Ind_Port      PORTC  // indication port
#define LLED          PIN2   // Low level LED
#define HLED          PIN3   // High level LED
#define Buzzer        PIN1   // Buzzer 

#define bit_get(p,m) ((p) & (m)) 
#define bit_set(p,m) ((p) |= (m)) 
#define bit_clear(p,m) ((p) &= ~(m)) 
#define bit_flip(p,m) ((p) ^= (m)) 
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m)) 
#define BIT(x) (0x01 << (x)) 
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))

#define FAMILY_DS18B20 0x28
#define FAMILY_DS18S20 0x10

#define OK 0
#define HIGH_LIMIT   1
#define LOW_LIMIT    2
#define NO_SENSOR    3
#define WRONG_LIMITS 4


int8_t temperature = 0 ;         // measured temperature
volatile int8_t low_limit;
volatile int8_t high_limit;
volatile int8_t status;          // 0-OK 1-HIGH_LIMIT 2-LOW_LIMIT 3-NO_SENSOR 4-WRONG_LIMITS
volatile int8_t check_temp ;     // 1-time to check temperature
volatile uint8_t  sanity_check ; // for wdr();

int abs  (int i); // to avoid warning 

/* Get temperature limits
 * sw_port -> 2 rotary switches (0-9 with real code,lower nibble *10, upper nibble *1)
 * all sw_port bits inverted
 * s_port  -> sign swith "0" - negative "1" - positive
 * limit = ((lower_nibble *10) + higher_nibble) * sign
*/
 int8_t get_limit(volatile uint8_t *sw_port, volatile uint8_t *s_port, uint8_t s_pin) 
 {
  if (bit_get(*s_port, BIT(s_pin)))
  return ((*sw_port ^ 0xFF) & 0x0F) * 10 + ((*sw_port ^ 0xFF) >> 4);
  else
  return (((*sw_port ^ 0xFF) & 0x0F) * 10 + ((*sw_port ^ 0xFF) >> 4)) * -1 ;
 }

/* Check status */

int8_t check_status (int8_t low_limit, int8_t high_limit, int8_t temperature)
 {
  if (temperature == -127) 
    return NO_SENSOR;
  else if ((low_limit>=high_limit) || ( abs(low_limit) > 99) || (abs(high_limit) > 99)) 
    return WRONG_LIMITS;
  else if (temperature < low_limit)
    return LOW_LIMIT;
  else if (temperature > high_limit)
    return HIGH_LIMIT;
  else 
    return OK;	
 }

// for debugging
/*
static int uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,_FDEV_SETUP_WRITE); //Error here

static int uart_putchar(char c, FILE *stream) 
{ 
   if (c == '\n') 
      uart_putchar('\r', stream); 
   loop_until_bit_is_set(UCSRA, UDRE); 
   UDR = c; 
   return 0; 
}
*/

//Watchdog initialize
// prescale: 2048K
void watchdog_init(void)
{
 wdt_reset() ; //this prevents a timeout on enabling
 WDTCR |= (1<<WDCE) | (1<<WDE);
 WDTCR = 0x0F; //WATCHDOG ENABLED - dont forget to issue WDRs
}


void port_init(void)
{
 PORTB = 0xFF;
 DDRB  = 0x00;
 PORTC = 0x71; 
 DDRC  = 0x0E;
 PORTD = 0xFF;
 DDRD  = 0x00;
}

//TIMER0 initialize - prescale:1024
// desired value: 20Hz
// actual value: 20,032Hz (0,2%)
void timer0_init(void)
{
 TCCR0 = 0x00; //stop
 TCNT0 = 0x3D; //set count
 TCCR0 = 0x05; //start timer
}

///TIMER1 initialize - prescale:1024
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 1Sec
// actual value:  1,000Sec (0,0%)
void timer1_init(void)
{
 TCCR1B = 0x00; //stop
 TCNT1H = 0xF0; //setup
 TCNT1L = 0xBE;
 OCR1AH = 0x0F;
 OCR1AL = 0x42;
 OCR1BH = 0x0F;
 OCR1BL = 0x42;
 ICR1H  = 0x0F;
 ICR1L  = 0x42;
 TCCR1A = 0x00;
 TCCR1B = 0x05; //start Timer
}


SIGNAL(SIG_OVERFLOW0)
{
 //TIMER0 has overflowed
  TCNT0 = 0x3D; //reload counter value

 // code executed each 0.5s (2Hz)
 // Timer owerflow interrupt 20 Hz

 uint8_t static countExt,countInt ; 
 countExt++;
 
 bit_set(sanity_check,BIT(0));
 
 if (countExt > 9)
 {
 countExt = 0;
 
 switch(status)
  {
  case HIGH_LIMIT:
  bit_clear(Ind_Port, BIT(LLED)); // Low limit LED off
  bit_flip (Ind_Port, BIT(HLED)); // Blink high limit LED
  if bit_get(Ind_Port, BIT(HLED))
     bit_set(Ind_Port, BIT(Buzzer));
	else bit_clear(Ind_Port, BIT(Buzzer));
  break;
  
  case LOW_LIMIT:
  bit_clear(Ind_Port, BIT(HLED)); // High limit LED off
  bit_flip (Ind_Port, BIT(LLED)); // Blink low limit LED
  if bit_get(Ind_Port, BIT(LLED))
     bit_set(Ind_Port, BIT(Buzzer));
	else bit_clear(Ind_Port, BIT(Buzzer));
  break;
  
  case WRONG_LIMITS :
  bit_set(Ind_Port, BIT(HLED) |BIT(LLED) ); // LED's ON
  bit_clear(Ind_Port, BIT(Buzzer));         //  Buzzer OFF
  break;
  
  case NO_SENSOR :
  bit_set(Ind_Port, BIT(Buzzer));         //  Buzzer ON
  bit_flip (Ind_Port, BIT(HLED));         // Blink both LED's
  if bit_get(Ind_Port, BIT(HLED))
     bit_set(Ind_Port, BIT(LLED));
	else bit_clear(Ind_Port, BIT(LLED));
  break;
  
  case OK :
  countInt++;
  if (countInt>1)
   {
    countInt = 0;
	bit_clear(Ind_Port, BIT(Buzzer));
	bit_flip (Ind_Port, BIT(HLED));         // Blink both LED's
    if bit_get(Ind_Port, BIT(HLED))
     bit_set(Ind_Port, BIT(LLED));
	else bit_clear(Ind_Port, BIT(LLED));
   }
  break;
  
  
  default:                            // should never go here
  bit_set(Ind_Port, BIT(Buzzer));
  bit_set(Ind_Port, BIT(HLED));
  bit_set(Ind_Port, BIT(LLED));
  }
 }
 
}



SIGNAL(SIG_OVERFLOW1)
{
  //TIMER1 has overflowed
 TCNT1H = 0xF0; //reload counter high value
 TCNT1L = 0xBE; //reload counter low value
 
 uint8_t static count; 
 
 count++;
 bit_set(sanity_check,BIT(2));
 
 if (count> 4)
  {
   check_temp = 1;
   count = 0;
//   printf("Temperatura: %d \n", temperature);
  }
 
}

/*
//UART0 initialize
// desired baud rate: 9600
// actual: baud rate:9615 (0,2%)
void uart0_init(void)
{
 UCSRB = 0x00; //disable while setting baud rate
 UCSRA = 0x00;
 UCSRC = 0x86;
 UBRRL = 0x19; //set baud rate lo
 UBRRH = 0x00; //set baud rate hi
 UCSRB = 0x08;
}
*/

//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 port_init();
 timer0_init();
 timer1_init();
// uart0_init();
 watchdog_init();

 MCUCR = 0x00;
 GICR  = 0x00;
 TIMSK = 0x05; //timer interrupt sources
 sei(); //re-enable interrupts
 //all peripherals are now initialized

 
}


 int8_t DS1820_ReadTemperature(uint8_t bus)
{
    
	uint8_t tmp1;
	uint8_t tmp2;
	static uint8_t FAMILY_CODE;
    
    // Reset, presence.
    if (!OWI_DetectPresence(bus))
    {
        return -127; // Error
    }
    // Read ROM to check family code
	OWI_SendByte(OWI_ROM_READ, bus);
    FAMILY_CODE = OWI_ReceiveByte(bus); 
	
    if (!OWI_DetectPresence(bus))
    {
        return -127; // Error
    }
    
    OWI_SendByte(OWI_ROM_SKIP, bus);
    // Send start conversion command.
    OWI_SendByte(OWI_START_CONVERSION, bus);
    // Wait until conversion is finished.
    // Bus line is held low until conversion is finished.
    while (!OWI_ReadBit(bus))
    {
    
    }
    // Reset, presence.
    if(!OWI_DetectPresence(bus))
    {
        return -127; // Error
    }
    
    OWI_SendByte(OWI_ROM_SKIP, bus);
    // Send READ SCRATCHPAD command.
    OWI_SendByte(OWI_READ_SCRATCHPAD, bus);
 
   // Read only two first bytes (temperature low, temperature high)
    tmp1 = OWI_ReceiveByte(bus);
	tmp2 = OWI_ReceiveByte(bus);
	
	
	switch(FAMILY_CODE)
	{
     case FAMILY_DS18S20 :
     return  ((tmp2 & 0x80) | (tmp1 >> 1)); // DS18S20
     break;
	 
	 case FAMILY_DS18B20 :
     return  ((tmp2 << 4) | (tmp1 >> 4)); // DS18B20
     break;
   
     default:                            // should never go here
     return -127; // Error
    }
    
    
}  


//
int main(void)
{
 init_devices();
 
// stdout = &mystdout; 

// on reset all outputs set to "1".

 OWI_Init(BUSES);
 bit_set(Ind_Port, BIT(LLED));
 bit_set(Ind_Port, BIT(HLED));
 bit_set(Ind_Port, BIT(Buzzer));

 
 while(1)
 {
     if (check_temp)
	  {
       low_limit = get_limit(&LowLimitPort,&SignPort,LLSignPin);
       high_limit = get_limit(&HighLimitPort,&SignPort,HLSignPin); 
	   temperature = DS1820_ReadTemperature(BUSES) ;
	   status = check_status (low_limit, high_limit,temperature);
	   check_temp = 0 ;
	  }
    
	// watchdog

   if (sanity_check == 0x05)
       {
	   wdt_reset() ;
	   sanity_check = 0 ;
	   }

 }
 
 
 return 0;
 
}
