/**************************************************************************************
***************************************************************************************
***
***     File: main.c
***     Project: AVRBoot
***     Copyright 2010 Peter Eckstrand
***     Description: Flash bootloader for AVR targets. The bootloader is started at 
***     reset and waits for programming request on the uart. If no programming request
***     has been received within APPL_START_DELAY s, the application already in the 
***     flash will be started. Otherwise a programming session will begin. The new 
***     application is transfered from a PC via the serial port to the AVR uart, page 
***     by page.
***
***     Configurations: 
***         - You must define your clock frequency with the symbol F_CPU.
***         - The time the bootloader will wait for programming command before 
***           starting the application is configurable with the symbol APPL_START_DELAY
***         - You must specify the number of flash pages and their size in const.h
***         - You must build with the linker option: -Wl,--section-start=.text=0x3c00
***         - You must set the BOOTSZ fuse to at least 512 words.
***         - You must set the BOOTRST fuse.
***		Version: 1.0.0
***
***		Change log:
***		2010-03-01: 1.0.0 First release
***
***************************************************************************************
***************************************************************************************
***
***		This file is part of AVRBoot.
***
***		AVRBoot is free software: you can redistribute it and/or modify
***     it under the terms of the GNU General Public License as published by
***     the Free Software Foundation, either version 3 of the License, or
***     (at your option) any later version.
***
***     AVRBoot is distributed in the hope that it will be useful,
***     but WITHOUT ANY WARRANTY; without even the implied warranty of
***     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
***     GNU General Public License for more details.
***
***     You should have received a copy of the GNU General Public License
***     along with AVRBoot.  If not, see <http://www.gnu.org/licenses/>.
***
***
***************************************************************************************
***************************************************************************************
*/



#include <stdint.h>
#include <io.h>
#include <interrupt.h>
#include "const.h"
#include "flash.h"



static void uartSendChar( char c );
static void FlashPageWrite( uint8_t page, uint16_t *data );

static uint16_t flashData[ N_WORDS_IN_PAGE ];
static uint8_t nReceived;
static volatile uint8_t startApplication;


void main( void )  {
    uint8_t rxChar;
    uint8_t page;
    uint8_t *pBuf;
    void (*rst)();
    rst = 0x0000;    
    startApplication = 0;

    /* Timer1: CTC mode, prescaler /256 */
    TCCR1A = 0x00;

    /* Output compare value */
    OCR1A = ( F_CPU/256 );

    /* Reset Counter, Enable Output compare match interrupt, Reset interrupt flag */
    TCNT1 = 0;
    TIMSK = 0x10;
    TIFR = 0;

    /* Setup UART 38400, 8bit, no parity, one stopbit */
    UBRRH = 0;
    UBRRL = (F_CPU/(16*38400)-1);
    UCSRB = (1<<RXEN) | (1<<TXEN);
    
    /* Relocate interrupt vector to boot vector */
    BOOT_VECT();
    
    /* Send start character to PC */
    uartSendChar( 'b' );

    sei();

    /* CTC mode, clk/256 */
    TCCR1B = 0x0c;
    
    /* Wait for PC app to send p (program) command */
    do {
        if ( UCSRA & (1<<RXC) ) {
            rxChar = UDR;
            if ( rxChar == 'p' ) {
                goto START_BOOT;
            }
        }
    } while ( startApplication == 0 );

    /* Time out, no program command was received. Start the application */
    goto START_APPLICATION;
    
START_BOOT:
    /* Disable interupts, stop timer */
    cli();
    TCCR1B = 0x00;
    
    /* Send page size and number of pages to PC */
    uartSendChar( N_BYTES_IN_PAGE );
    uartSendChar( N_PAGES );

    pBuf = (uint8_t*)flashData;

    /* Receive program page by page. */
    do {
        /* First byte should be the page number. Page = 0xff means we are finished */
        while  (!( UCSRA & (1<<RXC) ));
        page = UDR;
        
        if ( page == 0xff ) {
            break;
        }

        asm("clr r1");
        nReceived = 0;
        
        /* Receive the page bytes */
        do  {
            while  (!( UCSRA & (1<<RXC) ));
            pBuf[ nReceived ] = UDR;
            ++nReceived;
        } while ( nReceived < N_BYTES_IN_PAGE );

        /* Write to flash memory */
        FlashPageWrite( page, flashData );

        /* Inform PC app to send next page */
        uartSendChar( 'w' );
    } while (1);
    
    
START_APPLICATION:
    uartSendChar( 'a' );
    cli();

    /* Relocate interrupt vector to normal vector */
    APPL_VECT();

    /* Start application */
    rst();
}


void FlashPageWrite( uint8_t page, uint16_t *data ) {
    register uint8_t zh asm("r3");
    register uint8_t zl asm("r4");
    uint8_t startZl;
    uint8_t nWordsLeftInPage;

    nWordsLeftInPage = N_WORDS_IN_PAGE;

    zh = ( ( page >> 1 ) & 0x3F );
    zl = ( ( page & 0x01 ) << 7 );
    startZl = zl;
        
    while( SPMCR & 0x01 );

    FLASH_ERASE_PAGE();
        
    while ( nWordsLeftInPage > 0 ) {
        FLASH_LOAD_TEMP_BUF( *data );
        zl += 2;
        --nWordsLeftInPage;
        ++data;
    }

    zl = startZl;
    FLASH_WRITE_PAGE();

    FLASH_REENABLE_RWW();

}


ISR(SIG_OUTPUT_COMPARE1A) {
    static uint8_t cnt = 0;
    
    if ( ++cnt == APPL_START_DELAY ) {
        startApplication = 1;
        /* Stop timer */
        TCCR1B = 0x00;
    }
}


void uartSendChar( char c ) {
    while (!( UCSRA & (1<<UDRE) ));
    UDR = c;
}
