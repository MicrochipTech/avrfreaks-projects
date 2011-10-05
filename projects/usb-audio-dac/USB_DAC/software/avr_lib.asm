LANG OCTASM,0.1
;los registros r4-r19 se usan solo en las isr
;r10=0
;SPH no se toca,el stack esta alineado y tiene 256bytes de tamaño
;por lo que solo cambia SPL

;;
la variable heap apunta al primer bloque libre
cada bloque libre contiene un puntero al siguiente o 0 si es el ultimo
tras los 2 bytes del puntero hay 2 bytes que indican el tamaño del bloque
le tamaño de los bloques se redondea a 4
el tamaño de los bloques en uso esta limitado a 256 bytes
;;

#malloc
	{;r24+1=bytes to allocate
	;returns cf=0,ry  -> allocated memory ,[ry]=r24
	;cf=1 if no memory
	push r24,wr26,wr30 r24+=4 r24&=!3
	wr28=heap
 #next  wr30=wr28
	r28=[wr30] r29=[wr30+1]
	wr28+=0 jz no_mem
	;comprobar tamaño
	r26=[wr28+2] r27=[wr28+3]
	r26-=r24 sbb r27,ZERO jc next jz no_split
     #split
	push wr28 r28+=r24 adc r29,ZERO
	[wr30]=r28 [wr30+1]=r29
	[wr28+2]=r26 [wr28+3]=r27
	wr30=wr28
	pop wr28
     #no_split
	r24=[wr28] [wr30]=r24 r24=[wr28+1] [wr30+1]=r24 ;point to next block
	pop r24,wr26,wr30 [ry]=r24 ret
     #no_mem
	pop r24,wr26,wr30 wr28=0 stc ret
	}

#free   r24=[wr28]
#free2  ;r24=size­1
	{
	;ry->bufer
	push wr24,wr26,wr30 cmp r29,RAMEND>>8+1 jnc exit
	cmp r28,ZERO cmpc r29,ZERO je exit
	r24+=4 r24&=!3
	[wr28+2]=r24 [wr28+3]=ZERO
	wr26=heap
 #next  wr30=wr26
	r26=[wr30] r27=[wr30+1]
	cmp r26,r28 cmpc r27,r29 jnc >1
	cmp r26,ZERO cmpc r27,ZERO jnz next
      # ;enlazar
	[wr30]=r28 [wr30+1]=r29
	[wr28]=r26 [wr28+1]=r27
	cmp r30,heap>>8 jne >1
	cmp r31,heap&8 je >2
      # merge()
      # cmp r26,ZERO cmpc r27,ZERO je exit
	wr30=wr28 wr28=wr26 merge()
  #exit pop wr24,wr26,wr30 wr28=0 ret
 #merge r24=[wr30+2] r25=[wr30+3]
	r24+=r30 adc r25,r31
	cmp r24,r28 cmpc r25,r29 je >1
	ret
      # r24=[wr30+2] r25=[wr28+2] r24+=r25 [wr30+2]=r24
	r24=[wr30+3] r25=[wr28+3] adc r24,r25 [wr30+3]=r24
	r24=[wr28] r25=[wr28+1] [wr30]=r24 [wr30+1]=r25
	wr28=wr30
	ret
	}
stack_end=RAMEND+1
;;
La multitarea usa una pila compartida de 256 bytes en cada cambio de tarea
se intercambian los datos de la pila.Pilas contiene los punteros al bloque
de memoria con los datos de la pila de cada tarea,si es 0 no existe esa tarea.
Buzones contiene listas
enlazadas de mensajes.task_list contiene un array de bytes que indican la
siguiente tarea a ejecutar ,el numero de tarea va de 0 hasta ntasks-1,el
numero -1 indica que la tarea no esta en la lista.
La tarea numero 0 es el servicio despertador y siempre esta activa.
;;

#task{

SRAM
	#n rb 1   ;numero de tarea 0­>ntasks-1
	n_tasks=8
	#Pilas rb 2*n_tasks
	#Buzones rb 2*n_tasks
	#task_list rb n_tasks ;indica el numero de la siguiente tarea
FLASH

#salva4 push wr24,wr26,wr28,wr30
	salva()
	pop wr24,wr26,wr28,wr30 ret

#load   pop r23,r22 ;return address
	;stack:  b n_bytes,w func,data1,data2
	;retorna numero de tarea en r25
	rz=Pilas-2 r25=-1
      # rz+=2 ++r25 cmp r25,n_tasks jnc error
	r26=[rz] r27=[rz+1] r26|=r27 jnz <1  ;buscar slot libre
	pop r24 malloc() [rz++]=r28 [rz++]=r29 ;colocar la pila
	r26=[SPL] r27=[SPH] ++wr26 ++wr28 movedata()
	--wr26 [SPL]=r26
	;alistar
	r24=[n] wr26=task_list wr28=wr26
	r26+=r24 adc r27,ZERO
	r28+=r25 adc r29,ZERO
	r24=[wr26] [wr26]=r25 [wr28]=r24
	rz=wr22 ijmp

#error
#l1     r25=[n] jmp l2
#salva
	r25=[n] r25+=r25 wr30=Pilas r30+=r25 adc r31,ZERO
	r28=[wr30] r29=[wr30+1] r26=[SPL] r27=[SPH]
	r23=-1 xor r23,r26 r24=[wr28] cmp r24,r23 je >1
	free2() r24=r23 malloc() [wr30]=r28 [wr30+1]=r29
      # ++wr26 ++wr28 movedata()
	r23=-1 [SPL]=r23
	;siguiente tarea
	wr30=task_list r25=[n]
	r30+=r25 adc r31,ZERO
	r25=[wr30] [n]=r25
#l2     r25+=r25 wr30=Pilas r30+=r25 adc r30,ZERO
	r26=[wr30++] r27=[wr30]
#restaura r24=[wr26++] wr28=stack_end-1 r28-=r24
	[SPL]=r28 ++wr28
	;jmp movedata
	}
#movedata ;rx=source ry=dest r24=bytes to move
	push r2
      # r2=[rx++] [ry++]=r2
	--r24 jnz <1
	pop r2 ret

#memset  ;ry=dest r24=bytes to write r0
      # [ry++]=r0 --r24 jnz <1
	ret

correo{
;;
	formato del correo:
	byte que indica el tamaño del mensaje ,sin contar este byte.
	word puntero al siguiente mensaje
	byte direccion de destino/retorno
	nbytes datos del mensaje
ejemplo:
	db 7 ;tamaño
	dw 0 ;siguiente
	db 0 ;direccion de la primera tarea (kronos32)
	dd 123456 ;datos (responder cuando [timerdata]>123456)
;;
;puntero a correo en wr28
#reenviar push wr24 r25=[wr28+3] jmp >1 ;r25=destinatario
#enviar push wr24
      # r24=[task\n] [wr28+3]=r24
	;cmp r25,task\n_tasks jnc error
	r25+=r25 wr30=task\Buzones
	r30+=r25 adc r31,ZERO
	;[r30++]=r28 [wr30++]=r29


#recoge2 ;si no hay mensajes la tarea se pone a dormir hasta que haya algun mensaje

#recoge  ;retorna wr28->correo
	 ;si no hay correo retorna 0 y zf=1

	}

;#wait_ms ;milisegundos a esperar en eax
;;
      # push kronos32 eax=[esi]
	cmp eax,[timerdata] js correo\reenviar
	esi=esp+4
      # ebp=esi esi=[ebp] test esi jz >1
	ebx=b[esi+4] cmp [esi+ebx+5],eax js <1
	[edi]=esi
      # [ebp]=edi ret
      # push [timerdata],[correo\msgs],edi task\salva()
	pop ecx,edx
	ebx=[correo\msgs] cmp ebx,edx jne >1
	eax=[timerdata] cmp eax,ecx jne >2
	edi=active interval() hlt
	edi=inactive
	push >2
#interval _rdtsc()
  ecx=[tmp] ebx=[tmp+4]
  [tmp]=eax [tmp+4]=edx
  sub eax,ecx sbb edx,ebx
  add [edi],eax adc [edi+4],edx
  ret
#kronos32
      # correo\recoge() jnz <5
      # pop edi test edi jz <3
	ebx=b[edi+4] eax=[edi+ebx+5] eax-=[timerdata] jns <3
	push d[edi] d[edi]=0 correo\reenviar() free() jmp <1

;;
\usb.asm

#eeprom_w ;wr30->offset r24=value
	cmp r31,EEPROMEND>>8+1 jnc >2
	push r25
      # r25=[EECR] sbrc r25,0 jmp <1
	[EEARH]=r31 [EEARL]=r30 ++rz
	[EEDR]=r24
	r25=4 [EECR]=r25 r25=6 [EECR]=r25
	pop r25
      # ret
#eeprom_r ;wr30->offset r24=return value
	cmp r31,EEPROMEND>>8+1 jnc <1
      # r24=[EECR] sbrc r24,0 jmp <1
	[EEARH]=r31 [EEARL]=r30 ++rz r24=1 [EECR]=r24
	r24=[EEDR] ret


#desconectar sleep sleep sleep
	usb_off() [PLLCSR]=ZERO
	r20=[PRR1] r20|=80h [PRR1]=r20
	r20=[PORTD] r20&=!20h [PORTD]=r20
	ret

;estas funciones se colocan en el stack con los parametros antes de la direccion de retorno
;usan los mismos registros que las interrupciones
#setup_clock
	cli
	pop TMP1 ;sys_clk
	TMP2=80h
	[CLKPR]=TMP2 [CLKPR]=TMP1
	sti ret

