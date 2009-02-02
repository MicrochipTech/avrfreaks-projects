/*
               BUTTLCD -- Butterfly LCD Driver

               Copyright (C) Dean Camera, 2008

            dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

         added joystick driver  3/2008
         Jim Remington (sjames_remington at yahoo.com) 
         last update 2/1/2009. Fixed joystick routine to initialize only those pins of PORTE that are used.

*/

/*
   This is a basic driver for the Butterfly LCD. It offers the ability to
   change the contrast and display strings (scrolling or static) from flash
   or SRAM memory only.
   
   This has been completly rewritten from the Atmel code; in this version, as
   much processing as possible is performed by the string display routines
   rather than the interrupt so that the interrupt executes as fast as possible.

   Added features (3/2008): 
   Flashing Characters in LCD driver
   Joystick driver getkey() with character autorepeat
   
   global variable LCD_Flash controls flashing characters.
   nonzero = flash characters corresponding to bits set in LCD_Flash
   (numbering display from left to right, leftmost digit=low bit in LCD_Flash)
   e.g. 
   LCD_Flash=1, flash leftmost character
   LCD_Flash=0x3F, flash all characters
   Note: Flash only works for static (non scrolling) display!

*/


#define INC_FROM_DRIVER
#include "LCD_Joystick.h"

// define globals for LCD routines
//                                  LCD Text            + extra nulls for scrolling + Null Termination
static volatile char     TextBuffer[LCD_TEXTBUFFER_SIZE + LCD_DISPLAY_SIZE    + 1] = {};
static volatile uint8_t  StrStart        = 0;
static volatile uint8_t  StrEnd          = 0;
static volatile uint8_t  ScrollCount     = 0;
static volatile uint8_t  UpdateDisplay   = false;
static volatile uint8_t  ShowColons      = false;
       volatile uint8_t  ScrollFlags     = 0;
static volatile uint8_t  FlashTimer      = 0;   //10 refresh frames; 5 frames on, 5 frames off
static volatile uint8_t    LCD_Flash       = 0;   //flag for blinking digits, bits set=digits to flash

// define globals for joystick routines

static volatile uint8_t  DebounceTimeout   = 0;      //timer for joystick/button debounce
static volatile uint8_t  AutoRepeatTimeout  = 0;    //timer for joystick/button autorepeat frequency
static volatile uint8_t  KeyAutoRepeat = false;      //joystick auto repeat enable if true
static volatile uint8_t    KEY = 0;               //buffer containing virtual key pressed
static volatile uint8_t    KEY_VALID = false;         //key valid (debounced) flag

#define AutoRepeatPeriod 32                     //key repeat period in units of LCD frame rate (64 Hz)

const           uint16_t LCD_SegTable[] PROGMEM =
{
    0xEAA8,     // '*'
    0x2A80,     // '+'
    0x4000,     // ','
    0x0A00,     // '-'
    0x0A51,     // '.' Degree sign
    0x4008,     // '/'
    0x5559,     // '0'
    0x0118,     // '1'
    0x1e11,     // '2
    0x1b11,     // '3
    0x0b50,     // '4
    0x1b41,     // '5
    0x1f41,     // '6
    0x0111,     // '7
    0x1f51,     // '8
    0x1b51,     // '9'
    0x0000,     // ':' (Not defined)
    0x0000,     // ';' (Not defined)
    0x8008,     // '<'
    0x1A00,     // '='
    0x4020,     // '>'
    0x0000,     // '?' (Not defined)
    0x0000,     // '@' (Not defined)
    0x0f51,     // 'A' (+ 'a')
    0x3991,     // 'B' (+ 'b')
    0x1441,     // 'C' (+ 'c')
    0x3191,     // 'D' (+ 'd')
    0x1e41,     // 'E' (+ 'e')
    0x0e41,     // 'F' (+ 'f')
    0x1d41,     // 'G' (+ 'g')
    0x0f50,     // 'H' (+ 'h')
    0x2080,     // 'I' (+ 'i')
    0x1510,     // 'J' (+ 'j')
    0x8648,     // 'K' (+ 'k')
    0x1440,     // 'L' (+ 'l')
    0x0578,     // 'M' (+ 'm')
    0x8570,     // 'N' (+ 'n')
    0x1551,     // 'O' (+ 'o')
    0x0e51,     // 'P' (+ 'p')
    0x9551,     // 'Q' (+ 'q')
    0x8e51,     // 'R' (+ 'r')
    0x9021,     // 'S' (+ 's')
    0x2081,     // 'T' (+ 't')
    0x1550,     // 'U' (+ 'u')
    0x4448,     // 'V' (+ 'v')
    0xc550,     // 'W' (+ 'w')
    0xc028,     // 'X' (+ 'x')
    0x2028,     // 'Y' (+ 'y')
    0x5009,     // 'Z' (+ 'z')
    0x1441,     // '['
    0x8020,     // '\'
    0x1111,     // ']'
    0x0000,     // '^' (Not defined)
    0x1000      // '_'
};

// ======================================================================================

/*
 NAME:      | LCD_Init
 PURPOSE:   | Initializes the Butterfly's LCD for correct operation, ready to display data
 ARGUMENTS: | None
 RETURNS:   | None
*/
void LCD_Init(void)
{
   // Set the initial contrast level to maximum:
   LCD_CONTRAST_LEVEL(0x0F);

    // Select asynchronous clock source, enable all COM pins and enable all segment pins:
    LCDCRB  = (1<<LCDCS) | (3<<LCDMUX0) | (7<<LCDPM0);

    // Set LCD prescaler to give a framerate of 64Hz:
    LCDFRR  = (0<<LCDPS0) | (3<<LCDCD0);   

   // Enable LCD and set low power waveform, enable start of frame interrupt:
    LCDCRA  = (1<<LCDEN) | (1<<LCDAB) | (1<<LCDIE);
}

/*
 NAME:      | LCD_puts
 PURPOSE:   | Displays a string from flash onto the Butterfly's LCD
 ARGUMENTS: | Pointer to the start of the flash string
 RETURNS:   | None
*/
void LCD_puts_f(const char *FlashData)
{
   /* Rather than create a new buffer here (wasting RAM), the TextBuffer global
      is re-used as a temp buffer. Once the ASCII data is loaded in to TextBuffer,
      LCD_puts is called with it to post-process it into the correct format for the
      LCD interrupt.                                                                */

   strcpy_P((char*)&TextBuffer[0], FlashData);
   LCD_puts((char*)&TextBuffer[0]);
}

/*
 NAME:      | LCD_puts
 PURPOSE:   | Displays a string from SRAM onto the Butterfly's LCD
 ARGUMENTS: | Pointer to the start of the SRAM string
 RETURNS:   | None
*/
void LCD_puts(const char *Data)
{
   uint8_t Nulls,LoadB = 0;
   uint8_t CurrByte;

   do
   {
      CurrByte = *(Data++);
      
      switch (CurrByte)
      {
         case 'a'...'z':
            CurrByte &= ~(1 << 5);                   // Translate to upper-case character
         case '*'...'_':                                // Valid character, load it into the array
            TextBuffer[LoadB++] = (CurrByte - '*');
            break;
         case 0x00:                                   // Null termination of the string - ignore for now so the nulls can be appended below
            break;
         default:                                     // Space or invalid character, use 0xFF to display a blank
            TextBuffer[LoadB++] = LCD_SPACE_OR_INVALID_CHAR;
      }
   }
   while (CurrByte && (LoadB < LCD_TEXTBUFFER_SIZE));

   ScrollFlags = ((LoadB > LCD_DISPLAY_SIZE)? LCD_FLAG_SCROLL : 0x00);

   for (Nulls = 0; Nulls < LCD_DISPLAY_SIZE; Nulls++)   // was: Nulls < 7, should be 6 anyway.
     TextBuffer[LoadB++] = LCD_SPACE_OR_INVALID_CHAR;  // Load in nulls to ensure that when scrolling, the display clears before wrapping
   
   TextBuffer[LoadB] = 0x00;                           // Null-terminate string
   
   StrStart      = 0;
   StrEnd        = LoadB;
   ScrollCount   = LCD_SCROLLCOUNT_DEFAULT + LCD_DELAYCOUNT_DEFAULT;
   UpdateDisplay = true;
   if (LCD_Flash) FlashTimer=FLASH_TIMER_SEED;      //start frame counter for flashing digits

}

/*
 NAME:      | LCD_vect (ISR, blocking)
 PURPOSE:   | ISR to handle the display and scrolling of the current display string onto the LCD
 ARGUMENTS: | None
 RETURNS:   | None
*/

ISR(LCD_vect)
{
uint8_t Character;

//**** begin Joystick stuff

// Debounce timeout and key validity check for the joystick

    if(DebounceTimeout>0) 
      {
      DebounceTimeout--;
      if (DebounceTimeout==0) //completed timeout for key debounce
         {   
         if (KEY==ReadButtons())    //same key still down?
         KEY_VALID=true;  //yes, tell the world
         }
      }

// Autorepeat timeout for joystick data entry.

    if(AutoRepeatTimeout>0)
   { 
   AutoRepeatTimeout--;   
   if (AutoRepeatTimeout==0) //got here so AutoRepeatTimeout must have been set previously
      {
      if (KEY==ReadButtons()) KEY_VALID=true;   //if button still pressed, send last virtual key again
      if (KeyAutoRepeat) AutoRepeatTimeout=AutoRepeatPeriod;   //reload counter only if autorepeat flag on
      }
   }


//**** end Joystick stuff

   if (ScrollFlags & LCD_FLAG_SCROLL)
   {
      if (!(ScrollCount--))
      {
         UpdateDisplay = true;
         ScrollCount   = LCD_SCROLLCOUNT_DEFAULT;
      }
   }

   if (LCD_Flash)   //Are any digits blinking? DOES NOT WORK WITH SCROLLING DISPLAY
   {
      if (FlashTimer == FLASH_TIMER_SEED)       //(8 frames on, 8 off)
       {
        for (Character = 0; Character < LCD_DISPLAY_SIZE; Character++)
         {
            LCD_WriteChar(TextBuffer[Character], Character);
         }
      }

      if ( FlashTimer == (FLASH_TIMER_SEED>>1) )   //(8 frames on, 8 off)
      {
         for (Character = 0; Character < LCD_DISPLAY_SIZE; Character++)
            {
              if( LCD_Flash&(1<<Character) )  //check that character position
            LCD_WriteChar(LCD_SPACE_OR_INVALID_CHAR, Character);
         }
      }

      if (LCD_Flash && ((FlashTimer--) == 0)) FlashTimer=FLASH_TIMER_SEED;   //Reset blanking timer (16 frame cycle)
   }

   if (UpdateDisplay)
   {
      for (Character = 0; Character < LCD_DISPLAY_SIZE; Character++)
      {
         uint8_t Byte = (StrStart + Character);

      if (Byte >= StrEnd) 
          Byte -= StrEnd;

      LCD_WriteChar(TextBuffer[Byte], Character);
      }

      if ((StrStart + LCD_DISPLAY_SIZE) == StrEnd)    // Done scrolling message on LCD once
        ScrollFlags |= LCD_FLAG_SCROLL_DONE;
      
      if (StrStart++ == StrEnd)
        StrStart     = 1;

       if (ShowColons)
            *((uint8_t*)(LCD_LCDREGS_START + 8)) = 0x01;
        else
            *((uint8_t*)(LCD_LCDREGS_START + 8)) = 0x00;

      UpdateDisplay  = false;                         // Clear LCD management flags, LCD update is complete
   }
}

/*
 NAME:      | LCD_WriteChar (static, inline)
 PURPOSE:   | Routine to write a character to the correct LCD registers for display
 ARGUMENTS: | Character to display, LCD character number to display character on
 RETURNS:   | None
*/
static inline void LCD_WriteChar(const uint8_t Byte, const uint8_t Digit)
{
    uint8_t BNib;
   uint8_t* BuffPtr = (uint8_t*)(LCD_LCDREGS_START + (Digit >> 1));
   uint16_t SegData = 0x0000;

   if (Byte != LCD_SPACE_OR_INVALID_CHAR)              // Null indicates invalid character or space
     SegData = pgm_read_word(&LCD_SegTable[Byte]);   

   for (BNib = 0; BNib < 4; BNib++)
   {
      uint8_t MaskedSegData = (SegData & 0x0000F);

      if (Digit & 0x01)
        *BuffPtr = ((*BuffPtr & 0x0F) | (MaskedSegData << 4));
      else
        *BuffPtr = ((*BuffPtr & 0xF0) | MaskedSegData);

      BuffPtr += 5;
      SegData >>= 4;
   }   
}

/*
 NAME:      | LCD_ShowColons
 PURPOSE:   | Routine to turn on or off the LCD's colons
 ARGUMENTS: | Boolean - true to turn on colons
 RETURNS:   | None
*/
void LCD_ShowColons(const uint8_t ColonsOn)
{
   ShowColons    = ColonsOn;
   UpdateDisplay = true;
}

/*
** Joystick driver, derived mostly from original code by Atmel. 
** Some processing removed from interrupt routine
** Jim Remington (sjames_remington@yahoo.com)
*/

#define BUTTON_A    6   // UP
#define BUTTON_B    7   // DOWN
#define BUTTON_C    2   // LEFT
#define BUTTON_D    3   // RIGHT
#define BUTTON_O    4   // PUSH

#define PINB_MASK ((1<<PINB4)|(1<<PINB6)|(1<<PINB7))
#define PINE_MASK ((1<<PINE2)|(1<<PINE3))

void Joystick_Init(void)
{
    // Init button port pins
   DDRB &= ~(PINB_MASK);
    PORTB |= PINB_MASK;  //pullups on
    DDRE &= ~(PINE_MASK);
    PORTE |= PINE_MASK;   // pullups on

    // Enable pin change interrupt on PORTB and PORTE
   PCMSK0 |= PINE_MASK;
   PCMSK1 |= PINB_MASK;
   EIFR |= (1<<PCIF0)|(1<<PCIF1);
   EIMSK |= (1<<PCIE0)|(1<<PCIE1);
}

/*
** Interrupt Service Routines
*/

ISR (PCINT0_vect)
{
   PinChangeInterrupt();
}

ISR (PCINT1_vect)
{
   PinChangeInterrupt();    
}

/*
** Process pin change interrupt. Set up LCD_Driver.c to process key debounce.
*/

void PinChangeInterrupt(void)
{
   KEY_VALID=false;   //set global valid flag false as not debounced yet.
   KEY=ReadButtons();   //put current key value in global KEY
   DebounceTimeout=3;   //start debounce timer (count LCD frames). After timeout, key debounce completed in LCD_Driver.c
}
/*
    Read the joystick buttons:

    Bit             7   6   5   4   3   2   1   0
    ---------------------------------------------
    PORTB           B   A       O
    PORTE                           D   C
    ---------------------------------------------
    PORTB | PORTE   B   A       O   D   C
    =============================================

   Returns: Virtual Key pressed.

*/

char ReadButtons(void)
{
    char buttons;
    char key;

    buttons  = (~PINB) & PINB_MASK;
    buttons |= (~PINE) & PINE_MASK;

    // Output virtual keys
    if (buttons & (1<<BUTTON_A))
        key = KEY_PLUS;
    else if (buttons & (1<<BUTTON_B))
        key = KEY_MINUS;
    else if (buttons & (1<<BUTTON_C))
        key = KEY_PREV;
    else if (buttons & (1<<BUTTON_D))
        key = KEY_NEXT;
    else if (buttons & (1<<BUTTON_O))
        key = KEY_ENTER;
    else
        key = KEY_NULL;         //invalid combination

    EIFR = (1<<PCIF1) | (1<<PCIF0);     // Reset pin change interrupt flags
   return key;                     // return key currently pressed
}

// get key from 1-character key buffer.
// returns KEY_NULL (=0) if none

char getkey(void)
{
    char k;

    cli();          // disable interrupts so 'KEY' won't change while in use
    if (KEY_VALID)   // Check for unread key in buffer
    {
        k = KEY;
        KEY_VALID = false;      //got that key!
   if (KeyAutoRepeat) AutoRepeatTimeout=AutoRepeatPeriod;   //load counter for key repeat if autorepeat flag set
    }
    else
        k = KEY_NULL; // No key stroke available
    sei(); // enable interrupts

    return k;
}
