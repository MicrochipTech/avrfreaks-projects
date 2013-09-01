/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
    \file bootloader.c
    \brief atmega328p Bootloader
    
    \page BOOTLOAD1 The Mark3 Bootloader

    The Mark3 bootloader module implements a custom FunkenSlip-encoded 
    bootloader, which fits in a 1KB bootloader block.  
    
    FunkenSlip encoding is used by Mark2 to provide a robust, common, and
    standards-based communications protocol for host-to-target interaction.
    
    Why use FunkenSlip encoding?
    
    -It's packet-driven.  As a result, the host utility and the target device
     can work together, producing fast, bullet-proof loads.
    
    -FunkenSlip encoding is similar to many popular programming formats, and 
     as a result it's easy to use as a generic abstraction for intel hex,
     S-records, etc.
    
    -Each packet is terminated with a special end-of-packet character, which
     makes it especially easy to parse and decode.
     
    -Each packet contains framing information (channel ID, message size, and
     CRC16) which makes data easy to validate.
     
    -Only devices supporting the Flash-write channel will respond to requests
     to write the flash.
    
    -It's lightweight - we can fit our whole bootloader in under 1KB of code.
    
    Bootloader implementation:
    
    On bootup, we disable interrupts and configure the serial port in busy-
    read/write mode.  We then wait a few seconds to see whether or not a 
    client utility is going to try and flash the device.  If we receive the 
    proper framing messages, we will enter bootloader mode; at which point 
    we will wait for packets until we recieve an EOF message from the host
    flashing utility.  If no attempt to communicate with the bootloader is
    made in the first few seconds, the bootloader will exit, attempting to 
    jump to the main application.  In the event that there is no main
    application, the device will jump right back to the bootloader.
    
    Once in bootloader mode, we wait on FunkenSlip encoded packets.  These
    can be used to either SEEK to a new address in flash, WRITE a packet of
    data to the current flash location, or commit remaining data and reboot
    to the newly-flashed application on EOF.
    
    In the event of an error, an error is returned to the host application
    via the UART, and corrective action can be attempted.  However, in most
    cases, it's probably easiest to reboot the device and attempt to 
    re-flash.  
    
    In this implementation, the device always enters the bootloader - there
    is no special bit that must be held before the bootloader will be invoked.
    While this adds delay, the convenience of always being able to access
    the bootloader is certainly a benefit during development.
    
*/

/*-------------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include <avr/wdt.h>

/*-------------------------------------------------------------------------*/
/*!
    generic types/defines used throughout the bootloader
*/
/*-------------------------------------------------------------------------*/
#define K_UCHAR         unsigned char
#define K_USHORT        unsigned short
#define K_ULONG         unsigned long
#define K_BOOL          unsigned char
#define K_TRUE          1
#define K_FALSE         0

/*-------------------------------------------------------------------------*/
/*!
    Serial port register configuration.  This is used to abstract-out the 
    registers from device-to-device.
*/
/*-------------------------------------------------------------------------*/
#define UDR             UDR0
#define UCSRA           UCSR0A
#define UCSRB           UCSR0B
#define UCSRC           UCSR0C
#define RXC             RXC0
#define RXEN            RXEN0
#define TXEN            TXEN0
#define UDRE            UDRE0
#define BAUD_H          UBRR0H
#define BAUD_L          UBRR0L

/*-------------------------------------------------------------------------*/
/*!
    Define the baud rate that the bootloader will operate at
*/
/*-------------------------------------------------------------------------*/
#define BAUD_RATE       ((K_ULONG)57600)

/*-------------------------------------------------------------------------*/
/*!
    Define the frequency that the system is running at.  This MUST match the
    actual running frequency, otherwise the serial port won't run at the 
    correct baud rate.
*/
/*-------------------------------------------------------------------------*/
#define SYSTEM_FREQ     ((K_ULONG)16000000)

/*-------------------------------------------------------------------------*/
/*!
    Buffer definitions - we have two buffers in our bootloader.
    
    -The serial buffer 
         This is where packets transmitted to the device are staged until the
         contents can be verified.  Verification in this case involves correct
         SLIP decoding, channel matching (Funkenslip framing), sub-msg matching
         and 16-bit payload checkusm matching.
         
    -The page buffer
         Once the individual packets have been validated as being correct - the
         data is copied into the page buffer.  Once the page-buffer is full
         (the size varies between parts), the page buffer is committed to 
         FLASH.
*/
#define PAGE_SIZE       (128)				/*!< 64 words on mega328p.*/
#define RX_BUF_SIZE     (64)                /*!< Maximum single funkenslip packet size */

/*-------------------------------------------------------------------------*/
/*!
    Protocol configuration
    
    The FunkenSlip protocol defines 256 channels on which messages can be 
    addressed.  Only devices which implement certain channels will respond
    to messages on those channels without a NACK.  
    
    This section defines the channel ID used by our bootloader, as well as 
    the sub-commands (data-byte 0) used to implement the specific operations
    required by the bootloader.
    
    Speaking of which, we can get away with three simple commands:
    
    -SEEK-  Set the write pointer to a specific address, generally set on 
            startup.  If a partially-written page exists, assume the seek 
            will take us out of the page currently being written.  Write the 
            contents of the partial page to flash, and then set the write 
            pointer to the given address
    
    -WRITE- Write a packet of data to the current write address, committing to 
            flash whenever a page boundary is crossed.
            
    -EOF-   No more data to be transmitted.  Commit partial-pages to flash, and
            boot into the newly-flashed application.
*/
#define PROGRAM_CHANNEL			(127)		/*!< FunkenSlip channel to program on*/
#define PROGRAM_CMD_SEEK		(0)			/*!< Seek to address*/
#define PROGRAM_CMD_WRITE		(1)			/*!< Write current buffer*/
#define PROGRAM_CMD_EOF			(2)			/*!< End of file - stop programming */

/*-------------------------------------------------------------------------*/
static K_ULONG ulPageAddr;                  /*!< Page address pointer*/
static K_UCHAR ucPageIdx;                   /*!< Index in the data page*/
static K_UCHAR aucPageBuf[PAGE_SIZE];       /*!< Data buffer written to FLASH*/
static K_UCHAR aucSerialBuf[RX_BUF_SIZE];   /*!< Buffer filled with FunkenSlip packets*/

/*-------------------------------------------------------------------------*/
typedef void (*main_func_t)(void);          /*!< Function pointer type used to jump to app*/

/*-------------------------------------------------------------------------*/
/*! 
    Forward declarations
*/

/*-------------------------------------------------------------------------*/
static void     BL_Exit(void); 
static void     Serial_Init(void);
static K_UCHAR  Serial_Read(void);
static void     Serial_Write(K_UCHAR ucByte_);
static K_BOOL   Slip_FillPacket(void);
static K_UCHAR  Slip_DecodePacket(void);
static K_BOOL   Serial_ValidatePacket(K_USHORT *pusLen_);
static void		Flash_WritePage(void);

/*-------------------------------------------------------------------------*/
/*!
    Exit out of the bootloader, jumping to the main application.  Writes
    a message to the UART "Booting App", before performing the reboot.
    
    \fn static void BL_Exit(void)
*/
static void BL_Exit(void)
{
    /* Set a function pointer to the start of the user-app. */
    main_func_t app_start = (void*)0;
	
    /* Write our farewell message to the UART */
	Serial_Write('B');
	Serial_Write('o');
	Serial_Write('o');
	Serial_Write('t');
	Serial_Write('i');
	Serial_Write('n');
	Serial_Write('g');
	Serial_Write(' ');
	Serial_Write('A');
	Serial_Write('p');
	Serial_Write('p');
	Serial_Write('\n');

    /* Reboot! */
    app_start();
}

/*-------------------------------------------------------------------------*/
/*!
    Initialize the serial port to the default baud rate specified in the 
    port configuration.  The bootloader uses a polling/busy-waiting RX and 
    TX mechanism (no ISRs), so only basic RX and TX operations are supported.
    
    \fn static void Serial_Init(void) 
*/
static void Serial_Init(void) 
{
    K_USHORT usBaudTemp = 0;
    
    /* Clear port config*/
    UCSRA = 0;
    UCSRB = 0;
    
    /* Set baud rate*/
    usBaudTemp = (K_USHORT)(((SYSTEM_FREQ/16)/BAUD_RATE) - 1);
    BAUD_H = (K_UCHAR)(usBaudTemp >> 8);
    BAUD_L = (K_UCHAR)(usBaudTemp & 0x00FF);
    
    /* Set 8N1 format on the port */
    UCSRC = 0x06;
    
    /* Enable RX & TX, but no interrupts. */
    UCSRB = (1 << RXEN) | (1 << TXEN);
}

/*-------------------------------------------------------------------------*/
/*!
    Read a byte of data from the serial port, returning it to the user.
    \fn static K_UCHAR Serial_Read(void) 
    \return A single byte of data read from the serial port
*/
static K_UCHAR Serial_Read(void) 
{
    while (!(UCSRA & (1 << RXC0))){};
    return UDR;
}

/*-------------------------------------------------------------------------*/
/*!
    Poll until we recieve the SLIP end-of-packet character (192).  If a 
    specified internal (ad-hoc) timeout occurs before receiving a valid
    framing character, bail.
    \fn static K_BOOL Serial_RxPoll(void) 
    \return K_TRUE if a SLIP end-of-packet character recieved within the 
            predefined tiemout interval, K_FALSE if a timeout occurred.
*/
static K_BOOL Serial_RxPoll(void) 
{
    volatile K_ULONG ulTimeout = 2000000UL;
    
    while (ulTimeout--) 
    {
        if (UCSRA & (1 << RXC0))
        {
            if (UDR == 192)
            {
                return K_TRUE;
            }
        }
    }
        
    return K_FALSE;
}

/*-------------------------------------------------------------------------*/
/*!
    Push a byte of data out of the serial port.  Waits until the port is free
    before attempting to write the character.
    
    \fn static void Serial_Write(K_UCHAR ucByte_)
*/
static void Serial_Write(K_UCHAR ucByte_) 
{
    while (!(UCSRA & (1 << UDRE))){};
    UDR = ucByte_;
}

/*-------------------------------------------------------------------------*/
/*!
    Busy-wait until a full packet of data is received.
    \fn static K_BOOL Slip_FillPacket(void)     
    \return K_TRUE if a packet was successfully read, K_FALSE if the packet
            failed as a result of overflowing the buffer/garbage data on the
            port.
*/
static K_BOOL Slip_FillPacket(void) 
{
    K_UCHAR i = 0;
    K_UCHAR ucData;
    while (i < RX_BUF_SIZE)
    {
        ucData = Serial_Read();
        aucSerialBuf[i] = ucData;
        
        if (ucData == 192)
        {
            return K_TRUE;
        }
        i++;
    }
    return K_FALSE;
}
/*-------------------------------------------------------------------------*/
/*!
    Perform in-place decoding on SLIP data in the serial buffer.
    \fn static K_UCHAR Slip_DecodePacket(void) 
    \return Number of characters read
*/
static K_UCHAR Slip_DecodePacket(void) 
{
    K_UCHAR i = 0;
    K_UCHAR *pucSrc = aucSerialBuf;
    K_UCHAR *pucDst = aucSerialBuf;
    
    /* Perform slip decoding in-place in the serial buffer packet*/
    while ((*pucSrc != 192) && i < RX_BUF_SIZE)
    {
        if (*pucSrc == 219)
        {
            if (*(pucSrc+1) == 220)
            {
                *pucDst = 192;				
            }
            else if (*(pucSrc+1) == 221)
            {
                *pucDst = 219;				
            }
            pucSrc++;
        }
        else
        {
            *pucDst = *pucSrc;	
        }
        i++;
        pucSrc++;
        pucDst++;
    }
    
    *pucDst = 192;
		
    return i;
}

/*-------------------------------------------------------------------------*/
/*!
    Attempt to validate a packet of serial data which has already been 
    SLIP decoded.  This function validates the contents of the payload as 
    being FunkenSlip encoded data - We search for a correct channel, correct
    CRC's, and correct message length before allowing a packet to be valid.
    \fn static K_BOOL Serial_ValidatePacket(K_USHORT *pusLen_)
    \param K_USHORT *pusLen_ Length of packet data returned to the user
    \return K_TRUE if packet is valid, K_FALSE if any of the checks fail
*/
static K_BOOL Serial_ValidatePacket(K_USHORT *pusLen_) 
{    
    K_UCHAR ucChannel;
    K_USHORT usCRC_Calc = 0;
    K_USHORT usCRC_Read = 0;
    K_USHORT usLen;
    K_UCHAR *pucData;
    
    ucChannel = aucSerialBuf[0];
    
    /* Ensure the channel is correct */
	if (ucChannel != PROGRAM_CHANNEL)
    {
	    return K_FALSE;
    }
	
    usCRC_Calc += aucSerialBuf[0];
    
    /* Read the length out */
    usLen = ((K_USHORT)aucSerialBuf[1]) << 8;
    usCRC_Calc += aucSerialBuf[1];
    
    usLen += (K_USHORT)aucSerialBuf[2];
    usCRC_Calc += aucSerialBuf[2];
    
    /* Length returned to the user is -1 because of the sub-command byte,*/
    /* which is part of the actual FunkenSlip data payload.*/
    *pusLen_ = usLen - 1; 
    
    /* Continue reading through the packet to compute the CRC */
	pucData = &aucSerialBuf[3];
    while (usLen--)
    {
        usCRC_Calc += *pucData;
        pucData++;
    }
    
    /* The CRC is stored at the end of the packet */
    usCRC_Read = ((K_USHORT)*pucData) << 8;
    pucData++;
    
    usCRC_Read += ((K_USHORT)*pucData);
    
    /* Make sure the read CRC matches the generated CRC*/
    if (usCRC_Read != usCRC_Calc)
    {
        *pusLen_ = 0;		
	    return K_FALSE;
    }
    
    return K_TRUE;
}

/*-------------------------------------------------------------------------*/
/*!
    Shamelessly lifted from the AVR libc docs.  This uses the functions and 
    macros defined in boot.h in order to safely commit our page buffer to 
    flash.
    \fn static void Flash_WritePage(void)
*/
static void Flash_WritePage(void)
{
    K_USHORT i;
    K_UCHAR *pucData = aucPageBuf;
	
    eeprom_busy_wait();

    boot_page_erase(ulPageAddr);
    boot_spm_busy_wait();      /* Wait until the memory is erased. */

    for (i=0; i<PAGE_SIZE; i+=2)
    {
        /* Set up little-endian word. */
        K_USHORT w = *pucData++;
        w += ((K_USHORT)(*pucData++)) << 8;

        boot_page_fill (ulPageAddr + i, w);
    }

    boot_page_write (ulPageAddr);     /* Store buffer in flash page. */
    boot_spm_busy_wait();       /* Wait until the memory is written. */

    /* Reenable RWW-section again. We need this if we want to jump back */
    /* to the application after bootloading. */
    boot_rww_enable ();
}

/*-------------------------------------------------------------------------*/
/*!
    Copy data in from the serial buffer to the page buffer.  Whenever a page
    buffer is full, commit the page to flash, and start the next.
    \fn static K_BOOL Flash_WriteBuffer(K_USHORT usLen_)
    \return K_TRUE on success, K_FALSE on failure
*/
static K_BOOL Flash_WriteBuffer(K_USHORT usLen_) 
{
    K_UCHAR ucIdx = 4; /* Size of the header... */
    
    /* Write from the serial buffer to the flash staging buffer */
    while (usLen_--)
    {
        aucPageBuf[ucPageIdx] = aucSerialBuf[ucIdx++];
        
        ucPageIdx++;
        if (ucPageIdx == PAGE_SIZE)
        {
            /* Write the page of data to flash...        */
            Flash_WritePage();
			
            /* Update indexes/pages. */
            ucPageIdx = 0;
            ulPageAddr += PAGE_SIZE;			
        }
    }
    
    return K_TRUE;
}

/*-------------------------------------------------------------------------*/
/*!
    Commit a partial-page of data to flash.  This completes the page buffer 
    with 0xFF bytes, before running Flash_WritePage() to seal the deal.
    \fn static void Flash_WritePartialPage(void)
*/
static void Flash_WritePartialPage(void)
{
	if (ucPageIdx != 0)
	{
		while (ucPageIdx < PAGE_SIZE)
		{
			aucPageBuf[ucPageIdx] = 0xFF;
			ucPageIdx++;
		}
		Flash_WritePage();	
	}
}

/*-------------------------------------------------------------------------*/
int main(void) 
{
    /* Ensure interrupts are disabled when running the bootloader... */
    cli();

    /* Clear the watchdog timer... */
	{
		volatile K_UCHAR ucTemp = MCUSR;
		MCUSR = 0;
		WDTCSR |= (1 << WDCE) | (1 << WDE);
		WDTCSR = 0;
        ucTemp = ucTemp;
	}

    /* Start off by initializing the serial port */
    Serial_Init();

    /* Send a banner message, indicating we're in the Mark3 Boot Loader */
	Serial_Write('M');
	Serial_Write('a');
	Serial_Write('r');
	Serial_Write('k');
    Serial_Write('3');
	Serial_Write('B');
	Serial_Write('L');
	Serial_Write('\n');
		
#if 1
    /* Check to see if we're getting our start character to begin using the BL */
    if (!Serial_RxPoll())
    {
        /* Timed out - exit the bootloader */
        BL_Exit();
    }
    
    /* Acknowledge the request to start programming. */
	Serial_Write(69);	
#endif

    /* Main programming loop.  Program until we can't program no more! */
	while (1)
	{
		K_USHORT usLen;
		
        /* Wait until we receive a packet of data. */
		Slip_FillPacket();		
		
        /* Decode the serial buffer */
		if (!Slip_DecodePacket())
		{
			Serial_Write('D');
			continue;
		}
		
        /* Make sure the packet is VALID before trying to operate on it. */
		if (!Serial_ValidatePacket(&usLen))
		{
			Serial_Write('V');
			continue;
		}
		
        /* Figure out what action to take based on the command field... */
		if (aucSerialBuf[3] == PROGRAM_CMD_SEEK)
		{
            /* Seek to new address... */
			K_USHORT usNewAddr = ((K_USHORT)aucSerialBuf[4]) << 8;
			usNewAddr += aucSerialBuf[5];
			Flash_WritePartialPage();
			ulPageAddr = (K_ULONG)usNewAddr;
			ucPageIdx = 0;			
		}	
		else if (aucSerialBuf[3] == PROGRAM_CMD_WRITE)
		{
            /* Write contents of buffer to staging buffer, then flash.*/
			Flash_WriteBuffer(usLen);	
		}
		else if (aucSerialBuf[3] == PROGRAM_CMD_EOF)
		{
            /* End of file.  Commit current page to flash (if non-empty) and boot to */
            /* application, if possible. */
			Flash_WritePartialPage();
			BL_Exit();
		}			
		else
		{
            /* error, invalid command  */
			Serial_Write('I');
			continue;
		}
		
        /* If we get here, the packet was valid. */
		Serial_Write(69);
	}		
    
    /* Return to app on exit. */
    BL_Exit();
}
