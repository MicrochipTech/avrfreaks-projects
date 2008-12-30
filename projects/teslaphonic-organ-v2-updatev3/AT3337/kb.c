/*  kb.c

Atmel AVR Design Contest 2006 Registration Number AT3337

*/
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "scancodes.h"
#include "ATmega16MIDI.h"

#define BUFF_SIZE 64

unsigned char edge, bitcount;                // 0 = neg.  1 = pos.

unsigned char kb_buffer[BUFF_SIZE];
unsigned char *inpt, *outpt;
unsigned char buffcnt;


void init_kb(void)
{
    inpt =  kb_buffer;                        // Initialize buffer
    outpt = kb_buffer;
    buffcnt = 0;

    MCUCR = 2;                                // INT0 interrupt on falling edge
    edge = 0;                                // 0 = falling edge  1 = rising edge
    bitcount = 11;
}

void print_hexbyte(unsigned char i)
{
    unsigned char h, l;
    
    h = i & 0xF0;               // High nibble
    h = h>>4;
    h = h + '0';
    
    if (h > '9')
        h = h + 7;

    l = (i & 0x0F)+'0';         // Low nibble
    if (l > '9')
        l = l + 7;


    putchar(h);
    putchar(l);
}   

SIGNAL(SIG_INTERRUPT0)
{
    static unsigned char data;                // Holds the received scan code

    if (!edge)                                // Routine entered at falling edge
    {
        if(bitcount < 11 && bitcount > 2)    // Bit 3 to 10 is data. Parity bit,
        {                                    // start and stop bits are ignored.
            data = (data >> 1);
            if(PIND & 8)
                data = data | 0x80;            // Store a '1'
        }

        MCUCR = 3;                            // Set interrupt on rising edge
        edge = 1;
        
    } else {                                // Routine entered at rising edge

        MCUCR = 2;                            // Set interrupt on falling edge
        edge = 0;

        if(--bitcount == 0)                    // All bits received
        {
            decode(data);
            bitcount = 11;
        }
    }
}


void decode(unsigned char sc)
{
    static unsigned char is_up=0, shift = 0, mode = 0;
    unsigned char i;

    if (!is_up)                // Last data received was the up-key identifier
    {
        switch (sc)
        {
          case 0xF0 :        // The up-key identifier
            is_up = 1;
            break;

          case 0x12 :        // Left SHIFT
            shift = 1;
            break;

          case 0x59 :        // Right SHIFT
            shift = 1;
            break;

          case 0x05 :        // F1
            if(mode == 0)
                mode = 1;    // Enter scan code mode
            if(mode == 2)
                mode = 3;    // Leave scan code mode
            break;

          default:
            if(mode == 0 || mode == 3)        // If ASCII mode
            {
                if(!shift)                    // If shift not pressed,

                {                            // do a table look-up
                    for(i = 0; pgm_read_byte(&unshifted[i][0])!=sc && pgm_read_byte(&unshifted[i][0]); i++);
                    if (pgm_read_byte(&unshifted[i][0]) == sc) {
                        put_kbbuff(pgm_read_byte(&unshifted[i][1]));
                    }
                } else {                    // If shift pressed
                    for(i = 0; pgm_read_byte(&shifted[i][0])!=sc && pgm_read_byte(&shifted[i][0]); i++);
                    if (pgm_read_byte(&shifted[i][0]) == sc) {
                        put_kbbuff(pgm_read_byte(&shifted[i][1]));
                    }

                }
            } else{                            // Scan code mode
                print_hexbyte(sc);            // Print scan code
                put_kbbuff(' ');
                put_kbbuff(' ');
            }
            break;
        }
    } else {
        is_up = 0;                            // Two 0xF0 in a row not allowed
        switch (sc)
        {
          case 0x12 :                        // Left SHIFT
            shift = 0;
            break;
            
          case 0x59 :                        // Right SHIFT
            shift = 0;
            break;
          
		  case 0x76 :
		    InitDisplay();
			break;

          case 0x05 :                        // F1
            if(mode == 1)
                mode = 2;
            if(mode == 3)
                mode = 0;
            break;
          case 0x06 :                        // F2
           	InitDisplay();
            break;
        } 
    }    
} 
void put_kbbuff(unsigned char c)
{
    if (buffcnt<BUFF_SIZE)                        // If buffer not full
    {
        *inpt = c;                                // Put character into buffer
        inpt++;                                    // Increment pointer

        buffcnt++;

        if (inpt >= kb_buffer + BUFF_SIZE)        // Pointer wrapping
            inpt = kb_buffer;
    }
}

int getchar(void)
{
    int byte;
    while(buffcnt == 0) 
    {     // Wait for data
    //run_led(30000, 30000);
    }
    byte = *outpt;                                // Get byte
    outpt++;                                    // Increment pointer

    if (outpt >= kb_buffer + BUFF_SIZE)            // Pointer wrapping
        outpt = kb_buffer;
    
    buffcnt--;                                    // Decrement buffer count

    return byte;
}

