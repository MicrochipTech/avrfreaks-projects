#if BTLDR
  .cseg
  #if defined(__ATmega168__) || defined(__ATmega168PA__)
   .org	SECONDBOOTSTART		;0x1F00 for OptiBoot-Loader
   BOOTIMAGE:
   .if f_clk == 4000000
		.message	"Notice: BootloaderCDC for m168 @ 4MHz enabled"
		.include	"Include/Code_BootloaderCDC-m168-4mhz.inc"
    .elif f_clk == 8000000
		.message	"Notice: BootloaderCDC for m168 @ 8MHz enabled"
		.include	"Include/Code_BootloaderCDC-m168-8mhz.inc"
    .elif f_clk == 16000000
		.message	"Notice: BootloaderCDC for m168 @ 16MHz enabled"
		.include	"Include/Code_BootloaderCDC-m168-16mhz.inc"
    .elif f_clk == 20000000
		.message	"Notice: BootloaderCDC for m168 @ 20MHz enabled"
		.include	"Include/Code_BootloaderCDC-m168-20mhz.inc"
    .else
		.error "Bootloader is not supported for this clock speed!"
    .endif
;--------------------------------------------------------------------------------
  #elif defined(__ATmega328__) || defined(__ATmega328PA__)
    .org	FIRSTBOOTSTART		;0x3F00 for OptiBoot-Loader
    BOOTIMAGE:
    .if f_clk == 4000000
		.message	"Notice: BootloaderCDC for m328 @ 4MHz enabled"
		.include	"Include/Code_BootloaderCDC-m328-4mhz.inc"
    .elif f_clk == 8000000
		.message	"Notice: BootloaderCDC for m328 @ 8MHz enabled"
		.include	"Include/Code_BootloaderCDC-m328-8mhz.inc"
    .elif f_clk == 16000000
		.message	"Notice: BootloaderCDC for m328 @ 16MHz enabled"
		.include	"Include/Code_BootloaderCDC-m328-16mhz.inc"
    .elif f_clk == 20000000
		.message	"Notice: BootloaderCDC for m328 @ 20MHz enabled"
		.include	"Include/Code_BootloaderCDC-m328-20mhz.inc"
    .else
		.error "Bootloader is not supported for this clock speed!"
    .endif
;--------------------------------------------------------------------------------
  #elif defined(__ATmega32U4__)
	.org		FOURTHBOOTSTART
  BOOTIMAGE:
  .if f_clk == 8000000
		.message	"Notice: BootloaderDFU for m32u4 @ 8MHz enabled"
		.include	"Include/Code_BootloaderDFU_atmega32u4-8M.inc"
    .elif f_clk == 16000000
		.message	"Notice: BootloaderDFU for m32u4 @ 16MHz enabled"
		.include	"Include/Code_BootloaderDFU_atmega32u4-16M.inc"
    .else
    	.message "Bootloader DFU is not supported for this clock speed!"
    	.message "Bootloader DFU will not be included."
  .endif
;--------------------------------------------------------------------------------
  #elif defined(__AT90USB1286__)
	#if TEENSY
	  .org		FIRSTBOOTSTART	;The TEENSY++ 2.0 bootloader is 2K words
	#else
	  .org		FOURTHBOOTSTART	;LUFA DFU is 4K words
	#endif
   BOOTIMAGE:
   .if f_clk == 8000000
		.message	"Notice: BootloaderDFU for AT90usb1286 @ 8MHz enabled"
		.include	"Include/Code_BootloaderDFU_at90usb1286-8M.inc"
    .elif f_clk == 16000000
		.message	"Notice: BootloaderDFU for AT90usb1286 @ 16MHz enabled"
		.include	"Include/Code_BootloaderDFU_at90usb1286-16M.inc"
    .else
    	.message "Bootloader DFU is not supported for this clock speed!"
    	.message "Bootloader DFU will not be included."
  .endif
;--------------------------------------------------------------------------------
  #elif defined(__ATmega2560__)
  .org		FOURTHBOOTSTART			;STK500V2 is 4K words
   BOOTIMAGE:
    .if f_clk == 4000000
		.message	"Notice: BootloaderCDC for m2560 @ 4MHz enabled"
		.include	"Include/Code_BootloaderCDC-m2560-4mhz.inc"
    .elif f_clk == 8000000
		.message	"Notice: BootloaderCDC for m2560 @ 8MHz enabled"
		.include	"Include/Code_BootloaderCDC-m2560-8mhz.inc"
    .elif f_clk == 16000000
		.message	"Notice: BootloaderCDC for m2560 @ 16MHz enabled"
		.include	"Include/Code_BootloaderCDC-m2560-16mhz.inc"
    .elif f_clk == 20000000
		.message	"Notice: BootloaderCDC for m2560 @ 20MHz enabled"
		.include	"Include/Code_BootloaderCDC-m2560-20mhz.inc"
    .else
		.error "Bootloader is not supported for this clock speed!"
    .endif
  #endif
#endif
