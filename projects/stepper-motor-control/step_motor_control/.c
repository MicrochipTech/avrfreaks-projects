/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Stdio demo
 *
 * $Id: stdiodemo.c,v 1.1 2005/12/28 21:38:59 joerg_wunsch Exp $
 */

#include "defines.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/eeprom.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <util/delay.h>
#define outp(val, port)   (port) = (val)

//#include "lcd.h"
#include "uart.h"

/*
 * Do all the startup-time peripheral initializations.
 */
static void
ioinit(void)
{
  uart_init();
//  lcd_init();
}

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
//FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);

/*
static void
delay_1s(void)
{
  uint8_t i;

  for (i = 0; i < 100; i++)
    _delay_ms(10);
}
*/

void message( )
{
  long unsigned int unit;
  long unsigned int bc;
  char dir;
  long unsigned int speed, pot;

  unit = eeprom_read_dword(0);
  bc = eeprom_read_dword(10);
  dir = eeprom_read_byte(100);
  speed = eeprom_read_dword(110);
  pot = eeprom_read_dword(120);
  fprintf(stderr, "----------------------------------------\n");
  fprintf(stderr, "Stored parameters::\n");
  fprintf(stderr, "   Power ups:   ");
  printf("%lu\n", pot);
  fprintf(stderr, "   Unit steps:  ");
  printf("%lu\n", unit);
  fprintf(stderr, "   Batch count: ");
  printf("%lu\n", bc);
  fprintf(stderr, "   Direction:   ");
    if (dir == 1)
	printf("Left\n");
    if (dir == 2)
	printf("Right\n");
  fprintf(stderr, "   Speed:       ");
  printf("%lu\n", speed);
  fprintf(stderr, "----------------------------------------\n");
  fprintf(stderr, "Keys:\n");
  fprintf(stderr, "   <		Left turn one unit\n");
  fprintf(stderr, "   >		Right turn one unit\n");
  fprintf(stderr, "   S		START batch\n");
//  fprintf(stderr, " SPC		STOP\n");
  fprintf(stderr, "   Z		Define batch count\n");
  fprintf(stderr, "   D		Define unit steps\n");
  fprintf(stderr, "   A		Define speed\n");
  fprintf(stderr, "----------------------------------------\n");
}


int main(void)
{
  char buf[20];

  long unsigned int unit;
  long unsigned int bc;
  char dir;
  long unsigned int speed;
  long unsigned int pot;
  long unsigned int i;

  unit = eeprom_read_dword(0);
  bc = eeprom_read_dword(10);
  dir = eeprom_read_byte(100);
  speed = eeprom_read_dword(110);
  pot = eeprom_read_dword(120);
  eeprom_write_dword(120, ++pot);

  outp(0xff,DDRB);            /* use all pins on PortB for output */
  outp(9, PORTB);      /* invert the output since a zero means: LED on */
  char phase[4] = {9,3,6,12}; // por dve fazi ednovremenno

  int run=0;

  ioinit();

  stdout = stdin = stderr = &uart_str;
//  stderr = &lcd_str;

  fprintf(stderr, "\nWelcome to Smooker Stepper Control v1.00\n");
  message();
  for (;;)
    {
      printf_P(PSTR("Enter command: "));
      if (fgets(buf, sizeof buf - 1, stdin) == NULL)
	break;
//      if (tolower(buf[0]) == 'q')
//	break;

      switch (tolower(buf[0]))
	{
	default:
	  printf("Unknown command: %s\n", buf);
	break;

	case 'r':
	    printf("Refresh command\n");
	    message();
	break;

	case '>':
	    printf("One unit right\n");
	    eeprom_write_byte(100,2);
	    dir=2;
		    for (unit;unit>0;unit--)
		    {
			if (dir == 1 && ++run > 3) run=0;
			if (dir == 2 && --run < 0) run=3;
//			printf("   U:%lu\t%i\t%i\n",unit,run,phase[run]);
		        outp(phase[run], PORTB);      /* invert the output since a zero means: LED on */
    			for (i=0; i<speed; i++)     /* outer delay loop */
    			_delay_ms(1);
		    }
	    unit = eeprom_read_dword(0);
	break;

	case '<':
	    printf("One unit left\n");
	    eeprom_write_byte(100,1);
	    dir=1;
		    for (unit;unit>0;unit--)
		    {
			if (dir == 1 && ++run > 3) run=0;
			if (dir == 2 && --run < 0) run=3;
//			printf("   U:%lu\t%i\t%i\n",unit,run,phase[run]);
		        outp(phase[run], PORTB);      /* invert the output since a zero means: LED on */
    			for (i=0; i<speed; i++)     /* outer delay loop */
    			_delay_ms(1);
		    }
	     unit = eeprom_read_dword(0);
	break;

	case 's':
	    printf("Start of batch\n");
	    for (bc;bc>0;bc--)
	    {
		unit = eeprom_read_dword(0);
		printf("B:%lu\n",bc);
		    for (unit;unit>0;unit--)
		    {
			if (dir == 1 && ++run > 3) run=0;
			if (dir == 2 && --run < 0) run=3;
			//printf("   U:%lu\t%i\t%i\n",unit);
		        outp(phase[run], PORTB);      /* invert the output since a zero means: LED on */
    			for (i=0; i<speed; i++)     /* outer delay loop */
    			_delay_ms(1);
		    }
	    }
	    unit = eeprom_read_dword(0);
	    bc = eeprom_read_dword(10);
	    dir = eeprom_read_byte(100);
	    speed = eeprom_read_dword(110);
	break;

	case 'd':
	  if (sscanf(buf, "%*s %lu", &unit) > 0)
	    {
//	      fprintf(&lcd_str, "Got %s\n", s);
	      fprintf(&uart_str, "Unit steps=%lu\n", unit);
		eeprom_write_dword(0, unit);
	    }
	  else
	    {
	      printf("Command error! Syntax: D 123456\n");
	    }
	  break;

	case 'a':
	  if (sscanf(buf, "%*s %lu", &speed) > 0)
	    {
//	      fprintf(&lcd_str, "Got %s\n", s);
	      fprintf(&uart_str, "Speed=%lu\n", speed);
		eeprom_write_dword(110, speed);
	    }
	  else
	    {
	      printf("Command error! Syntax: A 123456\n");
	    }
	  break;


	case 'z':
	  if (sscanf(buf, "%*s %lu", &bc) > 0)
	    {
	      fprintf(&uart_str, "Batch count=%lu\n", bc);
	      eeprom_write_dword(10, bc);
	    }
	  else
	    {
	      printf("Command error! Syntax: Z 123456\n");
	    }
	  break;
	}
    }
return 0;
}
