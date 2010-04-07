
/*Demonstrator.c Real Time Clock version
	by Matt Durkin
	
	Displays the time on the AVR Butterfly LCD.
	Pressing the center joystick button toggles the time set mode.
	In time set mode, press joystick in these directions to
	increment the time setting:
		Hours --> Left
		Minutes--> Up
		Seconds --> Right
	To leave time set mode, press the center button again.
	
	Adapted from SmileyMicros C programming book project. */
	
	
#include "Demonstrator.h"
#include "LCD_functions.h"
#include "LCD_driver.h"
#include "BCD.h"

volatile char KEY = 0;
volatile char KEY_VALID = 0;
volatile char ENABLED = 1; // start with joystick interrupt routine enabled
volatile unsigned char set_enable = 0;

unsigned char gSECOND;
unsigned char gMINUTE;
unsigned char gHOUR;

int main(void)
{
	initializer();

	for(;;)
	{
		showClock();	
		sleep_mode();
	}
	return 0;
}

void initializer()
{
	
	// Initialize the RTC
	RTC_init();
		
	LCD_Init();

	LCD_Colon(1);
	
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
	
	// Enable "Power Save" mode in SMCR register
	SMCR |= (0<<SM2) | (1<<SM1) | (1<<SM0);
	
	// Disable ADC
	ACSR |= (1<<ACD);
	
	// Set Power Reduction Register
	// Disable SPI, USART,and ADC all the time
	PRR |= (1<<PRSPI) | (1<<PRUSART0) | (1<<PRADC);
}

void setSecond(void)
{
	if (gSECOND < 59)
		gSECOND++;
	else gSECOND = 0;
}

void setMinute(void)
{
	if (gMINUTE < 59)
		gMINUTE++;
	else gMINUTE = 0;
}

void setHour(void)
{
	if (gHOUR < 12)
		gHOUR++;
	else gHOUR = 1;
}

void showClock(void)
{	
	// Convert H,M,S to BCD, store in a variable for each digit
	
	uint8_t HH, HL, MH, ML, SH, SL;

    HH = CHAR2BCD2(gHOUR);   
        
    HL = (HH & 0x0F) + '0';
    HH = (HH >> 4) + '0';
	
	//Get rid of leading zero
	if (HH == '0')
		HH = ' '; 

    MH = CHAR2BCD2(gMINUTE);
    ML = (MH & 0x0F) + '0';
    MH = (MH >> 4) + '0';

    SH = CHAR2BCD2(gSECOND);
    SL = (SH & 0x0F) + '0';
    SH = (SH >> 4) + '0';
	
	//Display each digit on the LCD
	LCD_putc(0,HH);
	LCD_putc(1,HL);
	LCD_putc(2,MH);
	LCD_putc(3,ML);
	LCD_putc(4,SH);
	LCD_putc(5,SL);
	LCD_UpdateRequired(1,0);
 }

void RTC_init(void)
{	
	// initialize Timer/counter2 as asynchronous using the 32.768kHz watch crystal.
    // wait for external crystal to stabilise
	for(int i = 0; i < 10; i++)
			_delay_loop_2(30000);   
			
    cli();					// disabel global interrupt

    cbi(TIMSK2, TOIE2);		// disable OCIE2A and TOIE2

    ASSR = (1<<AS2);		// select asynchronous operation of Timer2

    TCNT2 = 0;				// clear TCNT2A
	
	// select precaler: 32.768 kHz / 128 = 1 sec between each overflow
    TCCR2A |= (1<<CS22) | (1<<CS20);
	
	// wait for TCN2UB and TCR2UB to be cleared
    while((ASSR & 0x01) | (ASSR & 0x04));	

    TIFR2 = 0xFF;			// clear interrupt-flags
    sbi(TIMSK2, TOIE2);		// enable Timer2 overflow interrupt

    sei();					// enable global interrupt

    // initial time and date setting
    gSECOND  = 0;
    gMINUTE  = 0;
    gHOUR    = 1;
}

void PinChangeInterrupt(void)
{
    char buttons;
    char key;

    buttons = (~PINB) & PINB_MASK;
    buttons |= (~PINE) & PINE_MASK;

    // Output virtual keys
    if (buttons & (1<<BUTTON_A))
        key = KEY_UP;
    else if (buttons & (1<<BUTTON_B))
        key = KEY_DOWN;
    else if (buttons & (1<<BUTTON_C))
        key = KEY_LEFT;
    else if (buttons & (1<<BUTTON_D))
        key = KEY_RIGHT;
    else if (buttons & (1<<BUTTON_O))
        key = KEY_PUSH;
    else
        key = KEY_INVALID;
  
    if(key != KEY_INVALID)
    {
        if (!KEY_VALID)
        {
            KEY = key;          // Store key in global key buffer
            KEY_VALID = TRUE;
        }
    }
    
    EIFR = (1<<PCIF1) | (1<<PCIF0);     // Delete pin change interrupt flags 
	
	
	if(ENABLED)
	{
		getkey();
	}
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
        k = KEY_INVALID; // No key stroke available

    sei(); // enable interrupts

	if(k != KEY_INVALID)
	{		
		switch(k)
		{
			case KEY_UP:
				if (set_enable) 	
					setMinute();
				break;
			case KEY_DOWN:
				
				break;
			case KEY_LEFT:
				if (set_enable)
					setHour();
				break;
			case KEY_RIGHT:
				if (set_enable)
					setSecond();
				break;
			case KEY_PUSH:
				if (set_enable == 0)
					set_enable = 1;
				else if (set_enable == 1)
					set_enable= 0;
				break;
			default:
				
				break;
		}
	}	

    return k;
}

ISR(SIG_PIN_CHANGE0)
{
    PinChangeInterrupt();
}

ISR(SIG_PIN_CHANGE1)
{
    PinChangeInterrupt();    
}


ISR(SIG_OVERFLOW2)
{    // one second interrupt from 32kHz watch crystal

    gSECOND++;			// increment second

    if (gSECOND == 60)
    {
        gSECOND = 0;
        gMINUTE++;		// increment minute
        
        if (gMINUTE > 59)
        {
            gMINUTE = 0;
            gHOUR++;	// increment hour
            
            if (gHOUR > 12)
            {             
                gHOUR = 1;
			}
        }
    }
}

