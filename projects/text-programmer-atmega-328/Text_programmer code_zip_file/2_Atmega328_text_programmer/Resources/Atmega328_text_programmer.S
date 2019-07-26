;Basic assembler subroutines developed for use with the Atmega 328 bootloader program

;Note however one important change: 

;cli/sei has been added to the do_spm subroutine

;without this addition, the requirement to initiate the spm command within 3 clock cycles of setting 
;the SPMCSR register is occaisionally violated and gaps appear in the text where characters
;are not written to the page buffer.

; It is by pure change that this ommission did not result in a bug in the bootloader program.  However the
;bootloader was tested quite extensively and no issues were tested.  No changes will therefore be made to the
;bootloader at this time.



#include <avr/io.h>			

.extern Flash_readout			;
.extern loc_in_mem_H			;LOcations in SRAM such as the location of the next command to be 
.extern loc_in_mem_L			;written to the page buffer or the location of the new lock byte
.extern Prog_mem_address_H		;Address in flash to read from, the address of the page to write or erase,  
.extern Prog_mem_address_L		;the next address on the page buffer, or the address of a configuration/lock byte


.global read_flash				;Assembly subroutines
.global clear_read_block		;Required afer programming before flash can be read
.global Page_erase				;
.global write_to_page_buffer	;
.global page_write				;



;***************************************************
read_flash:						
push r0;
push ZH
push ZL
lds ZH, Prog_mem_address_H;
lds ZL, Prog_mem_address_L;
lpm r0, Z;
sts Flash_readout,  r0;

pop ZL
pop ZH
pop r0;
ret	


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
do_spm:
push r16
Wait_spm_2:
	lds r16, SPMCSR					;load direct from SRAM. r16 becomes SPMCSR
	sbrc r16, SELFPRGEN				;skip if bit in register cleared
	rjmp Wait_spm_2					;relative jump
cli
sts SPMCSR, r20						;store direct to SRAM. SPMCSR becomes r20					
spm									;store program memory command
sei
pop r16
ret



	