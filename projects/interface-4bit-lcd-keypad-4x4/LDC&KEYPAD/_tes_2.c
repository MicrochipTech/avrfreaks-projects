/*
by ricky rawung : (indonesian-SULUT-Minahasa)
email : henry_ryq@yahoo.com

    Menampilkan tulisan di LCD
*/

#include <avr/io.h>             // Definisi register
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/lcdroutines.h>

#define LCD_PORT    PORTC
#define LCD_DDR     DDRC


#define F_CPU 11059200



prog_char Tulisan1[] = "Pencet aja Boss";


int main(void){

	// inisialisasi PORT
	// nibel atas untuk output (columns) nibel bawa untuk input (rows);

	DDRB=0xF0;

	//set pulup bit untuk pb0-pb3

	PORTB=0xF0;

 
    LCD_DDR = 0xff;


    uint8_t c, i;

	//inisialisasi	LCD
	
	lcd_init();
	lcd_command(0x0d);

	// baris 1    
    i = 0;
   while((c = pgm_read_byte(&Tulisan1[i++])))
   {
      lcd_data(c);  
    }

	// baris 2

	set_cursor(0,2);

    while(1)
	{           // Eternal loop
	    

		//first column

		PORTB =0b10000000;


		if (bit_is_set(PINB, 3)) 
			{
			lcd_data('a');
			_delay_ms(500);
			}

		if (bit_is_set(PINB, 2)) 
			{
			lcd_data('3');
			_delay_ms(500);
			}

		if (bit_is_set(PINB, 1)) 
			{
			lcd_data('2');
			_delay_ms(500);
			}

		if (bit_is_set(PINB, 0)) 
			{
			lcd_data('1');
			_delay_ms(500);
			}

		//second column

		PORTB =0b01000000;

		if (bit_is_set(PINB, 3)) 
			{
			lcd_data('b');
			_delay_ms(500);
			}

		if (bit_is_set(PINB, 2)) 
			{
			lcd_data('6');
			_delay_ms(500);
			}

		if (bit_is_set(PINB, 1)) 
			{
			lcd_data('5');
			_delay_ms(500);
			}

		if (bit_is_set(PINB, 0)) 
			{
			lcd_data('4');
			_delay_ms(500);
			}

		//third column

		PORTB =0b00100000;

		if (bit_is_set(PINB, 3)) 
			{
			lcd_data('c');
			_delay_ms(500);
			}

		if (bit_is_set(PINB, 2)) 
			{
			lcd_data('9');
			_delay_ms(500);
			}

		if (bit_is_set(PINB, 1)) 
			{
			lcd_data('8');
			_delay_ms(500);
			}

		if (bit_is_set(PINB, 0)) 
			{
			lcd_data('7');
			_delay_ms(500);
			}

		//fourth column

		PORTB =0b00010000;

		if (bit_is_set(PINB, 3)) 
			{
			lcd_data('d');
			_delay_ms(500);
			}

		if (bit_is_set(PINB, 2)) 
			{
			lcd_data('e');
			_delay_ms(500);
			}

		if (bit_is_set(PINB, 1)) 
			{
			lcd_data('0');
			_delay_ms(500);
			}

		if (bit_is_set(PINB, 0)) 
			{
			lcd_clear();
			i = 0;
   			while((c = pgm_read_byte(&Tulisan1[i++])))
   				{
    			  lcd_data(c);  
   				 }
			set_cursor(0,2);
			}
    }
    
}
