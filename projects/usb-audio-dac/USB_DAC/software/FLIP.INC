LANG OCTASM,0.1
define us  disk\usb
	load_files()
	eax=[program_size] edx=[eprom_size]
	printf("%10 program size: %d bytes%10 eeprom size: %d bytes %0",eax,edx) esp+=12
	ebx=[us\devices] jmp >3
      # ret
      # ebx=[ebx+us\info2\next_device]
      # test ebx jz <3
	chip_id() jc <2
	or eax,edx jz <3
	us\retirar() or b[ebx+us\info2\status],80h
define do()  printf("%10 #1 %0") esp+=4 #1()
define do2()  do(#1) get_status() jc error
	do2(full_chip_erase)
	ecx=[program_size] jecxz >1
	do2(write_flash,read_flash)
      # ecx=[eprom_size] jecxz >1
	do2(write_eeprom,read_eeprom)
      # do(verify,start_aplication)
	us\add_device() jmp <4
      #error us\add_device()
	printf("%10 reset the device and try again %0") esp+=4
	ret

#verify esi=firmware_start edi=verify_bufer
	ecx=[program_size] call >1
	esi=firmware_start2 edi=verify_bufer2 ecx=[eprom_size]
      # jecxz >2
	rep cmpsb esi="ok %0" je >1
	esi="error %0" ++ecx
      # printf(esi) esp+=4 test ecx jz >1
	esp+=4 jmp error
      # ret

#load_files
	{
	f1(firmware_start,program_size)
      # o\next_section2() cmp al,8 jc <1
	f1(firmware_start2,eprom_size)
	ret
    #f1
	test b[bo\bits],10 jnz >1
	eax=[bo\f_out] test eax jz >1
	trunc() filesize() test ecx jnz error1
	edi=[esp+4] ebx=[esp+8]
	ecx=edx cmp edx,[ebx] ja error
	lseek0() read() edi+=ecx
	[ebx]=ecx esi=dfuh ecx=4 rep movsd
      # retp 8
     #error1 edx=-1
     #error
	[bo\num_linea]=edx
	esi="%nb program is too big" jmp o\error\grave
	   }

#chip_id
	cmp d[ebx+us\info2\vendor],vendor je >1
	stc ret
      # printf("%10 %dev_name detected%0") esp+=4 ret

#start_aplication  ;hardware reset
	pusha
	w[dfu_dnload+6]=3 d[ebx+us\info2\tmp_bufer]=304h
	us\descriptor(dfu_dnload)
	w[dfu_dnload+6]=0
	us\descriptor(dfu_dnload)
	popa ret

#get_status
	pusha us\descriptor(dfu_getstatus) jc >1
	eax=b[edi] test eax jz >1
	printf(" status error: %x %0",eax) esp+=8 stc
      # popa ret

#full_chip_erase
	pusha w[dfu_dnload+6]=3
	d[ebx+us\info2\tmp_bufer]=0ff0004h
	us\descriptor(dfu_dnload)
	popa ret

#write_eeprom edi=firmware_start2-20h jmp >1
#write_flash edi=firmware_start-20h
      # pusha eax=ecx+30h --ecx xchg cl,ch
	[dfu_dnload+6]=ax [edi+4]=cx
	us\descriptor\bufer(dfu_dnload)
	popa ret

#read_eeprom eax=203h esi=verify_bufer2 jmp >1
#read_flash eax=3 esi=verify_bufer
      # pusha w[dfu_dnload+6]=6
	d[ebx+us\info2\tmp_bufer]=eax
	push cx --cx xchg cl,ch [ebx+us\info2\tmp_bufer+4]=cx
	 us\descriptor(dfu_dnload)
	pop w[dfu_upload+6] edi=esi
	us\descriptor\bufer(dfu_upload)
	popa ret

DAT4
#dfuh
dd 0 ;crc
db 16,'DFU'
dw 100h
dw -1,-1,-1

align 20h
dw 1,0,0
align 20h
#firmware_start rb flash_size
#verify_bufer rb flash_size
#program_size dd flash_size

align 20h
dw 101h,0,0
align 20h
#firmware_start2 rb eeprom_size
#verify_bufer2 rb eeprom_size
#eprom_size dd eeprom_size

#dfu_dnload dd 0121h,4h;+lenght
#dfu_upload dd 02a1h,4h;+lenght
#dfu_getstatus dd 03a1h,60004h

