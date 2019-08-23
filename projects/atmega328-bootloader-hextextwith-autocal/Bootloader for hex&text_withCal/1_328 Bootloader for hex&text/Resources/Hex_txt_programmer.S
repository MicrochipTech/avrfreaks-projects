

;The HW subroutines required to clock commands and data 
;in and out of a target device have been replaced by the
;following assembly routines which are based on prototytpes
;given in the Atmega 328 data sheet in the section
;"27. Boot Loader Support- Read-While-Write Self-Programming"



#include <avr/io.h>			

.extern Flash_readout			;
.extern loc_in_mem_H			;LOcations in SRAM such as the location of the next command to be 
.extern loc_in_mem_L			;written to the page buffer or the location of the new lock byte
.extern Prog_mem_address_H		;Address in flash to read from, the address of the page to write or erase,  
.extern Prog_mem_address_L		;the next address on the page buffer, or the address of a configuration/lock byte


;.global read_flash				;Assembly subroutines
.global clear_read_block		;Required afer programming before flash can be read
.global Page_erase				;
.global write_to_page_buffer	;
.global page_write				;
.global read_config_bytes		;
.global set_lock_byte			;Protects application space from further programming





;***************************************************
clear_read_block:
push r20
ldi r20,  (1 << RWWSRE) | (1 << SELFPRGEN)
call do_spm
pop r20
ret




;***************************************************
Page_erase:					
push r20
push ZH
push ZL

lds ZH, Prog_mem_address_H;
lds ZL, Prog_mem_address_L;

ldi r20, (1 << PGERS) | (1 << SELFPRGEN)
call do_spm

pop ZL
pop ZH
pop r20
ret





;***************************************************
write_to_page_buffer:

push r0
push r1
push r20
push YH
push YL
push ZH
push ZL

lds YH,loc_in_mem_H
lds YL, loc_in_mem_L

lds ZH, Prog_mem_address_H;
lds ZL, Prog_mem_address_L;

ld r0, Y+
ld r1, Y
ldi r20, (1 << SELFPRGEN)
call do_spm

pop ZL
pop ZH
pop YL
pop YH
pop r20
pop r1
pop r0
ret




;**************************************************
page_write:
push r20
push ZH
push ZL

ldi r20, (1 << PGWRT) | (1 << SELFPRGEN)

lds ZH, Prog_mem_address_H;
lds ZL, Prog_mem_address_L;

call do_spm

pop ZL
pop ZH
pop r20
ret



;**************************************************
set_lock_byte:
push r0
push r20
push YH
push YL
push ZH
push ZL

lds ZH, Prog_mem_address_H;
lds ZL, Prog_mem_address_L;

lds YH,loc_in_mem_H
lds YL, loc_in_mem_L
ld r0, Y

ldi r20, (1 << BLBSET) | (1 << SELFPRGEN)
call do_spm
pop ZL
pop ZH
pop YL
pop YH
pop r20
pop r0
ret



;***************************************************
read_config_bytes:	
push r0;
push ZH
push ZL
push r16
push r20

lds ZH, Prog_mem_address_H;
lds ZL, Prog_mem_address_L;

ldi r20, (1 << BLBSET) | (1 << SELFPRGEN)

Wait_lpm_2:
	lds r16, SPMCSR
	sbrc r16, SELFPRGEN
	rjmp Wait_lpm_2

sts SPMCSR, r20
lpm r0, Z;
sts Flash_readout,  r0;

pop r20
pop r16
pop ZL
pop ZH
pop r0;
ret	







;**************************************************
do_spm:
push r16
Wait_spm_2:
	lds r16, SPMCSR
	sbrc r16, SELFPRGEN
	rjmp Wait_spm_2

cli
sts SPMCSR, r20
spm
sei
pop r16
ret



	