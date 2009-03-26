//			MADE BY ROBOKITS INDIA.
//			VISIT US AT WWW.ROBOKITS.ORG
//			DEFAULT PROGRAM SHOWING SOME OF THE CAPABILITIES OF THIS BOAD
//			WRITTEN FOR ROBOGRID

/*-------------------------------------------------------------------          
 Description:
 This example runs lcd in 4bit mode using only 7 I\O pins of AVR.

 Note:
 
 Parameters are as per board and defined in lcd.h file.

 Default values are as below.
 
 #define XTAL 1000000
 
 #define LCD_CONTROLLER_KS0073 0  
 
 #define LCD_PORT         PORTB       port for the LCD lines   
 #define LCD_DATA0_PORT   LCD_PORT      port for 4bit data bit 0 
 #define LCD_DATA1_PORT   LCD_PORT      port for 4bit data bit 1 
 #define LCD_DATA2_PORT   LCD_PORT      port for 4bit data bit 2 
 #define LCD_DATA3_PORT   LCD_PORT      port for 4bit data bit 3 
 #define LCD_DATA0_PIN    4             pin for 4bit data bit 0  
 #define LCD_DATA1_PIN    5             pin for 4bit data bit 1  
 #define LCD_DATA2_PIN    6             pin for 4bit data bit 2  
 #define LCD_DATA3_PIN    7             pin for 4bit data bit 3  
 #define LCD_RS_PORT      LCD_PORT      port for RS line         
 #define LCD_RS_PIN       0             pin  for RS line         
 #define LCD_RW_PORT      LCD_PORT      port for RW line         
 #define LCD_RW_PIN       1             pin  for RW line         
 #define LCD_E_PORT       LCD_PORT      port for Enable line     
 #define LCD_E_PIN        3             pin  for Enable line     
 
 Connect 7 I\O pins as shown before with corresponding lcd pins.
 
  _delay_ms() function parameter passed should not exeed
 262.14ms / F_CPU in mhz.
 
 so for 16MHz the maximum paramer should be 16ms.
 
 For 1MHz it can be upto 262ms (250 is used for this example).
  
 Must be changed to get accurate delay at higher MHz.
 
 The timing will differ if the operating frequency is changed.
 
---------------------------------------------------------------------*/

/*----------------------------------------------------------------
-----------------HEADER FILES-------------------------------------
-----------------------------------------------------------------*/

#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <compat/deprecated.h>		//HEADER FILE FOR FUNCTIONS LIKE SBI AND CBI
#include<util/delay.h>				//HEADER FILE FOR DELAY
#include <setjmp.h>


#include "lcd.h"
#include "uart.h"

#define UART_BAUD_RATE      4800    //Change baud rate here

/*----------------------------------------------------------------
-----------------FUNCTION PROTOTYPES------------------------------
-----------------------------------------------------------------*/

void wait_debounce(void);
void menu(void);
void subprog(void);

/*----------------------------------------------------------------
-----------------CONSTANTS----------------------------------------
-----------------------------------------------------------------*/

#define TIMER_1_CNT     0xfc2e  // 1 sec, use AVRcalc to calculate these values
                                // ((TCNT1H=0xfc, TCNT1L=0x2e)
								// DECIMAL VALUE = 64558
								// MAX VALUE = 65535
								// TIMER OVERFLOW ON 65535-64558 = 977 COUNTS

#define TMC16_CK1024	(_BV(CS12)+_BV(CS10))	// 16 BIT TIMER1 AT 1024 PRESCALAR
												// GIVES 1024*977 = 1000448 CYCLES
												// AT 1MHZ FREQUENCY DELAY OF 1.000448 SECONDS

/*----------------------------------------------------------------
-----------------GLOBAL VARIABLE----------------------------------
-----------------------------------------------------------------*/

static volatile uint8_t led; 					// use volatile when variable is accessed from interrupts
char current;

jmp_buf env;

/*----------------------------------------------------------------
-----------------FUNCTIONS----------------------------------------
-----------------------------------------------------------------*/

void menu(void)
{
	lcd_clrscr();
	if(current==0)
	{
		lcd_puts("BLINK LEDS PORTA\n");
		lcd_puts("1SELEC 2UP 3DOWN\n");			
	}
	else if(current==1)
	{
		lcd_puts("MOTOR SPEED CONT\n");
		lcd_puts("1SELEC 2UP 3DOWN\n");			
	}	
	else if(current==2)
	{
		lcd_puts("UART PUT&GET DEM\n");
		lcd_puts("1SELEC 2UP 3DOWN\n");			
	}		
	else if(current==3)
	{
		lcd_puts("TIMER1 BLINKY   \n");
		lcd_puts("1SELEC 2UP 3DOWN\n");			
	}		
	else if(current==4)
	{
		lcd_puts("DEBOUNCE SW1    \n");
		lcd_puts("1SELEC 2UP 3DOWN\n");			
	}	
	else if(current==5)
	{
		lcd_puts("STEPPER MOTOR   \n");
		lcd_puts("1SELEC 2UP 3DOWN\n");	
	}	
}

void subprog(void)
{
	lcd_clrscr();
	if(current==0)
	{
		lcd_puts("BLINK LEDS PORTA\n");
		lcd_puts("PRESS 4 TO EXIT \n");			
	}
	else if(current==1)
	{
		lcd_puts("MOTOR SPEED CONT\n");
		lcd_puts("PRESS 4 TO EXIT \n");			
	}	
	else if(current==2)
	{
		lcd_puts("UART PUT&GET DEM\n");
		lcd_puts("PRESS 4 TO EXIT \n");			
	}		
	else if(current==3)
	{
		lcd_puts("TIMER1 BLINKY   \n");
		lcd_puts("PRESS 4 TO EXIT \n");	
	}		
	else if(current==4)
	{
		lcd_puts("DEBOUNCE SW1    \n");
		lcd_puts("PRESS 4 TO EXIT \n");	
	}
	else if(current==5)
	{
		lcd_puts("1 FWD   2 BWD   \n");
		lcd_puts("PRESS 4 TO EXIT \n");	
	}		
}

void wait_debounce(void)
{
    unsigned char temp1, temp2;
    unsigned int i,j;
    
    do {
        temp1 = PIND;                  		// read input
        for(i=0;i<65535;i++)
			for(j=0;j<50;j++);
        temp2 = PIND;                  		// read input
        temp1 = (temp1 & temp2);       		// debounce input
    } while ( temp1 & _BV(PIND1) );
}

SIGNAL(SIG_INTERRUPT1)     /* signal handler for external interrupt int0 */
{
	menu();
	longjmp (env, 1);
}

SIGNAL(SIG_OVERFLOW1)            	// signal handler for tcnt1 overflow interrupt
{
	if (bit_is_clear(PINA, 6))		//BLINK LED3 ON TIMER INTERRUPT
		sbi(PORTA,6);				//LED3 ON
	else							//ELSE
		cbi(PORTA,6);				//LED3 OFF	
    TCNT1 = TIMER_1_CNT;         	// reset counter to get this interrupt again
}

/*----------------------------------------------------------------
-----------------MAIN PROGRAM-------------------------------------
-----------------------------------------------------------------*/

int main(void)
{
	unsigned int c;
    char buffer[7];
    int  num=134;
    unsigned char i;	
	
	DDRA=0xF0;						//SET DATA DIRECTION REGISTER
									//SET 1 for OUTPUT PORT
									//SET 0 FOR INPUT PORT
									//PA.4, PA.5, PA.6 AND PA.7 ARE OUTPUT
									//ALL OTHERS ARE INPUT
									
	DDRB=0XFB;						//SET DATA DIRECTION REGISTER
									//SET 1 for OUTPUT PORT
									//SET 0 FOR INPUT PORT
									//PB.2 IS  INPUT
									//ALL OTHERS ARE OUTPUT
	
	DDRD=0XF1;						//SET DATA DIRECTION REGISTER
									//SET 1 for OUTPUT PORT
									//SET 0 FOR INPUT PORT
									//PD.1, PD.2 AND PD.3 ARE INPUT
									//ALL OTHERS ARE OUTPUT
	
	DDRC=0xFF;
		
	sbi(PORTB,2);					//ENABLE PULL UP FOR SWITCH INT2
	sbi(PORTD,1);					//ENABLE PULL UP FOR SW1
	sbi(PORTD,2);					//ENABLE PULL UP FOR SWITCH INT0
	sbi(PORTD,3);					//ENABLE PULL UP FOR SWITCH INT1
    
    lcd_init(LCD_DISP_ON);				/* initialize display, cursor off */
	lcd_clrscr();						/* clear display and home cursor */        
	lcd_puts("ROBOTICS BY\n");		/* put string to display (line 1) with linefeed */		
	lcd_puts("    SUDARSHAN");		/* cursor is now on second line, write second line */		
	lcd_gotoxy(0,1);  					/* move cursor to position 0 on line 2 */        
	lcd_puts("A.");					/* write single char to display */    


	for(i=0;i<15;i++)
		_delay_ms(250);
	
	lcd_clrscr();

	lcd_puts("INT2 FOR OPTION1\n");
	lcd_puts("SW1 FOR OPTION2 ");
	for(i=0;i<15;i++)
		_delay_ms(250);

	lcd_clrscr();

	lcd_puts("INT0 FOR OPTION3\n");
	lcd_puts("INT1 FOR OPTION4 ");
	for(i=0;i<15;i++)
		_delay_ms(250);
   	
	current=0;
	menu();
	
	GICR = _BV(INT1);           	// enable external int0
    MCUCR = _BV(ISC11);          	// falling egde: int0
    
    sei();                       	// enable interrupts 
	
	setjmp (env);

	for(;1;)
	{
		if(bit_is_clear(PINB,2))
		{
			if(current==0)
			{
				subprog();
				while(1)
				{
					PORTA=0X00;						//ALL LED'S OFF
					_delay_ms(250);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
					_delay_ms(250);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
					_delay_ms(250);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
					_delay_ms(250);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
					PORTA=0xF0;						//ALL LED'S ON
					_delay_ms(250);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
					_delay_ms(250);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
					_delay_ms(250);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
					_delay_ms(250);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
				}			
			}
			else if(current==1)
			{
				subprog();
				sbi(PORTC,0);					//MOTOR1 forward
				sbi(PORTC,3);					//MOTOR2 reverse	
		
				TCCR1A =  _BV(WGM10) | _BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0);    	// enable 8 bit PWM, select inverted PWM
					
				// timer1 running on 1/8 MCU clock with clear timer/counter1 on Compare Match
				// PWM frequency will be MCU clock / 8 / 512, e.g. with 1Mhz Crystal 244 Hz.
				TCCR1B = _BV(CS11) | _BV(WGM12);
			 
				for (;;)
				{   
					for (i=0; i<150; i++)								/* Decrease speed of motor */
					{
						OCR1AL = i;
						OCR1BL = i;
						_delay_ms(25);									// delay 25 ms
					}
					
					for( i=150; i>0; i--)								/* Increase speed of motor */
					{
						OCR1AL = i;            
						OCR1BL = i;
						_delay_ms(25);									// delay 25 ms
					}
				}			
			}
			else if(current==2)
			{
				subprog();
				uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU)); 
				uart_puts_P("\r\n\nSample code made by Robokits India for ROBOGRID. ");
				uart_puts_P("\r\n\nVisit Us at www.robokits.org. ");
				uart_puts_P("\r\n\nWriting number to UART: ");
			 
				itoa( num, buffer, 10);   // convert interger into string (decimal format)    
				uart_puts(buffer);        // and transmit string to UART     
			
				uart_putc('\r');				
				uart_puts_P("\n\nPress any key on keyboard: ");
				
				for(;;)
				{
					c = uart_getc();
					if (!(c & UART_NO_DATA))
						uart_putc( (unsigned char)c );
				}			
			}
			else if(current==3)
			{
				subprog();
				sbi(PORTA,4);
				TCCR1A = 0x00;               	// disable PWM and Compare Output Mode
				TCCR1B = TMC16_CK1024;       	// use CLK/1024 prescale value
				TCNT1  = TIMER_1_CNT;        	// reset TCNT1
				
				TIMSK  = _BV(TOIE1);         	// enable TCNT1 overflow 
				
				sei();                       	// enable interrupts 
				
				for (;;) 						// loop forever
				{                      
					if (bit_is_clear(PINA, 5))	//BLINK LED2 WITH NORMAL DELAY
						sbi(PORTA,5);			//LED2 ON
					else						//ELSE
						cbi(PORTA,5);			//LED2 OFF
					_delay_ms(250);				
					_delay_ms(250);	
					_delay_ms(250);	
					_delay_ms(250);	
				}			
			}
			else if(current==4)
			{
				subprog();
				sbi(PORTA,4);
				for (;;)								/* loop forever */ 
				{                           
					sbi(PORTA,5);
					wait_debounce();				/* wait until push button sw1 is pressed */
					cbi(PORTA,5);
					wait_debounce();				/* wait until push button sw1 is pressed */
				}				
			}
			else if(current==5)
			{
				subprog();
				sbi(PORTD,4);								//Enable on
				sbi(PORTD,5);								//Enable on
				while(1)
				{
					mainloop:
					if (bit_is_clear(PINB,2))				//IF SW1 IS PRESSED STEPPER1 FORWARD
					{
						while(1)
						{
							PORTC=0x01;						//COIL1 +
							_delay_ms(5);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
							PORTC=0x02;						//COIL2 +
							_delay_ms(5);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
							PORTC=0x04;						//COIL1 -
							_delay_ms(5);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
							PORTC=0x08;						//COIL2 -
							_delay_ms(5);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
							if (bit_is_set(PINB,2))		//CHECK WHETHER THE SWITCH IS STILL PRESSED
							{
								PORTC=0x00;					//CLEAR PORTC				
								goto mainloop;				//IF NOT GOTO MAINLOOP
							}
						}
					}
					else if (bit_is_clear(PIND,1))			//IF SW2 IS PRESSED STEPPER1 BACKWARD
					{
						while(1)
						{
							PORTC=0x08;						//COIL1 +
							_delay_ms(5);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
							PORTC=0x04;						//COIL2 -
							_delay_ms(5);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
							PORTC=0x02;						//COIL1 -
							_delay_ms(5);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
							PORTC=0x01;						//COIL2 +
							_delay_ms(5);					//DELAY IN MILISECOND ARGUMENT SHOULD NOT BE MORE THAN 255
							if (bit_is_set(PIND,1))
							{
								PORTC=0x00;					//CLEAR PORTC			
								goto mainloop;				//IF NOT GOTO MAINLOOP
							}
						}		
					}
					else
						PORTC=0x00;
				}			
			}	
		}
		else if(bit_is_clear(PIND,1))
		{
			if(current==0)
				current=5;
			else
				current--;
			menu();
			_delay_ms(250);
			_delay_ms(250);
		}
		else if(bit_is_clear(PIND,2))
		{
			if(current==5)
				current=0;
			else
				current++;
			menu();
			_delay_ms(250);
			_delay_ms(250);
		}		
		else
		{
			PORTA=0;	
			cbi(PORTC,0);				
			cbi(PORTC,3);	
			TCCR1A=0;
			TCCR1B=0;
			TCNT1=0;
			OCR1AL=0;
			OCR1BL=0;
			PORTC=0;
		}		
		
	}
}

