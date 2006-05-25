/*             -----------------------
             // AVR SIMPLE SIMON GAME \\
             \\ (C) Dean Camera, 2005 //
              -----------------------
              dean_camera@hotmail.com
              -----------------------

   Includes a demonstration of a simple state machine,
     random number generation, timers and PWM sound.

   Designed for the MEGA8515 MCU running at 1MHz. May
   be placed into a STK500, with PORTD.5 connected to
   a speaker (via an amp), PORTC connected to the LEDs
   and PORTA conected to the Switches. Any AVR with 9
   free I/Os and an 8 and 16 bit timer which has PWM
   capability may be used.

        Compile with the free AVR-GCC C compiler.

   ** This is version 2 of the code. This code generates
   the ext random sequence byte as the last sequence byte
   is entered by the use by timing how long the button was
   held down with by timer 0. This code thus ommits the
   Rand() function and so is a good half a kilobyte smaller
   than the previous version of the code.

   =====================================================
                  ++ INSTRUCTIONS: ++

   When the MCU powers up, all four lights will be active.
   Press any of the four corresponding buttons to start
   the game. Once the button is released, a single LED
   will flash briefly - press the corresponding button.

   All four LEDs will flash three times to indicate a
   correct entry. The same LED will again light, followed
   by another random LED. Repeat the sequence, the LEDs
   will all flash again and the sequence will begin again,
   getting longer by one LED after each correct entry. If
   at any point you make an error in repeating the sequence,
   all four LEDs will turn on and you will need to reset
   the MCU to begin another game.
   =====================================================

              [[{{ Feedback appreciated. }}]]
*/

// INCLUDES
#include <avr/io.h>
#include <util/delay.h>

// PORT/PIN DEFINES
#define LEDport     PORTC
#define LEDddr      DDRC
#define BUTTONport  PORTA
#define BUTTONpin   PINA
#define BUTTONddr   DDRA
#define SPEAKERport PORTD /* Cannot change this, timer 1 is connected to PORTD.5 */
#define SPEAKERddr  DDRD

// CONSTANT DEFINES
#define OUTPUTS     0xFF
#define INPUTS      0x00
#define PULLUPS     0xFF
#define BUFFERLEN   20   /* Change to set sequence length */

// STATE DEFINES
#define STATE_Setup          0
#define STATE_PlayNextSeq    1
#define STATE_WaitForPlayer  2
#define STATE_CorrectSeq     3
#define STATE_LoseGame       4
#define STATE_WinGame        5

// MACROS
#define GetButton() (uint8_t)(~BUTTONpin & 0x0F)
#define Timer1On()  TCCR1B |= _BV(CS10)
#define Timer1Off() TCCR1B &= ~(_BV(CS10)); PORTD &= ~_BV(5)
#define Timer0On()  TCCR0  |= _BV(CS00);
#define SoundOn()   Timer1On()    /* This define just aliases to another macro, to make the program clearer */
#define SoundOff()  Timer1Off()   /* This define just aliases to another macro, to make the program clearer */

// PROTOTYPES
void GenerateTone(uint8_t SeqByte);
uint8_t CreateTimerRand(void);
void Delay10MS(uint8_t Num);

// TONE TABLE
uint8_t PWMValues[] = {100, 115, 130, 145};   // Lookup table for the tone compare values: (1000000 / x) = Resulting frequency

// PROGRAM ROUTINES
int main(void)
{
	uint8_t CurrentState              = STATE_Setup;
	uint8_t SequenceBuffer[BUFFERLEN] = {};
	uint8_t CurrSeqPosC               = 0;
	uint8_t CurrentLevel              = 0;

	SPEAKERddr   = OUTPUTS;       // SPEAKER port set as outputs
	LEDddr       = OUTPUTS;       // LED port set as outputs
	BUTTONddr    = INPUTS;        // BUTTON port set as inputs

	SPEAKERport  = 0x00;          // Speaker off initially
	LEDport      = 0xFF;          // All LEDs off initially
	BUTTONport   = PULLUPS;       // Enable pullups on the BUTTON port

	TCCR1A       = _BV(COM1A0);   // Toggle timer1 output on match
	TCCR1B       = _BV(WGM12);    // Clear timer on compare mode

	Timer0On();                   // Turn on the timer 0, to run at 1MHz (clock). This is used to determine the
	                              // Random sequence bytes; it's value is read after the last sequence byte is
	                              // entered by the user.

	while (1)                     // Infinite Loop
	{
		switch (CurrentState)
		{
			case STATE_Setup:
				CurrentLevel = 1;          // Reset current level variable
				CurrSeqPosC  = 0;          // Reset current sequence position variable

				LEDport      = 0xF0;       // All LEDs on

				while (!(GetButton())) {}  // Wait until a button is pressed, store pressed button

				LEDport      = 0xFF;       // All LEDs off

				SequenceBuffer[0] = CreateTimerRand(); // Create a random sequence byte from the timer value for the first sequence

				Delay10MS(40);             // Wait a 400ms before continuing

				CurrentState = STATE_PlayNextSeq;
				break;
			case STATE_PlayNextSeq:
				GenerateTone(SequenceBuffer[CurrSeqPosC]);           // Turn on the tone generator

				LEDport &= (0xF0 | ~SequenceBuffer[CurrSeqPosC]);    // Turn on sequence LED
				Delay10MS(20);                                       // Wait 200ms
				LEDport |= (SequenceBuffer[CurrSeqPosC]   | 0xF0);   // Turn off sequence LED
				Delay10MS(20);                                       // Wait 200ms
				
				SoundOff();                                          // Turn off the tone

				if (++CurrSeqPosC == CurrentLevel)                   // Sequence playing complete, wait for player input
				{
					CurrSeqPosC  = 0;                                  // Reset sequence position counter to 0
					CurrentState = STATE_WaitForPlayer;
				}
				else                                                 // Sequence still playing
				{
					CurrentState = STATE_PlayNextSeq;
				}
				break;
			case STATE_WaitForPlayer:
				while (GetButton()) {};                              // Wait until all buttons released before accepting key

    		uint8_t PressedButton = 0;

				while (!(PressedButton))                             // Wait until a button is pressed, store pressed button
					PressedButton = GetButton();

				GenerateTone(PressedButton);

				LEDport &= (~PressedButton | 0xF0);                  // Light up the pressed button's LED
				Delay10MS(20);                                       // Wait 200ms
				LEDport |= (PressedButton  | 0xF0);                  // Turn off the pressed button's LED
				Delay10MS(20);                                       // Wait 200ms

				SoundOff();

				if (PressedButton == SequenceBuffer[CurrSeqPosC])    // Correct button pressed
				{
					if (++CurrSeqPosC == CurrentLevel)                 // Sequence finished by player
					{
						CurrentLevel++;                                  // Increase the level by one
						CurrSeqPosC = 0;                                 // Reset sequence position counter to 0

						if (CurrentLevel > BUFFERLEN)                    // A genious has completed the entire sequence
						{
							CurrentState = STATE_WinGame;
						}
						else                                             // Still more room in the buffer, create a new random byte and set the state accordingly
						{
							SequenceBuffer[CurrentLevel - 1] = CreateTimerRand(); // Create the next sequence byte from the timer

							CurrentState = STATE_CorrectSeq;
						}
					}
					else
					{
						CurrentState = STATE_WaitForPlayer;
					}
				}
				else
				{
					CurrentState = STATE_LoseGame;
				}
				break;
			case STATE_CorrectSeq:
				for (uint8_t FlashCount=0; FlashCount<3; FlashCount++) // Flash the LEDs three times
				{
					LEDport = 0xF0;                                 // Turn on all the LEDs
					Delay10MS(8);                                   // Wait 80ms
					LEDport = 0xFF;                                 // Turn off all the LEDs
					Delay10MS(8);                                   // Wait 80ms
				}

				CurrentState = STATE_PlayNextSeq;
				break;
			case STATE_LoseGame:
				LEDport = 0xF0;                // Turn on all LEDs

				CurrentState = STATE_LoseGame;
				break;
			case STATE_WinGame:
				LEDport = (~LEDport | 0xF0);   // Invert LEDs status
				Delay10MS(20);                 // Wait 200ms before continuing

				CurrentState = STATE_WinGame;
				break;
		}
	}
}

void GenerateTone(uint8_t SeqByte)
{
	// Translates the bit shift back into decimal:
	if      (SeqByte == _BV(0))  SeqByte = 0;
	else if (SeqByte == _BV(1))  SeqByte = 1;
	else if (SeqByte == _BV(2))  SeqByte = 2;
	else                         SeqByte = 3;

	TCNT1 = 0;                  // Reset the timer value register
	OCR1A = PWMValues[SeqByte]; // Set the compare register to the translated value
	SoundOn();                  // Turn on timer 1 to start the sound generation
}

uint8_t CreateTimerRand(void)
{
	uint8_t RVal = TCNT0;   // Get the timer0 value into a variable

	// Priority encoder: Uses ordered tests to save code so that only
	// the first matching test code is executed.
	if      (RVal <= 64)  RVal = _BV(0);
	else if (RVal <= 128) RVal = _BV(1);
	else if (RVal <= 192) RVal = _BV(2);
	else                  RVal = _BV(3);

	return RVal;            // Shift 1 by the new random number between 0 and 3, return the new sequence byte
}

void Delay10MS(uint8_t Num)
{
	// _delay_ms(x) has a limitation on the maximum delay, which depends on the MPU clock
	// frequency. The maximum safe value for _delay_ms that is guaranteed to be correct for
	// any frequency up to about 20MHz (current AVR maximum) is about 10ms. Because of this,
	// I have added in this tiny function (smaller than repeating the while loop inside the
	// main code) to handle large delays. Delays are in blocks of 10ms.

	while(Num--)
	   _delay_ms(10);
}
