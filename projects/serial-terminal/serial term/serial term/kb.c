//#include <pgmspace.h>
#include "kb.h"
//#include "serial.h"
#include "gpr.h"

#include "scancodes.h"

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

interrupt [EXT_INT0] void ext_int0_isr(void)
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
    static unsigned char is_up=0, shift = 0, mode = 0, scrollup = 0,ctrl =0;
    unsigned char i;

    if (!is_up)                // Last data received was the up-key identifier
    {
        switch (sc)
        {
              
                
          case 0xF0 :        // The up-key identifier
            is_up = 1;
            break;
                   
          case 0x14 :       //ctrl
            ctrl = 1;
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
                if(ctrl == 1)
                {   
                    if(sc == 0x21)
                    putchar(0x03);
                }
                if(!shift )                    // If shift not pressed,
                {                            // do a table look-up
                    for(i = 0; unshifted[i][0]!=sc && unshifted[i][0]; i++);
                    if (unshifted[i][0] == sc) 
                    {
                        put_kbbuff(unshifted[i][1]);
                    }
                } 
                else 
                {   
                    
                                    // If shift pressed
                        for(i = 0; shifted[i][0]!=sc && shifted[i][0]; i++);
                        if (shifted[i][0] == sc) 
                            {
                                put_kbbuff(shifted[i][1]);
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
        is_up = 0;
                                    // Two 0xF0 in a row not allowed
        switch (sc)
        {
        case 0x14:              //left  ctrl
        ctrl = 0;
        break;
        
          case 0x12 :                        // Left SHIFT
            shift = 0;
            break;
            
          case 0x59 :                        // Right SHIFT
            shift = 0;
            break;
          case 0x77:
          back=!back;
          break;
          
          case 0x05 :                        // F1
            if(mode == 1)
                mode = 2;
            if(mode == 3)
                mode = 0;
            break;
          case 0x06 :                        // F2
            put_kbbuff(0xff);
           // printf("scroll up");
           break;
           case 0x04:
            put_kbbuff(0xfe);
            break;
            case 0x03:
            put_kbbuff(0xfd);
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

int getchar_kb(void)
{
    int byte;  
   
    //while(buffcnt == 0);                        // Wait for data
    if(buffcnt == 0)
    {         
    byte=0;
    return byte;
    }
    else
    { 
    byte = *outpt;                                // Get byte
    outpt++;                                    // Increment pointer

    if (outpt >= kb_buffer + BUFF_SIZE)            // Pointer wrapping
        outpt = kb_buffer;
    
    buffcnt--;                                    // Decrement buffer count

    return byte;
    }
}


