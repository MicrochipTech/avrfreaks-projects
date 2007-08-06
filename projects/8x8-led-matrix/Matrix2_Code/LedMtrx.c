#include <avr/io.h>
#include "deprecated.h"

#include "LEDMtrx.h"
#include "font5x7.h"
#include "font3x5.h"

// global variables
uint8_t MAX_CS[4] = {0x03, 0x02, 0x05, 0x04};
uint8_t Screen1[32];
uint8_t Screen2[32];
uint8_t Screen3[32];

volatile uint8_t Screen1_pos;
volatile uint8_t Screen2_pos;
volatile uint8_t Screen3_pos;

/*************************************************************/
/********************** LOCAL FUNCTIONS **********************/
/*************************************************************/
void LEDInitHW(void)
{
	// initialize I/O ports
	
	// initialize LED control lines levels
	cbi(LED_CTRL_PORT, LED_CTRL_DATA);
	cbi(LED_CTRL_PORT, LED_CTRL_CLK);
	sbi(LED_CTRL_PORT, LED_CTRL_CS0);
	sbi(LED_CTRL_PORT, LED_CTRL_CS1);
	sbi(LED_CTRL_PORT, LED_CTRL_CS2);
	sbi(LED_CTRL_PORT, LED_CTRL_CS3);

	// initialize LED control port to output
	sbi(LED_CTRL_DDR, LED_CTRL_DATA);
	sbi(LED_CTRL_DDR, LED_CTRL_CLK);
	sbi(LED_CTRL_DDR, LED_CTRL_CS0);
	sbi(LED_CTRL_DDR, LED_CTRL_CS1);
	sbi(LED_CTRL_DDR, LED_CTRL_CS2);
	sbi(LED_CTRL_DDR, LED_CTRL_CS3);

}

/*
*********************************************************************************************************
* MAX7219_Init()
*
* Description: Initialize MAX7219 module; must be called before any other MAX7219 functions.
* Arguments  : none
* Returns    : none
*********************************************************************************************************
*/
void MAX7219_Init (void)
{
uint8_t x;
	LEDInitHW();
	for (x = 0; x <=3; x++)
	{
		MAX7219_Write(x, REG_SCAN_LIMIT, 7);        	// set up to scan all eight digits
		MAX7219_Write(x, REG_DECODE, 0x00);         	// set to "no decode" for all digits
		MAX7219_ShutdownOff(x);                         // select normal operation (i.e. not shutdown)
		MAX7219_DisplayNorm(x);                         // select normal operation (i.e. not test mode)
		MAX7219_Clear(x);                               // clear all digits
		MAX7219_SetBrightness(x, INTENSITY_HALF);       // set to half intensity
	}
}

/*
*********************************************************************************************************
* MAX7219_ShutdownOn()
*
* Description: Shut down the display.
* Arguments  : chip to affect
* Returns    : none
*********************************************************************************************************
*/
void MAX7219_ShutdownOn (uint8_t chip)
{
	MAX7219_Write(chip, REG_SHUTDOWN, 0);                     // put MAX7219 into "shutdown" mode
}


/*
*********************************************************************************************************
* MAX7219_ShutdownOff()
*
* Description: Take the display out of shutdown mode.
* Arguments  : chip to affect
* Returns    : none
*********************************************************************************************************
*/
void MAX7219_ShutdownOff (uint8_t chip)
{
	MAX7219_Write(chip, REG_SHUTDOWN, 1);                     // put MAX7219 into "normal" mode
}


/*
*********************************************************************************************************
* MAX7219_DisplayTest()
*
* Description: Start a display test.
* Arguments  : chip to affect
* Returns    : none
*********************************************************************************************************
*/
void MAX7219_DisplayTest (uint8_t chip)
{
	MAX7219_Write(chip, REG_DISPLAY_TEST, 1);                 // put MAX7219 into "display test" mode
}


/*
*********************************************************************************************************
* MAX7219_DisplayNorm()
*
* Description: Stop a display test.
* Arguments  : chip to affect
* Returns    : none
*********************************************************************************************************
*/
void MAX7219_DisplayNorm (uint8_t chip)
{
	MAX7219_Write(chip, REG_DISPLAY_TEST, 0);                 // put MAX7219 into "normal" mode
}


/*
*********************************************************************************************************
* MAX7219_SetBrightness()
*
* Description: Set the LED display brightness
* Arguments  : chip to affect(0 - 3), brightness (0-15)
* Returns    : none
*********************************************************************************************************
*/
void MAX7219_SetBrightness (uint8_t chip, char brightness)
{
	brightness &= 0x0f;                                 // mask off extra bits
	MAX7219_Write(chip, REG_INTENSITY, brightness);           // set brightness
}


/*
*********************************************************************************************************
* MAX7219_Clear()
*
* Description: Clear the display (all digits blank)
* Arguments  : chip to affect(0 - 3)
* Returns    : none
*********************************************************************************************************
*/
void MAX7219_Clear (uint8_t chip)
{
	char i;
	for (i=0; i < 8; i++)
		MAX7219_Write(chip, i, 0x00);                           // turn all segments off
}

/*
*********************************************************************************************************
* MAX7219_Write()
*
* Description: Write to MAX7219
* Arguments  : chip to affect(0 - 3)
*	           reg_number = register to write to
*              dataout = data to write to MAX7219
* Returns    : none
*********************************************************************************************************
*/
static void MAX7219_Write (uint8_t chip, uint8_t reg_number, uint8_t dataout)
{
	cbi (LED_CTRL_PORT, MAX_CS[chip]);
	MAX7219_SendByte(reg_number);                       // write register number to MAX7219
	MAX7219_SendByte(dataout);                          // write data to MAX7219
	sbi (LED_CTRL_PORT, MAX_CS[chip]);
}


/*
*********************************************************************************************************
* MAX7219_SendByte()
*
* Description: Send one byte to the MAX7219
* Arguments  : dataout = data to send
* Returns    : none
*********************************************************************************************************
*/
static void MAX7219_SendByte (uint8_t dataout)
{
	char i;
	for (i=8; i>0; i--) {
		uint8_t mask = 1 << (i - 1);
		if (dataout & mask)
			sbi (LED_CTRL_PORT, LED_CTRL_DATA);
		else
			cbi (LED_CTRL_PORT, LED_CTRL_DATA);

		cbi (LED_CTRL_PORT, LED_CTRL_CLK);
		sbi (LED_CTRL_PORT, LED_CTRL_CLK);
	}
}

/*
*********************************************************************************************************
* Disp_Char()
*
* Description: Character to display
* Arguments  : none
* Returns    : none
*********************************************************************************************************
*/
void Disp_Char(uint8_t screen, uint8_t character, uint8_t padding)
{
uint8_t col;
	for (col = 0; col <5; col++)
	{
		if (screen == 1)
		{
			Screen1[Screen1_pos] = pgm_read_byte(&font5x7[((character - 32)*5) + col]);
			Screen1_pos++;
		}
		else if (screen == 2)
		{
			Screen2[Screen2_pos] = pgm_read_byte(&font5x7[((character - 32)*5) + col]);
			Screen2_pos++;
		}
		else
		{
			Screen3[Screen3_pos] = pgm_read_byte(&font5x7[((character - 32)*5) + col]);
			Screen3_pos++;
		}
	}

	if (screen == 1)
		Screen1_pos += padding;
	else if (screen == 2)
		Screen2_pos += padding;
	else
		Screen3_pos += padding;
}

/*
*********************************************************************************************************
* Clear_Screen()
*
* Description: Clear the Screen1 array
* Arguments  : 1,2, or 3
* Returns    : none
*********************************************************************************************************
*/
void Clear_Screen(uint8_t screen)
{
uint8_t x;
	for (x = 0; x <32; x++)
	{
		if (screen == 1)
			Screen1[x] = 0;
		else if (screen == 2)
			Screen2[x] = 0;
		else
			Screen3[x] = 0;
	}
}

/*
*********************************************************************************************************
* Copy_Screen()
*
* Description: Copy one screen buffer to another
* Arguments  : Source = 1,2, or 3 - Dest = 1,2, or 3
* Returns    : none
*********************************************************************************************************
*/
void Copy_Screen(uint8_t Source, uint8_t Dest)
{
uint8_t x;
	
	for (x = 0; x <32; x++)
	{
		if (Source == 1)
		{
			if (Dest == 2)
			{
				Screen2[x] = Screen1[x];
			}
			else
			{
				Screen3[x] = Screen1[x];
			}

		}
		else if (Source == 2)
		{
			if (Dest == 1)
			{
				Screen1[x] = Screen2[x];
			}
			else
			{
				Screen3[x] = Screen2[x];
			}

		}
		else
		{
			if (Dest == 1)
			{
				Screen1[x] = Screen3[x];
			}
			else
			{
				Screen2[x] = Screen3[x];
			}

		}

	}
}

/*
*********************************************************************************************************
* set_led()
*
* Description: Set LED at col, row on or off
* Arguments  : Screen, col, row, state
* Returns    : none
*********************************************************************************************************
*/
void set_led(uint8_t screen, uint8_t col, uint8_t row, uint8_t state)
{
	if (screen == 1)
	{
		if (state == 0)
		{
			if (col >= 8)
				cbi (Screen1[row+16], (col-8));
			else
				cbi (Screen1[row], (col));
		}
		else if (state == 1)
		{
			if (col >= 8)
				sbi (Screen1[row+16], (col-8));
			else
				sbi (Screen1[row], (col));
		}

	}
	else if (screen == 2)
	{
		if (state == 0)
		{
			if (col >= 8)
				cbi (Screen2[row+16], (col-8));
			else
				cbi (Screen2[row], (col));
		}
		else if (state == 1)
		{
			if (col >= 8)
				sbi (Screen2[row+16], (col-8));
			else
				sbi (Screen2[row], (col));
		}
	}
	else
	{
		if (state == 0)
		{
			if (col >= 8)
				cbi (Screen3[row+16], (col-8));
			else
				cbi (Screen3[row], (col));
		}
		else if (state == 1)
		{
			if (col >= 8)
				sbi (Screen3[row+16], (col-8));
			else
				sbi (Screen3[row], (col));
		}
	}
}
/*
*********************************************************************************************************
* Trans_1()
*
* Description: Transition screen2 to screen1 - Style 1
* Arguments  : none
* Returns    : none
*********************************************************************************************************
*/
void Trans_1(void)
{
uint8_t c2;
	Copy_Screen(1, 2);
	Clear_Screen(1);
	Show_Screen(1);

	for (c2 = 0; c2<16; c2++)
	{
		Screen1[c2] |= (Screen2[c2] & 0x55);
		Screen1[c2+16] |= (Screen2[c2+16] & 0x55);

		Screen1[15 - c2] |= (Screen2[15 - c2] & 0xAA);
		Screen1[15 - c2 + 16] |= (Screen2[15 - c2 + 16] & 0xAA);

		Show_Screen(1);
		delay(1);
	}
	delay(10);
}

/*
*********************************************************************************************************
* Trans_2()
*
* Description: Transition screen2 to screen1 - Style 2
* Arguments  : none
* Returns    : none
*********************************************************************************************************
*/
void Trans_2(void)
{
uint8_t c, t, x;
uint8_t mask[8] = {0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

	Clear_Screen(1);
	Show_Screen(1);

	for (x = 0; x<8; x++)
	{
		for (c = 0; c<32; c++)
		{
			t = (Screen2[c] & mask[x]) << (7 - x);
			Screen1[c] = t;
		}

		Show_Screen(1);
		delay(1);
	}
	delay(10);
}

/*
*********************************************************************************************************
* Trans_3()
*
* Description: Transition screen2 to screen1 - Style 2
* Arguments  : none
* Returns    : none
*********************************************************************************************************
*/
void Trans_3(void)
{
uint8_t x, chip;


	Clear_Screen(1);
	for (chip = 0; chip<4; chip++)
	{
		MAX7219_ShutdownOn (chip);
	}

	for (x = 0; x<32; x++)
	{
		Screen1[x] = Screen2[x];
	}
	Show_Screen(1);

	for (x = 0; x<8; x++)
	{
		for (chip = 0; chip<4; chip++)
		{
			MAX7219_ShutdownOff(chip);
		}
		delay(2);

		for (chip = 0; chip<4; chip++)
		{
			MAX7219_ShutdownOn (chip);
		}
		delay(2);

		for (chip = 0; chip<4; chip++)
		{
			MAX7219_ShutdownOff(chip);
		}
	}
	delay(10);
}

/*
*********************************************************************************************************
* Trans_4()
*
* Description: Transition screen2 to screen1 - Style 4
* Arguments  : none
* Returns    : none
*********************************************************************************************************
*/
void Trans_4(void)
{
uint8_t row, col, step, seq;


	Clear_Screen(1);


	for (step=0; step<8; step++)
	{
		col = 0;
		row = step;
		for (seq=0; seq<=step; seq++)
		{

			Screen1[col] = Screen1[col] | (Screen2[col] & (1<<row));
			Screen1[col+8] = Screen1[col+8] | (Screen2[col+8] & (1<<row));
			Screen1[col+16] = Screen1[col+16] | (Screen2[col+16] & (1<<row));
			Screen1[col+24] = Screen1[col+24] | (Screen2[col+24] & (1<<row));

			col++;
			row--;
		}
		Show_Screen(1);
		delay(2);

	}
//	delay(10);

	for (step=1; step<8; step++)
	{
		row = 7;
		col = step;
		for (seq=step; seq<8; seq++)
		{

			Screen1[col] = Screen1[col] | (Screen2[col] & (1<<row));
			Screen1[col+8] = Screen1[col+8] | (Screen2[col+8] & (1<<row));
			Screen1[col+16] = Screen1[col+16] | (Screen2[col+16] & (1<<row));
			Screen1[col+24] = Screen1[col+24] | (Screen2[col+24] & (1<<row));

			col++;
			row--;
		}
		Show_Screen(1);
		delay(2);

	}
	delay(10);

}

/*
*********************************************************************************************************
* Show_Screen()
*
* Description: Transfer the contents of screen buffer to the leds
* Arguments  : 1, 2 or 3 - Screen to display
* Returns    : none
*********************************************************************************************************
*/
void Show_Screen(uint8_t screen)
{
uint8_t x, y;
	for (x = 0; x <4; x++)
	{
		for (y = 0; y <8; y++)
		{
		if (screen == 1)
			MAX7219_Write(x, y + 1, Screen1[x*8+y]);
		else if (screen == 2)
			MAX7219_Write(x, y + 1, Screen2[x*8+y]);
		else
			MAX7219_Write(x, y + 1, Screen3[x*8+y]);
		}
	}
}

/*
*********************************************************************************************************
* Delay()
*
* Description: 
* Arguments  : 
* Returns    : none
*********************************************************************************************************
*/
void delay(uint8_t time) 
{ 
uint8_t i,j,k,x; 

for (i=0; i<=time; ++i) 
	for (j=0; j<255; ++j) 
		for (k=0; k<255; ++k) 
			++x; 
} 


