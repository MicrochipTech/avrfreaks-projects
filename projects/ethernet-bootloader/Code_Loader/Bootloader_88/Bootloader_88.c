/***********************************************************
 * Project: Ethernet(Wrieless) Survilliance Car
 *
 * Title		: Bootloader Main Control
 * FILE			: Bootloader_88.c
 * Chip type    : ATMEGA88 with ENC28J60 Through SPI
 * Note: ARP & PING has not been Implemented in Bootloader
 ***********************************************************/

#include <avr/io.h>            			// Basic Definitions for AVR
#include <util/delay.h>					// Difinitions for Delay Functions
#include <avr/interrupt.h>				// Definitions for Interrupts
#include "enc28j60.h"					// Definitions for ENC28J60
#include "net.h"						// Definitions for specific parameter addresses
#include "ip_arp_udp.h"					// Conatains defintions for ARP and UDP Functions
#include "timeout.h"					// Definitions related to delay functions
#include "avr_compat.h"					//
#include <string.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

void boot_program_page (uint32_t page, uint8_t *buf);
// ------------- IP & MAC Declarations --------------------------
static uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x24}; 
//static uint8_t myip[4] = {192,168,1,5};
static uint8_t spage[63];
static char reply[3] = { 0x41,0x53,0x00 };
//---------------------------------------------------------------

#define BUFFER_SIZE 150
static uint8_t buf[BUFFER_SIZE+1];

int main(void){

        CLKPR=(1<<CLKPCE);
        CLKPR=0; 			// Changing Clock Speed to 8Mhz
		MCUCR = 0x01;		// Change Interrupt Vector Table to Bootloader	
		MCUCR = 0x02;
		SREG = (1<<7);      // Enable Global Interrupts --> External Interrupt INT0  

 //---------------ENC28J60 RESET-----------------REF TB1.1
        DDRB|= (1<<DDB0);
        PORTB &= ~(1<<PB0);
        delay_ms(20);
        PORTB|= (1<<PB0);
        delay_ms(100);
         
        /* enable PB1, LED as output */
        DDRB|= (1<<DDB1);
//--------------------------------------------------------
        enc28j60Init(mymac);	//---------------REF T1.2
        delay_ms(100);
        
        //init the ethernet/ip layer:
        //init_ip_arp_udp(mymac,myip);

		EIMSK = 0x01;			//-----------Enable INT0
		EICRA = 0x00;

        while(1)
		{ 
		delay_ms(500);
		PORTB+= 0x2;
		}
        return (0);
}


SIGNAL(INT0_vect) 
 {
				uint8_t count=0;
				uint8_t payloadlen;
				uint16_t plen;
				uint8_t page_count;
                plen = enc28j60PacketReceive(BUFFER_SIZE, buf);
                if(plen==0){
                        return;
                }

				                              
                /*if(eth_type_is_ip_and_my_ip(buf,plen)==0)
					{
                        return;
                	}        
                */  
				                  
		 		if (buf[IP_PROTO_P]==IP_PROTO_UDP_V&&buf[UDP_DST_PORT_H_P]==07&&buf[UDP_DST_PORT_L_P]==0xD0)
					{
					payloadlen=buf[UDP_LEN_L_P]-UDP_HEADER_LEN-1;
					page_count = buf[UDP_DATA_P];
					while(payloadlen)
						{
						spage[count] = buf[UDP_DATA_P+count+1];
						count++;
						payloadlen--;
						}
						spage[count] = 0x00;
						if(page_count == 'a')
							{
								reply[0] = 'A';
								make_udp_reply_from_request(buf,reply,strlen(reply),2000);
								//SREG = 0x00;		// Diable Interrupts
								MCUCR = 0x01;		// Change Interrupt Vector Table to App Section
								MCUCR = 0x00;
								asm("sub R0,0x0000");
						    	asm("movw R30,0x0000");   
  						 		asm("ijmp");	    // Jump to 0x0000
							} else {
								boot_program_page(page_count*64,spage);
								reply[0] = page_count + 1;
								make_udp_reply_from_request(buf,reply,strlen(reply),2000);
							}
					return;
					}
 }	
	
void boot_program_page (uint32_t page, uint8_t *buf)
    {
        uint16_t i;
        // Disable interrupts.

        SREG = 0x00;
        cli();
        eeprom_busy_wait ();

        boot_page_erase (page);
        boot_spm_busy_wait ();      // Wait until the memory is erased.

        for (i=0; i<SPM_PAGESIZE; i+=2)
        {
            // Set up little-endian word.

            uint16_t w = *buf++;
            w += (*buf++) << 8;
        
            boot_page_fill (page + i, w);
        }

        boot_page_write (page);     // Store buffer in flash page.
        boot_spm_busy_wait();       // Wait until the memory is written.
        // Reenable RWW-section again. 
        boot_rww_enable ();
        // Re-enable interrupts
        SREG = (1<<7); 	
    }
