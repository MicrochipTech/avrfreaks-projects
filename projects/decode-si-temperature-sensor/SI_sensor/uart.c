// TX = PORTA.0
// 8N1

#define BAUD 2400
#define US_BIT 1000000UL/BAUD    //microseconds per bit

void TX_init(int baud)  //baud not used
{
	DDRA |= 1;				//PORTA.0 = output
	PORTA &=~1;				//TX idle
}


void TX_putc(unsigned char c)
{
	char i,t;
	t=c;					//copy character to send
//	cli();					//disable interrupts

	PORTA |= 1;				//start bit
	_delay_us(US_BIT);  	//417 us for 2400 baud

    for (i=0; i<8; i++) 
	{
	
	if ( t & 0x01 ) PORTA &= ~1;	// If the LSB of char=1, set logic 1 on TX_PIN
	else PORTA |= 1;			// Otherwise: send 0

	_delay_us(US_BIT);

	t >>= 1;					// shift TX buffer right
	}
	 
	PORTA &= ~1;					//send stop bit
	_delay_us(US_BIT);
//	sei();							//enable interrupts
}
void cls(void)
{
	TX_putc(0xFE);  //initialize
	TX_putc(1);
	Delay10ms(1);
}
void crlf(void) 
{
	TX_putc(0xFE);
	TX_putc(0xC0);  //next line on LCD
//	TX_putc(0x0d);  
//	TX_putc(0x0a);  //for terminal emulator
}

void TX_puts(char *c )
{
  while(*c != 0) 
  {
//  if (*c == '=') crlf();
  TX_putc( *c++ );
  }
}

void TX_puts_f(const char *FlashData)
{
// Print data from program memory

   strcpy_P( (char*)&TextBuffer[0], FlashData);
   TX_puts(TextBuffer);
}
