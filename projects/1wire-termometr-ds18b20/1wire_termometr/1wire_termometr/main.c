#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <stdio.h>
// MCU at90s8535 (7.37Mhz)
//#define   HZ 7372800L
// Paterns UART 
//BUF_SIZE == 2^,115200 bod - 3
#define BUF_SIZE  16  
#define MASK	(BUF_SIZE-1)
#define B115200 3

// Message "Error CRC" == -80
#define ERROR_CRC 175
//Location 1 Wire Net in PORT B pin 0.
#define OW_DQ   PB0
#define OW_PIN  PINB
#define OW_DDR  DDRB
#define OW_PORT PORTB
// 1 Wire Commands
#define SKIP_ROM  0xCC
#define CONVERT_T 0x44
#define READ_SCRATCHPAD 0xBE

volatile uint8_t delay_done;
volatile uint8_t transmit_buf[BUF_SIZE];
volatile uint8_t transmit_in; //init 0
volatile uint8_t transmit_out; // init 0

/********************************************************************************************* 
 *  Function name : OW_ComputeCRC8
 *	  
 *  Purpose :Compute the CRC8 value of a data set.
 *	     This function will compute the CRC8 or DOW-CRC of inData using seed
 *  	     as inital value for the CRC.
 *
 *  Parameters:	 inData  One byte of data to compute CRC from.
 *
 *  	 	seed    The starting value of the CRC.
 *
 *  Return: The CRC8 of inData with seed as initial value.
 *
 *  Note:   Setting seed to 0 computes the crc8 of the inData.
 *          Constantly passing the return value of this function 
 *          As the seed argument computes the CRC8 value of a
 *          longer string of data.
 ************************************************************************************************ */
unsigned char OW_ComputeCRC8(unsigned char inData, unsigned char seed)
{
    unsigned char bitsLeft;
    unsigned char temp;

    for (bitsLeft = 8; bitsLeft > 0; bitsLeft--)
    {
        temp = ((seed ^ inData) & 0x01);
        if (temp == 0)
        {
            seed >>= 1;
        }
        else
        {
            seed ^= 0x18;
            seed >>= 1;
            seed |= 0x80;
        }
        inData >>= 1;
    }
    return seed;    
}
/*********************************************************************************************
*
*	Function name : Delay
*
*	Returns :	None
*
*	Parameters :	unsigned int millisec
*
*	Purpose :	Delay-loop
*
*********************************************************************************************/
inline void  Delay( unsigned int millisec )
{
	while( millisec-- )
	 for(uint8_t i = 0;i < 125;  i++)
	  asm volatile ("nop"::);
}
/****************************************************************************************************** 
* 
*   Function name : start_timer0
*
*   Returns :        none
*                       
*   Parameters :     none
*
*   Purpose :     Routine to stop the timer
*
*******************************************************************************************************/
inline void start_timer0(const uint8_t  div, const uint8_t inital_value) {
    cli(); // stop all interrupts
    
    // set the timer count to its initial value
    TCNT0 = inital_value;
    // set the timer clock divisor
    TCCR0 = div ;
    // enable the timer overflow interrupt
    TIMSK |= (1<<TOIE0); 
    
    sei(); // start all interrupts, which will start the timer
}
/****************************************************************************************************** 
* 
*   Function name :  msdelay
*
*   Returns :        none
*                       
*   Parameters :     uint16_t ms
*
*   Purpose :     Routine  the timer
*
*******************************************************************************************************

inline  void msdelay(const uint16_t ms) {
    // This routine is equivalent to udelay, except it functions in
    // milliseconds and uses less CPU time all the while. (and can delay for
    // longer times, up to 65k msec)
    uint8_t MS_CLOCKDIV = 2;
    uint16_t wait_cycles = ((HZ/(8 * 1000))) * ms;
    uint8_t wait_low = wait_cycles & 0xFF;
    uint8_t wait_high = wait_cycles>>8;
    delay_done = wait_high + 1;
    start_timer0(MS_CLOCKDIV, (255-wait_low));
    while (delay_done != 0);
    
/****************************************************************************************************** 
* OK
*
*   Function name :  udelay
*
*   Returns :        none
*                       
*   Parameters :     uint16_t us
*
*   Purpose :     Routine the timer0 (min.index = 10 (6,75um))
*
*******************************************************************************************************/

void udelay(uint16_t us) 
{
    // figure out how many timer overflow cycles we should wait, as well as
    // what the inital value of the timer should be.
    uint8_t  US_CLOCKDIV = 1;
    if (us<9) us=9; us-=9; 
    uint16_t wait_cycles = 6 * us;
    // delay_done is the count of how many cycles we'd like to wait. We
    // initalize it here, and its decremented in the timer interrupt. 
    delay_done = (wait_cycles>>8) + 1;
    start_timer0(US_CLOCKDIV, (~(wait_cycles & 0xFF)));
    // wait for delay_done to reach zero 
    while (delay_done);
}

/*********************************************************************************************
* OK
*	Function name : reset_1wire
*
*	Returns :	none
*
*	Parameters :	none
*
*	Purpose :	Reset 1-Wire net
*
*********************************************************************************************/
uint8_t OW_reset(void)
 {
  uint8_t answer;
//port B input, PB0 set 0
     OW_PORT &= ~(1<<OW_DQ); //set 0 in PORTB, bit PB0
     OW_DDR &= ~(1<<OW_DQ); // set 0 in DDRB line DQ -> 1
 
//TRANSMIT RESET PULSE.     
     OW_DDR |=(1<<OW_DQ); //set 1 in DDRB line DQ -> 0
     udelay(800); //~506,25us timeslot 480 us minimum (see datasheet to DS18B20)
     OW_DDR &= ~(1<<OW_DQ); //set0 in DDRB line DQ -> 1
	 
//RECEIVE RESET PULSE AND DETECT PRESENCE PULSE.
    udelay(100);  //index 100=~62,5-67,5us(63-78) timeslot  index 30 = ~20us (15-60) us 
    answer = (OW_PIN & (1<<OW_DQ));
// Complete the reset sequence recovery
    udelay(350); //~240us (timeslot 240 us minimum )
	return (answer);     
 }
/*********************************************************************************************
*OK
*	Function name :  write_1wire
*
*	Returns :	none
*
*	Parameters :	uint8_t bit
*
*	Purpose :	Write bit in the 1-Wire net
*
*********************************************************************************************/
void OW_write_bit(uint8_t bit)
 {
     OW_PORT &= ~(1<<OW_DQ); //set 0 in PORTB, bit PB0
     OW_DDR &= ~(1<<OW_DQ); //set 0 in DDRB, line DQ -> 1
    //Send 0 to 1wire (bit == 0)
    if (bit == 0) 
    {
        OW_DDR |= (1<<OW_DQ); //set 1 in DDRB line DQ->0
        udelay(100); //~67.5us hold low for at least 60us for 0
        OW_DDR &= ~(1<<OW_DQ);//set 0 in DDRB, line DQ -> 1
	udelay (15);// 10us recovery time
    } 
    else // Send 1 to 1wire (bit == 1)
    { 
	OW_DDR |= (1<<OW_DQ);// set 1 in DDRB, line DQ->0
        udelay(10); //~6,75us < 15us for 1 
        OW_DDR &= ~(1<<OW_DQ);// set 0 in DDRB, line DQ -> 1
	udelay(100);//~67.5us Complete the time slot and 10us recovery
    }

 }
/*********************************************************************************************
*OK
*	Function name :  read_1wire
*
*	Returns :	 uint8_t result
*
*	Parameters :	none
*
*	Purpose :	Read to 1-Wire net
*
*********************************************************************************************/
 uint8_t OW_read_bit(void) 
{
    uint8_t result; 
    OW_PORT &= ~(1<<OW_DQ); //set 0 in PORTB, bit PB0
    OW_DDR &= ~(1<<OW_DQ); //set 0 in DDRB line DQ -> 1
    
// hold PB0 low  6,75 us
    OW_DDR |= (1<<OW_DQ);//set 1 in DDRB line DQ->0
    udelay (10); //6,75us
    
// hold PB0 high  10 us
    OW_DDR &= ~(1<<OW_DQ);// set 0 in DDRB line DQ -> 1
    udelay (15);//~10us

    result = (OW_PIN & (1<<OW_DQ)); //check  1st pin ( PB0 )
    udelay(180); // make sure the slot is >60us
 
    return result; 
}
/**************************************************************************************************** 
* OK
*
*   Function name :   write_1wire_byte
*
*   Returns :       none
*                       
*   Parameters :     uint8_t command
*
*   Purpose :     Write byte to 1Wire
*
*******************************************************************************************************/
 void OW_write_byte(uint8_t command) 
 {
    for (uint8_t i = 0; i < 8; i++)
     { 
         OW_write_bit(command & 0x01); //send one bit 
         command >>= 1; //shift to next bit
     }
 }
/**************************************************************************************************** 
* OK
*
*   Function name :   read_1wire_byte
*
*   Returns :       uint8_t  r
*                       
*   Parameters :     none
*
*   Purpose :     Read byte to 1Wire
*
*******************************************************************************************************/
 uint8_t OW_read_byte(void) 
 {
    uint8_t r = 0;
    for (uint8_t i = 0; i < 8; i++) 
    {
        r |= (OW_read_bit() << (i)); //shift recive bit locate to r
    }
    return r; 
 }
 //----------------------------------------------------------------------------- 
 // Write a 1-Wire data byte and return the sampled result. 
 //
/*  uint8_t OW_TouchByte(uint8_t data)
   { 
    uint8_t loop, result=0;
    for (loop = 0; loop < 8; loop++)
     {
      // shift the result to get it ready for the next bit 
      result >>= 1;
       // If sending a  1  then read a bit else write a  0
         if (data & 0x01) 
	 { 
	 if (OW_read_bit()) result |= 0x80;
	  } else OW_write_bit(0);
	   // shift the data byte for the next bit 
	   data >>= 1; 
	   } return result; 
  }
  */
/****************************************************************************************************** 
* 
*   Function name : stop_timer0
*
*   Returns :        none
*                       
*   Parameters :     none
*
*   Purpose :     Routine to stop the timer
*
*******************************************************************************************************
 // Routine to stop the timer. 
inline void stop_timer0(void)
 {
  // stop the timer by setting CS00, CS01, CS02 all to zero     
  
  TCCR0 &= ~(1<<CS00);
  TCCR0 &= ~(1<<CS01);
  TCCR0 &= ~(1<<CS02);
  // disable the timer overflow interrupt
  TIMSK &= ~(1<<TOIE0);  
 }
 
*/
/******************************************************************************
*
*	  Interrupt Routine: Timer0 Overflow
*
*	Purpose: Delay counter
*
*******************************************************************************/

// This interrupt (Timer0 Overflow) will happen when the Timer0 counter
// rolls over from 255 to 0.  Remember that Timer0 is an 8-bit timer.   
INTERRUPT(SIG_OVERFLOW0) 
{
    delay_done--;
}

/****************************************************************************************************** 
* 
*   Function name :  read_romcode
*
*   Returns :        none
*                       
*   Parameters :     none
*
*   Purpose :     Read ROM to 1wire device
*
*******************************************************************************************************/
/*
void read_romcode(void)
 {
    uint8_t a,b,c,d;
    a=b=c=d=0;
    reset_onewire();
    write_onewire_byte(0x33);
    
    a = read_byte();
    b = read_byte();
    c = read_byte();
    d = read_byte();

  
}
*/
/****************************************************************************************************** 
* 
*   Function name :  convert_temp
*
*   Returns :        none
*                       
*   Parameters :     none
*
*   Purpose :     Start convert temperature
*
*******************************************************************************************************/
void convert_temp(void) 
{
    OW_reset();
    OW_write_byte(SKIP_ROM); // skip ROM command (0xCC)
    OW_write_byte(CONVERT_T); // convert T (0x44)
}
/****************************************************************************************************** 
* 
*   Function name :  Read_temp
*
*   Returns :        temperature
*                       
*   Parameters :     none
*
*   Purpose :     Get temperature
*
*******************************************************************************************************/
uint8_t read_temp(void) 
{
    uint8_t scratchpad[9];
    uint8_t crc = 0;
    // initiate array to 0
    for (uint8_t a = 0; a < 9; a++) scratchpad[a] = 0; 
    Delay (1000); //time measurement temperature in DS18B20 (780ms)
    OW_reset();
    OW_write_byte(SKIP_ROM); // skip ROM
    OW_write_byte(READ_SCRATCHPAD); // read scratchpad 0xBE
 //Recive data  in 1 Wire 
    for (uint8_t c = 0; c < 9; c++) 
    {
        scratchpad[c] = OW_read_byte();
    }
 //Check CRC8  to 1 Wire data   
    for(uint8_t i = 0; i < 8; i++)
      {
         crc =  OW_ComputeCRC8(scratchpad[i], crc);
       }
       if (crc == scratchpad[8])
       {
       	return  ((scratchpad[1] << 4) | (scratchpad[0] >> 4));
        }
	return (ERROR_CRC); // Message -80
}

/******************************************************************************
*
*	  Interrupt Routine data register empty in UART
*
*	Purpose: indicating next char can be transmitted
*
*******************************************************************************/

SIGNAL(SIG_UART_DATA) 
{
	/* Data register empty indicating next char can be transmitted */
	if(transmit_in != transmit_out)
	{
	 UDR = transmit_buf[transmit_out & MASK]; //Send data to out.Counter in byte (transmit_out & MASK) send out bytes.
		transmit_out++;	
	}
	else 
	{
	  UCR &= ~(1<<UDRIE); // Clear flag UDR empty interrupt
	}
}
/*********************************************************************************************
*
*	Function name : transmit_buf_length
*
*	Returns :	counter length buffer
*
*	Parameters :	none
*
*	Purpose :	counter length buffer
*
*********************************************************************************************/
inline uint8_t transmit_buf_length(void)
 {
	return(transmit_in - transmit_out);
 }

/*********************************************************************************************
*
*	Function name : UART_putchar
*
*	Returns :	0
*
*	Parameters :	char c
*
*	Purpose :	put char in UART
*
*********************************************************************************************/
 int UART_putchar(char c) 
 {
	/* Fills the transmit buffer, if it is full wait */
	while(( BUF_SIZE - transmit_buf_length()) <= 2);
	
	/* Add data to the transmit buffer */
	transmit_buf[transmit_in & MASK] = c; //(transmit_in & MASK) counter in one byte
	transmit_in++;
	
	UCR |= (1<<UDRIE);// Enable UDR empty interrupt
	
	return(0);
}
/******************************************************************************
*	Function name: UART_init
*
*	returns:	none
*
*	parameters:  	none
*
*	Purpose: Start first init UART .
*
*******************************************************************************/
 void UART_init(void) {
	/* First init for the UART */
	// 115200 BPS - 3,
	UBRR = B115200;	
	// 8 Databits, receive and transmit enabled
	UCR = (1<<TXEN)|(1<<RXEN)|(1<<RXB8)|(1<<TXB8);	
	
	fdevopen(UART_putchar,0, 0); // initial stdio
				
}
/*********************************************************
*
*		MAIN
*
*********************************************************/
int main(void) 
{
  UART_init();
  
       do
 	{
       
	uint8_t celcius;
        convert_temp();
        celcius = read_temp();
//	printf ("%X\r\n",celcius);
    
	 if( ( celcius & 0x80 ) == 0x80) //mask 0x80 0b10000000
	 {
	  
	  celcius = (~celcius)+1;
	  printf ("-%d \r\n",celcius);
	 }	
	 else
	 {
	  printf ("+%d \r\n",celcius);
	  } 		
       } 
	    while (1) ;
  }     