LANG OCTASM,0.1 \\RD\OCTA.ASM
{
;;
falta traducir instrucciones de 16 y r32 bits
falta rcr/sar wr/r,n
no se detecta error si se usa mas memoria de la que tiene el chip
r1=[label] #label ;no definido label
reg8+=n8   ;translated to reg8-=-n8   incorrecto,hay que traducirlo
	como reg8-=-n8 cmc  , o mejor no traducirlo
;;
define o octasm
define bo ebp+octasm
	;clear dictionary
	edi=[bo\group0] o\free_dire2() ;clear dictionary
	d[bo\group0]=0
	;clear hash table
	ecx=o\hash_size edi=[bo\tabla0] al=0 memset()
	esi=o\nm ebx=o\x86\f_ptrs o\add_keywords()
	cmp esi,o\nm_end jne o\error\dic
	esi=nm o\add_keywords()
	cmp esi,nm_end jne o\error\dic
	edx=o\b_ret*404h+0c0404h
	esi=fptrs2 lodsd o\copy_fptrs()
	d[bo\after1]=flip
	;delete sections
	ecx=60 edi=0
      # xchg edi,[bo\sections+ecx] free() ecx-=4 jns <1
	;new sections
	ecx=36 malloc() [bo\sections]=edi
	d[bo\section_c]=03020100h
	eax=0 [bo\bits]=eax ;avr8 code is in a data section
	[bo\s_nlinea]=eax [bo\s_file]=eax
	al=1 o\set_section()
	al=2 o\set_section()
	al=3 o\set_section() b[bo\bits]=8 ;virtual
	al=0 o\set_section()
	ret

define xchg_ops()  #1 xchg esi,edi xchg ecx,edx xchg bl,bh

#sreg2  dx=dh cx=ch
	cmp bl,o\sreg_t jne >1
	++ch bl=o\reg_t
      # cmp bh,o\sreg_t jne >1
	++dh bh=o\reg_t
      # ret

#sreg1 dx=dh cmp bl,o\sreg_t jne >1
       ++dh bl=o\reg_t
     # ret

#get2opm o\get2opbm() jmp >1
#get2op o\get2opb()
      # o\ajusta_ops() ah=cl sreg2()
	cmp bl,o\reg_t jne o\error\arg
#ready{
	cmp bh,o\sym_t je nr2
	cmp bh,o\undef_t je nr2
	cmp bh,o\exp_t je nr2
	cmp bh,o\ptr_t je >1
	ret
      # test b[esi+2],12h jnz nr2
	pusha esp+=8 ecx=b[esi+3]
	esi+=4 ecx+=esi o\calc_ptr_loop() edi-=12
	push edi,edi
	popa ret

   #nr2 push cx cl=11 o\new_block3() b[edi]=b_ready w[edi+1]=bx w[edi+3]=ax
	pop w[edi+5],d[edi+7] d[bo\inst_ptr]+=2 ret
	}

#ready1
	ecx+=esi push w[esi],[esi+2],[esi+6] esi+=10
	o\calc_ptr_loop() edi-=12 esi=edi
	pop bx,ax,cx
	cmp bh,o\ptr_t je >1
	test d[edi] jnz o\error\arg
	esi-=2 edi=esi fld q[esi+6] fstp t[esi+4] bh=o\dt_t
      # ret

#sort_op al=0 cmp bl,o\reg_t je >1
	al=2 xchg_ops() cmp bl,o\reg_t jne o\error\arg
      # ret

#num1   push ecx cl=2 o\x86\get_size() jc o\error\arg
	je >1
	test dh jz >1 ++dh jz >1 size()
      # pop ecx edx=dl ret

#size   edx=0 cmp [bo\ajuste],edx je o\error\num_size ret


#inst1  lodsw
#inst1_code ecx=2
      # esi=last_code [esi]=eax jmp [bo\outcode]
#inst2_code ecx=4 jmp <1

;oooooood ddddoooo
#ir     o\get1op()
#ir_c   cmp dl,1 jne o\error\arg
	sreg1() cmp bl,9 jne o\error\arg
	dl<<=4 or ax,dx jmp inst1_code

;oooooooo kkddkkkk
#irw    num1()
#irw2   cmp cl,12 jc o\error\arg
	cl&=3 cl<<=4 cmp dl,63 jna >1 size()
      # dh=dl dh<<=2 dx&=0c00fh or dl,dh or dl,cl
	ah=al al=dl jmp inst1_code

;oooooord ddddrrrr ;d=reg1 r=reg2
#irr    lodsb get2op()
#irr_code cmp ah,1 jne o\error\arg
	cmp bh,o\reg_t jne o\error\arg
	ah=al cl<<=4 al=dl or al,cl or ah,ch dh<<=1 or ah,dh jmp inst1_code

;oooooooo odddorrr
#irr2  lodsw o\get2opb() sreg2() cmp bx,909h jne o\error\arg
	xor ch,1 xor dh,1
	cmp cx,7 ja o\error\arg
	cmp dx,7 ja o\error\arg
	cl<<=4 or al,cl or al,dl jmp inst1_code

;00000010 ddddrrrr
#_muls
	get2op()
	al=ch ah=dh cmp ax,101h jne o\error\arg
	ah=2
	cmp bh,o\reg_t jne o\error\arg
	al=cl al<<=4 or al,dl jmp inst1_code

;ooookkkk ddddkkkk
;#iri    lodsb get2op()
#iri_code num1()
#iri_c1 cmp edx,255 jna >1 size()
      # test ch jz >1
	ax<<=4
	al=dl al&=0f0h or al,cl ax<<=4 dl&=0fh or al,dl jmp inst1_code
      # test edx jnz o\error\arg
      # al=24h ;xor
	bh=bl dx=cx jmp irr_code

;oooooood ddddokkk
#irbit  lodsb get2op() cmp ah,1 jne o\error\arg
	num1() cmp dl,7 jna >1 size()
      # ah=al al<<=3 ax&=0fe08h
	or ah,ch cl<<=4 or al,cl or al,dl jmp inst1_code

;oooooooo dddddkkk
#irbit2 ;sbic sbis

#iobit  lodsb o\get2opb() cmp bx,404h jne o\error\arg
	edx=[esi+4] cmp edx,7 jna >1 size()
      # ebx=[edi+4] ebx-=20h jc o\error\arg
	cmp ebx,31 ja o\error\num_size
	ah=al al=bl al<<=3 or al,dl jmp inst1_code

;ooooooKK KKKKKooo
#ijmp  ;0h->0f400h jc ,80h->0f000h jnc
	o\get1op() ah=al al=b_disp cl=2 o\new_block3() stosw ret

#ijmp2  o\get1op()
	test dl jz >1
	cmp dl,2 je >1
	cmp dl,4 je >1
      # push ax al=b_dispw cl=4 o\new_block3() stosb
	pop ax stosw al=dl stosb d[bo\inst_ptr]+=2 ret

#_cmp   get2op()
	al=14h cmp bh,o\reg_t je irr_code
	al=30h jmp iri_code

;GRP(irr,sbb,8,sub,18h)
;GRP(iri,sbci,40h,subi,50h)
#_sbb   get2op() al=8 jmp _sub1
;HLA
#_sub
	get2opm() al=97h cmp ah,2 je irw
	al=18h
 #_sub1 cmp bh,o\reg_t je irr_code
	al+=50h-18h jmp iri_code

#_add
	get2opm() al=96h cmp ah,2 je irw
	al=12 cmp bh,o\reg_t je irr_code
	num1() neg dl al=50h jmp iri_c1

#_and
	get2opm() al=20h cmp bh,o\reg_t je irr_code
	al=70h jmp iri_code

#push_pop cmp dl,2 jc ir_c jne o\error\arg
	;outputs 2*8 bit instructions
	sreg1() cmp bl,9 jne o\error\arg
	dx<<=5 or ax,dx edx=eax+10h
	test ah,2 jnz >1
	xchg ax,dx ;reverse pop
      # eax<<=16 ax=dx jmp inst2_code

#_call2
	push edi
	cmp b[esi+1],')' jne >1
	d[bo\src_ptr]+=2 jmp >2
      # o\sintax_tree() edx=eax
	esi=edi
	ecx=push_pop ax=920fh ;push
	o\inst1op_r2()
      # cl=9 o\new_block() w[bo\p_align]=100h ;el offset puede variar
	al=b_dispw stosb ax=094deh stosw al=0 stosb
	d[bo\inst_ptr]+=2
	pop esi al=0 stosb
	esi+=o\dicc\type lodsb ebx=[esi]
	cmp al,o\sym_t je >1
	cmp al,o\dd_t je >1
	cmp al,o\undef_t jne o\error\exp
	al=o\undef_t2 [esi]=edi
      # [edi-1]=al [edi]=ebx
	ret

#dec2   al=10 jmp >1
#inc2   al=3
      # ah=94h
	edi=[edi+4] ebx=[edi] edx=ebx dx>>=4 dl>>=4
	cmp dl,2 jne ir_c
	cl=dh dl=1 al&=dl xor al,97h jmp irw2

#_mov1  o\get2opbm() cmp bl,o\undef_t je o\x86\_mov_nn
	o\ajusta_ops() ah=cl sreg2() sort_op() ready()
	cmp bh,o\ptr_t je _movm
	test al jnz o\error\arg
	cmp ah,2 je _movw
	al=2ch cmp bh,o\reg_t je irr_code
	num1() al=0e0h jmp iri_c1

#_movm  push eax,ecx cmp ah,1 jne o\error\arg
	eax=[edi+2] ecx=0 xchg cl,[edi+3]
	ebx=[edi] test ah jz >1
	;int 1
	cmp al,1 jne o\error\arg
	cx=1001h cmp ah,-2 jc o\error\exp ja >1
	cx=1002h
      # test bl,0e0h jnz _lpm
	test ebx,0fffef0efh jnz o\error\arg ;solo un registro
	test bl,16 jz m_i_k
   #m_r ax=900ch bh-=13 jc o\error\arg je m_r2
	ax=8008h --bh jz m_r2
	al=0
  #m_r2 pop ebx or ah,bl
  #m_r3 pop ebx bl<<=4 or ax,bx or ax,cx jmp inst1_code
;             o=ld,st
;       100100od dddd1100  ;ld rd,x
;       100100od dddd1101  ;ld rd,x+
;       100100od dddd1110  ;ld rd,-x
;       100000od ddddi000  ;ldd rd,i ;i=z,y
;       100100od ddddi001  ;ldd rd,i+
;       100100od ddddi010  ;ldd  rd,-i
;       10k0kkod ddddikkk  ;ldd  rd,i+k


;       10010101 11001000  ;lpm
;       1001000d dddd0101  ;lpm rd,z+
;       10010101 11101000 ;spm  ;rampz:z
;       10010101 11111000 ;spm z+ ;rampz:z+

;1001000d dddd0100  ;lpm rd,z
#_lpm   xor bl,16 test ebx,0fffef0dfh jnz o\error\arg ;solo un registro
	cmp bh,15 jne o\error\arg
	ax=9004h pop ebx test bl jz m_r3
	jmp o\error\arg

#m_i_k  test cl jnz o\error\exp
	fld q[edi+4]
	fistp q[edi+4]
	eax=[edi+8] test eax jz >1 size() eax=0
      # edx=[edi+4] test ebx jz m_k
	test edx jz m_r
	ax=8008h bh-=14 jc o\error\arg je >1
	al=0
      # cmp edx,63 jna >1 size()
      # dh=dl bl=dl dh>>=1 dx&=0c07h bl&=20h
	or ah,bl or ax,dx jmp m_r2

;       100100od dddd0000 KKKKKKKK KKKKKKKK ;lds rd,K
#m_k
	cmp edx,0ffffh jna >1 size()
      # cmp edx,20h jc >1
	cmp edx,60h jc in_out
      # eax=edx eax<<=16 ah=90h
	pop ebx or ah,bl
	pop ebx bl<<=4 or ax,bx jmp inst2_code

;oooooKKd ddddKKKK   in,0b0h,out,0b8h
#in_out
	ah=0b0h dl-=20h al=dl al&=15 dl>>=4 dl+=dl ah|=dl
	pop ebx bl<<=2 or ah,bl
	pop ebx bl<<=4 or ax,bx jmp inst1_code

;00000001 ddddrrrr
#_movw
	cmp bh,o\reg_t jne >1
	ah=1 al=dl cl<<=4 or al,cl jmp inst1_code
      # push ecx cl=2 o\x86\get_size() jc o\error\arg
	pop ecx
	al=0e0h cx<<=5 cl>>=4
	pusha edx=dl iri_c1()
	popa ++cl edx=dh jmp iri_c1

#_or
	get2opm() al=28h cmp bh,o\reg_t je irr_code
	al=60h jmp iri_code

#_shl   get2opm() ax=cx ah+=ah or ah,0ch jmp >1
#_shr   get2opm() ax=9406h
      # cmp bh,4 jne o\error\arg
	cl<<=4 or ax,cx
	edx=[esi+4] cmp edx,7 jna >1 size()
      # test edx jz >2
      # inst1_code() --edx jnz <1
      # ret

; ooooooKK KKKKKooo
#b_disp1
	ecx+=esi eax=b[esi] ++esi
	push eax o\calc_n_loop()
	esi=edi-8 fld q[esi] fistp q[esi]
	pop ecx edx=[esi] eax=[bo\inst_ptr] eax+=2 edx-=eax
	movsx eax,dl cmp eax,edx je >1 size()
      # test al,1 jnz invalid_address
	eax=al eax=eax*4+0f400h ch=cl ch>>=5 cl&=7
	xor eax,ecx [esi]=eax ecx=2 jmp [bo\outcode]

#invalid_address esi="%nb invalid_address" jmp o\error\leve

#b_dispw1 {
	push [esi] ecx+=esi esi+=3 o\calc_n_loop()
	esi=edi-8 fld q[esi] fistp q[esi]
	pop cx,bx edx=[esi] eax=[bo\inst_ptr] eax+=2 edx-=eax
	movsx eax,dx cmp eax,edx jne >1
	[esi]=edx
	edx<<=3 movsx eax,dx cmp eax,edx jne >1
	cmp bl,4 ja >1
	eax>>=4 jc invalid_address
;ooookkkk kkkkkkkk  ;o=bits 4ญ7
    #l1 ax&=0fffh cl&=0f0h or ah,cl ecx=2 jmp l3
      # eax>>=1 jc invalid_address
	cmp bl,2 jne >1 size() jmp l1
;oooooooK KKKKoooK KKKKKKKK KKKKKKKK  o=bits 0ญ3,8ญ15
      # eax=[esi] eax>>=1 cmp eax,3fffffh jna >1 size()
      # ecx&=0ff0fh
	rol eax,16 bl=al ax<<=4 bl&=1 or al,bl or eax,ecx
	[esi]=eax
	ecx=4
    #l3 [esi]=eax jmp [bo\outcode]
	}

#flip {\FLIP.INC }

STRUCT org o\b_ret+1 enum 1,b_disp,b_dispw,b_ready
DAT
#fptrs2 db >2->1,>3->2,>4->3,>5->4
# ;_size
dd b_disp1,b_dispw1,ready1
# ;_code
dd b_disp1,b_dispw1,ready1
# ;ptrs_1op
dd push_pop
# ;_hla
dd _add,_and,_call2,dec2,inc2,_mov1,_or,_shl,_shr,_sub
#
#end
#last_code dd 0
;ญญญญญญญญญญญญญญญDICTIONARYญญญญญญญญญญญญญญญญญญญญญญญญญญญ
#nm {
define S() db '%nb #1' db >2->1 # db #2 #
define REGS(tp1,tp2)  S(#1,(tp1,tp2+10h*#0))
define GRP(fp)  db '%nb #1' db >2->1 # db 0c0h dd fp db #2 #
define GRP1()  db '%nb #1' db 5,0c0h dd _#1
define GRP2(fp)  db '%nb #1' db >2->1 # db 0c0h dd fp db #2,94h #
STRUCT
enum 1,_db,_dw,_float
enum 1,push_pop
DAT

REGS(o\reg_t,o\reg8,r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15)
REGS(o\sreg_t,o\reg8,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31)
REGS(o\reg_t,o\reg16,wr0,wr2,wr4,wr6,wr8,wr10,wr12,wr14,wr16,wr18,wr20,wr22,wr24,wr26,wr28,wr30)
REGS(o\reg_t,o\reg16+10h*13,rx,ry,rz)
REGS(o\sreg_t,o\reg16,cs)

GRP(ir,sar,(5,94h),not,(0,94h),neg,(1,94h),swap,(2,94h),rcr,(7,94h))
GRP(irbit,bld,0f8h,bst,0fah,sbrc,0fch,sbrs,0feh)
GRP(irbit2,sbic,99h,sbis,9bh)
GRP(iobit,cbi,98h,sbi,9ah)
GRP(ijmp,jnc,0h,jne,1,jnz,1,jns,2,jno,3,jnl,4,jnhc,5,jnt,6,jni,7)
GRP(ijmp,jc,80h,je,81h,jz,81h,js,82h,jo,83h,jl,84h,jhc,85h,jt,86h,ji,87h)
GRP(irr,adc,1ch,cmpc,4,cpse,10h,xor,24h,mov,2ch,mul,9ch)
GRP1(sbb,muls,cmp)
GRP(irr2,fmul,(8,3),fmuls,(80h,3),fmulsu,(88h,3),mulsu,(0,3))
GRP(inst1,break,(98h,95h),icall,(9,95h),ijmp,(9,94h),spm,(0e8h,95h),spmp,(0f8h,95h))
GRP(inst1,nop,(0,0),ret,(8,95h),iret,(18h,95h),sleep,(88h,95h),wdr,(0a8h,95h))
GRP2(inst1,clc,88h,clh,0d8h,cli,0f8h,cls,0a8h,cll,0c8h,clt,0e8h,clo,0b8h,clz,98h)
GRP2(inst1,stc,8,sth,58h,sti,78h,sts,28h,stl,48h,stt,68h,sto,38h,stz,18h)
GRP(ijmp2,jmp,(0cch,94h),call,(0deh,94h))
GRP(o\inst1op_r,push,(push_pop,15,92h))
GRP(o\inst1op,pop,(push_pop,15,90h))
db 0

	}
#nm_end
} OCTASM\compile
;ญญญญญญญญญญญญญญญavr8 program starts hereญญญญญญญญญญญญญญญ
code define FLASH section 0
section 8 org 0 define EEPROM section 8
virtual section 12 org SRAM_START define SRAM section 12
code
;file_sym \\RD\avr8\atmega.sym
;EEPROM file_out \\RD\avr8\atmega.e.bin
;FLASH file_out \\RD\avr8\atmega.bin

