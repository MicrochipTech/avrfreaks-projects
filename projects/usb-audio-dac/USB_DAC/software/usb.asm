LANG OCTASM,0.1
;los registros r4-r19 se usan solo en las isr
;r10=0

#USB_GEN{
	;TMP1=[UDINT] sbrc TMP1,2 jmp sof_int
	push rz,r24,r25
  #eorst
	rz=endpoints_config r25=0
#endp_config
	[UENUM]=r25  ;select endp
	r24=1 [UECONX]=r24 ;activate endp
	eeprom_r() [UECFG0X]=r24 ;direction,type
	eeprom_r() [UECFG1X]=r24 ;memory allocation
	eeprom_r() [UEIENX]=r24 ;interrupts enable
	++r25 cmp r25,n_endpoints jc endp_config
	r25=0 [UDCON]=r25
	r24=[UDINT] r24-=8 [UDINT]=r24
	r24=DEFAULT_ENDPOINT [UENUM]=r24
	r24=[status] r24&=3fh [status]=r24
	pop rz,r24,r25 IRET
	}

#USB_COM{ stack_size=2*7
define ERROR(A) r22=A usb_error()
	push rz,ry,rx,wr24,wr22,wr0,wr2
	r25=[UEINT] [UENUM]=ZERO
	r24=[UEINTX]
	sbrs r25,0 jmp stall ;jmp endp1
	;[UENUM]=ZERO
	sbrc r24,3 jmp rkstp
#transfer
	r29=[usb_cmd] r30=[transfer_ptr] r31=[transfer_ptr+1]
	r26=[transfer_size] r27=[transfer_size+1] wr2=wr26
	cmp r27,0 je >2
      # wr26=mps_e0
      # cmp r26,mps_e0+1 jnc <2
	r2-=r26 sbb r3,r27
	[transfer_size]=r2 [transfer_size+1]=r3
	r1=r26 r26+=r30 adc r27,r31
	[transfer_ptr]=r26 [transfer_ptr+1]=r27
	sbrc r24,2 jmp rxout
	sbrc r24,0 jmp txini
	ERROR(2)
 #exit  r24=DEFAULT_ENDPOINT [UENUM]=r24
	pop rz,ry,rx,wr24,wr22,wr0,wr2 IRET
 #rxout
	r25=84h r1&=r1 jnz >1 jmp end_w0
      # --r29 js write_sram jz write_flash
	--r29 jz write_eeprom
	--r29 jz exit ;jnz >1 jmp write_bufer_c
      # --r29 jz write_byte
	--r29 jz write_aox
	;write_stack
	r26=[SPL] r27=[SPH] ;sp apunta a stack-1
	ry=rx r28-=r1 sbb r29,ZERO
	[SPL]=r28 [SPH]=r29 ++rx ++ry
	r24=stack_size movedata() rz=ry
#write_sram r0=[UEDATX] [rz++]=r0 --r1 jnz write_sram
      #end_w
	r24=[UEIENX] r24|=1 [UEIENX]=r24 ;enable txini int
	jmp end_w0
#write_byte r1>>=1
      # r30=[UEDATX] r0=[UEDATX] [rz]=r0 --r1 jnz <1 ;9 clocks
#write_flash     jmp end_w
#write_eeprom
	r24=[UEDATX] eeprom_w()
	--r1 jnz write_eeprom jmp end_w
#write_aox r1>>=2
      # r30=[UEDATX] r0=[rz]
	r28=[UEDATX] r0&=r28
	r28=[UEDATX] r0|=r28
	r28=[UEDATX] xor r0,r28
	[rz]=r0 --r1 jnz <1
	jmp end_w

      # r24=[UEIENX] r24&=!1 [UEIENX]=r24 ;disable txini int
	jmp end_read0
#txini r25=1 r1&=r1 jz <1
	--r29 js read_sram jz read_flash
	--r29 jz read_eeprom
	jmp exit
#read_eeprom
	eeprom_r()
	[UEDATX]=r24 --r1 jnz read_eeprom jmp end_read
#read_flash r0=[cs+rz++] [UEDATX]=r0 --r1 jnz read_flash
	jmp end_read
#read_sram r0=[rz++] [UEDATX]=r0 --r1 jnz read_sram
#end_w0
#end_read0
#end_read
   #eoi r24=[UEINTX] not r25
	r24&=r25 [UEINTX]=r24 jmp exit

 #rkstp
	r24=[UEDATX] r25=[UEDATX] r26=[UEDATX] r27=[UEDATX]
	r28=[UEDATX] r29=[UEDATX] r30=[UEDATX] r31=[UEDATX]
	;save_request()
	r22=[UEINTX] r22&=!8 [UEINTX]=r22
	sbrc r24,6 jmp vendor
	sbrc r24,0 jmp interface
	r28|=r29 jz >1 jmp stall #
	cmp r24,80h je setup_in
	cmp r24,0 jne stall
;setup_out
	cmp r25,9 je set_config
	cmp r25,5 jne stall
	;set adress
	[UDADDR]=r26 r22=[UEINTX] r22&=!1 [UEINTX]=r22 ;in command of 0 bytes
      # r22=[UEINTX] r22&=1 jz <1
	r26|=80h [UDADDR]=r26 ;jmp stall
	jmp exit
#set_config
	r22=[UEINTX] r22&=!1 [UEINTX]=r22 ;in command of 0 bytes
	jmp exit

#setup_in
	cmp r26,0 jne stall
	cmp r25,6 jne stall
	r26=r30
	rz=device_descriptor
	--r27 jz get_device
	--r27 jnz stall
	 rz=config_descriptor
#get_device
	 r27=[cs+rz++] ry=rz
	 cmp r26,r27 jc >1
	 r26=r27
       # r25=1 ;readflash
	 r31=0 r30=r26
#vendor [usb_cmd]=r25
	[transfer_ptr]=r28 [transfer_ptr+1]=r29
	[transfer_size]=r30 [transfer_size+1]=r31
	r25=0 sbrs r24,7 jmp eoi
	jmp end_w

 #stall ERROR(4)
	r25=[UECONX] r24=20h;STALLRQ
	r25|=r24 [UECONX]=r25
	r25=8 jmp eoi

;#endp1  ERROR(1)
;        ++r0 sbrs r25,1 jmp endp2

#usb_error r1=[usb_errors+0] r1|=r22 [usb_errors+0]=r1 ret
;;
#endp2  ++r0 [UENUM]=r0
	[endp]=r0 [endp+1]=r25 [endp+2]=r24
	r24=[UEINTX] [endp+3]=r24
	r24=[UESTA0X] [endp+4]=r24
	r24=[UESTA1X] [endp+5]=r24
	r24=[UEBCLX] [endp+6]=r24
	r24=[UEBCHX] [endp+7]=r24
	pop rz,ry,rx,wr24,wr22,wr0,wr2
	ret

	r25=[UEINT]
	r24=[UEINTX] r0=0
	sbrs r25,0 jmp endp1

 ;       r24=[UEDATX] r25=[UEDATX] r26=[UEDATX] r27=[UEDATX]
 ;       r28=[UEDATX] r29=[UEDATX] r30=[UEDATX] r31=[UEDATX]
#save_request
;rz,ry,rx,wr24,wr22,wr0,wr2
	push r22,r0,wr2
	wr2=rz rz=adc_bufer r22=[adc_index+1]
	r0=0 r30+=r22 adc r31,r0
	[rz++]=r24 [rz++]=r25 [rz++]=r26 [rz++]=r27 [rz++]=r28
	[rz++]=r29 [rz++]=r2 [rz++]=r3 r22+=8 jz >1
	[adc_index+1]=r22
      # rz=wr2 pop r22,r0,wr2
	ret
;;

SRAM    #endp rb 8
	#usb_errors db 0

	#usb_cmd db 0
	#transfer_size dw 0
	#transfer_ptr dw 0

	}

#usb_on
	push r20
	r20=0 [UDIEN]=r20 ;disable ints
	r20=1 [UHWCON]=r20
	r20=10010000b [USBCON]=r20 ;frzclk se pone a 1
	[USBCON]=r20 ;poner frzclk a 0
	r20=0 [UDCON]=r20
	pop r20
	ret

;detach,disable usb,disable pll,disable regulator

#usb_off
	push wr24
	r24=0 [UEIENX]=r24 [UDIEN]=r24
	++r24 [UDCON]=r24
	r24=00100000b
	[USBCON]=r24
	r24=0 [UHWCON]=r24
	pop wr24 ret

