;Basic assembler subroutines develped for use with the Atmega 328 bootloader program

;The HW subroutines required to clock commands and data 
;in and out of a target device have been replaced by the
;following assembly routines which are based on prototytpes
;given in the Atmega 328 data sheet in the section
;"27. Boot Loader Support- Read-While-Write Self-Programming"



#include <avr/io.h>			

.extern Flash_readout			;
.extern Prog_mem_address_H		;Address in flash to read from, the address of the page to write or erase,  
.extern Prog_mem_address_L		;the next address on the page buffer, or the address of a configuration/lock byte


.global read_flash				;Assembly subroutines



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



	