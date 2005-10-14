/******** Example Code : Accessing Atmel AT45Dxxx dataflash on STK500 *******

Device      : 	AT90S8515

File name   : 	dataflash.h

Description : 	Defines and prototypes for AT45Dxxx
                 
Last change:    16 Aug 2001   AS
 
****************************************************************************/

#ifndef DATAFLASH_INCLUDED
#define DATAFLASH_INCLUDED

#define FALSE  0
#define TRUE !FALSE

//General macro definitions
#define sbi(port,bit)	(port |=  (1<<bit))
#define cbi(port,bit)	(port &= ~(1<<bit))

#define SetBit(x,y)		(x |= (y))
#define ClrBit(x,y)		(x &=~(y))
#define ChkBit(x,y)		(x  & (y))


//Dataflash opcodes
#define FlashPageRead				0x52	// Main memory page read
#define FlashToBuf1Transfer 		0x53	// Main memory page to buffer 1 transfer
#define Buf1Read					0x54	// Buffer 1 read
#define FlashToBuf2Transfer 		0x55	// Main memory page to buffer 2 transfer
#define Buf2Read					0x56	// Buffer 2 read
#define StatusReg					0x57	// Status register
#define AutoPageReWrBuf1			0x58	// Auto page rewrite through buffer 1
#define AutoPageReWrBuf2			0x59	// Auto page rewrite through buffer 2
#define FlashToBuf1Compare    		0x60	// Main memory page to buffer 1 compare
#define FlashToBuf2Compare	    	0x61	// Main memory page to buffer 2 compare
#define ContArrayRead				0x68	// Continuous Array Read (Note : Only A/B-parts supported)
#define FlashProgBuf1				0x82	// Main memory page program through buffer 1
#define Buf1ToFlashWE   			0x83	// Buffer 1 to main memory page program with built-in erase
#define Buf1Write					0x84	// Buffer 1 write
#define FlashProgBuf2				0x85	// Main memory page program through buffer 2
#define Buf2ToFlashWE   			0x86	// Buffer 2 to main memory page program with built-in erase
#define Buf2Write					0x87	// Buffer 2 write
#define Buf1ToFlash     			0x88	// Buffer 1 to main memory page program without built-in erase
#define Buf2ToFlash		         	0x89	// Buffer 2 to main memory page program without built-in erase

//Dataflash macro definitions
#define DF_CS_active	cbi(PORTB,4)
#define DF_CS_inactive	sbi(PORTB,4)


#ifdef __cplusplus
extern "C" {
#endif

//Function definitions
void DFSPIInit (void);
unsigned char DFReadStatus (void);
void DFPageToBuffer (unsigned int PageAdr, unsigned char BufferNo);
unsigned char BufferReadByte (unsigned char BufferNo, unsigned int IntPageAdr);
void DFBufferReadStr (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr);
void DFBufferWriteEnable (unsigned char BufferNo, unsigned int IntPageAdr);
void DFBufferWriteByte (unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data);
void DFBufferWriteStr (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr);
void DFBufferToPage (unsigned char BufferNo, unsigned int PageAdr);
void DFContFlashReadEnable (unsigned int PageAdr, unsigned int IntPageAdr);


#ifdef __cplusplus
}
#endif


#endif  //DATAFLASH_INCLUDED
