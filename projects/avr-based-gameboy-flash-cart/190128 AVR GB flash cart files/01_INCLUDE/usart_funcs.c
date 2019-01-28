#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "usart_funcs.h"

// FUNCTION PROTOTYPES
// Set up the usart
void usart_init(uint16_t baud);
// Send data to the usart buffer
// FOR SETTING UP VERY FAST BAUDS. CAUSES TRUNCATION OTHERWISE! BUT VERY SLOW USING 32-bit VALUES!!!
void usart_init_Xlarge(uint32_t baud);
void usart_transmit(unsigned char data);
// Receives data
char usart_receive(void);
// Send a string to the usart buffer
void usart_transmit_str(const char* data);
// Send a string from PROGMEM the usart buffer
void usart_transmit_str_P(const char* data);
// Send an integer to the usart buffer
void usart_transmit_int(int16_t num);
// Send a big integer to the usart buffer
void usart_transmit_int32(uint32_t num);
// Disable usart - just a test function. not sure if needed
void usart_rec_disable(void);
// Enable usart - just a test function. not sure if needed
void usart_rec_enable(void);
// Clear Hyperterminal Screen
void usart_clear_ht(void);
// Move cursor to position x,y on the hyperterminal screen
void move_cursor(int x, int y);
// returns true if there's data ready to be taken from UDR0
uint8_t usart_data_received(void);

///////////////////////////////////////////////////////////
/////// USART 1 FUNCTION PROTOTYPES////////////////////////
///////////////////////////////////////////////////////////
#ifdef __AVR_ATmega644P__
void usart1_init(uint16_t baud);
void usart_init_Xlarge(uint32_t baud);
void usart1_transmit(unsigned char data);
char usart1_receive(void);
void usart1_transmit_str(const char* data);
void usart1_transmit_int32(uint32_t num);
#endif

int (*_rx_func)(unsigned char var);

// TEST FUNCTION FLOGGED FROM SOME RANDOM CODE
void usart_transmit_dec( unsigned char value );

void usart_init(uint16_t baud)
{
/*	//DEBUG!!!!!!!!!
	// Set baud rate: 19200bps when @ 8Mhz
	UBRR0H = (unsigned char)(25>>8);
	UBRR0L = (unsigned char)25;
*/
	// Calculate the baud value
	uint16_t num = (uint32_t)F_CPU / (16 * (uint32_t)baud) - 1;
	
	// Set baud rate 
	UBRR0H = (unsigned char)(num>>8);
	UBRR0L = (unsigned char)num;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

void usart_transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)))
	{
	}
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

char usart_receive(void)
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) )
	{
	}
	/* Get and return received data from buffer */
	return UDR0;
}

void usart_transmit_str(const char* data)
{
	// There's still data to go so keep sending it until it ends
	while(*data)
	{
		usart_transmit(*data);
        data++;
	}
}

void usart_transmit_str_P(const char* data)
{
  while (pgm_read_byte(data) != 0x00) {
	  usart_transmit(pgm_read_byte(data++));
  }
}

void usart_transmit_int(int16_t num)
{
	char buffer[10];
    uint16_t n, i;
    for(n = 10000, i = 0; n > 0; n /= 10, i++)
    {
        buffer[i] = ((num / n) % 10) + '0';
    }
    buffer[i] = '\0';
    usart_transmit_str(buffer);

}

void usart_transmit_int32(uint32_t num)
{
	char buffer[11];
  uint8_t i = 0;

  buffer[0] = '0';

  if(num == 0) {
    i = 1; // Need to push at least one value out
  }

  while(num > 0) {
    buffer[i] = num % 10 + '0';
    num /= 10;
    i++;
  }

  // Now we've converted it in reverse, pump it back out correctly
  for(; i > 0; i--) {
    usart_transmit(buffer[i - 1]);
  }
  usart_transmit('\0');
}

void usart_rec_disable(void)
{
	UCSR0B |= (0<<RXEN0);
}

void usart_rec_enable(void)
{
	UCSR0B |= (1<<RXEN0);
}

void usart_clear_ht(void)
{
	usart_transmit_str("\x1b[2J");
}

void move_cursor(int distance, int direction) 
{
	usart_transmit('\x1b');
	usart_transmit('[');

	usart_transmit_dec(distance);
	usart_transmit(direction);
}

void cursor_goto(int x, int y)
{
	usart_transmit('\x1b');
	usart_transmit('[');

	usart_transmit_dec(y);
	usart_transmit(';');
	usart_transmit_dec(x);
	usart_transmit('H');
}

void save_cursor_pos(void)
{
    usart_transmit('\x1b');
	usart_transmit('[');

    usart_transmit( 's' );
}

void restore_cursor_pos(void)
{
    usart_transmit('\x1b');
	usart_transmit('[');

    usart_transmit( 'u' );
}

void erase_screen_bottom(void)
{
    usart_transmit('\x1b');
	usart_transmit('[');

    usart_transmit( 'J' );
}

void erase_screen_top(void)
{
    usart_transmit('\x1b');
	usart_transmit('[');         
    
    usart_transmit( '1' );
    usart_transmit( 'J' );
}

void erase_screen_endline(void)
{
    usart_transmit('\x1b');
	usart_transmit('[');
    
    usart_transmit( 'K' );
}

void erase_screen_startline(void)
{
    usart_transmit('\x1b');
	usart_transmit('[');
    
    usart_transmit( '1' );
    usart_transmit( 'K' );
}

void erase_screen_line(void)
{
    usart_transmit('\x1b');
	usart_transmit('[');
    
    usart_transmit( '2' );
    usart_transmit( 'K' );
}

void ht_set_display_colour( unsigned char fg_bg, unsigned char colour )
{
    usart_transmit('\x1b');
	usart_transmit('[');
    
    usart_transmit( fg_bg );                 // Select foreground/background
    usart_transmit( colour );
    usart_transmit( 'm' );
}

void ht_set_display_attribute_mode( unsigned char mode )
{
    usart_transmit('\x1b');
	usart_transmit('[');
    
    usart_transmit( mode );
    usart_transmit( 'm' );
}

void ht_scroll_all(void)
{
    usart_transmit('\x1b');
	usart_transmit('[');
    
    usart_transmit( 'r' );
}

void ht_scroll_limit( unsigned char start, unsigned char end )
{
    usart_transmit('\x1b');
	usart_transmit('[');
    
    usart_transmit_dec( start );            // Convert start line byte
    usart_transmit( ';' );
    usart_transmit_dec( end );              // Convert end line byte
    usart_transmit( 'r' );
}

void ht_print_screen(void)
{
    usart_transmit('\x1b');
	usart_transmit('[');                         // Send escape sequence start
   
    usart_transmit( 'i' );
}

void draw_horizontal_line(int y, int startx, int endx) 
{
	int i;
	cursor_goto(startx, y);
	ht_set_display_attribute_mode(MODE_REVERSED);
	for(i=startx; i <= endx; i++) 
	{
		usart_transmit(' ');
	}
	ht_set_display_attribute_mode(MODE_NONE);
}

void draw_vertical_line(int x, int starty, int endy) 
{
	int i;
	cursor_goto(x, starty);
	ht_set_display_attribute_mode(MODE_REVERSED);
	for(i=starty; i < endy; i++) 
	{
		usart_transmit(' ');
		// Move down one and back to the left one
		move_cursor(1,MOVE_DOWN);
		move_cursor(1,MOVE_LEFT);
	}
	usart_transmit(' ');
	ht_set_display_attribute_mode(MODE_NONE);
}

//***************************
// Convert byte to 3 ASCII digits and send
//***************************
void usart_transmit_dec( unsigned char value )
{
    unsigned char digit;
    
    digit = '0';
    while( value >= 100 )                // Still larger than 100 ?
    {
        digit++;                         // Increment first digit
        value -= 100;
    }
    
    usart_transmit( digit );                  // Send first digit
    
    digit = '0';
    while( value >= 10 )                 // Still larger than 10 ?
    {
        digit++;                         // Increment second digit
        value -= 10;
    }
    
    usart_transmit( digit );                  // Send second digit
    
    usart_transmit( '0' + value );            // Send third digit
}


void usart_init_Xlarge(uint32_t baud)
{
/*	//DEBUG!!!!!!!!!
	// Set baud rate: 19200bps when @ 8Mhz
	UBRR0H = (unsigned char)(25>>8);
	UBRR0L = (unsigned char)25;
*/
	// Calculate the baud value
	uint16_t num = (uint32_t)F_CPU / (16 * baud) - 1;
	
	// Set baud rate 
	UBRR0H = (unsigned char)(num>>8);
	UBRR0L = (unsigned char)num;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

uint8_t usart_data_received(void)
{
	if(UCSR0A & (1<<RXC0))
	{
		return 1;
	}
	return 0;
}

int usart_int_enable(int(*func)(unsigned char))
{
  UCSR0B |= (1 << RXCIE0);
  
  _rx_func = func;

  return 0;
}

int usart_int_disable(int(*func)(unsigned char))
{
  UCSR0B &= ~(1 << RXCIE0);

  return 0;
}

ISR(USART0_RX_vect)
{
  (*_rx_func)(UDR0);
}

char nibble_to_hex(uint8_t value) {

  char ASCII[] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};

  return ASCII[(value & 0x0F)];
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////////////////////// USART 1 FUNCTIONS//////////////////////////////
//////////////////////////////////////////////////////////////////////
#ifdef __AVR_ATmega644P__
void usart1_init(uint16_t baud)
{
/*	//DEBUG!!!!!!!!!
	// Set baud rate: 19200bps when @ 8Mhz
	UBRR0H = (unsigned char)(25>>8);
	UBRR0L = (unsigned char)25;
*/
	// Calculate the baud value
	uint16_t num = (uint32_t)F_CPU / (16 * (uint32_t)baud) - 1;
	
	// Set baud rate 
	UBRR1H = (unsigned char)(num>>8);
	UBRR1L = (unsigned char)num;
	/* Enable receiver and transmitter */
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	/* Set frame format: 8data, 1stop bit */
	UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);
}

void usart1_init_Xlarge(uint32_t baud)
{
/*	//DEBUG!!!!!!!!!
	// Set baud rate: 19200bps when @ 8Mhz
	UBRR0H = (unsigned char)(25>>8);
	UBRR0L = (unsigned char)25;
*/
	// Calculate the baud value
	uint16_t num = (uint32_t)F_CPU / (16 * baud) - 1;
	
	// Set baud rate 
	UBRR1H = (unsigned char)(num>>8);
	UBRR1L = (unsigned char)num;
	/* Enable receiver and transmitter */
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	/* Set frame format: 8data, 1stop bit */
	UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);
}

void usart1_transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1)))
	{
	}
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

char usart1_receive(void)
{
	/* Wait for data to be received */
	while ( !(UCSR1A & (1<<RXC1)) )
	{
	}
	/* Get and return received data from buffer */
	return UDR1;
}

void usart1_transmit_str(const char* data)
{
	// There's still data to go so keep sending it until it ends
	while(*data)
	{
		usart1_transmit(*data);
        data++;
	}
}

void usart1_transmit_int32(uint32_t num)
{
	char buffer[11];
  uint8_t i = 0;

  buffer[0] = '0';

  if(num == 0) {
    i = 1; // Need to push at least one value out
  }

  while(num > 0) {
    buffer[i] = num % 10 + '0';
    num /= 10;
    i++;
  }

  // Now we've converted it in reverse, pump it back out correctly
  for(; i > 0; i--) {
    usart1_transmit(buffer[i - 1]);
  }
  //usart_transmit('\0');
}

#endif

