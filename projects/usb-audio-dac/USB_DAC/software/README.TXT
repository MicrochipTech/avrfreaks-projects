	This directory contains the required files for
programing some avr8 microcontrollers using the usb port.
the directory '\H\' contains header files with port address
for some avr8 chips.Directory '\PRG\' contains some examples
of firmware '*.avr' for microcontrollers,and some '.asm'
aplications that runs on the PC to interface the external
 devices. 'AVRASM.INC' is the avr assembler and
'FLIP.INC' is for programing the flash memory on the chip
using the DFU protocol.
The avr8 source files must include the apropiate file
header and then can be assembled like any other
Octasm program, if there is no errors a binary file is
produced (\\RD\avr8\atmega.bin) and if the chip is connected
and the bootloader is active the chip will be programmed and
reset to start the new aplication.The typical output messages
after assembling the source without errors will be something
like this:
program size :910 bytes
eeprom size: 16 bytes
ATmega16U4 detected
full_chip_erase
write_flash
read_flash
write_eeprom
read_eeprom
verify ok ok
start_aplication

The avr assembler is included in the avr source program and
assembled everytime just before avr8 intructions,then
it takes control of the assembling process to generate
avr8 code instead of the default x86 code.
You can edit the sources to change some setings like the
output file or the keywords and then assemble your program.

The octa­avr­assembler does not use the same language as the
original atmel­avr­assembler,so here is a resume
of the instructions and registers names:

Data declarations and other instructions not cpu related
are the same than for the x86-octasm (see \DOCS\OCTASM.TXT).

IMPORTANT NOTE
	The atmel avr assembler uses word adress for program memory and
byte adress for sram,while octasm always uses bytes adress,so when using
function pointers the offset needs to be divided by two.
	The atmel avr assembler uses memory adress and port adress that
start on memory adress 20h,while octasm only uses memory adress,thats why
'PORTB' adress is 025h for octasm and 5h for atmel avr assembler.
	The file 'M16U4.INC' already has these adress translated.


n3 0-7 number
n6 0-63 number
n8 8 bits number
n16 16 bits number

reg8 r0-r31
reg16 wr0,wr2,wr4 .... wr30 ;rx=wr26,ry=wr28,rz=wr30

flags
i=interrupt enable
t=temp
h=half carry ;carry from bit3
l=less =s xor o        used to compare signed numbers
o=overflow
s=negative =msb of result
z=zero
c=carry                used to compare unsigned numbers

post increment or pre decrement do not change flags

instructions that use the flags,* means some flag name:
	j*  jmp if flag=1
	jn* jmp if flag=0
	se* flag=1
	cl* flag=0

mem valid expressions to read or write on sram memory
	[rx],[ry+n6],[rz+n6],[n16]
	[rx++],[ry++],[rz++]   post increment
	[--rx],[--ry],[--rz]   pre decrement
	'[--' and '++]' must be written together
	'[ --' is not correct.
mem valid expressions to read on flash memory
	[cs+rz],[cs+rz++]



=  ;mov
reg8=reg8
reg16=reg16
[mem]=reg8
reg8=[mem]
reg8=n8
reg16=n16 ;translated to 2*8 bits instructions

+= ;add
reg8+=reg8
reg8+=n8   ;translated to reg8-=-n8
reg16+=n6  ;only wr24 and above

-= ;sub
reg8-=reg8
reg8-=n8
reg16-=n6  ;only wr24 and above

++ ;inc
++reg8
++reg16 ;translated to reg16+=1

-- ;dec
--reg8
--reg16 ;translated reg16-=1

sar reg,n arithmetic shift right
not reg
neg reg
swap reg swap nibbles,xchanges bits 7:4<->3:0
rcr reg  rotate right with carry
bld reg,n copy bit from temp flag to n bit in reg
bst reg,n copy n bit from reg to tmp flag
sbrc reg,n  skip if bit n in reg is 0
sbrs reg,n  skip if bit n in reg is 1
sbic port,n skip if bit n in port is 0 port 0-31 mem 32-63
sbis port,n
cbi port,n  set bit n to 0 0-31 mem 32-63
sbi port,n  set bit n to 1
adc reg,reg
cmp reg,reg
cmp reg,k  ;reg16-31,n0-255
cmpc rd,rr  (rd-rr-cf) compare with carry flag
cpse rd,rr     skip if rd=rr
xor rd,rr
reg=reg (reg8 or reg16)
[mem]=reg
reg=[mem]
reg=k
mul rd,rr  wr0=rd*rr
sbb rd,rr  rd=rd-rr-cf
muls rd,rr wr0=rd*rr multiply signed           reg16-31
mulsu rd,rr wr0=rd*rr multiply signed*unsigned reg16-23
fmul
fmuls
fmulsu
sbr reg,n   or reg,n reg16-31
break
nop
ret
iret (reti) ret,if=1
sleep enter power save modes
wdr  watchdog reset
clc sec
clh
cli
cls
icall   ­>call [rz]
ijmp    ­>jmp [rz]
jmp offset ­>rjmp\jmp
jmp w offset ->rjmp  2bytes 2 clks
jmp d offset ->jmp   4bytes 3 clks
func_name(regs) ­>push regs  rcall\call func_name
push/pop reg8/16 ;if reg is 16 bits,2*8bits instructions are output push wr0->push r0 push r1
spm  ->spm
spmp ->spm z+

clocks
5       ret,iret,call32
4       call16
3       [flash],jmp32,skip inst32
2       [sram],push,pop,jmp16,16bit arithmetic,cbi,sbi,conditional instructions,mov wreg,inm
1       [sram<60h],conditional instructions when condition is false,mov wreg,wreg
	other instructions.

