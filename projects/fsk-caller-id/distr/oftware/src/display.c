/*! \file display.c
 *This program controls an PC1602-F LCD display.  This display contains
 *a S6A0069 controller chip, which is HD44780 compliant.
 *This program is meant for an ATMEGA8535 controller running at 16MHz.
 */


#include "define.h"
#include <avr/delay.h>


/*Local functions
*********************************************************************/
void display(BOOL rs, uint8_t data);
void setCharacter(uint8_t character);
void setPosition(uint8_t position);
void cls(void);

/*Local variables
*********************************************************************/
static uint8_t position=0;//!<Remembers current cursor position

/*int main(void){
	char *s="Hallo, koele krijger.";
	//set portB for output
	DDRB=0xFF;
	//setPosition(0);
	//for(int i=0x00;i<=0x0F;i++)setCharacter(i);
	return 0;
}//main*/

/*!This function initializes the display.  So it's ready to show the
 * data we send to it.  See the datasheet of the S6A0069 controller chip
 * of that display for more info on these commands.
 */
void initDisplay(void){
	uint8_t counter=0;
	//wait for more than 30ms after VDD rises to 4.5V
	while(counter!=16){
		_delay_loop_2(3e4);			//wait for 3e4 * 4 cycles
		counter++;
	}
	display(FALSE,0x38);			//Function set
	_delay_loop_2(620);				//Wait for more than 39µs
	display(FALSE,0x0C);			//Display ON/OFF control
	_delay_loop_2(620);				//Wait for more than 39µs
	display(FALSE,0x01);			//Display clear
	_delay_loop_2(24300);			//Wait for 1.53ms
	display(FALSE,0x06);			//Entry mode set
}//initDisplay

//!Clear LCD
void cls(){
	display(FALSE,0x01);
	_delay_loop_2(24300);			//Wait for 1.53ms
	position=0;
}

/*! Function that shows a sentence on the LC-display. 
 *\param s 			Pointer to the sentence.
 *\param length 	Length of the sentence.
 *\param pos		Start position to show the message on the screen.
 *					This number must be in the range of 0 up to 31.
 */
void setSentence(char* s, uint8_t length, uint8_t pos){
	uint8_t i=0;
	setPosition(pos);
	do{
		setCharacter(s[i++]);
	}while(--length);
}//setSentence

/*! Set position of the cursor.
 *\param pos For first line: 0 to 15, 16 to 31 for second line.
 */
void setPosition(uint8_t pos){
	position=pos;
	//Only keep lower five bits
	position&=0x1F;
	/*If position on second line, then change address by
	 *setting bit6 and clearing bit5.
	 */
	if(TST_BIT(position,4))position^=0x50;
	PORTB=position;
	//Set position
	display(FALSE,0x80 | position);
	_delay_loop_2(700);
}//set position

/*! Write a character to the LCD on the current cursorposition.
 *\param character Character to write to the LCD.
 */
void setCharacter(uint8_t character){
	if(position++==16)setPosition(16);
	display(TRUE,character);
	_delay_loop_2(690);
}//setCharacter

/*! Low-level routine for writing characters and commands to the LCD.
 *\param rs 	Determines whether we want to write data (TRUE) or 
 *				instructions (FALSE).
 *\param data	Byte to write to the LCD.
 */
void display(BOOL rs, uint8_t data){
	cli();								//clear interrupts
	if(!rs)CLR_BIT(PORTA,RS);			//set or reset RS
	else SET_BIT(PORTA,RS);
	SET_BIT(PORTA,E);					//set E
	PORTB=data;							//set data
	CLR_BIT(PORTA,E);					//reset E
	CLR_BIT(PORTA,RS);					//reset RS
	PORTB=0;							//reset data
	sei();								//set interrupts
}//display