#include <avr/io.h>	/* Device specific declarations */
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ff.h"		/* Declarations of FatFs API */
#include <stdio.h>


FATFS FatFs;		/* FatFs work area needed for each volume */
FIL Fil;			/* File object needed for each open file */

ISR(TIMER0_COMPA_vect)
{
	disk_timerproc();	/* Drive timer procedure of low level disk I/O module */
}

typedef union usbDWord 
{
    unsigned long	dword;
    unsigned char	bytes[4];
} usbDWord_t;

static int uart_putchar(char c, FILE *stream)
{
     if (c == '\n')
        uart_putchar('\r', stream);
      loop_until_bit_is_set(UCSR0A, UDRE0);
      UDR0 = c;
      return 0;
}

void init_printf(uint16_t baudrate)
{

	// UART INIT // -----------------start
	usbDWord_t br;

	UCSR0B = 0x00; //disable while setting baud rate
	UCSR0A |= (1<<U2X0);
	//UCSR0C = (1 << URSEL) | 0x06; // Atmega16
	UCSR0C = 0x06;	// Atmega328p

	br.dword = F_CPU / (8L * baudrate) - 1;
	
	UBRR0L  = br.bytes[0];
	UBRR0H  = br.bytes[1];
	UCSR0B = 0x18;
	// UART INIT // -----------------end

	static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

	stdout = &mystdout;
}

int main (void)
{
	FRESULT fr;
	DIR dj;     
	FILINFO fno;

	/* Start 100Hz system timer with TC0 */
	OCR0A = F_CPU / 1024 / 100;
	// CTC mode, OCRA top
	TCCR0A = _BV(WGM01); // User
	TCCR0B = 0b101; // CLKio / 1024           
	TIMSK0 = _BV(OCIE0A); // User
	sei();
	//DDRB |=_BV(5); // LED output (same as SCK wrong)
	init_printf(9600);// 9600 Baudrate
	_delay_ms(5000); 
	//PORTB |= _BV(5);// Turn ON LED (same as SCK wrong)
	printf("FatFS02 v1.1\r\n");

	fr=f_mount(&FatFs, "0:", 1);		/* Give a work area to the default drive */
	if (fr == FR_OK)
		printf("Mount successful\r\n");
	else
		printf("Mount failed (%d)\r\n",fr);	
	
	printf("\r\nListing files\r\n\t.... \r\n");
    fr = f_findfirst(&dj, &fno, "", "*"); /* Start to search for all files */

    while (fr == FR_OK && fno.fname[0]) {         /* Repeat while an item is found */
	if (fno.fattrib & AM_DIR )	
		printf("%s\/\n", fno.fname);	// Print directory name
	else
	        printf("%s\n", fno.fname);                /* Print the file name */
        fr = f_findnext(&dj, &fno);               /* Search for next item */
    }
	printf("\t....\r\nListing files done.\r\n");

    f_closedir(&dj);



	f_unmount("0:");


	for (;;) ;
}


