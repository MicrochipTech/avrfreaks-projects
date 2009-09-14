#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>

#define SBI(x,y) (x |= (1<<y));
#define CBI(x,y) (x &= (~(1<<y)));

/*----- LCD Pins -----*/
//	RS
#define	LCD_DDR_RS	DDRD
#define	LCD_PRT_RS	PORTD
#define	LCD_RS		4
//	EN
#define	LCD_DDR_EN	DDRD
#define	LCD_PRT_EN	PORTD
#define	LCD_EN		3
//	D4
#define	LCD_DDR_D4	DDRD
#define	LCD_PRT_D4	PORTD
#define	LCD_D4		2
//	D5
#define	LCD_DDR_D5	DDRD
#define	LCD_PRT_D5	PORTD
#define	LCD_D5		1
//	D6
#define	LCD_DDR_D6	DDRD
#define	LCD_PRT_D6	PORTD
#define	LCD_D6		0
//	D7
#define	LCD_DDR_D7	DDRC
#define	LCD_PRT_D7	PORTC
#define	LCD_D7		6
/*--- LCD Pins End ---*/

void lcd_init(void);
void lcd_cmd(unsigned char);
void lcd_dat(unsigned char);

int main(void){
	lcd_init();

	lcd_dat('T');
	lcd_dat('e');
	lcd_dat('S');
	lcd_dat('t');
	lcd_dat('r');
	lcd_dat('i');
	lcd_dat('n');
	lcd_dat('g');
	lcd_dat('.');
	lcd_dat('.');
	lcd_dat('.');

	lcd_cmd(0xBF);	// Move cursor to second line

	for(signed char i=10;i>=0;i--){
		lcd_dat(i+'0');
	}

	lcd_cmd(0x02); _delay_ms(2); // Returns cursor to home position (address 0). Also returns display being shifted to the original position. DDRAM contents remains unchanged.

	return 0;
}

void lcd_init(void){
	SBI(LCD_DDR_RS,LCD_RS);
	SBI(LCD_DDR_EN,LCD_EN);
	SBI(LCD_DDR_D4,LCD_D4);
	SBI(LCD_DDR_D5,LCD_D5);
	SBI(LCD_DDR_D6,LCD_D6);
	SBI(LCD_DDR_D7,LCD_D7);

	CBI(LCD_PRT_RS,LCD_RS);
	_delay_ms(2);
	SBI(LCD_PRT_EN,LCD_EN);
	_delay_us(6);
	CBI(LCD_PRT_D4,LCD_D4);
	SBI(LCD_PRT_D5,LCD_D5);
	CBI(LCD_PRT_D6,LCD_D6);
	CBI(LCD_PRT_D7,LCD_D7);
	_delay_us(6);
	CBI(LCD_PRT_EN,LCD_EN);
	_delay_ms(2);
	lcd_cmd(0x28);	// 2 Lines
	//lcd_cmd(0x06);// Cursor Move Direction: cursor is in increment position
	lcd_cmd(0x0D);	// Display on, Cursor off, Cursor blink on
	lcd_cmd(0x01);	// Clear Screen and returns cursor to the home position (address 0)
	_delay_ms(2);
	lcd_cmd(0x80);	// Sets the DDRAM address. DDRAM data is sent and received after this setting.	
}

void lcd_cmd(unsigned char cmd){
	CBI(LCD_PRT_RS,LCD_RS);
	SBI(LCD_PRT_EN,LCD_EN);
	if(cmd&0x10){SBI(LCD_PRT_D4,LCD_D4);}
	else CBI(LCD_PRT_D4,LCD_D4);
	if(cmd&0x20){SBI(LCD_PRT_D5,LCD_D5);}
	else CBI(LCD_PRT_D5,LCD_D5);
	if(cmd&0x40){SBI(LCD_PRT_D6,LCD_D6);}
	else CBI(LCD_PRT_D6,LCD_D6);
	if(cmd&0x80){SBI(LCD_PRT_D7,LCD_D7);}
	else CBI(LCD_PRT_D7,LCD_D7);
	CBI(LCD_PRT_EN,LCD_EN);
	_delay_us(40);
	SBI(LCD_PRT_EN,LCD_EN);
	if(cmd&1){SBI(LCD_PRT_D4,LCD_D4);}
	else CBI(LCD_PRT_D4,LCD_D4);
	if(cmd&2){SBI(LCD_PRT_D5,LCD_D5);}
	else CBI(LCD_PRT_D5,LCD_D5);
	if(cmd&4){SBI(LCD_PRT_D6,LCD_D6);}
	else CBI(LCD_PRT_D6,LCD_D6);
	if(cmd&8){SBI(LCD_PRT_D7,LCD_D7);}
	else CBI(LCD_PRT_D7,LCD_D7);
	CBI(LCD_PRT_EN,LCD_EN);
	_delay_us(40);
}

void lcd_dat(unsigned char cmd){
	SBI(LCD_PRT_RS,LCD_RS);
	SBI(LCD_PRT_EN,LCD_EN);
	if(cmd&0x10){SBI(LCD_PRT_D4,LCD_D4);}
	else CBI(LCD_PRT_D4,LCD_D4);
	if(cmd&0x20){SBI(LCD_PRT_D5,LCD_D5);}
	else CBI(LCD_PRT_D5,LCD_D5);
	if(cmd&0x40){SBI(LCD_PRT_D6,LCD_D6);}
	else CBI(LCD_PRT_D6,LCD_D6);
	if(cmd&0x80){SBI(LCD_PRT_D7,LCD_D7);}
	else CBI(LCD_PRT_D7,LCD_D7);
	CBI(LCD_PRT_EN,LCD_EN);
	_delay_us(40);
	SBI(LCD_PRT_EN,LCD_EN);
	if(cmd&1){SBI(LCD_PRT_D4,LCD_D4);}
	else CBI(LCD_PRT_D4,LCD_D4);
	if(cmd&2){SBI(LCD_PRT_D5,LCD_D5);}
	else CBI(LCD_PRT_D5,LCD_D5);
	if(cmd&4){SBI(LCD_PRT_D6,LCD_D6);}
	else CBI(LCD_PRT_D6,LCD_D6);
	if(cmd&8){SBI(LCD_PRT_D7,LCD_D7);}
	else CBI(LCD_PRT_D7,LCD_D7);
	CBI(LCD_PRT_EN,LCD_EN);
	_delay_us(40);
}
