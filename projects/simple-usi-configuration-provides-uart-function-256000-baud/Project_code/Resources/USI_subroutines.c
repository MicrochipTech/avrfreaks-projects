
#define receiveBit \
while (!(TIFR0 & (1 << OCF0A)));\
USICR |= (1 << USICLK);\
TIFR0 = 0xFF;\
OCR0A +=  Rx_clock;

/*
Wait for output compare match flag
Clock the USI
Clear the flag
Increment the Output compare register
*/


#define transmitBit \
while (!(TIFR0 & (1 << OCF0A)));\
USICR |= (1 << USICLK);\
TIFR0 = 0xFF;\
OCR0A +=  Tx_clock;


/***********************************************************************/

#define fetch_char \
Start_clock;\
OCR0A = Half_Rx_clock;\
receiveBit;receiveBit;receiveBit;receiveBit;\
receiveBit;receiveBit;receiveBit;receiveBit;\
receiveBit;\
keypress =  USIDR;\
receiveBit;\
TCCR0B = 0;\
TCNT0 = 0;



unsigned char ReverseByte (unsigned char x) {
	x = ((x >> 1) & 0x55) | ((x << 1) & 0xAA);
	x = ((x >> 2) & 0x33) | ((x << 2) & 0xCC);
	x = ((x >> 4) & 0x0F) | ((x << 4) & 0xF0);
return x;}



/**************************************************************************************************************/
void Initialise_USI_Tx (void)
{		
	USICR = 0;										//Reset USI
	OCR0A = Tx_clock;								//Initialise the Output compare register
	TIFR0 = 0xFF;									//Clear spurious interrupts
	TCNT0 = 0;										//Clear TCNT0
	USICR |= (1 << USIWM0);							//Select USI 3-wire mode
	Configure_DO_pin_as_Output;
	USIDR = 0xFF;									//Load USIDR with 0xFF
USISR = 0xFF;}										//Clear spurious interrupt flags




/**************************************************************************************************************/
void Char_to_USI(unsigned char Txdata){				//USI already initialised for transmit by default
	
	Txdata = ReverseByte(Txdata);
	
	Start_clock;	
	transmitBit;										//Initialise transmission
	USIDR = (Txdata >> 2) | 0x80;						//Add start bit

	for(int p = 0; p <= 4; p++){transmitBit;}			//First half of byte
		
	USIDR = (Txdata << 3) | 0x07;						//Add stop bit
	for(int p = 0; p <= 4; p++){transmitBit;}			//Second half of byte
	
	TCCR0B = 0;											//Halt baud rate clock
	TCNT0 = 0;}
	


/************************************************************************************************************/
unsigned char Char_from_USI (char timeout)				//Receive char
{int p = 8000;											//Time out variable
	unsigned char keypress;
	
	WPU_on_DO_pin;
	if (timeout)										//Timeout required
	while (DI_paused && (p--));							//Wait here for start bit or timeout
	else
	while (DI_paused);									//Or just wait indefinitely for start bit
	if(start_bit){
		fetch_char;
	keypress = ReverseByte(keypress);}
	else keypress = 0;									//Timeout occurred
	Initialise_USI_Tx ();								//Leave USI ready to transmit char
return (keypress);}





/**********************************************************************************************************/
void String_from_USI (unsigned char* string)						//Receive string from PC
{ unsigned int p = 0;
	unsigned char keypress;
	
	 WPU_on_DO_pin;
		
	while(DI_paused);												//Wait for start bit of first character
	fetch_char;														//Detects 1 start bit, 8 data bits and ONE stop bit
	string[0] = keypress;
	for(int n = 1; n < (buffer_size); n++)							//Max permissible string length set by user
	{
	while(DI_paused && (++p));
	if(start_bit)
		{fetch_char;
		string[n] = keypress;}
	
	else{string[n] = '\0';											//receiver times out: terminate string
	break;}}
	
	for(int n = 0; n < (buffer_size); n++)
	{if (string[n]){string[n] = ReverseByte(string[n]);}
		else break;}
	Initialise_USI_Tx ();}											//Leave USI ready to transmit char


