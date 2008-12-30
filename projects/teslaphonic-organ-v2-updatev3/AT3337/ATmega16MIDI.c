/*  ATmega16MIDI.c   

Atmel AVR Design Contest 2006 Registration Number AT3337

*/


#define F_CPU 8000000UL  // 8 MHz

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "ATmega16MIDI.h"
#include "OptrexLCD20x4.h"



// Menu text

const char MT_MAIN_P[]                 PROGMEM    = "<M> Main Menu       ";
const char MT_PULSE_P[]                PROGMEM    = "<P> Single Pulse    ";
const char MT_FREQUENCY_MENU_P[]       PROGMEM    = "<F> PWM Menu        ";
const char MT_MIDI_P[]                 PROGMEM    = "<I> MIDI Menu       ";
const char MT_TOT_MENU_P[]             PROGMEM    = "<T> PWM Total Time  ";
const char MT_OFF_MENU_P[]             PROGMEM    = "<N> PWM On Time     ";
const char MT_BLANK_LINE_P[]           PROGMEM    = "                    ";
const char MT_COUNT_P[]                PROGMEM    = "= 7.7us*count + 16us";
const char MT_COUNT2_P[]               PROGMEM    = "Enter 00000 - 65535 ";
const char MT_KEYBOARD_MENU_P[]        PROGMEM    = "<K> Play Keyboard   ";
const char MT_KEYBOARD_START_P[]       PROGMEM    = "<S> To Enable       ";
const char MT_KEYBOARD_STOP_P[]        PROGMEM    = "<s> To Disable      ";
const char MT_KEYBOARD_MENUA_P[]       PROGMEM    = "<1> - <=> To Play   ";
const char MT_KEYBOARD_EXIT_P[]        PROGMEM    = "<SPACE> To Exit     ";
const char MT_MIDI_EXIT_P[]            PROGMEM    = "<RESET> To Exit     ";
const char MT_MIDI_NOTE_P[]            PROGMEM    = "Playing MIDI Note   ";
const char MT_MIDI_MULTI_P[]           PROGMEM    = "Playing PolyPhonics ";

int main (void)
{

    unsigned char key;
    int index;
    initialize();
    InitDisplay();
  	  _delay_loop_2(60000);
	DisplayChar_P(PSTR("TeslaPhonic Organ   "));
  	  _delay_loop_2(60000);
//	DisplayChar_P(PSTR("                    "));
//  	  _delay_loop_2(60000);
//    DisplayChar_P(PSTR("Ver 2 Build 5 August"));
//  	  _delay_loop_2(60000);
	DisplayChar_P(PSTR("Press <M> for Menu  "));
  	  _delay_loop_2(60000);
    LCDInstruction(LCD_CURSOR_OFF);
   	for(index = 1; index < 15; index++)
  	  _delay_loop_2(60000);
    LCDInstruction(LCD_DISPLAY_HOME);

	while(1)
	{
        key=getchar();
		DisplayChar(key);
        switch (key)
        {
          case 'm' :        // 
            {
		    InitDisplay();
			DisplayChar_P(MT_MIDI_P);
			DisplayChar_P(MT_KEYBOARD_MENU_P);
			DisplayChar_P(MT_TOT_MENU_P);
			DisplayChar_P(MT_OFF_MENU_P);
            }
            break;

          case 'M' :        // 
            {
		    InitDisplay();
			DisplayChar_P(MT_MIDI_P);
			DisplayChar_P(MT_KEYBOARD_MENU_P);
			DisplayChar_P(MT_TOT_MENU_P);
			DisplayChar_P(MT_OFF_MENU_P);
            }
            break;
          case 'v' :        // 
            {
    InitDisplay();
  	  _delay_loop_2(60000);
	DisplayChar_P(PSTR("TeslaPhonic Organ   "));
  	  _delay_loop_2(60000);
	DisplayChar_P(PSTR("R. Scott Coppersmith"));
  	  _delay_loop_2(60000);
    DisplayChar_P(PSTR("Ver 2 Build 6 August"));
  	  _delay_loop_2(60000);
	DisplayChar_P(PSTR("Press <M> for Menu  "));
  	  _delay_loop_2(60000);
    LCDInstruction(LCD_CURSOR_OFF);


            }
            break;

          case 'P' :        // 
            {
		    InitDisplay();
        	DisplayChar_P(MT_MIDI_MULTI_P);
			MultiFreq();
            }
            break;

          case 'p' :        // 
            {
		    InitDisplay();
        	DisplayChar_P(MT_PULSE_P);
	        DisplayChar_P(MT_COUNT2_P);
	        DisplayChar_P(MT_COUNT_P);
			SinglePulse();
            }
            break;

          case 'i' :        // 
            {
		    InitDisplay();
		    DisplayChar_P(MT_MIDI_P);
			MIDIStartFast();
            }
            break;

          case 'I' :        // 
            {
		    InitDisplay();
		    DisplayChar_P(MT_MIDI_P);
			MIDIStartFast();
            }
            break;

          case 'n' :        // 
            {
		    InitDisplay();
		    DisplayChar_P(MT_OFF_MENU_P);
	        DisplayChar_P(MT_COUNT2_P);
	        DisplayChar_P(MT_COUNT_P);
			PWM1change();
            }
			break;			

          case 'N' :        // 
            {
		    InitDisplay();
		    DisplayChar_P(MT_OFF_MENU_P);
	        DisplayChar_P(MT_COUNT2_P);
	        DisplayChar_P(MT_COUNT_P);
			PWM1change();
            }
			break;
          case 't' :        // 
            {
		    InitDisplay();
		    DisplayChar_P(MT_TOT_MENU_P);
	        DisplayChar_P(MT_COUNT2_P);
	        DisplayChar_P(MT_COUNT_P);
			PWM2change();
            }
			break;			

          case 'T' :        // 
            {
		    InitDisplay();
		    DisplayChar_P(MT_TOT_MENU_P);
	        DisplayChar_P(MT_COUNT2_P);
	        DisplayChar_P(MT_COUNT_P);
			PWM2change();
            }
			break;
          case 'k' :        // 
            {
		    InitDisplay();
		    DisplayChar_P(MT_KEYBOARD_START_P);
			DisplayChar_P(MT_KEYBOARD_EXIT_P);
			DisplayChar_P(MT_KEYBOARD_MENUA_P);
			DisplayChar_P(MT_KEYBOARD_STOP_P);
			PlayKeyboard();
            }
			break;			

          case 'K' :        // 
            {
		    InitDisplay();
		    DisplayChar_P(MT_KEYBOARD_START_P);
			DisplayChar_P(MT_KEYBOARD_EXIT_P);
			DisplayChar_P(MT_KEYBOARD_MENUA_P);
			DisplayChar_P(MT_KEYBOARD_STOP_P);
			PlayKeyboard();
            }
			break;
         }
				
	}
	
}

void MultiFreq()
{
  int index;
  DisplayChar_P(MT_MIDI_EXIT_P);
  LCDInstruction(LCD_CURSOR_OFF);
  while(1)
    {
      setOCR1A(1000);
//Play C
    setICR1_P(36);
    TCCR1A = (1<<COM1A1)|(1<<WGM11);
    for (index=1;index<10;index++)
	  _delay_loop_2(60000);
	TCCR1A = 0;
    for (index=1;index<10;index++)
	  _delay_loop_2(60000);
//Play E
	setICR1_P(40);
    TCCR1A = (1<<COM1A1)|(1<<WGM11);
    for (index=1;index<10;index++)
	  _delay_loop_2(60000);
	TCCR1A = 0;
    for (index=1;index<10;index++)
	  _delay_loop_2(60000);
//Play G
	setICR1_P(43);
    TCCR1A = (1<<COM1A1)|(1<<WGM11);
    for (index=1;index<10;index++)
  	  _delay_loop_2(60000);
    TCCR1A = 0;
    for (index=1;index<10;index++)
  	  _delay_loop_2(60000);
//Play C
    setICR1_P(36);
    TCCR1A = (1<<COM1A1)|(1<<WGM11);
    for (index=1;index<50;index++)
	  _delay_loop_2(60000);
//Play C & E
	for (index=1;index<20;index++)
	  {
      setICR1_P(36);
      TCCR1A = (1<<COM1A1)|(1<<WGM11);
      _delay_loop_2(60000);
//      _delay_loop_2(60000);
//      _delay_loop_2(60000);
      _delay_loop_2(20000);
      setICR1_P(40);
      TCCR1A = (1<<COM1A1)|(1<<WGM11);
  	  _delay_loop_2(60000);
//      _delay_loop_2(60000);
//      _delay_loop_2(60000);
      _delay_loop_2(20000);
      }
//Play C & E & G
    for (index=1;index<30;index++)
	  {
      setICR1_P(36);
      TCCR1A = (1<<COM1A1)|(1<<WGM11);
      _delay_loop_2(60000);
//      _delay_loop_2(60000);
//      _delay_loop_2(60000);
      _delay_loop_2(20000);
      setICR1_P(40);
      TCCR1A = (1<<COM1A1)|(1<<WGM11);
  	  _delay_loop_2(60000);
//      _delay_loop_2(60000);
//      _delay_loop_2(60000);
      _delay_loop_2(20000);
      setICR1_P(43);
      TCCR1A = (1<<COM1A1)|(1<<WGM11);
  	  _delay_loop_2(60000);
//      _delay_loop_2(60000);
//      _delay_loop_2(60000);
      _delay_loop_2(20000);
  	  }

//Higher Octive
      setOCR1A(50);
//Play C
    setICR1_P(72);
    TCCR1A = (1<<COM1A1)|(1<<WGM11);
    for (index=1;index<10;index++)
	  _delay_loop_2(60000);
	TCCR1A = 0;
    for (index=1;index<10;index++)
	  _delay_loop_2(60000);
//Play E
	setICR1_P(76);
    TCCR1A = (1<<COM1A1)|(1<<WGM11);
    for (index=1;index<10;index++)
	  _delay_loop_2(60000);
	TCCR1A = 0;
    for (index=1;index<10;index++)
	  _delay_loop_2(60000);
//Play G
	setICR1_P(79);
    TCCR1A = (1<<COM1A1)|(1<<WGM11);
    for (index=1;index<10;index++)
  	  _delay_loop_2(60000);
    TCCR1A = 0;
    for (index=1;index<10;index++)
  	  _delay_loop_2(60000);
//Play C
    setICR1_P(72);
    TCCR1A = (1<<COM1A1)|(1<<WGM11);
    for (index=1;index<50;index++)
	  _delay_loop_2(60000);
//Play C & E
	for (index=1;index<50;index++)
	  {
      setICR1_P(72);
      TCCR1A = (1<<COM1A1)|(1<<WGM11);
      _delay_loop_2(20000);
      setICR1_P(76);
      TCCR1A = (1<<COM1A1)|(1<<WGM11);
  	  _delay_loop_2(20000);
      }
//Play C & E & G
    for (index=1;index<100;index++)
	  {
      setICR1_P(72);
      TCCR1A = (1<<COM1A1)|(1<<WGM11);
      _delay_loop_2(20000);
      setICR1_P(76);
      TCCR1A = (1<<COM1A1)|(1<<WGM11);
  	  _delay_loop_2(20000);
      setICR1_P(79);
      TCCR1A = (1<<COM1A1)|(1<<WGM11);
  	  _delay_loop_2(20000);
  	  }

      setOCR1A(100);
//Play C & E & G
    for (index=1;index<100;index++)
	  {
      setICR1_P(36);
      TCCR1A = (1<<COM1A1)|(1<<WGM11);
      _delay_loop_2(20000);
      setICR1_P(76);
      TCCR1A = (1<<COM1A1)|(1<<WGM11);
  	  _delay_loop_2(20000);
      setICR1_P(79);
      TCCR1A = (1<<COM1A1)|(1<<WGM11);
  	  _delay_loop_2(20000);
  	  }


  }
}


void SinglePulse()
{
  int k_index;
  char k_data[6]; 
  unsigned char key;
  long int pwm_data = 0;
  char k_out[6];

for(k_index = 0; k_index < 5; k_index++)
    {
    key=getchar();
    if( (key >= '0') && (key <= '9') )
		{
		 k_data[k_index] = key;	
	     DisplayChar(key);
		}
		else
		 (k_index--);

	}

      InitDisplay();
	  k_data[5] = 0;
      pwm_data = atol(k_data);
	  DisplayChar_P(PSTR("Setting OCR1A OnTime"));
	  DisplayChar_P(MT_BLANK_LINE_P);
	  DisplayChar_P(MT_COUNT_P);
	  ltoa(pwm_data,k_out,10);
      for(k_index = 0; k_index < 5; k_index++)
        {
	    if ((k_out[k_index]>= '0')&&(k_out[k_index]<= '9'))
            DisplayChar(k_out[k_index]);
	    }
      setOCR1A(pwm_data);    //15.5us per count
	  setICR1(pwm_data);
      TCCR1A = (1<<COM1A1)|(1<<WGM11); 

}

void PWM1change()
{
  int k_index;
  char k_data[6]; 
  unsigned char key;
  long int pwm_data = 0;
  char k_out[6];

for(k_index = 0; k_index < 5; k_index++)
    {
    key=getchar();
    if( (key >= '0') && (key <= '9') )
		{
		 k_data[k_index] = key;	
	     DisplayChar(key);
		}
		else
		 (k_index--);

	}

      InitDisplay();
	  k_data[5] = 0;
      pwm_data = atol(k_data);
	  DisplayChar_P(PSTR("Setting OCR1A OnTime"));
	  DisplayChar_P(MT_BLANK_LINE_P);
	  DisplayChar_P(MT_COUNT_P);
	  ltoa(pwm_data,k_out,10);
      for(k_index = 0; k_index < 5; k_index++)
        {
	    if ((k_out[k_index]>= '0')&&(k_out[k_index]<= '9'))
            DisplayChar(k_out[k_index]);
	    }
      setOCR1A(pwm_data);    //15.5us per count
      TCCR1A = (1<<COM1A1)|(1<<WGM11); 
     
}

void PWM2change()
{
  int k_index;
  char k_data[6]; 
  unsigned char key;
  long int pwm_data = 0;
  char k_out[6];

for(k_index = 0; k_index < 5; k_index++)
    {
    key=getchar();
    if( (key >= '0') && (key <= '9') )
		{
		 k_data[k_index] = key;	
	     DisplayChar(key);
		}
		else
		 (k_index--);

	}

      InitDisplay();
	  k_data[5] = 0;
      pwm_data = atol(k_data);
	  DisplayChar_P(PSTR("Setting ICR1 TotTime"));
	  DisplayChar_P(MT_BLANK_LINE_P);
	  DisplayChar_P(MT_COUNT_P);
	  ltoa(pwm_data,k_out,10);
      for(k_index = 0; k_index < 5; k_index++)
        {
	    if ((k_out[k_index]>= '0')&&(k_out[k_index]<= '9'))
            DisplayChar(k_out[k_index]);
	    }
      setICR1(pwm_data);    //15.5us per count
      TCCR1A = (1<<COM1A1)|(1<<WGM11);  
     
}

void PlayKeyboard()
{
  unsigned char pindex;
  int m_index;
  char MIDI_out[3];
  unsigned char sreg;
  int notdone = 1 ;
  unsigned char key;
  sreg = SREG;
  while(notdone)
    {
	key=getchar();
    if(key >= '0') 
		{
        switch (key)
        {
          case 'q' :
		  {
		  InitDisplay();
		  LCDInstruction(LCD_CURSOR_OFF);
		  DisplayChar_P(PSTR("Playing MIDI Note "));
          for (pindex=0;pindex<129;pindex++)
		    {
			MIDI_out[1] = ' ';
            itoa(pindex,MIDI_out,16);
            for(m_index = 0; m_index < 2; m_index++)
            DisplayChar(MIDI_out[m_index]);
			LCDInstruction(LCD_CURSOR_SHIFT_L);
			LCDInstruction(LCD_CURSOR_SHIFT_L);
			setICR1_P(pindex);
        	TCCR1A = (1<<COM1A1)|(1<<WGM11);
			_delay_loop_2(60000);
            _delay_loop_2(60000);
			_delay_loop_2(60000);
			_delay_loop_2(60000);
			_delay_loop_2(60000);
            TCCR1A = 0;
			_delay_loop_2(60000);
			_delay_loop_2(60000);
			_delay_loop_2(60000);
			_delay_loop_2(60000);
			}
          break;
          }
		  case '1' :        // 
            {
		    InitDisplay();
            DisplayChar_P(PSTR("Playing MIDI Note060"));
			DisplayChar_P(PSTR("Note 'C' 261.6 Hz   "));
			setICR1_P(60);
        	TCCR1A = (1<<COM1A1)|(1<<WGM11);
            }
            break;
          case '2' :        // 
            {
		    InitDisplay();
            DisplayChar_P(PSTR("Playing MIDI Note061"));
			DisplayChar_P(PSTR("Note 'C#' 277.2 Hz  "));
			setICR1_P(61);
        	TCCR1A = (1<<COM1A1)|(1<<WGM11);
            }
            break;
          case '3' :        // 
            {
		    InitDisplay();
            DisplayChar_P(PSTR("Playing MIDI Note062"));
			DisplayChar_P(PSTR("Note 'D' 293.7 Hz   "));
			setICR1_P(62);
        	TCCR1A = (1<<COM1A1)|(1<<WGM11);
            }
            break;
          case '4' :        // 
            {
		    InitDisplay();
            DisplayChar_P(PSTR("Playing MIDI Note063"));
			DisplayChar_P(PSTR("Note 'Eb' 311.1 Hz  "));
			setICR1_P(63);
        	TCCR1A = (1<<COM1A1)|(1<<WGM11);
            }
            break;
          case '5' :        // 
            {
		    InitDisplay();
            DisplayChar_P(PSTR("Playing MIDI Note064"));
			DisplayChar_P(PSTR("Note 'E' 329.6 Hz   "));
			setICR1_P(64);
        	TCCR1A = (1<<COM1A1)|(1<<WGM11);
            }
            break;
          case '6' :        // 
            {
		    InitDisplay();
            DisplayChar_P(PSTR("Playing MIDI Note065"));
			DisplayChar_P(PSTR("Note 'F' 349.2 Hz   "));
			setICR1_P(65);
        	TCCR1A = (1<<COM1A1)|(1<<WGM11);
            }
            break;
          case '7' :        // 
            {
		    InitDisplay();
            DisplayChar_P(PSTR("Playing MIDI Note066"));
			DisplayChar_P(PSTR("Note 'F#' 370.0 Hz  "));
			setICR1_P(66);
        	TCCR1A = (1<<COM1A1)|(1<<WGM11);
            }
            break;
          case '8' :        // 
            {
		    InitDisplay();
            DisplayChar_P(PSTR("Playing MIDI Note067"));
			DisplayChar_P(PSTR("Note 'G' 392.0 Hz   "));
			setICR1_P(67);
        	TCCR1A = (1<<COM1A1)|(1<<WGM11);
            }
            break;
          case '9' :        // 
            {
		    InitDisplay();
            DisplayChar_P(PSTR("Playing MIDI Note068"));
			DisplayChar_P(PSTR("Note 'Ab' 415.3 Hz  "));
			setICR1_P(68);
        	TCCR1A = (1<<COM1A1)|(1<<WGM11);
            }
            break;
          case '0' :        // 
            {
		    InitDisplay();
            DisplayChar_P(PSTR("Playing MIDI Note069"));
			DisplayChar_P(PSTR("Note 'A' 440.0 Hz   "));
			setICR1_P(69);
        	TCCR1A = (1<<COM1A1)|(1<<WGM11);
            }
            break;
          case 'p' :        // 
            {
		    InitDisplay();
            DisplayChar_P(PSTR("Playing MIDI Note070"));
			DisplayChar_P(PSTR("Note 'Bb' 466.2 Hz  "));
			setICR1_P(70);
        	TCCR1A = (1<<COM1A1)|(1<<WGM11);
            }
            break;
          case '=' :        // 
            {
		    InitDisplay();
            DisplayChar_P(PSTR("Playing MIDI Note071"));
			DisplayChar_P(PSTR("Note 'B' 493.9 Hz   "));
			setICR1_P(71);
        	TCCR1A = (1<<COM1A1)|(1<<WGM11);
            }
            break;
          case 's' :        // 
            {
			AllNotesOff();
			DisplayChar_P(MT_KEYBOARD_EXIT_P);
			DisplayChar_P(MT_KEYBOARD_MENUA_P);
			DisplayChar_P(MT_KEYBOARD_STOP_P);
			}
            break;
          case 'S' :        // 
            {
		    InitDisplay();
            DisplayChar_P(PSTR("Timer Enabled       "));
            DisplayChar_P(PSTR("Playing MIDI Note069"));
			DisplayChar_P(PSTR("Note 'A' 440.0 Hz   "));
			setICR1_P(69);
        	pwmInit();
			SREG = sreg;
            }
            break;
		  }
		}
		else
		  {
		  notdone = 0;
		  InitDisplay();
		  DisplayChar_P(MT_MAIN_P);
		  }
	}
}


void MIDIStartFast()
{
   unsigned char MIDIData;
   char MIDI_out[3];
   int MIDINoteNumber;
   int MIDINoteVelocity;
   int MIDIState; 
    
   setbaud(BAUD31250);

   pwmInit();
   InitDisplay();
   DisplayChar_P(MT_OFF_MENU_P);
   DisplayChar_P(MT_COUNT2_P);
   DisplayChar_P(MT_COUNT_P);
   PWM1change();
   TCCR1A = 0;
   MIDI_out[2] = 0;
   InitDisplay();
   DisplayChar_P(PSTR("Ready for MIDI Data "));
   DisplayChar_P(MT_BLANK_LINE_P);
   DisplayChar_P(MT_MIDI_NOTE_P);
   DisplayChar_P(MT_MIDI_EXIT_P);
   LCDInstruction(LCD_CURSOR_OFF);
   MIDIState = 0;
   while(1)
   {
     if (UCSRA & (1<<RXC))
	  {

	  MIDIData = UDR;

        switch (MIDIData)
        {
          case 0xfe : 
            break;
          case 0xf8 :
            break;
          case 0x90 :
            {
			MIDIState = 1;
			break;
			}
          case 0x80 :
		    {
			MIDIState = 3;
			break;
		 default :
		    {
			switch (MIDIState)
			  {
              case 1 :
			    {
				MIDINoteNumber = MIDIData;
			    MIDIState = 2;
				break;
	            }
              case 2 :
			    {
				if (MIDIData == 0)
				    TCCR1A = 0;
				else
				  {  
			      setICR1_P(MIDINoteNumber);
				  TCCR1A = (1<<COM1A1)|(1<<WGM11);
				  LCDInstruction(LCD_CURSOR_SHIFT_L);
				  LCDInstruction(LCD_CURSOR_SHIFT_L);
	              itoa(MIDINoteNumber,MIDI_out,16);
                  DisplayChar(MIDI_out[0]);
                  DisplayChar(MIDI_out[1]);
				  //DisplayChar(0x20);
        	      }
  	            MIDINoteVelocity = MIDIData;
			    MIDIState = 1;
				break;
			    }
              case 3 :
			    {
				TCCR1A = 0;
				break; 
				}
				}
              }
			}
         }
      }
   }
}

void AllNotesOff()
{
  InitDisplay();
  DisplayChar_P(PSTR("All Notes Off       "));
  DisplayChar_P(PSTR("                    "));
  DisplayChar_P(PSTR("                    "));
  DisplayChar_P(PSTR("                    "));
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK = 0;
  TIFR = 0;
  sei();

}
void InitDisplay()
{
//-------- Initialize LCD Display ---------------

// Function set
    LCDInstruction(LCD_FUNCTION_SET);
//Clear LCD display
    LCDInstruction(LCD_DISPLAY_CLEAR);
//Turn on display and cursor
    LCDInstruction(LCD_DISPLAY_ON);
//Set RAM address = 0
    LCDInstruction(LCD_HOME);
}


void DisplayChar(char data)
{
//Display Character
    PORTC = data;
    PORTA = LCD_DATA_OUT;
	_delay_loop_2(20000);
	PORTA = LCD_DATA_OUT_NOT;
}

void DisplayChar_P(const char *addr)
{
    char c;
	while((c = pgm_read_byte(addr++)))
		DisplayChar(c);
}

void LCDInstruction(char data)
{
    PORTC = data;
    PORTA = LCD_INSTRUCTION;
	_delay_loop_2(20000);
	PORTA = LCD_INSTRUCTION_NOT;
}

void setbaud(BaudRate br)
   {
   UART_BAUD_REG = br;
   }



void initialize(void)
   {
   cli();


//	DDRB = (1<<DDB0)|(1<<DDB3);
//	DDRD = (1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7);

   DDRA  = 0xFF;
   DDRB  = 0x40;
   SPCR = (1<<SPE);
   DDRC  = 0xFF;
   DDRD  = 0xB2;     
    
   PORTA = 0x00;
   PORTC = 0x00;
   PORTD = 0x00;

   init_kb();
   init_uart();       

   UART_CONTROL_REG = 0x18;   //Transmitter enabled, receiver enabled, no ints
   setbaud(BAUD19K);
   
   // Enable INT0 interrupt
   GICR = 0x40; 
   TCCR1A = 0;
   TCCR1B = 0;
   TIMSK = 0;
   TIFR = 0;
   sei();
   }
