// HD44780 & compatible LCD driver.
// © Portia CyberLogic.
// www.portia-cyberlogic.com
// mail@portia-cyberlogic.com
//
// LCD.h
// LCD main include.
// By Mahmoud Attaher.

#include "lcd.h"

void LCDSend(uint8_t _Cmd, uint8_t _Byte)
{
	uint8_t i;

	// Send data serially..
	for(i = 0; i < 8; i++)
	{
		// Set data pin..
		if((_Byte & 0b10000000))
			LCD_DAT_SET(); // Set data pin.
		else
			LCD_DAT_CLR(); // Clear data pin.

		// Set clock pin..
		LCD_CLK_SET();

		// Shift the byte..
		_Byte =  _Byte << 1;

		// Clear clock pin..
		LCD_CLK_CLR();
	}

	// One more clock eadge to flush 8th bit..
	LCD_CLK_SET(); // Set clock pin..

	// Command or data..
	if(_Cmd)
		LCD_DAT_CLR(); // Command.
	else
		LCD_DAT_SET(); // Data.

	// Set enable pin..
	LCD_E_SET();

	// Wait for LCD processing..
	_delay_ms(1.65);

	// Clear enable pin..
	LCD_E_CLR();

	// Finalize..
	LCD_DAT_CLR();
	LCD_CLK_CLR(); // Clear clock pin..
}

void LCDInit(void)
{
	//Set IO Ports
	LCD_DAT_DDR |= (1<<LCD_DAT_PIN);
	LCD_CLK_DDR |= (1<<LCD_CLK_PIN);
	LCD_E_DDR |= (1<<LCD_E_PIN);

	// Reset..
	LCD_E_CLR();
	LCD_CLK_CLR();
	LCD_DAT_CLR();

	//After power on Wait for LCD to Initialize..
	_delay_ms(20);

	//Set 8-bit mode..
	//LCDCmd(0b00110000); // Initialize: 8-bit mode.
	//_delay_ms(10);

	LCDCmd(0b00111000); // Function set: 8-bit, 2 line, 5x8 font.

	LCDCmd(0b00001100);	// Display: On, cursor off, cursor blink off.
}

void LCDGotoXY(uint8_t _x, uint8_t _y)
{
	if(_x < 40)
	{
		if(_y)
			_x |= 0b01000000;
		
		_x |= 0b10000000;
		
		LCDCmd(_x);
	}
}

void LCDPrintString(const char *_Str)
{
	while(*_Str)
	{
		LCDData(*_Str);
		_Str++;
	}
}

void LCDPrintInt(int _Int,unsigned int _nDgts)
{
	char str[5] = {0,0,0,0,0};
	int i, j;

	i = 4;
	j = 0;
	while(_Int)
	{
		str[i] = _Int % 10;
		_Int = _Int / 10;
		i--;
	}

	if(_nDgts == -1)
	{
		while(str[j] == 0)
			j++;
	}
	else
		j = 5 - _nDgts;

	if(_Int < 0)
		LCDData('-');

	for(i = j; i < 5; i++)
	{
		LCDData((48 + str[i]));
	}
}





