// 16x16 pixels screen tetris game
// for ATmega168
// Stanislav Ponomarev
// hexorgstas@gmail.com
// 
// This code is distributed as is, you can modify it in any way, however
// I'm not responcible for any damage done to any electric devices done by this code. 
// If you plan to distribute this or modified code, please state my name as an original programmer.

#define F_CPU 14745600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h> // http://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html
#include <stdint.h>
#include <stdlib.h>
#include "draw.h"
#include "delay.h"
#include "tetris.h"

// Pin definition
// PB1 (pin 15) is the "next line" onRisingEdge clock. for the upCounter
// PB2 (pin 16) in the "Strobe" line for the shift registers to latch the data
// PC4 (pin 23) in the "Output enable" line for the shift registers to output the data
// PC5 (pin 24) in the "Reset" line for upCounter. keep low

//PB0 (pin 14) - 7th input button
//PD7 (pin 13) - 6th input button
//PD6 (pin 12) - 5th input button
//PD5 (pin 11) - 4th input button
//PD4 (pin 6)  - 3rd input button
//PD3 (pin 5)  - 2nd input button
//PD2 (pin 4)  - 1st input button


/*******************************************************************************
************************** Function definition *********************************
*******************************************************************************/
inline void setup();
inline void waitForVSync();
inline void flipBuffers();
uint8_t adc_get_next_bit();
uint16_t random_seed_from_ADC();
uint16_t adc_read();
//double sampleToFahrenheit(uint16_t sample);

/*******************************************************************************
************************** Global Variables ************************************
*******************************************************************************/
volatile uint8_t transferByte;				// |  Do
volatile uint8_t buf1[96] = {0x00};			// |  NOT
volatile uint8_t buf2[96] = {0x00};			// | Modify
volatile uint8_t *curBuf;					// | These
volatile uint8_t *otherBuf;					// | Variables
volatile uint8_t row, pdState, pbState;		// |  EVER!

volatile uint8_t x, rotation; // x position of the current shape, rotation of the current shape
volatile int8_t y; // y position of the shape

// Variable screen keeps track of what we are doing
	// screen == 0: Menu screen
	// screen == 1: Game Screen
	// screen == 2: Paused screen
	// screen == 3: Game over screen
volatile uint8_t screen = 0;
// Game Flags Variable:
volatile uint8_t gameFlags = 0; 
// define game flags:
#define NEW_SHAPE 0 	//Bit0 - generate new shape
#define FAST_DROP 1		//Bit1 - fast shape drop
#define PLACE_SHAPE 2 	//Bit2 - place shape

#define ROW_FOUND 4		//Bit4 - row to erase is found and set
#define CLEAR_SHIFT 6   //Bit6 - clear shift of the text for screen
#define NEW_GAME 7 		//Bit7 - prepare to start the new game
 	
volatile uint8_t currentShape; // first 3 bits are shape, next 3 bits are color

#define MAX_LEVEL 18	// Maximum level of a game. Warning! the higher the maximum level, the slower shapes will fall at level 1
#define LEVEL_GAIN 1000	// Points needed to increase the level
 	
 	
 

/*******************************************************************************
************************** VECTORS *********************************************
*******************************************************************************/
ISR(SPI_STC_vect)
{
	transferByte++;
	if (transferByte == 6)	// when transfered the whole row
	{
		transferByte = 0;
		PORTB |= (1<<PB1);	//  |  Switch to the 
		PORTB &= ~(1<<PB1); //  |    next row
		PORTB &= ~(1<<PB2); //  |  Latch
    	PORTB |= (1<<PB2);  //  |  Data	
		PORTC |= (1<<PC4);  // Enable output
		row++;
		return;
	}
	
	SPDR = curBuf[row*6+transferByte];
}

ISR(TIMER0_COMPA_vect)
{
	if (row == 16)
		row = 0;
	PORTC &= ~(1<<PC4); // Disable Output
	SPDR = curBuf[row*6+transferByte];
	uint8_t nextPdState, nextPbState;
	nextPdState = PIND;
	nextPbState = PINB;
	
	if ((pdState ^ nextPdState) & (1<<PD2))//PD2 (pin 4)  - 1st input button
	{
		if (nextPdState & (1<<PD2))
		{ //BUTTON RELEASE:
		}
		else
		{ //BUTTON PRESS:
			if (CanMove(curBuf, x, y, (currentShape & 0x07), rotation, 0))
				x--;		// Shift shape to the left
		}
	}
	if ((pdState ^ nextPdState) & (1<<PD3))//PD3 (pin 5)  - 2nd input button
	{
		if (nextPdState & (1<<PD3))
		{ //BUTTON RELEASE:
			gameFlags &= ~(1<<FAST_DROP); // Remove Fast Drop flag
		}
		else
		{ //BUTTON PRESS:
			gameFlags |= (1<<FAST_DROP); // Set Fast_drop flag
		}
	}
	if ((pdState ^ nextPdState) & (1<<PD4))//PD4 (pin 6)  - 3rd input button
	{
		if (nextPdState & (1<<PD4))
		{ //BUTTON RELEASE:
		}
		else
		{ //BUTTON PRESS:
			if (CanMove(curBuf, x, y, (currentShape & 0x07), rotation, 1))
				x++;		// Shift shape right
		}
	}
	if ((pdState ^ nextPdState) & (1<<PD5))//PD5 (pin 11) - 4th input button
	{
		if (nextPdState & (1<<PD5))
		{ //BUTTON RELEASE:
		}
		else
		{ //BUTTON PRESS:
			gameFlags |= (1<<CLEAR_SHIFT);
			screen = 0; // start new game
		}
	}
	if ((pdState ^ nextPdState) & (1<<PD6))//PD6 (pin 12) - 5th input button
	{
		if (nextPdState & (1<<PD6))
		{ //BUTTON RELEASE:
		}
		else
		{ //BUTTON PRESS:
			gameFlags |= (1<<CLEAR_SHIFT);
			switch (screen)
			{
				case 0: { screen++;  break; } // if in new game screen, start new game
				case 1: { screen++; break; }  // if in game screen, pause the game
				case 2: { screen--; break; }  // if in pause screen, resume the game
				case 3: { screen = 0; break; }// if in game over screen, go to new game screen
				default: { screen = 0; break; } // if anything else (in case somehow screen becomes > 3, go to new game screen
			}
		}
	}
	if ((pdState ^ nextPdState) & (1<<PD7))//PD7 (pin 13) - 6th input button
	{
		if (nextPdState & (1<<PD7))
		{ //BUTTON RELEASE:
		}
		else
		{ //BUTTON PRESS:
			if (rotation == 0)  // Cycle through the rotations down
				rotation=3;
			else
				rotation--;
			CheckX(&x, (currentShape & 0x07), rotation);
		}
	}
	if ((pbState ^ nextPbState) & (1<<PB0))//PB0 (pin 14) - 7th input button
	{
		if (nextPbState & (1<<PB0))
		{ //BUTTON RELEASE:
		}
		else
		{ //BUTTON PRESS:
			rotation++;  // Cycle through the rotations up
				if (rotation > 3)
					rotation = 0;
			CheckX(&x, (currentShape & 0x07), rotation);
		}
	}
	pdState = nextPdState;
	pbState = nextPbState;
}
/*******************************************************************************
************************** Function implementation *****************************
*******************************************************************************/
inline void flipBuffers()
{
	uint8_t *tmp;
	tmp = otherBuf;
	otherBuf = curBuf;
	curBuf = tmp;
}

inline void waitForVSync()
{
	while (row != 16) { }
}

uint8_t adc_get_next_bit() {
  while (ADCSRA & (1<<ADSC)) { }
  uint16_t adc_lo = ADCL;
  uint16_t adc_hi_ignored = ADCH ^ ADCH;
  ADCSRA |= (1<<ADSC);
  return adc_lo & 1;
}
 
uint16_t random_seed_from_ADC() {
  uint16_t seed = 0;
  int8_t i, b;
  for (i = 0; i < 100; i++) {
    for (b = 0; b < 16; b++) {
      seed ^= (adc_get_next_bit() << b);
    }
  }
  return seed;
}

uint16_t adc_read() {
	while(ADCSRA & (1<<ADSC)) {  }
	uint16_t result = ADCL;
	uint16_t temp = ADCH;
	result = result + (temp<<8);
	ADCSRA |= (1<<ADSC);
	return result;
}


inline void setup() // Prepare system for operation
{
	DDRB |= (1<<PB3) | (1<<PB5); // SPI
    DDRB |= (1<<PB1) | (1<<PB2); // | Setup outputs for 
    DDRC |= (1<<PC4) | (1<<PC5); // |  image transfer 
    DDRB &= ~(1<<PB0);												// | Button
    DDRD &= ~(1<<PD7 | 1<<PD6 | 1<<PD5 | 1<<PD4 | 1<<PD3 | 1<<PD2);	// | Inputs
    PORTB |= (1<<PB0);												// |  and
    PORTD |= (1<<PD7 | 1<<PD6 | 1<<PD5 | 1<<PD4 | 1<<PD3 | 1<<PD2);	// | Pullups
    SPCR = 0xD0; // 0b 11010000
    SPSR = 0; // set to 1 for twice the SPI clock
    PORTC |= (1<<PC5);  // |  Reset row
    PORTC &= ~(1<<PC5); // |   Counter
    transferByte = 0; // | Prepare
    row = 1;          // | volatile 
    curBuf = buf1;    // | variables
    otherBuf = buf2;
    // Set up AD converter:
    ADMUX = 0;
  	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
  	ADCSRA |= (1<<ADSC);
    uint16_t seed = random_seed_from_ADC(); // Get random number from cirquit noise
    srand(seed); // randomize
    // setup Timer0:
    // CTC (Clear Timer on Compare Match mode)
    // TOP set by OCR0A register
    TCCR0A |= (1<<WGM01);
    // clocked from CLK/1024
    // which is 14745600/1024, or 14400 increments per second
    // (14400 / 15 = 960 per second   960 / 16 = 60Hz refresh rate)
    TCCR0B |= (1<<CS02) | (1<<CS00);
    // set TOP to 14
    // because it counts 0, 1, 2, ... 12, 13, 14, 0, 1, 2 ...
    // so 0 through 14 equals 15 events
    OCR0A = 14;
    // enable interrupt on compare event
    TIMSK0 |= (1<<OCIE0A);
    pdState = PIND;
    pbState = PINB;
    
    sei(); // Global interrupt enable.

    // rand() generated random number, and seed is taken from the noise of the AD converter
}

/*******************************************************************************
************************** Main function ***************************************
*******************************************************************************/
int main()
{
	setup();
	
 	uint8_t field[96] = {0x00};
 	
 	int8_t shift = 0;		// | Variables for 
 	uint8_t direction = 1;	// | text output
 	
 	uint8_t nextShape[3] = {0x00}; // First 3 bits from LSB are shape, next 3 are color
 	uint8_t level, count;  // current level, frame count
 	uint32_t score; // current score
 	

  	
    while (1)
    {
    	FloodFill(otherBuf, 0x00); // clear the screen
    	if (gameFlags & (1<<CLEAR_SHIFT))
    	{
    		shift = 0;
    		direction = 1;
    		gameFlags &= ~(1<<CLEAR_SHIFT);
       	}
    	switch (screen)
    	{
    		case 0: // Menu Screen
    		{
    			gameFlags |= (1<<NEW_GAME);
    			PrintChar(otherBuf, (shift+1), 1, 29, 0x01); 	// T
    			PrintChar(otherBuf, (shift+8), 1, 40, 0x01);	// e
    			PrintChar(otherBuf, (shift+15), 1, 55, 0x01);	// t
    			PrintChar(otherBuf, (shift+22), 1, 53, 0x01);	// r
	    		PrintChar(otherBuf, (shift+29), 1, 44, 0x01);	// i
	    		PrintChar(otherBuf, (shift+36), 1, 54, 0x01);	// s
	    		DrawShape(otherBuf, (shift+9), 11, 'S', 0, 0x03);
	    		DrawShape(otherBuf, (shift+16), 13, 'I', 1, 0x06);
	    		DrawShape(otherBuf, (shift+23), 12, 'T', 1, 0x04);
	    		DrawShape(otherBuf, (shift+30), 11, 'L', 1, 0x02);
	    		if (direction)
	    			shift++;
	    		else 
	    			shift--;
	    		
	    		if ((shift >= 16) || (shift <= -42))
	    			direction = !direction;
	    		delay_ms(20);
	    		break;
    		} 
    		case 1: // Game screen
    		{
    			if (gameFlags & (1<<NEW_GAME)) // Game just started
    			{
    				nextShape[0] = ((rand() % 7) | (((rand() % 7) +1)<<3)); // | Fill in
    				nextShape[1] = ((rand() % 7) | (((rand() % 7) +1)<<3)); // | the shape
    				nextShape[2] = ((rand() % 7) | (((rand() % 7) +1)<<3)); // | stack
    				score = 0;
    				count = 0;
    				FloodFill(field, 0x00); // clear gaming field
    				shift = 0;
    				direction = 1;
    				// unset new_game flag and all others, just in case
    				gameFlags = (1<<NEW_SHAPE); // Generate new shape
    			}
    			
    			level = (score / LEVEL_GAIN) + 1; // increase level every LEVEL_GAIN points
    			if (level > MAX_LEVEL)
    				level = MAX_LEVEL;

    			if (gameFlags & (1<<PLACE_SHAPE)) // Check weither we need to save shape this frame
	   			{
	   				if (y < 1) // No way to build higher
	   				{
	   					gameFlags |= (1<<CLEAR_SHIFT);
	   					screen = 3; // Game over
	   					break;
	   				}
	   				gameFlags |= (1<<NEW_SHAPE); // Need To start new shape
	   				DrawShape(field, x, y, (currentShape & 0x07), rotation, ((currentShape & (0x07<<3))>>3)); // Save shape to the field
	   				score += 10; // add 10 points for every positioned shape
	   				uint8_t tmpRow, tmpCol, fflag;
	   				for (tmpRow=16; tmpRow>=1; tmpRow--) // scan the whole gaming field to look for the rows that need to be erased
	   				{
	   					fflag = 1;// Hope for the best
	   					for (tmpCol=1; tmpCol<=10; tmpCol++)
	   						if (GetPixel(field, tmpCol, tmpRow) == 0)
	   						{
	   							fflag = 0;
	   							break;	
	   						}
	   					if (fflag) // If the flag is still up, we must've found full row
	   					{
	   						DrawLine(field, 1, tmpRow, 10, tmpRow, 0x07); // Fill in Found Row with white
	   						score += 100; // add 100 points for every deleted row
	   						count = 0;
	   						gameFlags |= (1<<ROW_FOUND);
	   					}
	   				}
	   				
	   				gameFlags |= (1<<NEW_SHAPE);
	   				gameFlags &= ~(1<<PLACE_SHAPE);
	   			}
	   			
	   			if (gameFlags & (1<<NEW_SHAPE)) // pop shape in line and generate new shape
    			{
    				y = -2;
    				x = 5;
    				rotation = (rand() % 4);
    				currentShape = nextShape[0];
    				nextShape[0] = nextShape[1];
    				nextShape[1] = nextShape[2];
    				nextShape[2] = ((rand() % 7) | (((rand() % 7) +1)<<3));
    				gameFlags &= ~(1<<NEW_SHAPE); // unset new_shape flag
    			}
    			
    			
    			BitBlt(field, 1, 1, 16, 16, otherBuf, 1, 1); // copy the field to screen
    			
    			DrawLine(otherBuf, 11, 1, 11, 16, 0x07); // Line separating gaming area from shape stack
    			
    			DrawShape(otherBuf, 13, 2, (nextShape[0] & 0x07), 0, ((nextShape[0] & (0x07<<3))>>3));	// |   Shapes
    			DrawShape(otherBuf, 13, 7, (nextShape[1] & 0x07), 0, ((nextShape[1] & (0x07<<3))>>3));	// |    in a
    			DrawShape(otherBuf, 13, 12, (nextShape[2] & 0x07), 0, ((nextShape[2] & (0x07<<3))>>3));	// | Shape stack
	    		DrawShape(otherBuf, x, y, (currentShape & 0x07), rotation, ((currentShape & (0x07<<3))>>3)); // Draw current palyable shape
	    		
	    		if (gameFlags & (1<<ROW_FOUND))
	    		{
	   				if (count > (MAX_LEVEL - level)) // Full row stays white untill we reached needed frame count
		   			{
		   				uint8_t tmpRow, tmpCol, fflag;
						for (tmpRow=16; tmpRow>=1; tmpRow--)
						{
							fflag = 1;// Hope for the best
							for (tmpCol=1; tmpCol<=10; tmpCol++)
								if ((GetPixel(field, tmpCol, tmpRow) != 0x07))
								{
									fflag = 0;
									break;	
								}
						if (fflag) // If the flag is still up, we must've found full row
							{
								BitBlt(otherBuf, 1, 1, 10, (tmpRow-1), field, 1, 2); // shift gaming area 1 down
								BitBlt(field, 1, 1, 10, 16, otherBuf, 1, 1);  // make sure to save changes to the second buffer
								tmpRow++;
							}
						}
						gameFlags &= ~(1<<ROW_FOUND);
						count = 0;
					}
				}
	    		else
		   		if (((gameFlags & (1<<FAST_DROP)) && (count >1)) || ((!(gameFlags & (1<<FAST_DROP))) && (count > (MAX_LEVEL - level)))) // if we are dropping shape fast then wait till cound > 1, else wait till it's biggeer then MAX_LEVEL-level
			   	{	
   					gameFlags |= (ShouldPlace(field, x, y, (currentShape & 0x07), rotation)<<PLACE_SHAPE); //Check pixels to see when the shape reaches bottom or a pile
					if (!(gameFlags & (1<<PLACE_SHAPE)))
						y++;
					if (gameFlags & (1<<FAST_DROP))
						score++; // add one point for every lowering with a fast drop
   					count = 0;	
	   			}
	   				
   				
   				count++;
   				break;
    		} // End of main game code
    		case 2: // pause screen
    		{
    			uint8_t lvl1, lvl2;
    			uint32_t tmpLevel;
    			tmpLevel = level;
    			PrintChar(otherBuf, (shift+1), 1, 25, 0x04); 	// P
    			PrintChar(otherBuf, (shift+8), 1, 36, 0x04);	// a
    			PrintChar(otherBuf, (shift+15), 1, 56, 0x04);	// u
    			PrintChar(otherBuf, (shift+22), 1, 54, 0x04);	// s
	    		PrintChar(otherBuf, (shift+29), 1, 40, 0x04);	// e
	    		
	    		PrintChar(otherBuf, (shift+40), 1, 21, 0x02);	// L
	    		PrintChar(otherBuf, (shift+47), 1, 40, 0x02);	// e
	    		PrintChar(otherBuf, (shift+54), 1, 57, 0x02);	// v
	    		PrintChar(otherBuf, (shift+61), 1, 40, 0x02);	// e
	    		PrintChar(otherBuf, (shift+68), 1, 47, 0x02);	// l
	    		lvl2 = (tmpLevel / 10);
	    		tmpLevel -= lvl2*10;
	    		lvl1 = tmpLevel;
	    		
	    		PrintChar(otherBuf, (shift+31), 9, lvl2, 0x06);	// | Current
    			PrintChar(otherBuf, (shift+38), 9, lvl1, 0x06); // |  Level
	    		
    			if (direction)
	    			shift++;
	    		else 
	    			shift--;
	    		
	    		if ((shift >= 16) || (shift <= -74))
	    			direction = !direction;
	    		delay_ms(20);
	    		break;    		
    		}
    		case 3:
    		{
    			uint8_t ch1, ch2, ch3, ch4, ch5, ch6;
    			uint32_t tmpScore;
    			tmpScore = score;
    			PrintChar(otherBuf, (shift+1), 1, 16, 0x01); 	// G
    			PrintChar(otherBuf, (shift+8), 1, 36, 0x01);	// a
    			PrintChar(otherBuf, (shift+15), 1, 48, 0x01);	// m
    			PrintChar(otherBuf, (shift+22), 1, 40, 0x01);	// e
    			
	    		PrintChar(otherBuf, (shift+33), 1, 24, 0x01);	// O
	    		PrintChar(otherBuf, (shift+40), 1, 57, 0x01);	// v
	    		PrintChar(otherBuf, (shift+47), 1, 40, 0x01);	// e
	    		PrintChar(otherBuf, (shift+54), 1, 53, 0x01);	// r
	    		ch6 = (tmpScore / 100000);
	    		tmpScore -= ch6*100000;
	    		ch5 = (tmpScore / 10000);
	    		tmpScore -= ch5*10000;
	    		ch4 = (tmpScore / 1000);
	    		tmpScore -= ch4*1000;
	    		ch3 = (tmpScore / 100);
	    		tmpScore -= ch3*100;
	    		ch2 = (tmpScore / 10);
	    		tmpScore -= ch2*10;
	    		ch1 = tmpScore;
	    		
	    		PrintChar(otherBuf, (shift+10), 9, ch6, 0x02); // |
    			PrintChar(otherBuf, (shift+17), 9, ch5, 0x02); // | Final	
	    		PrintChar(otherBuf, (shift+24), 9, ch4, 0x02); // |		    		
	    		PrintChar(otherBuf, (shift+31), 9, ch3, 0x02); // |	
	    		PrintChar(otherBuf, (shift+38), 9, ch2, 0x02); // |	score
	    		PrintChar(otherBuf, (shift+45), 9, ch1, 0x02); // |		    		
	    		
    			if (direction)
	    			shift++;
	    		else 
	    			shift--;
	    		
	    		if ((shift >= 16) || (shift <= -60))
	    			direction = !direction;
	    		delay_ms(20);
	    		break; 
    		}
    		default:
    		{
    			screen = 0;
    		 break;
    		}
    	}
    	
    	waitForVSync();
    	flipBuffers();
    }
    
	while (1) { } // a loop to catch up wrong breaks, should be erased if the above code works well   
    return 0;
}
