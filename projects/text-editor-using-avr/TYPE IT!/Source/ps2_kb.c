/* 				
/			PS2_KB.C
/
/		 PS/2 Keyboard driver
/
*/


#include "ps2_kb.h"
#include "scancodes.h"


static void put_kbbuff(unsigned char c);


static uint8_t kb_buffer[BUFF_SIZE];
volatile static uint8_t buffcnt;
static uint8_t *inpt, *outpt;


void KbInit(void)
{
	buffcnt = 0;
	PS2_Init();
	inpt =  kb_buffer;                        // Initialize buffer
    outpt = kb_buffer;
    buffcnt = 0;
}

void Decode(unsigned char sc)
{
    static unsigned char is_up = 0, shift = 0, ext=0;
    unsigned char i;

    if (!is_up)                // previous data received was the up-key identifier
    {
        switch (sc)
        {
          case 0xF0 :        // The up-key identifier
            is_up = 1;
            break;

		  case 0xE0:		//do a lookup of extended keys
			ext = 1;
			break;
			
          case 0x12 :        // Left SHIFT
            shift = 1;
            break;

          case 0x59 :        // Right SHIFT
            shift = 1;
            break;
          
          default:
			
			if(ext) { //extended key lookup
			
				for(i = 0; (pgm_read_byte(&extended[i][0])!=sc) && pgm_read_byte(&extended[i][0]); i++) 
					;
				if (pgm_read_byte(&extended[i][0]) == sc) 
					//ext_char = pgm_read_byte(&extended[i][1]);
					put_kbbuff(pgm_read_byte(&extended[i][1]));
			}
			else {
			
				if(!shift)           // If shift not pressed, do a table look-up
				{                         
						for(i = 0; (pgm_read_byte(&unshifted[i][0])!=sc) && pgm_read_byte(&unshifted[i][0]); i++)
							;
						if (pgm_read_byte(&unshifted[i][0])== sc) 
							put_kbbuff(pgm_read_byte(&unshifted[i][1]));
				
				} 
				else {               // If shift pressed

						for(i = 0; (pgm_read_byte(&shifted[i][0])!=sc) && pgm_read_byte(&shifted[i][0]); i++) 
							;
						if (pgm_read_byte(&shifted[i][0])== sc)
							put_kbbuff(pgm_read_byte(&shifted[i][1]));
							
				}
				
				
			}
            
        } //switch(sc)
		
    } 
	
	else {			// is_up = 1
        
		is_up = 0;  // Two 0xF0 in a row not allowed
		ext=0;                          
        switch (sc)
        {
          case 0x12 :                        // Left SHIFT
            shift = 0;
            break;
            
          case 0x59 :                        // Right SHIFT
            shift = 0;
            break;
          
        } 
    }

} 

static void put_kbbuff(unsigned char c)
{
    if (buffcnt<BUFF_SIZE)                        // If buffer not full
    {
        *inpt = c;                                // Put character into buffer
        inpt++;                                    // Increment pointer

        buffcnt++;

        if (inpt >= (kb_buffer + BUFF_SIZE))        // Pointer wrapping
            inpt = kb_buffer;
    }
}


uint8_t KbGetChar(void)
{
    uint8_t byte;
    while(buffcnt == 0);                        // Wait for data

    byte = *outpt;                                // Get byte
    outpt++;                                    // Increment pointer

    if ( outpt >= (kb_buffer + BUFF_SIZE) )            // Pointer wrapping
        outpt = kb_buffer;
    
    buffcnt--;                                    // Decrement buffer count

    return byte;
	
}