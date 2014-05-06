//*****************************************************************************
//Author: 	Jidan Al-Eryani,	jidan@gmx.net
// Date:	3.4.2007 
//Tested with: 
//	+MCU: ATmega128(L) 
//	+Dataflash: AT45DB041B (512K X 8)
//	+Compiler: avr-gcc (GCC) 4.1.1
//
//Based on ATMEL AppNote: AVR350: Xmodem CRC Receive Utility for AVR
//
//******************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>

#define SOH 01
#define EOT 04
#define ACK 06
#define NAK 21
#define CRCCHR 'C'

#define TRUE 0x01
#define FALSE 0x0

#define full 0xff
#define empty 0x00

#define bad 0x00
#define good 0x01
#define dup 0x02
#define end 0x03
#define err 0x04
#define out 0x05

#define EOF 0x1A

// function prototypes
void xmodem_purge(void); //not needed!!
void xmodem_init(void);
unsigned char validate_packet(unsigned char *bufptr, unsigned char *packet_number);
void respond(unsigned char packet);
void recv_wait(void);
void sendc(void);
int calcrc(unsigned char *ptr, int count);
void xmodem_download(unsigned int DF_START_ADR);
void xmodem_upload(unsigned int DF_START_ADR, unsigned long FILE_SIZE);
