/*
,-----------------------------------------------------------------------------------------.
| update/flashloader
|-----------------------------------------------------------------------------------------
| this file implements some kind of bootloader
| - bootloader is not the correct term -> "flashloader"
| - NOT used yet !
|
| - it copies dataflash to avr flash
| - be careful: make sure that dataflash content is valid before calling
|   flashloader_load() !!! (for example do a crc8 check etc)
| - after avr is programmed we will do a clean watchdog triggered reset
|
| Author   : Simon Schulz [avr<AT>auctionant.de]
|-----------------------------------------------------------------------------------------
| License:
| This program is free software; you can redistribute it and/or modify it under
| the terms of the GNU General Public License as published by the Free Software
| Foundation; either version 2 of the License, or (at your option) any later
| version.
| This program is distributed in the hope that it will be useful, but
|
| WITHOUT ANY WARRANTY;
|
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with
| this program; if not, write to the Free Software Foundation, Inc., 51
| Franklin St, Fifth Floor, Boston, MA 02110, USA
|
| http://www.gnu.de/gpl-ger.html
`-----------------------------------------------------------------------------------------*/
#include <avr/io.h>

//-> copy defines from "../io/dataflash.h"
//dataflash pin config
#define DATAFLASH_PIN_CS  0
#define DATAFLASH_PIN_RST 1
#define DATAFLASH_PORT    PORTC
#define DATAFLASH_DDR     DDRC

#define DATAFLASH_BLOCK_ERASE         0x50
#define DATAFLASH_MEM_PAGE_READ       0x52
#define DATAFLASH_READ_BUFFER0        0xD4
#define DATAFLASH_READ_BUFFER1        0xD6
#define DATAFLASH_MEM_TO_BUFFER0      0x53
#define DATAFLASH_MEM_TO_BUFFER1      0x55
#define DATAFLASH_MEM_COMP_BUFFER0    0x60
#define DATAFLASH_MEM_COMP_BUFFER1    0x61
#define DATAFLASH_WRITE_BUFFER0       0x84
#define DATAFLASH_WRITE_BUFFER1       0x87
#define DATAFLASH_BUFFER0_TO_MEM_WE   0x83
#define DATAFLASH_BUFFER1_TO_MEM_WE   0x86
#define DATAFLASH_BUFFER0_TO_MEM_NE   0x88
#define DATAFLASH_BUFFER1_TO_MEM_NE   0x89
#define DATAFLASH_MEM_WR_TH_BUFFER0   0x82
#define DATAFLASH_MEM_WR_TH_BUFFER1   0x85
#define DATAFLASH_AUTO_RW_TH_BUFFER0  0x58
#define DATAFLASH_AUTO_RW_TH_BUFFER1  0x59
#define DATAFLASH_READ_STATUS         0xD7

#define DATAFLASH_READY 							0x80

//mega32 => 32K
//32*1024/256 = 32*4 
//-> "bootloader" code is stored in the last two "pages" (=512byte)
#define DATAFLASH_CODE_MAXPAGE				(8*1024/256-2)
#define PAGESIZE 32
#define PAGESIZEB PAGESIZE*2
/*
r16 = temp for helper apps
r17 = ?
r18 = param hi
r19 = param lo
r20 = counter dataflash pagenum
r21 = byte counter

r24 = page byte counter 
*/

#define ZL r30
#define ZH r31


.section .flashloader,"ax",@progbits
//////////////////////////////////////////////
//void flashloader_load()
.global flashloader_load
.func flashloader_load
flashloader_load:
	
	/// ///////DEBUG//////////
	ret
	
	//disable interrupts !!
	cli
	
	//disable enc28j60:	
	sbi _SFR_IO_ADDR(PORTB), 0


	rcall flashloader_spi_init
	

	//start with dataflash page 0:
	clr r20
	
	//start with flash page 0 -> setup Z-ptr
	ldi ZL, lo8(0x0000)
	ldi ZH, hi8(0x0000)


	ldi r24, PAGESIZE
	
	//loop through pages
flashloader_load_pageloop:

	//load dataflash page to dataflash buffer0:
	mov r18, r20
	rcall flashloader_dataflash_copy_page_to_buffer //r18,r19

	//set loop counter to zero
	clr r21

	flashloader_load_byteloop:	
		//load byte number r21 to r16:
		rcall flashloader_dataflash_read_buffer //r16 = dataflash_byte(r21)
		mov r18, r16
		inc r21
		
		rcall flashloader_dataflash_read_buffer //r16 = dataflash_byte(i)
		mov r19, r16
		inc r21
		
		//now store the byte in flash (also copies buf->flash)
		rcall flashloader_store_word     //flash<-r18,r19
	
		//loop if counter != 0x00 (=256 bytes transferred)
		tst r21
		brne flashloader_load_byteloop

	
	//increase pagecounter
	inc r20

	//if (current_page < maxpage) -> loop
	ldi r16, DATAFLASH_CODE_MAXPAGE
	cp r16, r20
	brsh flashloader_load_pageloop

	//if write last buffer to flash:
/*	ldi r18, 0xAA
	ldi r19, 0xBB
flashloader_load_buffer_fill:
	rcall flashloader_store_word
	tst r24
	brne flashloader_load_buffer_fill*/
	


	//-> flash programming finished
	//->do watchdog reset!

flashloader_load_wdloop:
	rjmp flashloader_load_wdloop
	//we never get here
.endfunc



//////////////////////////////////////////////
//input: r18,r19 = next word
//-> store word to avr flash
.func flashloader_store_word
flashloader_store_word:
	
	//debug output
	//mov r16, r18
	//rcall flashloader_uart_tx
	//mov r16, r19
	//rcall flashloader_uart_tx
	
	//debug: write pagenum
	//mov r18, r20
	//mov r19, r20
	
	//copy word to flash write buffer:
	mov r0, r18
	mov r1, r19
	
	//store r0:r1 in pagebuf
	ldi r18, (1<<SPMEN)
	rcall flashloader_do_spm
	
	//increase Z pointer (+2 byte)
	adiw ZL, 2
	
	//test if flash programming buffer is full:
	dec r24
	brne flashloader_load_not_full	
	
flashloader_load_full:	
	//page buffer full -> erase+program!
	/*ldi r16, 'X'
	rcall flashloader_uart_tx
	rcall flashloader_uart_tx
	rcall flashloader_uart_tx*/
	
	//set pointer to buffer startpos:
	subi ZL, lo8(PAGESIZE<<1)
	sbci ZH, hi8(PAGESIZE<<1)
	
	//erase flash page: 
	ldi r18, (1<<PGERS)|(1<<SPMEN)
	rcall flashloader_do_spm
	
	//now program page to flash:
	//send write cmd
	ldi r18, (1<<PGWRT)|(1<<SPMEN)
	rcall flashloader_do_spm
	
	//reenable RWW section
	ldi r18, (1<<RWWSRE)|(1<<SPMEN)
	rcall flashloader_do_spm
	
	//set pointer to next page:
	ldi r18, (PAGESIZE<<1)
	add ZL, r18
	brcc flashloader_load_no_carrc
	inc ZH
flashloader_load_no_carrc:	
	
	//count again <PAGESIZE> bytes
	ldi r24, PAGESIZE

flashloader_load_not_full:
	ret
.endfunc

/*
//DEBUGGING ONLY ////////////////////////////////
.func flashloader_uart_tx
flashloader_uart_tx:
	sbis _SFR_IO_ADDR(UCSRA),UDRE
	rjmp flashloader_uart_tx

	out _SFR_IO_ADDR(UDR), r16
	ret
.endfunc
*/
	
//////////////////////////////////////////////
.func flashloader_do_spm
flashloader_do_spm:

	//wait for complete any previous SPM cmd:
flashloader_do_spm_wait:
	in r16, _SFR_IO_ADDR(SPMCR)
	sbrc r16, SPMEN
	rjmp flashloader_do_spm_wait

	//INTERRUPTS MUST BE DISABLED !
	cli

	//check that no EE access is done
flashloader_do_spm_wait_ee:
	sbic _SFR_IO_ADDR(EECR), EEWE
	rjmp flashloader_do_spm_wait_ee

	//SPM timed sequence: 
	out _SFR_IO_ADDR(SPMCR), r18
	spm

	//return
	ret
.endfunc



//////////////////////////////////////////////
//input : r21 = byte
//output: none
.func flashloader_dataflash_read_buffer
flashloader_dataflash_read_buffer:
	//wait for flash is no longer busy
	rcall flashloader_dataflash_busywait

	//reselect dataflash:
	cbi _SFR_IO_ADDR(DATAFLASH_PORT), DATAFLASH_PIN_CS

	//send buffer select cmd:
	ldi r16, DATAFLASH_READ_BUFFER0
	rcall flashloader_spi_send

	//15 dont care + 9 address + 8 dont care:
	//send address: 0000 0000 0000 000X bbbb bbbb
	
	//send 0x00
	clr r16
	rcall flashloader_spi_send

	//send 0x00 -> we can only read byte 0...255 !!
	clr r16
	rcall flashloader_spi_send

	//r16 = r21
	mov r16, r21
	rcall flashloader_spi_send //TX

	//send one dummy byte
	clr r16
	rcall flashloader_spi_send

	//now read the data
	clr r16
	rcall flashloader_spi_send
	//r16 = data

	//deselect dataflash:
	sbi _SFR_IO_ADDR(DATAFLASH_PORT), DATAFLASH_PIN_CS
	ret
.endfunc

//////////////////////////////////////////////
//input : r18 = pagenum lo
//output: none
.func flashloader_dataflash_copy_page_to_buffer
flashloader_dataflash_copy_page_to_buffer:
	
	//wait for flash is no longer busy
	rcall flashloader_dataflash_busywait

	//reselect dataflash:
	cbi _SFR_IO_ADDR(DATAFLASH_PORT), DATAFLASH_PIN_CS


	//send store to buf0 cmd:
	ldi r16,DATAFLASH_MEM_TO_BUFFER0
	rcall flashloader_spi_send

/***************** DONT USE THIS
	//send 0000 hhhl llll lll0  (r18,r19)
	mov r16, r18
	//r16 = (r16<<1) & 0xE
	lsl r16
	andi r16, 0x0E
	//r16 = r16 | bit7(r15)
	sbrc r15,7
	ori r16,0x01
	//send first byte:
	rcall flashloader_spi_send
***************************************/

	//-> we can load max64kByte from dataflash (prepared for mega644)
	//-> dataflash page address hi = 0
	//send 0000 000l llll lll0  (r18)
	clr r16
	sbrc r18,7
	ori r16,0x01
	//send first byte
	rcall flashloader_spi_send

	//create second byte:
	mov r16, r18
	lsl r16
	//send second byte:
	rcall flashloader_spi_send

	//send 8 dont care bits:
	clr r16
	rcall flashloader_spi_send

	//deselect dataflash:
	sbi _SFR_IO_ADDR(DATAFLASH_PORT), DATAFLASH_PIN_CS

	ret	
.endfunc

//////////////////////////////////////////////
//input : none
//output: none
.func flashloader_dataflash_busywait
flashloader_dataflash_busywait:
	//select dataflash:
	cbi _SFR_IO_ADDR(DATAFLASH_PORT), DATAFLASH_PIN_CS

	//ldi r16, 0x80 //<-------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!remove
	//ret//<-------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!remove

	ldi r16, DATAFLASH_READ_STATUS
	rcall flashloader_spi_send
	
flashloader_dataflash_busywait_loop:
	//wait for bit7 of answr to be 1:
	rcall flashloader_spi_send
	sbrs r16, 7
	rjmp flashloader_dataflash_busywait_loop

	//deselect dataflash:
	sbi _SFR_IO_ADDR(DATAFLASH_PORT), DATAFLASH_PIN_CS

	//finished
	ret
.endfunc
	


//////////////////////////////////////////////
//input : r16 = data
//output: r16 = result
.func flashloader_spi_send
flashloader_spi_send:
	//load data into tx reg:
	out _SFR_IO_ADDR(SPDR), r16
	
	//wait for tx finished:
flashloader_spi_send_wait:	
	sbis _SFR_IO_ADDR(SPSR),SPIF
	rjmp flashloader_spi_send_wait

	//load tx result:
	IN r16, _SFR_IO_ADDR(SPDR)
	
	//return
	ret
.endfunc

//////////////////////////////////////////////
.func flashloader_spi_init
flashloader_spi_init:
	//Setup spi for dataflash io

	//sck = hi
	ldi r16, 5
	out _SFR_IO_ADDR(PORTB), r16

	//setup dataflash CS+RST signal as output:
	ldi r16, (1<<DATAFLASH_PIN_CS)|(1<<DATAFLASH_PIN_RST)
	out _SFR_IO_ADDR(DATAFLASH_DDR), r16

	//reset dataflash & set CS=hi
	cbi _SFR_IO_ADDR(DATAFLASH_PORT), DATAFLASH_PIN_RST
	sbi _SFR_IO_ADDR(DATAFLASH_PORT), DATAFLASH_PIN_CS
	
	//setup port directions
	ldi r16, (1<<2)|(1<<3)|(0<<4)|(1<<5) //SS,MOSI,SCK = OUT  MISO = IN
	out _SFR_IO_ADDR(DDRB), r16

	//init & enable SPI
	ldi r16, (1<<MSTR)|(0<<CPOL)|(0<<DORD)|(0<<SPR0)|(0<<SPR1)|(1<<SPI2X)|(1<<SPE) //f/2 baudrate, enable spi
	out _SFR_IO_ADDR(SPSR), r16

	//enable dataflash:
	//stop reset dataflash & set CS=hi
	sbi _SFR_IO_ADDR(DATAFLASH_PORT), DATAFLASH_PIN_CS
	sbi _SFR_IO_ADDR(DATAFLASH_PORT), DATAFLASH_PIN_RST
	
	//done -> return
	ret
.endfunc
