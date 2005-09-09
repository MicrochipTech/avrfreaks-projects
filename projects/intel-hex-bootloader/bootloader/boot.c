/******************************************************************************
 * Filename: Boot.c
 *
 * Author: Preston K. Manwaring
 *
 * Date: Monday, August 22, 2005 13:38:36 -0600
 *
 * License: Free to use. Please mention the contributing authors in the source
 * code.
 *
 * Description: This is a bootloader designed for the AVR processor family
 * which allows new firmware to be uploaded via the UART in Intel Hex format.
 * An interface for uploading such as Hyperterm is envisioned.
 * This code correctly parses the Intel Hex strings and upon reception of
 * a complete page (processor specific) of data, writes the data to flash.
 * Minimal error handling is implemented. Errors during program upload are
 *
 * ERR 1 - File upload synchronization is off (i.e. the ':' was not found)
 * ERR 2 - the checksum failed.
 *
 * Typical Intel hex format is supported in unix mode (LF not LF,CR). So
 * be sure to strip the extra CR from the hex file or have this program
 * strip the CR.
 *
 * The user menu is as follows:
 * U - upload the hex file. A 'READY' will return when the MCU is ready to
 *     receive the file. As the file uploads, a '*' prints for every page
 *     written to flash. When complete, 'OK' is transmitted. If an error
 *     occurs, 'ERR' followed by a number is printed.
 *
 * X - exit this program and start the user application.
 *
 * NOTE: This software is not optimized. It fits as it currently stands
 * in a BLS of 1024. Optimizations could be easily made, but the code is 
 * much easier to  understand with the way it currently stands. I spent
 * a lot of time reading manuals and looking at other's code to figure out
 * all I wanted to know. So I hope this is easily understood. Due to the 
 * lack of optimization, the most common error you will get using this
 * code will be ERR 1 as synchronization fails. This failure will occur
 * because your BAUD rate setting is too high and the program cannot
 * keep up the incoming data. Slowing down the data transfer rate should
 * fix this or you can optimize the code.
 *
 * A special thanks needs to go to Martin Thomas for writing some excelent code
 * which helped me get started. In fact, you will see some files and macros 
 * of his which I use including chipdef.h. His project can be found at
 * http://www.siwawi.arubi.uni-kl.de/avr_projects/
 *
 * If you have any questions or comments, feel free to write.
 * manjagu@byu.edu
 *
 ******************************************************************************/

/*--- INCLUDE FILES ----------------------------------------------------------*/
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include "sercom.h"

/* Select Boot Size (select one, comment out the others) */
// MAYBE: #define _B512 
#define _B1024 
//#define _B2048

/* From Martin Thomas */
#include "chipdef.h"

/*--- DEFINITIONS ------------------------------------------------------------*/
/* Pin "BLPNUM" on port "BLPORT" in this port has to grounded 
   (active low) to start the bootloader */
#define BLPORT  PORTD
#define BLDDR   DDRD
#define BLPIN   PIND
#define BLPNUM  PIND7

/* Bootloader Version */
#define VER_HIGH_BYTE '0'
#define VER_LOW_BYTE  '1'


/*--- GLOBAL VARIABLES -------------------------------------------------------*/
unsigned char page_data[SPM_PAGESIZE]; // create temporary location for program 
				       // data
unsigned long buf_address;

/*--- FUNCTION PROTOTYPES ----------------------------------------------------*/
unsigned char ascii_to_bin( char data );
char ihex_load(void);
void (*jump_to_app)(void) = 0x0000;

/*--- MAIN -------------------------------------------------------------------*/
int main(void)
{
    char temp_char;
    
    cli();
	
    /* the following code moves the interrupt vector pointer to the bootloader
       section. The problem is the compiler doesn't understand where to put
       the IV table. */
    GICR = _BV(IVCE);       
    GICR |= _BV(IVSEL); //move interruptvectors to the Boot sector       

    BLDDR  &= ~_BV(BLPNUM); // set as Input
    BLPORT |= _BV(BLPNUM); // Enable pullup
	
    /* check the programming button */
    if(BLPIN & _BV(BLPNUM))
    {  
	// jump to main app if pin is not grounded
	BLPORT &= ~_BV(BLPNUM);  // set to default
	GICR = _BV(IVCE); 
	GICR &= ~_BV(IVSEL); //move interruptvectors to the Application sector
	jump_to_app(); // Jump to application sector
    }
	
    /* The slow baud rate is required because of the intel hex parsing overhead.
       If I could figure out how to enable interrupts in the BLS (compiler issue)
       then a higher speed could be used by switching to an interrupt based
       UART ISR. The code could also be optimized. */

    SerCom0Init(BAUD_2400); // NOTE, the baud rates are constants defined in
                            // sercom.h. You need to adjust those constants
                            // to fit your MCU speed

    PutString0("Boot Ver. ");
    PutChar0(VER_HIGH_BYTE);
    PutChar0(VER_LOW_BYTE);
    PutString0("\r\n");

    /* this main loop is the user 'menu'. */
    while(1)                             
    {   
	if( Hit0() ) // more necessary for UART code running interrupts
	{
	    temp_char=GetChar0();
	    switch(temp_char)
	    {
		case 'U': // download new program
		{
		    /* erase the main flash excepting BLS */
		    buf_address = 0; 
		    while ( APP_END > buf_address )
		    {
			boot_page_erase(buf_address);	// Perform page erase
			boot_spm_busy_wait();		// Wait until the memory is erased.
			buf_address += SPM_PAGESIZE;
		    }
		    buf_address = 0;

		    /* load new program */
		    PutString0("READY");
		    if( temp_char = ihex_load() )
		    {
			PutString0("ERR ");
			PutInt0(temp_char);
		    }
		    else( PutString0("OK") );
		}
		break;
		
		case 'X':                   //Exit upgrade
		{
		    wdt_enable(WDTO_15MS); // Enable Watchdog Timer to give reset
		}
		break;
		default:
		{
		    PutString0("U or X\r\n");
		}
	    } // end switch
	} // end if( Hit0() )
    }	// end while(1)
    return 0;
}

/*--- char ihex_load( void ) -------------------------------------------------*/
/* This function pulls intel hex formatted code from the serial port and loads 
   it into a temporary location. Once a complete SPM page length is stored,
   it executes a flash memory write. */
char ihex_load(void)
{
    unsigned char
	i,
	byte_count = 0,
	data_pairs,
	data_type,
	temp_byte,
	temp_store,
	address_lo,
	address_hi;

    unsigned int data; // temporary location to store program words
    
    unsigned long temp_address;

    do
    {
	if(GetChar0() != ':') // check to make sure the first character is ':'
	{
	    return(1);
	}
	/* get the count of data pairs */
	data_pairs = ascii_to_bin( GetChar0() ) << 4;
	data_pairs |= ascii_to_bin( GetChar0() );
	
	/* get the address to put the data */
	/* although we collect this data, we do not use it.  All data
	   programmed through this bootloader starts at application program
	   space location 0x0000. The collection is neccessary only for
	   the checksum calculation. */
	address_hi = ascii_to_bin( GetChar0() ) << 4;
	address_hi |= ascii_to_bin( GetChar0() );
	
	address_lo = ascii_to_bin( GetChar0() ) << 4;
	address_lo |= ascii_to_bin( GetChar0() );
	
	/* get the data type */
	data_type = ascii_to_bin( GetChar0() ) << 4;
	data_type |= ascii_to_bin( GetChar0() );
	
	temp_store = data_pairs + address_hi + address_lo + data_type;
	
	for( i = 0; i < data_pairs; i++ )
	{
	    temp_byte = ascii_to_bin( GetChar0() ) << 4;
	    temp_byte |= ascii_to_bin( GetChar0() );
	    page_data[byte_count++] = temp_byte;	
	    temp_store += temp_byte;
	}
	    
	/* get the checksum */
	temp_byte = ascii_to_bin( GetChar0() ) << 4;
	temp_byte |= ascii_to_bin( GetChar0() );

	GetChar0(); // get and dispose of the LF
	//GetChar0(); // strip the CR character

	/* check the data and checksum */
	if( (char)(temp_store + temp_byte) )
	{
	    return(2);
	}

	/* fill the rest of the page buffer with 0xFF if the last records are not 
	   a full page in length */
	if( data_type )
	{
	    for( byte_count; byte_count < SPM_PAGESIZE; byte_count++ )
	    {
		page_data[byte_count] = 0xFF;
	    }
	}

	/* if the page buffer is full, write the buffer to the temp flash buffer */
	if( byte_count >= SPM_PAGESIZE )
	{
	    byte_count = 0;

	    /* store data in temp buffer and write to flash */
	    for( i = 0; i < SPM_PAGESIZE; i += 2 )
	    {
		/* swap the bytes for little endian format */
		data = page_data[ i ];
		data |= ( page_data[ i + 1 ] << 8 );
		boot_page_fill( temp_address, data ); // call asm routine to load temporary flash buffer
		temp_address += 2; // select next word in temporary buffer
	    }

	    /* write to the flash */
	    boot_page_write( temp_address - SPM_PAGESIZE );
	    boot_spm_busy_wait();	
	    boot_rww_enable();				//Re-enable the RWW section

	    PutChar0('*');
	}
    } while( !data_type );
    return(0);
}

/*--- unsigned char ascii_to_bin( char data ) --------------------------------*/
/* This fuction returns the binary value of the received ascii (HEX) character */
unsigned char ascii_to_bin( char data )
{
    if( data < 'A' )
    {
	return( data - '0' );
    }
    else
    {
	return( data - 55 );
    }
}

