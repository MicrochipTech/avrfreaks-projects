

// Display driver for DOGM128

// Display properties (for graphics functions)
#define DISP_WIDTH   128L
#define DISP_HEIGHT  64L
#define DISP_DEPTH   1   //monochrome




// Hardware pin definitions
#define A0_PORT  PORTB
#define A0_DDR   DDRB
#define A0_BIT   PB0
#define RES_PORT PORTB
#define RES_DDR  DDRB
#define RES_BIT  PB2

#define CS_PORT PORTB
#define CS_DDR  DDRB
#define CS_BIT  PB1

#define LCD_DOGM_DPORT  DDRB					// Portbelegung Atmeg644   EA DOGM128
#define LCD_DOGM_PORT   PORTB


// RAM data (top to bottom; left to right)
// Pixel(x,y):          index = y/8 + x*8
//                            = (y >> 3) + (x << 3)
//                      bit   = y%8
//                            = (y & 0x07)
// Set pixel (x,y):     disp_ram[(y >> 3) + (x << 3)] |=  (1 << (y & 0x07));
// Clear pixel (x,y):   disp_ram[(y >> 3) + (x << 3)] &= ~(1 << (y & 0x07));
uint8_t disp_ram[1024];


// (Pre-declare functions)
//void disp_clear()


void lcd_set_cs(unsigned char enable)
{
	if (enable) cbi(CS_PORT, CS_BIT);	// DOGM select = LOW
	else 		sbi(CS_PORT, CS_BIT);	// DOGM deselect = HIGH
}


void spi_init() 
{
	SPCR = (1 << SPE) | (1 << MSTR) | (1<<SPR0);	// SPI Master
	Delay(100);
}

void spi_master_send_char(unsigned char Dat) 
{
	delay_us(1);
	SPDR = Dat;				// Daten nach Sender und starten
	// while(!(SPSR & (1<<SPIF)));			// warte bis Bit 1: fertig
	loop_until_bit_is_set(SPSR, SPIF);		// warte bis Bit 1: fertig
	delay_us(1);
	Dat = SPDR;				// SPIF wider 0 (Pseudolesen)
}

// hardware functions (only used by "disp_..."-functions)

void dogm_send(unsigned char spi_data, unsigned char a0)
{
	//a0 = 0: Command
	//a0 = 1: Display data

	//set / clear A0-Bit
	if (a0)
		A0_PORT |= (1<<A0_BIT);
	else
		A0_PORT &= ~(1<<A0_BIT);

	//Byte senden
	lcd_set_cs(1);
	spi_master_send_char(spi_data);
	delay_us(1);
	lcd_set_cs(0);
}
#define dogm_send_command(command)      dogm_send(command, 0)
#define dogm_send_display_data(data)    dogm_send(data, 1)

void LCD_SetPageAddress(uint8_t Page)
{
	Page &= 0x0F;
	dogm_send_command(0xB0 | Page);
}

void LCD_SetColumnAddress(uint8_t Column)
{
	dogm_send_command(0x10 | (Column>>4));
	dogm_send_command(Column & 0x0F);
}


void dogm_reset()
{
	//Reset display (50ms)
	RES_PORT &= ~(1<<RES_BIT); // RES\ = LOW (Reset)
	Delay(200);
	RES_PORT |= (1<<RES_BIT); // RES\ = HIGH (kein Reset)
}

// basic implementations (used by graphics functions)

void disp_init()
{
	//uart_comment("[DOGM128] Initializing...");

	//SET DATA DIRECTION REGISTER
	//Outputs: (DDR=1)
	LCD_DOGM_DPORT = 0xff;			// set output
	LCD_DOGM_PORT = 0xff;			// clear LEDS

	A0_DDR |= (1<<A0_BIT); //Output: A0
	RES_DDR |= (1<<RES_BIT); //Output: RES\
	CS_DDR |= (1<<CS_BIT); //Output: RES\
	//Inputs: (DDR=0)
	//(none)


	dogm_reset(); //(100ms)


	//Display start line
	dogm_send_command(0x40); // Display start line 0

	//Bottom view
	dogm_send_command(0xA1); // ADC reverse
	dogm_send_command(0xC0); // Normal COM0~COM63

	//Normal / Inverted
	dogm_send_command(0xA6); // Display normal

	//Hardware options
	dogm_send_command(0xA2); // Set bias 1/9 (Duty 1/65)
	dogm_send_command(0x2F); // Booster, Regulator and Follower on
	dogm_send_command(0xF8); // Set internal Booster to 4x
	dogm_send_command(0x00);

	//Contrast options
	dogm_send_command(0x27); // Contrast set
	dogm_send_command(0x81);
	dogm_send_command(0x16);

	//Indicator options
	dogm_send_command(0xAC); // No indicator
	dogm_send_command(0x00);

	//(Init done)
	dogm_send_command(0xAF); // Display on

	disp_clear();
}


void disp_send_frame()
{

	uint8_t page;
	for (page = 0; page < 8; page++)
	{
		LCD_SetPageAddress(page); //Set page address to <page>
		LCD_SetColumnAddress(0);
		uint8_t column;
		for (column = 0; column < 128; column++)
			dogm_send_display_data(disp_ram[page + (column << 3)]);
	}
}

void disp_clear()
{
	//clears the local RAM and send this cleared frame
	uint16_t d;
	for (d = 0; d < 1024; d++)
		disp_ram[d] = 0x00;
	disp_send_frame();
}

void disp_clear_dont_refresh()
{
	//clears the local RAM but don't send it
	uint16_t d;
	for (d = 0; d < 1024; d++)
		disp_ram[d] = 0x00;
}


/*
void disp_frame_begin()
{
	//not relevant for this display
}
*/
//ignore "disp_frame_begin()" in source code
#define disp_frame_begin()  


void disp_frame_end()
{
	//send the display data
	disp_send_frame();
}


void disp_set_pixel(uint8_t x, uint8_t y, uint8_t pixel_status)
{
	if (x < DISP_WIDTH && y < DISP_HEIGHT)
		if (pixel_status != 0)
			disp_ram[(y >> 3) + (x << 3)] |=  (1 << (y & 0x07));
		else
			disp_ram[(y >> 3) + (x << 3)] &= ~(1 << (y & 0x07));
}



