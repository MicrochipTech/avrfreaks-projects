// ButtonTest

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <inttypes.h>

#define KEY_NULL    0
#define KEY_ENTER   1
#define KEY_NEXT    2
#define KEY_PREV    3
#define KEY_PLUS    4
#define KEY_MINUS   5

#define BUTTON_A    6   // UP
#define BUTTON_B    7   // DOWN
#define BUTTON_C    2   // LEFT
#define BUTTON_D    3   // RIGHT
#define BUTTON_O    4   // PUSH

#define FALSE   0
#define TRUE    (!FALSE)
#define NULL    0

#define PINB_MASK ((1<<PINB4)|(1<<PINB6)|(1<<PINB7))
#define PINE_MASK ((1<<PINE2)|(1<<PINE3))

// declare functions
void PinChangeInterrupt(void);
char getkey(void);

// declare global variables
volatile char KEY = NULL;
volatile char KEY_VALID = FALSE;

int main (void)
{

    // Init port pins
	DDRB |= 0xD8;
    PORTB |= PINB_MASK;
    DDRE = 0x00;
    PORTE |= PINE_MASK;

    // Enable pin change interrupt on PORTB and PORTE
	PCMSK0 = PINE_MASK;
	PCMSK1 = PINB_MASK;
	EIFR = (1<<6)|(1<<7);
	EIMSK = (1<<6)|(1<<7);
	
	DDRD = 0xFF; // set PORTD for output
	DDRB = 0X00; // set PORTB for input
	
    PORTB = 0xFF; // enable pullup on for input
   	PORTD = 0XFF; // set LEDs off
	
	char input;
	
	while(1)
	{
		input = getkey();
		
		if(input == KEY_PLUS)PORTD = ~0x01; 
		else if(input == KEY_NEXT)PORTD = ~0x02;
	    else if(input == KEY_PREV)PORTD = ~0x04; 
        else if(input == KEY_MINUS)PORTD = ~0x08;
		else if(input == KEY_ENTER)PORTD = ~0x10;
	}
}       



SIGNAL(SIG_PIN_CHANGE0)
{
    PinChangeInterrupt();
}

SIGNAL(SIG_PIN_CHANGE1)
{
    PinChangeInterrupt();    
}

void PinChangeInterrupt(void)
{
    char buttons;
    char key;

    buttons = (~PINB) & PINB_MASK;
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
        key = KEY_NULL;
  
    if(key != KEY_NULL)
    {
        if (!KEY_VALID)
        {
            KEY = key;          // Store key in global key buffer
            KEY_VALID = TRUE;
        }
    }
    
    EIFR = (1<<PCIF1) | (1<<PCIF0);     // Delete pin change interrupt flags 
}



char getkey(void)
{
    char k;

    cli(); // disable interrrupts so 'KEY' won't change while in use

    if (KEY_VALID) // Check for unread key in buffer
    {
        k = KEY;
        KEY_VALID = FALSE;
    }
    else
        k = KEY_NULL; // No key stroke available

    sei(); // enable interrupts

    return k;
}
