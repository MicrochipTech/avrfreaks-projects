

;Assembly routines required to read data from flash
;See 328 data sheet section 27 "Boot Loader Support- Read-While-Write 
;Self-Programming" for prototypes 



#include <avr/io.h>			

.extern Flash_readout			;
.extern loc_in_mem_H			;LOcations in SRAM such as the location of the next command to be 
.extern loc_in_mem_L			;written to the page buffer or the location of the new lock byte
.extern Prog_mem_address_H		;Address in flash to read from, the address of the page to write or erase,  
.extern Prog_mem_address_L		;the next address on the page buffer, or the address of a configuration/lock byte


.global read_flash				;Assembly subroutines
.global clear_read_block		;Required afer programming before flash can be read
.global read_config_bytes		;



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
sts SPMCSR, r20
spm
pop r16
ret



	