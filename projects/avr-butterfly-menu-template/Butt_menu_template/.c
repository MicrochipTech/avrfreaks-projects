/* 

  AVR Butterfly menu-driven template for API development.
  Intended for WinAVR/gcc
  
  The user interface outline and LCD drivers were taken from Dean Camera's BUTTLOAD
  package. I've added interrupt-driven joystick drivers and a tone generator,
  based on the original Atmel Butterfly example code.
  S. James Remington sjames_remington at yahoo.com

  Default RC CPU clock of 8 MHz assumed
  
*/
  
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define ARRAY_UPPERBOUND(array)   ((sizeof(array) / sizeof(array[0])) - 1)
   
typedef void (*FuncPtr)(void);  //typedef pointer to function call

void MAIN_1(void);  //dummy main menu routines "Options"
void MAIN_2(void);  //"Start"
void MAIN_3(void);  //another function

void MAIN_TopMenu(void);  //Displays version number

// prototypes, more in LCD_Joystick (included below)

void Delay1ms(uint8_t loops);
void Delay10ms(int loops);
void PlayTone(uint8_t tone, uint8_t duration);


const char   Func_1_str[]         PROGMEM = "Options";
const char   Func_2_str[]         PROGMEM = "Start";
const char   Func_3_str[]         PROGMEM = "Func 3";

const char*   MainFunctionNames[]  PROGMEM = {Func_1_str, Func_2_str, Func_3_str};
const FuncPtr MainFunctionPtrs[]   PROGMEM = {MAIN_1, MAIN_2, MAIN_3};

const char   Message1[]            PROGMEM = "*Options menu*";
const char   Message2[]            PROGMEM = "*Running*";
const char   Message3[]            PROGMEM = "Exec 3";
const char   AboutText[]           PROGMEM = "Version 1,0";

#include "LCD_Joystick.c"

int main(void)
{   
   uint8_t CurrFunc = 0, key = 0, i;  //CurrFunc = currently selected menu option

// initialize the odd ports and registers
 
   ACSR    = (1 << ACD);                        // Disable unused analogue comparator to save power
   PRR     = ((1 << PRADC) | (1 << PRSPI) | (1 << PRUSART0)); // Disable subsystems to save power
   
   DDRE &= ~(1<<PE6);         //PORTE.6 = counter input
   PORTE |= (1<<PE6);         //turn on pullup. 
   DDRB |=(1<<5);            //buzzer output

   Joystick_Init();
   sei();    

   LCD_Init();
   LCD_puts_f(AboutText);      //display version number, etc.
   Delay10ms(200);            //hang 2 seconds

// display default top menu selection

   LCD_puts_f((char*)pgm_read_word(&MainFunctionNames[CurrFunc]));

// announce with some beeps
   ToneVol=200;		        //(default 80)
   ToneDuration = 20;       //about 3/10 second
   for (i=0; i<5; i++) {PlayTone((100 + (i<<3)),ToneDuration); Delay10ms(10);}
   
    for (;;)   //main loop allows user to cycle through menu options with "up" and "down" keys, "enter" = execute
   {
    key = getkey();
     if (key)
      {  PlayTone(100,6);  //key recognition beep

         if (key == KEY_PLUS)
           (CurrFunc == 0)? CurrFunc = ARRAY_UPPERBOUND(MainFunctionPtrs) : CurrFunc--;
         else if (key == KEY_MINUS)
           (CurrFunc == ARRAY_UPPERBOUND(MainFunctionPtrs))? CurrFunc = 0 : CurrFunc++;
         else if (key == KEY_ENTER)
           ((FuncPtr)pgm_read_word(&MainFunctionPtrs[CurrFunc]))(); // Execute associated subroutine
         else if (key == KEY_PREV)
           MAIN_TopMenu();         //display AboutText
//      else if (key == KEY_NEXT);  //do nothing

// print text associated with currently selected main menu function

         LCD_puts_f((char*)pgm_read_word(&MainFunctionNames[CurrFunc]));

       }
   }
}

/*
 NAME:      | PlayTone
 PURPOSE:   | Plays the passed tone on piezo speaker for duration in 64ths of a second
 ARGUMENTS: | uint8_t ToneVol, ToneDuration are global, 
          | -- defined in LCD_Joystick.c 
 RETURNS:   | None
*/
void PlayTone(const uint8_t tone, uint8_t duration)
{

   ToneDuration=duration;                  // Set global countdown in 64ths of a second
   OCR1A  = ToneVol;                     // Set the tone volume  (default 80, lower is louder)
   TCCR1A = (1 << COM1A1);                  // OC1A set when counting up, cleared when counting down
   TCCR1B = ((1 << WGM13) | (1 << CS10));      // Phase/Freq correct PWM mode, turn on with prescale of 1
   
   TCNT1 = 0;                           // reset counter
   ICR1  = ((uint16_t)tone << 5);               // Set the ICR register - play the tone. Timing done in LCD interrupt

}

/*
 NAME:      | Delay10ms
 PURPOSE:   | Delays for specified blocks of 10 milliseconds
 ARGUMENTS: | Number of blocks of 10ms to delay
 RETURNS:   | None
*/
void Delay10ms(int loops)
{
  /* Prevents the use of floating point libraries. Delaying in groups of
     10ms increases accuracy by reducing the time overhead for each loop
     interation of the while.                                            */

   while (loops--)
     _delay_ms(10);
}

/*
 NAME:      | Delay1ms
 PURPOSE:   | Delays for specified blocks of 1 milliseconds
 ARGUMENTS: | Number of blocks of 1ms to delay
 RETURNS:   | None
*/
void Delay1ms(uint8_t loops)
{
  /* Prevents the use of floating point libraries. Less accurate than the
     Delay10MS routine, but nessesary for many commands. The overhead required
     to call the routine is substantially less than the overhead required to
     calculate the float at compile time, so this actually saves execution time. */

   while (loops--)
     _delay_ms(1);
}
/*
 NAME:      | MAIN_TopMenu
 PURPOSE:   | Top of menu indicator. Shows string AboutText
 ARGUMENTS: | None
 RETURNS:   | None
*/
void MAIN_TopMenu(void)
{
   LCD_puts_f(AboutText);
   Delay10ms(300); //show for 3 seconds
}

/*
 NAME:      | MAIN_1
 PURPOSE:   |
 ARGUMENTS: | None
 RETURNS:   | None
*/

void MAIN_1(void)
{
   LCD_puts_f(Message1);
   Delay10ms(300);   //show for 3 seconds
}

/*
 NAME:      | MAIN_2
 PURPOSE:   | 
 ARGUMENTS: | None
 RETURNS:   | None
*/
void MAIN_2(void)
{
    LCD_puts_f(Message2);
   Delay10ms(300); //show for 3 seconds
}

/*
 NAME:      | MAIN_3
 PURPOSE:   | 
 ARGUMENTS: | None
 RETURNS:   | None
*/
void MAIN_3(void)
{
    LCD_puts_f(Message3);
   Delay10ms(300); //show for 3 seconds
}

