.if DATA
;These equates are for the MICROCHIP 25AA256 32K byte Serial EEPROM
	.equ	DA_SIZE		= 32768	;device size in bytes
	.equ	DA_PGSZ		= 64	;page size in bytes
	.equ	DA_DASZ		= 8		;bit width of data
	.equ	DA_ADSZ		= 16	;bit width of addresses

	;Instruction set
	.equ	DA_WRSR		= 1		;Write STATUS register
	.equ	DA_WRIT		= 2		;Write data to memory array beginning at selected address
	.equ	DA_READ		= 3		;Read data from memory array beginning at selected address
	.equ	DA_WRDI		= 4		;Reset the write enable latch (disable write operations)
	.equ	DA_RDSR		= 5		;Read STATUS register
	.equ	DA_WREN		= 6		;Set the write enable latch (enable write operations)

	;Status Register bit definitions 
	.equ	DA_WIP		= 0		;"1" = Write in progress
	.equ	DA_WEL		= 1		;Write Enable Latch, "1" = write allowed
	.equ	DA_BP0		= 2		;Block Protection bit 0
	.equ	DA_BP1		= 3		;Block Protection bit 1
	.equ	DA_WPEN		= 7		;Write Protect Enable 

	.equ	DFI_BYTE	= 0x00	;data to use for bulk erase 
	.equ	DFD_SEP		= ','	;data separator for "DFD" command
;
.endif
