// ************************************************************************************************
//
//     			t10_prog.c
// 	    		Programming a Tiny10 with a Mega128
//
// Version      1.0
// Author  		Carsten Kögler, FTZ Leipzig
//				info@easytoweb.de
//
// License      Use and share freely. Just keep author information when file is modified.
//					
// 
//  We want to programm the new ATtiny4/5/9/10, wich use the new TPI (Tiny Programming Interface)
//	- See ATTiny10 datasheet for details
//  - We use bit-banging to interface the Tiny10, although the synchronous UART feature of some AVRs
//    could also be used
// - Firmware is stored in the Flash of the master-AVR (mega128 in this case) at address 0x8000
//
//  How to transfer Tiny10 firmware in the mega128-Flash? We have to manipulate the tiny10 hex file
//  to start at 0x8000.
//  Solution:
//  1. You need Tiny10 firmware as Intel hex file
//  2. Convert to binary file with python Script
//      python hex2bin.py ATtiny10_Test.hex ATtiny10_Test.bin
//     You get it here:     https://launchpad.net/intelhex/
//     You get python here: http://www.python.org/
//  3. Convert binary back to Intel hex with offset
//      python bin2hex.py --offset=32768 ATtiny10_Test.bin ATtiny10_Test2.hex  
// 
//  4. Program hex file in mega128 (Programmer software is already in the mega128).
//     For example with Codevision Programmer:
//         - open "Chip Programmer"
//         - File -> "Load Flash"
//         - Program -> Flash  (NOT "Programm All"!)
//  5. Execute program on Atmega128  Debug output UART0, 115200 Baud
//	6. Enjoy				
//
// ************************************************************************************************

#include <io.h>
#include <delay.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


/* Asserting reset to a Tiny10 may be: 
   - hold /RESET low (does not work, if /Reset-Pin is configured as GPIO with Fuse RSTDISBL
   - apply +12V to /RESET */
#define T10_RESET_ASSERT	(DDRE.2=1, PORTE.2=1)
#define T10_RESET_DEASSERT  (DDRE.2=0, PORTE.2=0)

#define T10_TPICLK_LOW		(PORTE.4=0, DDRE.4=1)
#define T10_TPICLK_HIGH     (PORTE.4=0, DDRE.4=0)

/* must be open collector */
#define T10_TPIDATA_LOW     (PORTE.3=0, DDRE.3=1)
#define T10_TPIDATA_HIGH    (PORTE.3=0, DDRE.3=0)
#define T10_TPIDATA_READ	(PINE.3 != 0)

#define T10_5V_ON			(PORTE.5=0, DDRE.5=1)	// output; low
#define T10_5V_OFF          (PORTE.5=1, DDRE.5=0)   // input

#define T10_CLOCK_DELAY_US	3             			// works also with 1 us

flash unsigned char* p_firmware = 0x8000;			// byte address of firmware location.


// ************************************************************************************************
/// \brief	Init TPI signals
///
// ************************************************************************************************
void tpi_phy_interface_enable(void)
{
	uint8_t i;
	
	T10_5V_ON;
	delay_ms(10);           // 10 Millisekunden
	T10_RESET_ASSERT;
	delay_ms(10);           // 10 Millisekunden
	for (i=0; i<16; i++)
		{
		T10_TPICLK_LOW;
		delay_us(T10_CLOCK_DELAY_US);
		T10_TPICLK_HIGH;
		delay_us(T10_CLOCK_DELAY_US);
		}
}


// ************************************************************************************************
/// \brief	Leave Program mode
///
// ************************************************************************************************
void tpi_phy_interface_disable(void)
{
	T10_RESET_DEASSERT;
	
	/* toggle power */
	T10_5V_OFF;    			// this induces low on all signals!
	delay_ms(10);
	T10_5V_ON;
}


// ************************************************************************************************
/// \brief	Write a character to Tiny10. 
///         This could also be done with UART in synchronous mode.
///
// ************************************************************************************************
uint8_t tpi_phy_write_byte(uint8_t data)
{
uint8_t i;
uint8_t bitmask;
uint8_t parity;

	/* Start Bit */
	T10_TPICLK_LOW;
	T10_TPIDATA_LOW;
	delay_us(T10_CLOCK_DELAY_US);
	T10_TPICLK_HIGH;
	delay_us(T10_CLOCK_DELAY_US);


    /* 8 data bits */
    bitmask = 0x01;
    parity = 0x00;
    for (i=0; i<8; i++)
    	{
		T10_TPICLK_LOW;
        if (data & bitmask) 
        	{
			T10_TPIDATA_HIGH;
			parity ^= 1;
			}
		else
			T10_TPIDATA_LOW;
		delay_us(T10_CLOCK_DELAY_US);
		T10_TPICLK_HIGH;
		bitmask <<= 1;
		delay_us(T10_CLOCK_DELAY_US);
    	}
	/* Parity Bit */
	T10_TPICLK_LOW;
    if (parity)
		T10_TPIDATA_HIGH;
	else
		T10_TPIDATA_LOW;
	delay_us(T10_CLOCK_DELAY_US);
	T10_TPICLK_HIGH;
	delay_us(T10_CLOCK_DELAY_US);

	/* 2 stop bits */
	T10_TPICLK_LOW;
	T10_TPIDATA_HIGH;
	delay_us(T10_CLOCK_DELAY_US);
	T10_TPICLK_HIGH;
	delay_us(T10_CLOCK_DELAY_US);
	if (T10_TPIDATA_READ == 0)
		putsf("Data collision\r");

	T10_TPICLK_LOW;
	T10_TPIDATA_HIGH;
	delay_us(T10_CLOCK_DELAY_US);
	T10_TPICLK_HIGH;
	delay_us(T10_CLOCK_DELAY_US);
	if (T10_TPIDATA_READ == 0)
		putsf("Data collision\r");

	return 0;
}


// ************************************************************************************************
/// \brief  Receive a character from Tiny10
///         This could also be done with UART in synchronous mode.
///
// ************************************************************************************************
uint8_t tpi_phy_read_byte(uint8_t* data)
{
uint8_t i;
uint8_t bitmask;
uint8_t parity;
uint8_t break_event=1;

	/* Guard time IDLE bits. Max: 128+2 */
    for (i=0; i<150; i++)
    	{
		T10_TPICLK_LOW;
		delay_us(T10_CLOCK_DELAY_US);
		T10_TPICLK_HIGH;
		delay_us(T10_CLOCK_DELAY_US);
		if (T10_TPIDATA_READ == 0)   // Start bit found
			break;
    	}
    if (i == 150)
    	{
		putsf("No Start bit found\r");
    	return 1;
    	}

	/* 8 Data Bits */
	*data = 0;
	bitmask = 0x01;
	parity = 0;
    for (i=0; i<8; i++)
    	{
		T10_TPICLK_LOW;
		delay_us(T10_CLOCK_DELAY_US);
		T10_TPICLK_HIGH;
		delay_us(T10_CLOCK_DELAY_US);
		if (T10_TPIDATA_READ)
			{
			*data |= bitmask;
			parity ^= 1;
			break_event = 0;	// no break character
			}
		bitmask	<<= 1;
		}

	/* Parity Bit */
	T10_TPICLK_LOW;
	delay_us(T10_CLOCK_DELAY_US);
	T10_TPICLK_HIGH;
	delay_us(T10_CLOCK_DELAY_US);

	if (T10_TPIDATA_READ != parity)
		putsf("Parity error\r");
	if (T10_TPIDATA_READ != 0)
		break_event = 0;	// no break character
	
	/* 2 Stop bits */
	T10_TPICLK_LOW;
	delay_us(T10_CLOCK_DELAY_US);
	T10_TPICLK_HIGH;
	delay_us(T10_CLOCK_DELAY_US);
	if (T10_TPIDATA_READ == 0)
		putsf("Stop bit 1 error\r");
	else
		break_event = 0;	// no break character
	
	T10_TPICLK_LOW;
	delay_us(T10_CLOCK_DELAY_US);
	T10_TPICLK_HIGH;
	delay_us(T10_CLOCK_DELAY_US);
	if (T10_TPIDATA_READ == 0)
		putsf("Stop bit 2 error\r");
	else
		break_event = 0;	// no break character
		
	if (break_event)
		{
		putsf("BREAK character!\r");
		return 2;
		}
		
	return 0;
}


// ************************************************************************************************
/// \brief	Set TPI Pointer register
///
// ************************************************************************************************
void t10_tpi_store_pointer(uint16_t address)
{
	// SSTPR Serial Store to pointer Register using direct addressing
	tpi_phy_write_byte(0x68);	// write low byte of pointer
	tpi_phy_write_byte((uint8_t)address);
	tpi_phy_write_byte(0x69);	// write high byte of pointer
	tpi_phy_write_byte((uint8_t)(address>>8));
}


// ************************************************************************************************
/// \brief	Write indirect to data memory with pointer increment (SST)
///
// ************************************************************************************************
void t10_tpi_write_with_increment(unsigned char data)
{
	tpi_phy_write_byte(0x64);
	tpi_phy_write_byte(data);
}


// ************************************************************************************************
/// \brief	Read indirect from data memory with pointer increment (SLD)
///
// ************************************************************************************************
uint8_t t10_tpi_read_with_increment(void)
{
	uint8_t data;
	tpi_phy_write_byte(0x24);	// read from data space indirect with post-increment.
	tpi_phy_read_byte(&data);
	return  data;
}


// ************************************************************************************************
/// \brief	Check NVM Busy Bit
///
// ************************************************************************************************
uint8_t t10_read_nvm_busy(void)
{
	uint8_t data;
	
	/* Reading NVM Busy Bit inside NVMCSR */
	// serial in from io space: 0x32 => 6 bit lang b110010
	tpi_phy_write_byte(0x72);
	tpi_phy_read_byte(&data);
	
//	printf("NVM Busy Bit: 0x%02X\r\n", data);
	return (data != 0x00);
}


// ************************************************************************************************
/// \brief	Write a byte into NVMCMD
///
// ************************************************************************************************
void t10_write_NVMCMD(unsigned char cmd)
{
	// direct store to i/o address: 0x33
	tpi_phy_write_byte(0xF3);
	tpi_phy_write_byte(cmd);
}


// ************************************************************************************************
/// \brief	Complete chip erase
///
// ************************************************************************************************
void t10_chip_erase(void)
{
	uint8_t data;
	
	t10_write_NVMCMD(0x10);		// NVMCMD: Chip erase

	// start erase by writing dummy byte to a high byte in code section
	t10_tpi_store_pointer(0x4001);
	t10_tpi_write_with_increment(0x00);

	do
		{
		data = t10_read_nvm_busy();
		if (data)
			printf("NVM busy\r\n");
		else
			printf("NVM not busy\r\n");
		}
	while (data);

	printf("Chip erased\r\n");
}


// ************************************************************************************************
/// \brief		Outputs an part of SRAM to the debug interface as hex values
///
/// @param[in]  p_data 		- SRAM-pointer to data
/// @param[in]  size 		- size of data
// ************************************************************************************************
void debug_display_mem_hex(unsigned char* p_data, unsigned int size)
{
	unsigned char act_column=0;
	unsigned int  byte_pos=1;
	
	while (size)
		{
		if (act_column == 0)
			printf("%3u) ", byte_pos);
		printf("%02x ", *p_data);
		p_data++;
		size--;
		act_column++;
		byte_pos++;
		if (act_column == 16)
			{
			act_column = 0;
			putsf("\r");
			}
		}
	putsf("\r");
}


// ************************************************************************************************
/// \brief	Read some bytes from Tiny10 memory space and sent it to debug output
///
// ************************************************************************************************
void t10_read_xbytes(uint16_t address, uint8_t size)
{
	uint8_t i;
	uint8_t flash_data[256];
	                        
	/* read some bytes from Flash */
	t10_tpi_store_pointer(address);
	for (i=0; i<size; i++)
		flash_data[i] = t10_tpi_read_with_increment();

	printf("%i Bytes Flash at 0x%04X: \r\n", size, address);
	debug_display_mem_hex(flash_data, size);
}


// ************************************************************************************************
/// \brief	Tiny10 Programmer main function
///
// ************************************************************************************************
void t10_programmer(void)
{
	uint8_t data;
	uint16_t i;
	uint8_t flash_data[10];

	tpi_phy_interface_enable();
	putsf("t10_interface_enable() executed\r");


    /* ---------------------- */
	/* Enable NVM Programming */
    /* ---------------------- */
	tpi_phy_write_byte(0xE0); // SKEY

	tpi_phy_write_byte(0xFF);
	tpi_phy_write_byte(0x88);
	tpi_phy_write_byte(0xD8);
	tpi_phy_write_byte(0xCD);
	tpi_phy_write_byte(0x45);
	tpi_phy_write_byte(0xAB);
	tpi_phy_write_byte(0x89);
	tpi_phy_write_byte(0x12);

	do
		{
		tpi_phy_write_byte(0x80);
		tpi_phy_read_byte(&data);
		//printf("Data received: 0x%02X\r\n", data);
		}
	while (data != 0x02);
	printf("NVM Programming enabled\r\n");


    /* ---------------------- */
	/*  Read signature Bytes  */
    /* ---------------------- */
	t10_tpi_store_pointer(0x3fc0);
	for (i=0; i<3; i++)
		{
		tpi_phy_write_byte(0x24);	// read from data space indirect with post-increment.
		tpi_phy_read_byte(&data);
		flash_data[i] = data;
		}
	if (flash_data[0] == 0x1E && 
		flash_data[1] == 0x90 && 
		flash_data[2] == 0x03)
		printf("Signature: ATTiny10\r\n", data);
	else
		printf("Signature: unknown \r\n");


    /* ---------------------- */
    /*      Output Flash      */
    /* ---------------------- */
	t10_read_xbytes(0x4000, 250);


    /* ---------------------- */
    /*      Chip erase        */
    /* ---------------------- */
	t10_chip_erase();
	

    /* ---------------------- */
    /*   Write configuration  */
    /* ---------------------- */
	t10_write_NVMCMD(0x1D);	// command: WORD_WRITE
	t10_tpi_store_pointer(0x3F40);
	t10_tpi_write_with_increment(0xFE);	// Configuration Byte 0 (RSTDISBL  programmed)
	t10_tpi_write_with_increment(0xFF);	// write dummy HIGH BYTE
	while (t10_read_nvm_busy());
    
	putsf("Configuration written\r");
	t10_read_xbytes(0x3F40, 10);



    /* ---------------------- */
    /*   Program 10 Bytes     */
    /* ---------------------- */
	/*
	// Read before
	t10_read_xbytes(0x4000, 10);

	// Programm 10 bytes of Flash
	t10_write_NVMCMD(0x1D);	// command: WORD_WRITE
	t10_tpi_store_pointer(0x4000);
	for (i=0; i<5; i++)
	{
		t10_tpi_write_with_increment(0x00);	// write LOW  BYTE
		t10_tpi_write_with_increment(0x00);	// write HIGH BYTE

		while (t10_read_nvm_busy());
	}
	putsf("10 Bytes written\r");

	// Read again
	t10_read_xbytes(0x4000, 10);*/


    /* ---------------------- */
    /*  Program Firmware      */
    /* ---------------------- */
	// Check if firmware is in our Flash.
	data = 0xFF;
	for (i=0; i<10; i++)
		data &= p_firmware[i];	// 'AND' first 10 bytes of firmware

	// Program
	if (data != 0xFF)
		{
		printf("Firmware ");
		t10_write_NVMCMD(0x1D);	// command: WORD_WRITE
		t10_tpi_store_pointer(0x4000);
	
		// Tiny10 has 1kByte Flash in 64 Pages á 8 Bytes
		// We don't care about pages. Just programming everything word-by-word
		// Program 512 words
		for (i=0; i<512; i++)
			{
			t10_tpi_write_with_increment(*p_firmware);	// LOW BYTE
			p_firmware++;
			t10_tpi_write_with_increment(*p_firmware);	// HIGH BYTE
			p_firmware++;
			while (t10_read_nvm_busy());
			if (i%51 == 0)
				printf("."); 		// 10 Punkte insgesamt
			}
		putsf("finished\r");
		}
	else
		printf("No Firmware available.\r\n");


    /* ---------------------- */
    /*   Disable Interface    */
    /* ---------------------- */
	tpi_phy_interface_disable();
	putsf("t10_interface_disable() executed\r");
}


#define	LED1_ON		 		      		PORTD&=~0x10
#define	LED1_OFF		    	  		PORTD|=0x10
#define	LED1_ACTIVE  					DDRD|=0x10
// ************************************************************************************************
/// \brief	Main routine
///
// ************************************************************************************************
void main(void)
{
	// USART0 initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART0 Receiver: Off
	// USART0 Transmitter: On
	// USART0 Mode: Asynchronous
	// USART0 Baud Rate: 115200 (Double Speed Mode)
	UCSR0A=0x02;
	UCSR0B=0x08;
	UCSR0C=0x06;
	UBRR0H=0x00;
	UBRR0L=0x10;

	putsf("Project: Tiny10-Programmer\r");
	putsf("Version: 1.0\r");
	delay_ms(10);
	
	t10_programmer();  // do the work.
	
	LED1_ACTIVE;
	while(1)
		{
		delay_ms(500);
		LED1_ON;
		delay_ms(500);
		LED1_OFF;
		}
}