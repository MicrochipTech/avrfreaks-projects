; *************************************************************************
;
; -------------------------------------------------------------------------
; PhoenixU1 Operating System
; Project by Yakimov Igor V.(AH0723705)  
;
; Derivation from UNIX Operating System (v1.0 for PDP-11) 
; (Original) Source Code by Ken Thompson (1971-1972)
; <Bell Laboratories (17/3/1972)>
; <Preliminary Release of UNIX Implementation Document>
;
; **************************************************************************

;MCU ATMEGA 32u4  

//Template

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:

.def    GR0H    = r17
.def    GR0L    = r16
.def    GR1H    = r19
.def    GR1L    = r18
.def    GR2H    = r21
.def    GR2L    = r20
.def    GR3H    = r23
.def    GR3L    = r22
.def    GR4H    = r25
.def    GR4L    = r24

.def    iGR0H    = r16
.def    iGR0L    = r17
.def    iGR1H    = r18
.def    iGR1L    = r19
.def    iGR2H    = r20
.def    iGR2L    = r21
.def    iGR3H    = r22
.def    iGR3L    = r23
.def    iGR4H    = r24
.def    iGR4L    = r25

.def	iXH	= r26
.def	iXL	= r27
.def	iYH	= r28
.def	iYL	= r29
.def	iZH	= r30
.def	iZL	= r31

.set	SP	= SPL
 
; Specify Device.
.include "m32U4def.inc"
.equ FCPU4 = 4000000
.equ FCPU8 = 8000000
.equ FCPU16 = 16000000
// подсчитываем во сколько 4MHZ больше(>) 50hz(20ms) ? => в 80*10^3 > 65536
//выбираем предделитель - самый максимальный - 1024
//4Mhz / 1024 = 3906 hz; 1/3906hz = 0,256ms; 20ms(50hz)/0,256ms = 78 
.equ kdel = 78 //coef. div.

.equ clistsize = 100 //100

.equ nproc = 3
.equ nfiles = 20//50-30
.equ usize = 64
.equ procdata = 256
.equ nbuf = 2
.equ ntty = 2

.equ nfb = ((240/2)+1) //absolute № first free in free strorage data
.set fsdblk = 0 //relative first free block in free strorage data (block)

.equ nfb_ee = ((48/2)+1) //absolute № first free in free strorage data
.set fsdblk_ee = 0 //relative first free block in free strorage data (block)

//priority level
.equ ipl7 = $07 
.equ ipl6 = $06
.equ ipl5 = $05
.equ ipl0 = $00

//not using
.equ ipl4 = $04

//const "not inside in system"
.equ OUTSIDE_S = -1

// core ecore
.equ core = $100
.equ ecore = core + procdata

//№ system calls
.equ n_sysrele		= 0
.equ n_sysexit		= 1
.equ n_sysfork		= 2
.equ n_sysread		= 3
.equ n_syswrite		= 4
.equ n_sysopen		= 5
.equ n_sysclose		= 6
.equ n_syswait		= 7
.equ n_syscreat		= 8

.equ n_sysexec		= 11
.equ n_syschdir		= 12

.equ n_sysstat		= 18
.equ n_sysseek		= 19

.equ n_sysmount		= 21
.equ n_sysumount	= 22

.equ n_sysgetuid	= 24

.equ n_sysquit		= 26
.equ n_sysintr		= 27


.MACRO xxx1 //garbage delete
        movw    @0l:@0h,@1l:@1h
		.db 0^2
.ENDMACRO

.MACRO xxx //garbage delete
        nop
.ENDMACRO

//FOR FLASH MEMORY
     //(flash only)
.MACRO addinode //Example, addicont inum,iflags,inlks,iuid,isize
	inode_rec @0,@1,@2,@3,@4
	loop_fwrd 0,((@4-1)/64),rsvblk_map
.set fsdblk = fsdblk+1+(@4-1)/64 ;go to next free block
	rsv_inode_map @0
.ENDMACRO

.MACRO rsvblk_map_32
	loop_fwrd 0,7,rsvblk_map
	loop_fwrd 8,15,rsvblk_map
	loop_fwrd 16,23,rsvblk_map
	loop_fwrd 24,31,rsvblk_map
.ENDMACRO

.MACRO addinode2 //Example, addicont2 inum,iflags,inlks,iuid,isize
	inode_rec_L @0,@1,@2,@3,@4
//	loop_fwrd 0,((@4-1)/64),rsvblk_map
//	loop_fwrd 0,(((@4-1)/64)/64),rsvblk_map
//.set fsdblk = fsdblk+1+((@4-1)/64)
//.org free_storage_data + (fsdblk * 32)
//.set fsdblk = fsdblk-1-((@4-1)/64)
	.MESSAGE "inode rec OK" 

	.IF (((@4-1)/64)/64 + 1) > 1
		.set fsdblk = fsdblk+1+((@4-1)/64)
		.org free_storage_data + (fsdblk * 32)
		.set fsdblk = fsdblk-1-((@4-1)/64)

		fill_indct_blk 0
		rsvblk_map_32
		rsvblk_map

		.MESSAGE "2 indirect block used" 
	.ENDIF
	.IF (((@4-1)/64)/64 + 1) > 2
		.set fsdblk = fsdblk+1+((@4-1)/64)
		.org free_storage_data + (fsdblk * 32)
		.set fsdblk = fsdblk-1-((@4-1)/64)

		fill_indct_blk 0
		rsvblk_map_32
		rsvblk_map	
		.MESSAGE "3 indirect block used" 
	.ENDIF
	.IF (((@4-1)/64)/64 + 1) > 3
		.set fsdblk = fsdblk+1+((@4-1)/64)
		.org free_storage_data + (fsdblk * 32)
		.set fsdblk = fsdblk-1-((@4-1)/64)

		fill_indct_blk 0
		rsvblk_map_32
		rsvblk_map	
		.MESSAGE "4 indirect block used" 
	.ENDIF
	.IF (((@4-1)/64)/64 + 1) > 4
		.set fsdblk = fsdblk+1+((@4-1)/64)
		.org free_storage_data + (fsdblk * 32)
		.set fsdblk = fsdblk-1-((@4-1)/64)

		fill_indct_blk 0
		rsvblk_map_32
		rsvblk_map	
		.MESSAGE "5 indirect block used" 
	.ENDIF
	.IF (((@4-1)/64)/64 + 1) > 5
		.set fsdblk = fsdblk+1+((@4-1)/64)
		.org free_storage_data + (fsdblk * 32)
		.set fsdblk = fsdblk-1-((@4-1)/64)

		fill_indct_blk 0
		rsvblk_map_32
		rsvblk_map	
		.MESSAGE "6 indirect block used" 
	.ENDIF
	.IF (((@4-1)/64)/64 + 1) > 6
		.set fsdblk = fsdblk+1+((@4-1)/64)
		.org free_storage_data + (fsdblk * 32)
		.set fsdblk = fsdblk-1-((@4-1)/64)

		fill_indct_blk 0
		rsvblk_map_32
		rsvblk_map	
		.MESSAGE "7 indirect block used" 
	.ENDIF
	.IF (((@4-1)/64)/64 + 1) > 7
		.set fsdblk = fsdblk+1+((@4-1)/64)
		.org free_storage_data + (fsdblk * 32)
		.set fsdblk = fsdblk-1-((@4-1)/64)

		fill_indct_blk 0
		rsvblk_map_32
		rsvblk_map	
		.MESSAGE "8 indirect block used" 
	.ENDIF

	.IF (((@4-1)/64) % 32) != 0 

		.set fsdblk = fsdblk+1+((@4-1)/64)
		.org free_storage_data + (fsdblk * 32)
		.set fsdblk = fsdblk-1-((@4-1)/64)

		fill_indct_blk (32-((((@4-1)/64) + 1) % 32))
		rsvblk_map_N (32-((((@4-1)/64) + 1) % 32))
		rsvblk_map	

		.MESSAGE "1 indirect block used" 
	.ENDIF

.set fsdblk = fsdblk+1+((@4-1)/64)+1+(((@4-1)/64)/32)

	rsv_inode_map @0
.ENDMACRO

     //(flash only)
.MACRO inode_rec_L ;(for L files) inode_rec inum,iflags,inlks,iuid,isize
		inode_rec_abstract @0,@1,@2,@3
		.dw @4
		loop_fwrd nfb+fsdblk+((@4-1)/64)+1,nfb+fsdblk+((@4-1)/64)+1+((@4-1)/64)/64,cfillmem_w
		.IF ((((@4-1)/64)/64)+1) <8
			loop_fwrd ((((@4-1)/64)/64)+1),7,sfillmem_w,$0000 //zeroed rest
		.ENDIF 
		.dw $0000,$0000 //creation time
		.dw $0000,$0000 //modification time
		.dw $0000       //unused
.ENDMACRO

//zeroed rest of block (and create indirect blocks, if needed)
.MACRO zrest_blk2
	zrest_blk @0
	.IF (((@0-1)/32)+1) > 8
		.overlap

		loop_fwrd 0,((@0-1)/32)/32,z_blk

		.nooverlap 
	.ENDIF
.ENDMACRO

.MACRO rsvblk_map_N //Example, zrest_blk quantity_blocks  
	.IF (@0 % 32) != 0
		.IF 32-(@0 % 32) >= 12
			loop_fwrd2 0,11,rsvblk_map
			.IF 32-(@0 % 32) >= 24
				loop_fwrd2 12,23,rsvblk_map
			.ENDIF
		.ENDIF
		.IF ((32-(@0 % 32)) % 12) != 0 
			loop_fwrd2 0,((32-(@0 % 32)) % 12)-1,rsvblk_map
		.ENDIF
	.ENDIF
.ENDMACRO

//fill indirect block
.MACRO fill_indct_blk //Example, fill_indct_blk quantity_not_used_blocks 
	.IF (@0 % 32 != 0) //|| (@0 != 0)
		.IF 32-(@0 % 32) >= 12
			loop_fwrd 0+nfb+fsdblk,11+nfb+fsdblk,cfillmem_w
			.IF 32-(@0 % 32) >= 24
				loop_fwrd 12+nfb+fsdblk,23+nfb+fsdblk,cfillmem_w
			.ENDIF
		.ENDIF
		.IF ((32-(@0 % 32)) % 12) != 0 
			loop_fwrd (12*((32-@0)/12))+nfb+fsdblk,(32-@0)-1+nfb+fsdblk,cfillmem_w
		.ENDIF
//	.ELSE
//		loop_fwrd 0+nfb+fsdblk,11+nfb+fsdblk,cfillmem_w
//		loop_fwrd 12+nfb+fsdblk,23+nfb+fsdblk,cfillmem_w
//		loop_fwrd 24+nfb+fsdblk,31+nfb+fsdblk,cfillmem_w
	.ENDIF
.ENDMACRO

//zeroed entire block
.MACRO z_blk   
	loop_fwrd 0,4,sfillmem_w,$0000
	loop_fwrd 5,9,sfillmem_w,$0000
	loop_fwrd 10,14,sfillmem_w,$0000
	loop_fwrd 15,19,sfillmem_w,$0000
	loop_fwrd 20,24,sfillmem_w,$0000
	loop_fwrd 25,29,sfillmem_w,$0000
	loop_fwrd 30,31,sfillmem_w,$0000
.ENDMACRO

     //(dual)(only word)
//zeroed rest of block
.MACRO zrest_blk //Example, zrest_blk №used_words (rest = 64 - used ) 
	.IF (@0 % 32 != 0) && (@0 != 0)
		.IF 32-(@0 % 32) >= 12
			loop_fwrd 0,11,sfillmem_w,$0000
			.IF 32-(@0 % 32) >= 24
				loop_fwrd 12,23,sfillmem_w,$0000
			.ENDIF
		.ENDIF
		.IF ((32-(@0 % 32)) % 12) != 0 
			loop_fwrd 0,((32-(@0 % 32)) % 12)-1,sfillmem_w,$0000
		.ENDIF
//	.ELSE
//		loop_fwrd 0,11,sfillmem_w,$0000
//		loop_fwrd 12,23,sfillmem_w,$0000
//		loop_fwrd 24,31,sfillmem_w,$0000
	.ENDIF
.ENDMACRO

     //(dual)(only word)
.MACRO addstr8pddng0;Example str8pddng "init1" = .db "init1",'\0','\0','\0'
	.IF strlen(@0) > 8
		.error "String long more than 8 bytes!"
	.ELIF (strlen(@0) <= 8) && (strlen(@0) != 0)
		.db @0
		.IF strlen(@0) <= 6
			loop_fwrd 0,((8-((strlen(@0)+1)&~1))/2)-1,sfillmem_w,$0000
		.ENDIF
	.ELIF strlen(@0) == 0 
		loop_fwrd 0,3,sfillmem_w,$0000
	.ENDIF
.ENDMACRO


/*.MACRO rol_1 ;Example, rol_1 position(0 thru 15);result - word //delete
.overlap
	.IF @0 > 15
		.error "Value "position" more than 15!"
	.ELSE
		.dw ~((1<<(@0+1))-1)
	.ENDIF
.nooverlap
.ENDMACRO */
     //(dual)
.MACRO ror_1 ;Example, ror_1 position(0 thru 15);result - word 
.overlap
	.IF @0 > 15
		.error "Value "position" more than 16!"
	.ELSE
		.dw ((low($FFFF>>(@0+1))<<8) | high($FFFF>>(@0+1)))
	.ENDIF
.nooverlap
.ENDMACRO
     //(dual)
.MACRO ror_1_neg ;Example, ror_1 position(0 thru 15);result - word 
.overlap
	.IF @0 > 15
		.error "Value "position" more than 16!"
	.ELSE
		.dw ~((low($FFFF>>(@0+1))<<8) | high($FFFF>>(@0+1)))
	.ENDIF
.nooverlap
.ENDMACRO

/*.MACRO rsvblk_map //delete
.org fs_map+(ffb_fs_map/16)
	rol_1 ffb_fs_map % 16
.set ffb_fs_map = ffb_fs_map+1
.ENDMACRO */

     //(flash only)
.MACRO rsvblk_map
.org fs_map+(ffb_fs_map/16)
	.overlap
	ror_1 (ffb_fs_map % 16)
	.nooverlap
.set ffb_fs_map = ffb_fs_map+1
.ENDMACRO
     //(flash only)
.MACRO rsv_inode_map
.org inode_map+((@0-41)/16)
	ror_1_neg ((@0-41) % 16)
.ENDMACRO

	//(dual)	;Example, loop val1,val2(val1 to val2),expression,{param}
				;max nested macros =15 (abs(val1-val2))
.MACRO loop_fwrd;equalent expr val1,{param};expr val+1,{param},...
	.IF @1 != @0
		loop_fwrd @0,@1-1,@2,@3,@4,@5,@6,@7,@8,@9
	.ENDIF
		@2 @1,@3,@4,@5,@6,@7,@8,@9
.ENDMACRO

	//(dual)	 ;Example, loop val1,val2(val1 to val2),expression,{param}
				 ;max nested macros =15 (abs(val1-val2))
.MACRO loop_fwrd2;equalent expr {param};expr {param},...
	.IF @1 != @0
		loop_fwrd @0,@1-1,@2,@3,@4,@5,@6,@7,@8,@9
	.ENDIF
		@2 @3,@4,@5,@6,@7,@8,@9
.ENDMACRO

/*
.MACRO loop_back ;Example loop expression paramcount(vice versa) {param} 

.ENDMACRO
*/
    //(dual)     ;Example, fillmem offset value addr_of_first_word
.MACRO fillmem_w ;Example, loop val1 val2 fillmem value addr_of_first_word 
	.org @2+@0
	.dw @1
.ENDMACRO
    //(dual)      ; Example, sfillmem offset value
.MACRO sfillmem_w ; Example, loop val1 val2 sfillmem value
	.dw @1
.ENDMACRO

    //(dual)      ; Example, cfillmem value
.MACRO cfillmem_w ; Example, loop val1 val2 cfillmem
	.dw @0
.ENDMACRO

;INODE RECORD (MACROS)
     //(flash only)
.MACRO inode_rec_abstract ;inode_rec inum,iflags,inlks,iuid
        .org free_storage_inodes+(@0-1)*(32/2) ;word position(not byte!!!)
		.dw @1 //iflags
		.db @2,@3 //inlks,iuid
.ENDMACRO
     //(flash only)
.MACRO s_inode_rec ;(special i-node) inode_rec inum,iflags,inlks,iuid
		inode_rec_abstract @0,@1,@2,@3
		.dw $0000
		.dw $0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000 //1-8 ind or con
		.dw $0000,$0000 //creation time
		.dw $0000,$0000 //modification time
		.dw $0000       //unused
.ENDMACRO
     //(flash only)
.MACRO inode_rec ;(usual i-node) inode_rec inum,iflags,inlks,iuid,isize
		inode_rec_abstract @0,@1,@2,@3
		.dw @4
		loop_fwrd nfb+fsdblk,nfb+fsdblk+((@4-1)/64),cfillmem_w
		.IF (((@4-1)/64)+1)<8
			loop_fwrd ((((@4)-1)/64)+1),7,sfillmem_w,$0000 //zeroed rest
		.ENDIF 
		.dw $0000,$0000 //creation time
		.dw $0000,$0000 //modification time
		.dw $0000       //unused
.ENDMACRO
//END FOR FLASH MEMORY

//FOR EEPROM
    //(eeprom only)(byte)  ; Example, sfillmem offset value
.MACRO sfillmem_b          ; Example, loop val1 val2 sfillmem value
	.db @1
.ENDMACRO

     //(eeprom only)(only word)
.MACRO addstr8pddng0_ee;Example str8pddng "init1" = .db "init1",'\0','\0','\0'
	.IF strlen(@0) > 8
		.error "String long more than 8 bytes!"
	.ELIF (strlen(@0) <= 8) && (strlen(@0) != 0)
		.db @0
		.IF strlen(@0) != 8
			loop_fwrd 0,(8-strlen(@0)-1),sfillmem_b,$00
		.ENDIF
	.ELIF strlen(@0) == 0 
		loop_fwrd 0,3,sfillmem_w,$0000
	.ENDIF
.ENDMACRO
     //(eeprom only)(byte)
//zeroed rest of block
.MACRO zrest_blk_ee
	.IF @0 % 2 != 0
		.db $00
	.ENDIF
	zrest_blk (((@0+1)^1)/2)
.ENDMACRO

     //(eeprom only)
.MACRO addinode_ee //Example, addicont inum,iflags,inlks,iuid,isize
	.IF (@0 % 2) != 0
	.ENDIF
	inode_rec_ee @0,@1,@2,@3,@4
	loop_fwrd 0,((@4-1)/64),rsvblk_map_ee
.set fsdblk_ee = fsdblk_ee+1+(@4-1)/64 ;go to next free block
	rsv_inode_map_ee @0
.ENDMACRO

     //(eeprom only)
.MACRO ee_map_val1
	.IF @0 == 0 
		.db $00
	.ELIF @0 == 1
		.db $00 
	.ELIF @0 == 2
		.db $07 & @1
	.ELIF @0 == 3
		.db $FF & @1
	.ELIF @0 == 4
		.db $F0 & @1
	.ELIF @0 == 5
		.db $00
	.ELSE
		.db $FF
	.ENDIF
.ENDMACRO

	//(eeprom only)
.MACRO ee_map_val
	ee_map_val1 @0,$FF
.ENDMACRO

	 //(eeprom only)
.MACRO rsvblk_map_ee_inum
.org fs_map_ee+(ffb_fs_map_ee/8)
.overlap
	ee_map_val1 (ffb_fs_map_ee/8), ($FF>>((ffb_fs_map_ee % 8)+1))
.nooverlap
.set ffb_fs_map_ee = ffb_fs_map_ee+1
.ENDMACRO

	 //(eeprom only)
.MACRO rsvblk_map_ee
.org fs_map_ee+(ffb_fs_map_ee/8)
.overlap
	ee_map_val1 (ffb_fs_map_ee/8), ($FF>>((ffb_fs_map_ee % 8)+1))
.nooverlap
.set ffb_fs_map_ee = ffb_fs_map_ee+1
.ENDMACRO

     //(eeprom only)
.MACRO rsv_inode_map_ee
.org inode_map_ee+((@0-41)/16)
	ror_1_neg ((@0-41) % 16)
.ENDMACRO

;INODE RECORD (MACROS)
     //(eeprom only)
.MACRO inode_rec_abstract_ee ;inode_rec inum,iflags,inlks,iuid
        .org free_storage_inodes_ee+(@0-41)*(64/2) ;byte position(not word!!!)
		.dw @1 //iflags
		.db @2,@3 //inlks,iuid
.ENDMACRO
     //(eeprom only)
.MACRO s_inode_rec_ee ;(special i-node) inode_rec inum,iflags,inlks,iuid
		inode_rec_abstract_ee @0,@1,@2,@3
		.dw $0000
		.dw $0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000 //1-8 ind or con
		.dw $0000,$0000 //creation time
		.dw $0000,$0000 //modification time
		.dw $0000       //unused
.ENDMACRO
     //(eeprom only)
.MACRO inode_rec_ee ;(usual i-node) inode_rec inum,iflags,inlks,iuid,isize
		inode_rec_abstract_ee @0,@1,@2,@3
		.dw @4
		loop_fwrd nfb_ee+fsdblk_ee,nfb_ee+fsdblk_ee+((@4-1)/64),cfillmem_w
		.IF (((@4-1)/64)+1)<8
			loop_fwrd ((((@4)-1)/64)+1),7,sfillmem_w,$0000 //zeroed rest
		.ENDIF 
		.dw $0000,$0000 //creation time
		.dw $0000,$0000 //modification time
		.dw $0000       //unused
.ENDMACRO

//END FOR EEPROM

.MACRO sts16io ;Store into IO memory. Example sts16io sp,GR0
	out @0l,@1l
	out @0h,@1h
.ENDMACRO

.MACRO sts8io ;Store into IO memory. Example sts8io sreg,GR0L
	out @0,@1
.ENDMACRO

.MACRO lds16io ;Load into IO memory. Example lds16io GR0,sp
	in @0l,@1l
	in @0h,@1h
.ENDMACRO

.MACRO lds8io ;Load into IO memory. Example sts8io GR0L,sreg
	in @0,@1
.ENDMACRO

;PSEUDO INSTRUCTION (MACROS)

.MACRO swap16 ;swap A,B,tmp_register
        movw    @2l:@2h,@1l:@1h ;B->tmp
		movw    @1l:@1h,@0l:@0h ;A->B
		movw    @0l:@0h,@2l:@2h ;tmp->A
.ENDMACRO

;ARITHMETIC AND LOGIC INSTRUCTIONS 

.MACRO add168 ;Example add168 GR2,GR3L
		add @0l,@1
		adc @0h,@1
		sub @0h,@1
.ENDMACRO

.MACRO add16 ;Example add16 GR2,GR3
        add @0l,@1l
		adc @0h,@1h
.ENDMACRO

.MACRO sub16 ;Example add16 GR2,GR3
        sub @0l,@1l
		sbc @0h,@1h
.ENDMACRO

.MACRO addi16 ;Example addi16 GR2,0x1234
        subi @0l,low(-@1)
		sbci @0h,high(-@1)
.ENDMACRO

.MACRO subi16 ;Example subi16 GR2,0x1234
        subi @0l,low(@1)
		sbci @0h,high(@1)
.ENDMACRO

.MACRO asr16 ;Example asr16 GR2
        asr @0h
		ror @0l
.ENDMACRO

.MACRO lsl16 ;Example lsl16 GR2
        lsl @0l
		rol @0h
.ENDMACRO

.MACRO lsr16 ;Example lsr16 GR2
        lsr @0h
		ror @0l
.ENDMACRO

.MACRO and16 ;Example and16 GR2,GR3 //not correct! correct only result
        and @0l,@1l
		and @0h,@1h
.ENDMACRO

.MACRO andi16 ;Example andi16 GR2,0x1234 //not correct! correct only result
        andi @0l,low(@1)
		andi @0h,high(@1)
.ENDMACRO

.MACRO or16 ;Example ori16 GR2,GR4 //not correct! correct only result
        or @0l,@1l
		or @0h,@1h
.ENDMACRO

.MACRO ori16 ;Example ori16 GR2,0x1234
        ori @0l,low(@1)
		ori @0h,high(@1)
.ENDMACRO

.MACRO eor16 ;Example eor16 GR2
        eor @0l,@0l
		eor @0h,@0h
.ENDMACRO

/*.MACRO neg16 ;Example neg16 GR2
		com @0h
		neg @0l
		subi @0h,high(-1)
.ENDMACRO ;not tested*/

.MACRO neg16 ;Example neg16 GR2
		com @0l
		com @0h
        subi @0l,low(-1)
		sbci @0h,high(-1)
.ENDMACRO

.MACRO com16 ;Example com16 GR2
		com @0l
		com @0h
.ENDMACRO

.MACRO tst16 ;Example tst16 GR0 ; Note:how about V flag ?
        subi @0l,0
		sbci @0h,0
.ENDMACRO

.MACRO clr16 ;Example clr16 GR0 ;
        clr @0l
		clr @0h
.ENDMACRO

.MACRO inc16 ;Example inc16 GR2
        subi @0l,-$01
		sbci @0h,-$01
.ENDMACRO

.MACRO dec16 ;Example dec16 GR2
        subi @0l,$01
		sbci @0h,$00
.ENDMACRO

;END ARITHMETIC AND LOGIC INSTRUCTIONS 

;BRANCH INSTRUCTIONS

.MACRO cp16 ;Example cp16 GR0,GR3 ;GR0(что сравнивают) - GR3(с чем сравнивают)
        cp @0l,@1l
		cpc @0h,@1h
.ENDMACRO

;END BRANCH INSTRUCTIONS

;DATA TRANSFER INSTRUCTION

//Copy register 16 bit ;Example mov16 GR0,GR3
.MACRO mov16 
        movw    @0h:@0l,@1h:@1l
.ENDMACRO


//Load Immediate 16 bit ;Example ldi16 GR0,0x1234
.MACRO ldi16
        ldi @0l,low(@1)
		ldi @0h,high(@1)
.ENDMACRO


//Load Direct 16 bit;Example lds16 GR0,0x1234
.MACRO lds16
        lds @0l,@1
		lds @0h,@1+1
.ENDMACRO


//Load Indirect 16 bit;Example ld16 GR0,X
.MACRO ld16
        ld @0l,@1+
        ld @0h,@1
		ld @0l,-@1
.ENDMACRO


//Load Indirect 16 bit (Post-Increment) ;Example ld16i GR0,X+
.MACRO ld16i 
        ld @0l,@1
        ld @0h,@1
.ENDMACRO


//Load Indirect 16 bit (Pre-Decrement) ;Example ld16d GR0,-X
.MACRO ld16d 
        ld @0h,@1
        ld @0l,@1
.ENDMACRO


//Load Indirect with displacement 16 bit;Example ldd16 GR0,Z+20
.MACRO ldd16
        ldd @0l,@1
		ldd @0h,@1+1
.ENDMACRO

//Store Indirect with displacement 16 bit;Example std16 Y+20,GR1
.MACRO std16
        std @0,@1l
		std @0+1,@1h
.ENDMACRO

//Store Direct 16 bit ;Example sts16 $FF00,GR0    
.MACRO sts16
        sts @0,@1l
		sts @0+1,@1h
.ENDMACRO


//Store Indirect 16 bit ;Example st16 X,GR0    
.MACRO st16
        st @0+,@1l
		st @0,@1h
		st -@0,@1l
.ENDMACRO


//Store Indirect 16 bit (Post-Increment) ;Example st16i X+,GR0    
.MACRO st16i
        st @0,@1l
		st @0,@1h
.ENDMACRO


//Store Indirect 16 bit (Pre-Decrement);Example st16d -X,GR0    
.MACRO st16d
        st @0,@1h
		st @0,@1l
.ENDMACRO


//Load Program Memory  16 bit (Post-Increment) ;Example lpm16i GR0,Z+    
.MACRO lpm16i
        lpm @0l,@1
		lpm @0h,@1
.ENDMACRO


//Push register on Stack 16 bit ;Example push16 GR0
.MACRO push16 
        push @0h
		push @0l
.ENDMACRO


//Pop register from Stack 16 bit ;Example pop16 GR0
.MACRO pop16 
        pop @0l
		pop @0h
.ENDMACRO

;END DATA TRANSFER INSTRUCTION
//work wrong!!!
//Skip address return(standart); Example: skipstd1(equalent skipstd) 
.MACRO skipstd1 
	lds16 Y,sp
	ld16 GR3,Y	;change in subroutine on ldd16 @0,Y+n
	addi16 GR3,$0002
	st16 Y,GR3
.ENDMACRO

//Skip address return(standart); Example: skipstd  work wrong!!!
.MACRO skipstd 
	push16 GR3
	skip0 GR3,4
	pop16 GR3
.ENDMACRO

//Skip address return; Example: skip N work wrong!!!
.MACRO skip1 
	push16 GR3
	skip0 GR3,@0
	pop16 GR3
.ENDMACRO

//Skip address return (skip0 P16, P16 - temp 16 bit reg <> Y); 
//Example skip0 GR3 N (GR3 - temp register ) work wrong!!!
.MACRO skip0 
	push16 Y
	lds16 Y,sp
	ldd16 @0,Y+@1	;change in subroutine on ldd16 @0,Y+n
	addi16 @0,$0002
	st16 Y,@0
	pop16 Y
.ENDMACRO

//Skip address return; Example: skip Reg,N 
//Изменяет адрес возврата увеличивая на 2,тоесть на следующую инструкцию.
//Параметр N - глубина стека до адреса возврата; Reg - регистр исп. для 
//данной операции(необходим один).Если,например, в стеке -> GR0 GR4 ret,
//то нужно вызвать макрос так - skip GR3,4(регистр GR3 не сохраняется).
//Пропускает только слово инструкцию
.MACRO skip 
	lds16io @0,sp
	addi16 @0,@1
	sts16io sp,@0
	pop16 @0
	inc16 i@0 //адрес подсчитывается в словах,поэтому 1,а не 2.
	push16 @0
	lds16io @0,sp
	subi16 @0,@1
	sts16io sp,@0
.ENDMACRO

;Обязательно должен быть после метки системного вызова или прерывания
;Example isPSWspl N ,where N - 0,5,6,7
.MACRO isPSWspl;emulate interrupt call - save PSW and set priority level{OK}
	cli

	rcall sPSW

	.IF @0 == ipl0
		rcall spl0
	.ELIF @0 == ipl5
		rcall spl5
	.ELIF @0 == ipl6
		rcall spl6
	.ELIF @0 == ipl7
		rcall spl7 
	.ELSE
		.error "Ilegal value N"
	.ENDIF
	
	sei //должно выполняется в spl,не требуется,только для подстраховки 
.ENDMACRO

.MACRO setpl ; Example setpl N,where N - 0,5,6,7
	cli
	.IF @0 == ipl0
		call spl0
	.ELIF @0 == ipl5
		call spl5
	.ELIF @0 == ipl6
		call spl6
	.ELIF @0 == ipl7
		call spl7 
	.ELSE
		.error "Ilegal value N"
	.ENDIF
	sei
	//Добавить nop'ы,число должно соответствовать кол-ву обработчиков.
	//Нужно для функций наподобие dskrd,чтобы не ставить там nop'ы.
.ENDMACRO

.MACRO iret
	cli

	call lPSW
	
	//sei в splx не должен выполнятся
	reti
.ENDMACRO

;END PSEUDO INSTRUCTION (MACROS)

.cseg					; CODE segment.

;Interrupt Vectors
.org 0
	;rjmp test
	rjmp init
.org OC1Aaddr
	rjmp clock
.org PCI0addr
	rjmp PC_INT0
.org ERDYaddr
	rjmp EEPROM_INT
.org URXC1addr
	rjmp ttyi
.org UTXC1addr
	rjmp ttyo

.org 100
init:

//init stack

	ldi16 GR1,ecore-1
	sts16io sp,GR1
//clear SRAM (except first 100h bytes)
	ldi16 Y,ecore
	ldi16 Z,RAMEND+1 //last byte not cleared
	call copyz
	ldi16 GR1,s_chrgt+2 //intialize clockp
	sts16 clockp,GR1

//allocate tty buffers
	ldi16 Y,buffer
	ldi16 X,tty+6
	ldi16 GR3,tty+(ntty*8)
al_tty_b_1:
	st16 X,Y
	addi16 Y,140
	addi16 X,8
	cp16 X,GR3
	brlo al_tty_b_1

// allocate disk buffers
	ldi16 GR0,bufp+nbuf+nbuf
	ldi16 X,bufp
al_dsk_b_1:
	st16i X+,Y
	mov16 GR3,Y
	addi16 GR3,8
	std16 Y+6,GR3
	ldi16 GR3,-64
	std16 Y+4,GR3
	addi16 Y,(64+8)    //wrong work expr 64+8 in avrasm2,using "(" and ")"
	cp16 X,GR0
	brlo al_dsk_b_1
	ldi16 GR3,sb0
	st16i X+,GR3
	ldi16 GR3,sb1
	st16i X+,GR3

	ldi16 Y,sb0
	ldi16 GR3,-64
	std16 Y+4,GR3
	ldi16 GR3,systm
	std16 Y+6,GR3

	ldi16 Y,sb1
	ldi16 GR3,-64
	std16 Y+4,GR3
	ldi16 GR3,mount
	std16 Y+6,GR3

//new; init prescaler and CTC mode of timer
	ldi GR0L,(1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10) //Prescaler = 1024 CTC
	sts TCCR1B,GR0L
	ldi GR0L,low(kdel)
	ldi GR0H,high(kdel)
	sts OCR1AH,GR0H  //старший идет первым из-за двойной буферезации
	sts OCR1AL,GR0L
//set devices to interrupt//
	ldi GR0L,1<<PB0  //устанавдиваем PB0 на режим вывода
	out DDRB,GR0L    //меняя направление в регистре направления DDR

	ldi GR0L,0<<PB0   //устанавливаем низкий уровень сигнала на контакте
	out PORTB,GR0L     //или ножке;ВАЖНО!!! это участок кода можно удалить

	ldi GR0L,1<<PCINT0 //выбираем ножку микросхемы,которая будет задейсв.
	sts PCMSK0,GR0L    //в pin change - (SS/PCINT)PB0(8) (ВАЖНО!!!поменять)

	ldi GR0L,1<<PCIE0 //активируем возможность прерывания по pin change
	sts PCICR,GR0L   //Будем эмулировать прерывание от FlashMemory,EEPROM(?)

	/*
	enable emulation interrupt from FlashMemory and EEPROM
	*/

	ldi GR0L,1<<OCIE1A  //включаем clocl (16 битный таймер)
	sts TIMSK1,GR0L	


	ldi GR0L,(1<<UCSZ11)|(1<<UCSZ10)
	sts UCSR1C,GR0L
	clr GR0L
	sts UBRR1H,GR0L
	ldi GR0L,12
	sts UBRR1L,GR0L
	ldi GR0L,(1<<RXCIE1)|(1<<TXCIE1)
	sts UCSR1B,GR0L

	sei					//РАЗРЕШЕНИЕ преваний!
//new; enable idle mode
	ldi GR0L,1<<SE  //вкл. возможности перехода в idle,но после вкл. прерыв,
	sts8io smcr,GR0L //чтобы вдруг не случилось зависания,на всякий случай.

;set up time out subroutines

// free all character blocks

	ldi16 GR2,clistsize
	ldi16 GR1,-1
fr_all_cblk:
	
	call put
	subi16 GR2,2
	breq fr_all_cblk_1
	brge fr_all_cblk
fr_all_cblk_1:	

// set up drum swap addresses
	ldi16 GR1,swpprocdata
	ldi16 X,p_swpa
	ldi16 GR3,p_swpa+nproc+nproc
set_swp_addr:
	st16i X+,GR1
	addi16 GR1,(procdata+usize)
	cp16 X,GR3
	brlo set_swp_addr

; free rest of drum

// make current program a user
	ldi16 GR0,41
	sts16 rootdir,GR0
	sts16 u_cdir,GR0

	ldi16 GR0,1
	sts u_uno,GR0L
	sts16 mpid,GR0
	sts16 p_pid,GR0
	sts p_stat,GR0L

; initialize inodes for special files (inodes 1 to 40.)

; initialize i-nodes r1.,...,47. and write the root device, binary, etc.,
; directories onto fixed head disk. user temporary, initialization prog.

// write init file into initparam ; ВАЖНО!!! возможно перемещение в пам.пол.
	ldi16 GR4,end_initparam<<1
	ldi16 X,initparam
	ldi16 Z,str_initparam<<1
cpy_initparam:
	lpm GR3L,Z+
	st X+,GR3L
	cp16 Z,GR4
	brlo cpy_initparam
	rjmp end_initparam
str_initparam: .db "/etc/init",0 
end_initparam: 

//set initargp
	ldi16 GR3,initparam
	sts16 initargp,GR3

// next 2 instructions not executed during cold boot.
	lds GR3L,sb0+1
	ori GR3L,$04
	sts sb0+1,GR3L
	
	rcall ppoke
exc_init_1:
	lds GR3L,sb0+1
	tst GR3L
	brne exc_init_1 //imitation of active wait
	
	lds GR3L,sysflg
	dec GR3L
	sts sysflg,GR3L
	ldi GR0L,n_sysexec
	ldi16 GR1,initparam
	ldi16 GR2,initargp
	rcall sysent
	rjmp panic

/*init:
    ldi16 GR0,-6
	ldi16 GR1,-8
	cp16 GR0,GR1
	brge bolshe_raven
	brlt menshe
	rjmp init			;Reset
	reti				;INT 0
	reti				;Pin Change
	reti				;Output compare
	reti				;Timer 1
menshe:
	rjmp init			;Timer 0
	reti				;EEPROM ready
	reti				;Analog Comparator
	reti				;ADC Complete 
bolshe_raven:
	xxx X,Y
	ldi16 GR0,0x1234
    lds16 GR0,init
    ld16 GR0,X
    ld16i GR0,X+
    ld16d GR0,-X
    ldd16 GR0,Z+20
    mov16 GR0,GR1 
    sts16 $FF00,GR0
	st16 Z,GR0
    st16i Z+,GR0
    st16d -Z,GR0
    lpm16i GR0,Z+
	push16 GR0
	pop16 GR0
	*/
test: //NOW testing routine CLOCK and IDLE,SWAP

	ldi16 GR1,ecore-1
	sts16io sp,GR1



//clear SRAM (except first 100h bytes)
	ldi16 Y,ecore
	ldi16 Z,RAMEND+1 //last byte not cleared
	call copyz
	ldi16 GR1,s_chrgt+2 //intialize clockp
	sts16 clockp,GR1

//new!!!init prescaler
	ldi GR0L,(1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10) //Prescaler = 1024 CTC
	sts TCCR1B,GR0L
	ldi GR0L,low(kdel)
	ldi GR0H,high(kdel)
	sts OCR1AH,GR0H
	sts OCR1AL,GR0L

	//set devices to interrupt
	ldi GR0L,1<<PB0  //устанавдиваем PB0 на режим вывода
	out DDRB,GR0L    //меняя направление в регистре направления DDR

	ldi GR0L,1<<PB0   //устанавливаем низкий уровень сигнала на контакте
	out PINB,GR0L     //или ножке;ВАЖНО!!! это участок кода можно удалить

	ldi GR0L,1<<PCINT0 //выбираем ножку микросхемы,которая будет задейсв.
	sts PCMSK0,GR0L    //в pin change - (SS/PCINT)PB0(8) (ВАЖНО!!!поменять)

	ldi GR0L,1<<PCIE0 //активируем возможность прерывания по pin change
	sts PCICR,GR0L   //Будем эмулировать прерывание от FlashMemory,EEPROM(?)


	ldi GR0L,1<<OCIE1A
	sts TIMSK1,GR0L	
	sei
/* eeprom macros test
.equ ccc = 0

	cbi EECR,EERIE
	lds GR0H,eereq
	subi GR0H,(1<<4)-1-(ccc<<4) 
	lds8io GR0H,sreg
	sbrc GR0H,SREG_Z        
	sbi EECR,EERIE
	nop	
*/
/*test eeprom ir 
	setpl ipl0
	setpl ipl7
	ser GR0L
	sts eereq,GR0L
	setpl ipl0
	setpl ipl7
*/
	;ldi16 X,s_idlet+2
	;rcall idle
	ldi GR0L,1<<SE
	sts8io smcr,GR0L
	clr GR0L
	inc GR0L
	sts toutt+3,GR0L
	ldi GR0L,10
	sts toutt+2,GR0L
	ldi GR0L,5
	sts pl,GR0L
	ldi16 Z,$AACC
	//ldi GR0L,OUTSIDE_S
	//sts sysflg,GR0L
	ldi GR0L,$02
	sts uquant,GR0L
	ldi16 GR0,$EFAB

	ldi GR0L,1<<PB0   //устанавливаем низкий уровень сигнала на контакте
	out PINB,GR0L     //или ножке;ВАЖНО!!! это участок кода можно удалить

	lds GR3L,sysflg
	dec GR3L
	sts sysflg,GR3L
	ldi GR0L,n_sysexec
	ldi16 X,initparam
	ldi16 Y,initargp
	rcall sysent
	nop
	nop
	sei
test00:
	sleep
	nop
	nop
	rjmp test00
/*	rjmp test
	.db 0,1,2,3,4,5,6,7,8,9
    .org 0x06+test
test:*/

//ROUTINES OF SUPPORT
sPSW: //save PSW(as PDP-11) on stack {OK}
	//положить sreg и pl на стэк
	push16 GR0
	push16 GR0 
	push16 Z    ;stack-> Z(0) GR0(2) GR0(4) addrret(6) PC+2(8)
	
	lds16io Z,sp
	ldd16 GR0,Z+6+1 
	std16 Z+4+1,GR0 //  stack-> Z(0) GR0(2) addrret(4) addrret(6) PC+2(8)
	ldd16 GR0,Z+8+1
	std16 Z+6+1,GR0 //  stack-> Z(0) GR0(2) addrret(4) PC+2(6) PC+2(8) 
	lds8io GR0L,sreg
	lds GR0H,pl	
	std16 Z+8+1,GR0 //указывает на 1-ый сохр. GR0,меняем его на sreg и pl
					//  stack-> Z GR0 addrret PC+2 PSW(sreg and pl)
	pop16 Z
	pop16 GR0
	ret

lPSW: // load PSW(as PDP-11) from stack {OK}
	push16 Z
	push16 GR0 ;stack-> GR0(0) Z(2) addrret(4) PC+2(6) PSW(8)

	lds16io Z,sp
	ldd16 GR0,Z+8+1 //load PSW
	sts pl,GR0H

	rcall splx // damage GR0L

	ldd16 GR0,Z+8+1 //load again PSW for sreg
	sts8io sreg,GR0L

	ldd16 GR0,Z+6+1 //stack-> GR0(0) Z(2) addrret(4) PC+2(6) PSW(8)
	std16 Z+8+1,GR0 //stack-> GR0(0) Z(2) addrret(4) PC+2(6) PC+2(8)

	ldd16 GR0,Z+4+1
	std16 Z+6+1,GR0 //stack-> GR0(0) Z(2) addrret(4) addrret(6) PC+2(8)

	ldd16 GR0,Z+1
	std16 Z+4+1,GR0 //stack-> GR0(0) Z(2) GR0(4) addrret(6) PC+2(8)

	pop16 GR0
	pop16 Z
	pop16 GR0
	ret
//END ROUTINE OF SUPPORT

panic:

sysent: //ok
	isPSWspl ipl0
	lds GR0H,sysflg
	inc GR0H
	sts sysflg,GR0H
	breq se_1
	rjmp panic
se_1:
	clr GR0H
	push16 GR0
	push16 GR1
	push16 GR2
	push16 GR3
	push16 GR4
	push16 X
	push16 Y
	push16 Z
	ldi16 Z,s_syst+2
	sts16 clockp,Z
	
	lds16io Z,sp
	sts16 u_sp,Z
	mov16 Y,Z
	addi16 Z,15
	sts16 u_r0,Z

	lsl GR0L
	cpi GR0L,((se_3-se_2)<<1)
	brsh badsys1
	mov16 Z,GR0
	addi16 Z,(se_2<<1)
	lpm16i GR0,Z+
	mov16 Z,GR0
	//bic	$341,20.(sp) / set priority to 0 and clear carry bit
	clr GR0L
	std Y+20,GR0L
	ldd GR0L,Y+19
	andi GR0L,$FE
	std Y+19,GR0L

	ijmp
se_2:
	.dw sysrele //0
	.dw sysexit //1
	.dw sysfork //2
	.dw sysread //3
	.dw syswrite //4
	.dw sysopen //5
	.dw sysclose //6
	.dw syswait //7
	.dw syscreat //8
	.dw $0000 //9
	.dw $0000 //10
	.dw sysexec //11
	.dw syschdir //12
	.dw $0000 //13
	.dw $0000 //14
	.dw $0000 //15
	.dw $0000 //16
	.dw $0000 //17
	.dw sysstat //18
	.dw sysseek //19
	.dw $0000 //20
	.dw sysmount //21
	.dw sysumount //22
	.dw $0000 //23
	.dw sysgetuid //24
	.dw $0000 //25
	.dw sysquit //26
	.dw sysintr //27
se_3:
badsys1:
	rjmp badsys	
error1:	
error:
	lds16 Y,u_sp

	ldd GR3L,Y+19
	ori GR3L,$01
	std Y+19,GR3L

sysret://незаконченная
	lds16 GR3,u_sp
	sts16io sp,GR3

	lds GR3L,uquant
	tst GR3L
	brne sysret_x
		
sysrele://nok-ok
	rcall tswap
sysret_x:

	pop16 Z
	pop16 Y
	pop16 X
	pop16 GR4
	pop16 GR3
	pop16 GR2
	pop16 GR1

	ldi16 GR0,s_chrgt+2
	sts16 clockp,GR0
	lds GR0L,sysflg
	dec GR0L
	sts sysflg,GR0L
	pop16 GR0   
	
	/*call isintr
	brcs intract*/

	iret

badsys:

//INPUT: None
//OUTPUT: u.r0 - pid(parent or child)
//USED: GR1,GR2,GR3,Y,Z
//ACTION:
//NOTE: По выходу из сист. вызова родитель пропускает одну инструкцию.
        //Процесс-ребенок нет.Если не получился fork,то кроме пропуска
		//родитель получает "код ошибки" через C-бит.

sysfork://ok
	//clr	r1

	clr GR1L //GR1L - номер процесса
	ldi16 Y,p_stat
fo_1: // search p.stat table for unused process number
	//inc	r1
	//tstb	p.stat-1(r1) / is process active, unused, dead
	//beq	fo_2 / it's unused so branch
	//cmp	r1,$nproc / all processes checked
	//blt	fo_1 / no, branch back
	//add	$2,18.(sp) //add 2 to pc when trap occured
	//br	error1 

	inc GR1L 
	ld GR1H,Y+
	tst GR1H //GR1H - статус процесса с номером GR1L
	breq fo_2
	cpi GR1L,nproc
	brlt fo_1
	lds16io Z,sp
	ldd16 GR3,Z+17
	inc16 iGR3       //iGR3
	std16 Z+17,GR3
	rjmp error1 
fo_2:
	//movb	u.uno,-(sp) 
	//movb	r1,u.uno
	//incb	p.stat-1(r1) 
	//mov	u.ttyp,r2 / put pointer to parent process' control tty buffer in r2
	//beq	fo_3 / branch, if no such tty assigned
	//clrb	6(r2) / clear interrupt character in tty buffer
	 
	lds GR3L,u_uno ; save u_uno in GR4L
	push GR3L
	sts u_uno,GR1L
	inc GR1H
	st -Y,GR1H
	clr GR1H
	lds16 Z,u_ttyp
	tst16 Z
	breq fo_3 
	clr GR3H     //bnok
	std Z+6,GR3H //enok
fo_3:
	//mov	$runq+4,r2
	//jsr	r0,putlu / put child process on lowest priority run queue
	//asl	r1 / multiply r1 by 2 to get index into p.pid table
	//inc	mpid / increment m.pid; get a new process name
	//mov	mpid,p.pid-2(r1) / put new process name in child process' name slot
	//movb	(sp),r2 / put parent process number in r2
	//asl	r2 / multiply by 2 to get index into below tables
	//mov	p.pid-2(r2),r2 / get process name of parent process
	//mov	r2,p.ppid-2(r1) / put parent process name in parent process slot for child
	//mov	r2,*u.r0 / put parent process name on stack at location where r0 was saved
	//mov	$sysret1,-(sp) /
	//mov	sp,u.usp / contents of sp at the time when user is swapped out
	//mov	$sstack,sp / point sp to swapping stack space
	//jsr	r0,wswap / put child process out on drum
	;jsr	r0,unpack / упаковки не будет
	//mov	u.usp,sp / restore user stack pointer
	//tst	(sp)+ / 
	//movb	(sp)+,u.uno / put parent process number in u.uno
	//mov	mpid,*u.r0 / put child process name on stack where r0  was saved
	//add	$2,18.(sp) / add 2 to pc on stack; gives parent process return
	//clr	r1

	ldi16 GR2,runq+4
	rcall putlu
	clr GR1H           //GR1 - process number of child
	mov16 Y,GR1
	lsl16 Y
	addi16 Y,(p_pid-2)
	lds16 GR3,mpid
	inc16 GR3
	sts16 mpid,GR3
	st16 Y,GR3         //mov mpid,p.pid-2(r1)
	pop GR3L           //restore old(parent) u.uno
	push GR3L
	clr GR3H           // (sp) = GR3
	mov16 Y,GR3
	lsl16 Y
	addi16 Y,(p_pid-2) //GR3 = r2
	ld16 GR3,Y         //mov p.pid-2(r2),r2 //получаем pid родителя
	mov16 Y,GR1
	lsl16 Y
	addi16 Y,(p_ppid-2)
	st16 Y,GR3         //mov r2,p.ppid-2(r1) //устанавливаем родителя       
	lds16 Y,u_r0       //возвращаем для процесса-ребенка pid родителя
	st16 Y,GR3
	ldi16 Y,sysret
	push16 iY         //iY
	lds16io Y,sp
	sts16 u_usp,Y
	ldi16 Y,sstack
	sts16io sp,Y
	rcall wswap
	lds16 Y,u_usp
	sts16io sp,Y
	pop16 Z            //tst (sp)+ //вытаскиваем sysret
	pop GR3L           
	sts u_uno,GR3L  
	lds16 GR3,mpid     //возвращаем для процесса-родителя pid ребенка
	lds16 Y,u_r0
	st16 Y,GR3         //для процесса-родителя pid ребенка 
	lds16io Z,sp
	ldd16 GR3,Z+17
	inc16 iGR3        //iGR3
	std16 Z+17,GR3
	clr16 GR1


fo_4://search u.fp list to find the files opened by the parent process

	//movb	u.fp(r1),r2 / get an open file for this process
	//beq	fo_5 / file has not been opened by parent, so branch
	//asl	r2 / multiply by 8
	//asl	r2 / to get index into fsp table
	//asl	r2
	//incb	fsp-2(r2) / increment number of processes using file,
		          // because child will now be using this file

	mov16 Y,GR1
	addi16 Y,u_fp
	ld GR2L,Y+
	tst GR2L
	breq fo_5
	clr GR2H
	lsl16 GR2
	lsl16 GR2
	lsl16 GR2
	mov16 Z,GR2
	addi16 Z,(fsp-2) //fsp[u_fp[i]+6]
	ld GR2H,Z
	inc GR2H
	st Z,GR2H

fo_5:
	//inc	r1 / get next open file
	//cmp	r1,$10. / 10. files is the maximum number which can be opened
	//blt	fo_4 / check next entry
	//br	sysret1

	inc GR1L
	cpi GR1L,10
	brlt fo_4
	rjmp sysret

sysret1:

error_9:
	rjmp error

//INPUT: namep (pointer) - GR1; argp (pointer) - GR2
                       //ВАЖНО!!! Изменены регистры X на GR1,Y на GR2
//OUTPUT: None
//USED: GR2,GR3,GR4,X,Y,Z
//ACTION:
//NOTE: 1)Особое отличие от pdp-11,что здесь не нужно считывать файл,точку входа 
        //достаточн определить по i-number'у.И все таки где-то счит. оставил.
        //2) ВАЖНО!!! Протестировать когда файл большой
		//3)Неясно что делать с областью данных.Кто должен с ними работать -
		//либо отдать на откуп самой программе,либо unix'у - потребует 
		//введение формата исп. файла, а также протокола и организации
		//данных - инициализированная,постоянная(отличается из-зи Гарвадской
		//архитектуры).Нужно глянуть на линукс или unix на гарвадской.
//TODO: 1) проверку на запускаемость,запускать 
        //можно только с FLASH (устройства).
		//2) заменитть входные регистры на GR1,GR2.Для единобразия(опц.).
sysexec://ok

	//jsr	r0,arg2 
	//jsr	r0,namei 
	//br error9
	//jsr	r0,iget 
	//bit	$40,i.flgs
	//beq	error9
	//jsr	r0,iopen
	//bit	$40,i.flgs 
	//beq	ex_1
	//tstb	u.uid / test user id
	//beq	ex_1 super user
	//movb	i.uid,u.uid 

	sts16 u_namep,GR1 //arg2
	sts16 u_off,GR2   //arg2
	mov16 GR0,GR2        //arg2
	rcall namei
	rjmp error_9
	rcall iget
	lds GR3L,i_flgs
	andi GR3L,$10 //файл запускаемый?
	breq error_9 //ошибка нет такого бита($10)
	rcall iopen
	lds16 GR3,i_flgs 
	andi GR3L,$20 //файл имеет бит user id on execution ?
	breq ex_1     //нет нету
	lds GR3L,u_uid
	tst GR3L		//тестируем пользовательский id
	breq ex_1		//суперползователь
	lds GR3L,i_uid
	sts u_uid,GR3L	//не супер,присваеваем uid'у текущего процесса uid файла

ex_1:
	;mov	(sp)+,r5 
	//mov	$1,u.quit 
	//mov	$1,u.intr 
	;mov	$rtssym,30 ;похоже,такого не будет
	;mov	$fpsym,*10 
	//mov	$sstack,sp
	;mov	r5,-(sp) ;не нужно,используем GR0
	//mov	$ecore,r5 
	//mov	$core,r4
	//mov	r4,u.base
	//mov	(sp),r2 ; будем использовать Z
	ldi16 GR3,$0001
	sts16 u_quit,GR3
	sts16 u_intr,GR3
	ldi16 GR3,sstack ;не требуется ,регистров хватает,в отличие от pdp-11 
	sts16io sp,GR3
	ldi16 X,ecore ;фактически сохранили в GR5,исп. X,т.к namep уже не требуется
	ldi16 GR4,core
	;sts16 u_base,GR4;Ненужный. u.base- указатель на память польз. участвует в чтение запускаемого файла
	mov16 Z,GR0 ;копируем GR0 (argp) в Z 

ex_2: 
	//tst	(r2)+ / argument char = "nul"
	//bne	ex_2
	//tst	-(r2)

	ld16i GR3,Z+ ;Z = r2	GR3 = temp
	tst16 GR3
	brne ex_2 //ищем конец списка аргументов ,тоесть \0 
	ld16d GR3,-Z //нашли ,но указатель уже дальше \0,возвращаем его на \0

	//Привязка рег. pdp11 к рег. atmega32u4: r3 = Y; r5 = X; r2 = Z;r4 = GR4
	
ex_3: //копируем аргументы в конец памяти ,а их адреса в начало 
	//mov	-(r2),r3 / (r3) last non zero argument ptr
	//cmp	r2,(sp) / is r2 = beginning of argument ptr list
	//blo	ex_6 / branch to 1f when all arguments are moved

	ld16d Y,-Z //берем указатели аргументов с конца
	cp16 Z,GR0 //GR0 - указатель на начало указателей аргументов
	brlo ex_6
	
ex_4://сканируем строку аргумента на завершаюший символ \0
	//tstb	(r3)+
	//bne	ex_4 / scan argument for \0 (nul)

	ld GR3L,Y+ ;Y = r3	GR3 = temp 
	tst GR3L 
	brne ex_4 

ex_5://считывание содерж-го аргументов(строк) в конец адр. прост-ва
	//movb	-(r3),-(r5) / move argument char by char starting at "ecore"
	//cmp	r3,(r2) / moved all characters in this argument
	//bhi	ex_5 / branch ex_5 if not
	//mov	r5,(r4)+ /move r5 into top of users core;r5 has pntr to nth arg
	//br	ex_3 / string

	ld GR3L,-Y //r3 = Y ;r5 = X
	st -X,GR3L
	ld16 GR3,Z //(r2)
	cp16 Y,GR3 //сравнили,теперь GR3 больше не нужен 
	breq ex_51 //wrap
	brsh ex_5  //wrap
ex_51:         //wrap
	swap16 Y,GR4,GR3
	st16i Y+,X
	swap16 Y,GR4,GR3
	rjmp ex_3
ex_6:
	//clrb	-(r5) ;поставим точно слово-нуль
	;bic	$1,r5 / делать выравнивание по границе слова не надо,не pdp-11 же
	//mov	$core,r2

	ldi16 GR3,$0000
	st16d -X,GR3
	ldi16 Z,core ;r2 и r3 нам уже не нужены
	//r3 = Y; r5 = X; r4 = GR4; r2 = Z(core);
ex_7:
	//cmp	r2,r4
	//bhis	ex_8 / branch to 1f when all pointers are moved
	//mov	(r2)+,-(r5)
	//br	ex_7

	cp16 Z,GR4 //копируем адреса аргументов из начала в конец,но перед арг-ми
	brsh ex_8
	ld16i GR3,Z+
	st16d -X,GR3
	rjmp ex_7
ex_8:
	//sub	$core,r4 / gives number of arguments *2
	//asr	r4 / divide r4 by 2 to calculate the number of args stored
	//mov	r4,-(r5) / save number of arguments ahead of the argument pointers
	//clr	-(r5) ;загружаем SREG = 0 и pl = 0   
	//mov	$u.off,u.fofp
	//clr	u.off / set offset in file to be read to zero
	//jsr	r0,mget
	//mov	$core,-(r5) нужно вычислить адрес входа в программу через i-number  
	//mov	r5,0f 
	//tst	-(r5) / decrement r5
	//mov	r5,u.r0 /
	//sub	$16.,r5 / skip 8 words
	//mov	r5,u.sp
	//jsr	r0,copyz; core; 0:0(tmp)
	//clr	u.break
	//sub	$core+40.,r5 
	//mov	r5,sp 

	subi16 GR4,core
	asr16 GR4
	st16d -X,GR4 ;можно было сохранить и 1 байт,колич. больше 256 вряд ли может
	clr16 GR3
	st16d -X,GR3 //SREG and pl
	ldi16 GR3,u_off
	sts16 u_fofp,GR3
	clr16 GR3
	sts16 u_off,GR3
	mov16 GR0,GR1
	rcall mget	 //X в данном случае вызова не изменяется(или так сделать)
	             //ВАЖНО!!! Протестировать когда файл большой
	mov16 Z,GR1 //GR1 = номер блока	
	lsl16 Z //evalution entry_point:=num_blk*32(т.к. слова)+free_storage;
	lsl16 Z
	lsl16 Z
	lsl16 Z
	lsl16 Z
	addi16 Z,free_storage
	st16d -X,iZ      //iZ

	mov16 Z,X; Z второй входной параметр copyz;copyz обнулит все включительно и рег.в стеке 
	subi16 X,$0002 //выделяем GR0 в стеке
	sts16 u_r0,X   //и сохраняем его местоположение в u_r0
	subi16 X,$000E ;пропустить не 8, а 7 слов(GR1 - GR7),особенности atmega
	dec16 X //особенности работы стека atmega.Отличается от pdp-11
	sts16 u_sp,X
	ldi16 Y,core
	rcall copyz
	clr16 GR3
	sts16 u_break,GR3 
	sts16io sp,X
ex_9:
	mov16 GR1,GR0
	rcall iclose // здесь ничего не делает.Действительно?

	rjmp sysret // через возврат начнем выполнять данный процесс

//INPUT: None
//OUTPUT: None
//USED: 
//ACTION:
//NOTE: Only EEPROM
EEPROM_INT:
	reti //del
	isPSWspl ipl6
	;rcall setisp
	push16 Y           //setisp
	push16 GR4         //setisp
	push16 GR3         //setisp
	lds16 GR3,clockp   //setisp
	push16 GR3         //setisp

	//trapt
	sbic EECR,EEPE //is EEPE(ready bit) of EECR set?
	rjmp ee_int_6  //device still active so branch
	lds GR3L,active
	mov GR3H,GR3L
	andi GR3L,$02
	breq ee_int_1_6   //GR3L <> 1 - stray interrupt,branch

	rjmp ee_int_1 //wrap
ee_int_1_6:       //wrap
	rjmp ee_int_6 //wrap
ee_int_1:         //wrap
	;rjmp retisp
	//end trapt emulation
	lds16 Y,eeap
	ldd GR3L,Y+1
	andi GR3L,$24 //read or reading?
	brne ee_int_2 //yes. branch
	lds GR3L,eecnt //no.write
	tst GR3L
	breq ee_int_2
	brlt ee_int_2
	//write next byte(request)
	dec GR3L
	sts eecnt,GR3L

	ldd16 GR4,Y+2
	lsl16 GR4      //num block * 64
	lsl16 GR4
	lsl16 GR4
	lsl16 GR4
	lsl16 GR4
	lsl16 GR4
	add168 GR4,GR3L


	ldd16 GR3,Y+6  //bus address
	mov16 Y,GR3
	lds GR3L,eecnt
	add168 Y,GR3L

	dec16 GR4
	sts16io eear,GR4
	ld GR3H,-Y
	sts8io eedr,GR3H
	sbi EECR,EEMPE
	sbi EECR,EEPE

	rjmp ee_int_6
ee_int_2:
	lds GR3L,active
	andi GR3L,~$02
	sts active,GR3L
	clr GR3L
	sts eereq,GR3L
	sts eecnt,GR3L
	lds16 Y,eeap
	ldd GR3L,Y+1
	andi GR3L,~$30 //clear 12,13 bits
	std Y+1,GR3L
	rcall poke
ee_int_6:
	;rjmp retisp
	pop16 GR3        //retisp
	sts16 clockp,GR3 //retisp
	pop16 GR3        //retisp
	pop16 GR4        //retisp
	pop16 Y          //retisp
	iret             //retisp

//INPUT: None
//OUTPUT: None
//USED: GR3 - temp ;X -pointer fmap
//ACTION:
//NOTE: Only Flash
PC_INT0:  //ok
	isPSWspl ipl6
	;rcall setisp
	push16 Y           //setisp
	push16 GR3         //setisp
	lds16 GR3,clockp   //setisp
	push16 GR3         //setisp

	//trapt(checks stray interrupt only)
	lds GR3L,active
	mov GR3H,GR3L
	andi GR3L,$01
	breq flsh_1   //GR3L <> 1 - stray interrupt,branch
	andi GR3H,~$01
	sts active,GR3H //clear bit 
	;rjmp retisp
	//end trapt emulation

	lds16 Y,fmap
	ldd GR3L,Y+1
	andi GR3L,~$30 //clear 12,13 bits
	std Y+1,GR3L
	rcall poke
flsh_1:
	;rjmp retisp
	pop16 GR3        //retisp
	sts16 clockp,GR3 //retisp
	pop16 GR3        //retisp
	pop16 Y          //retisp
	iret             //retisp

//interrupt requested device
.equ enb = 1
.equ dis = 0

//don't using GR0L in nnn_ir
.MACRO flashdisk_ir ; example flashdisk_ir enb; flashdisk_ir dis
	ldi GR0H,@0<<PCIE0
	sts PCICR,GR0H
.ENDMACRO

//don't using GR0L in nnn_ir
.MACRO timer_ir //возможно изменится
	ldi GR0H,@0<<OCIE1A
	sts TIMSK1,GR0H	
.ENDMACRO

//don't using GR0L in nnn_ir
.MACRO eeprom_ir             //with super safe test;interrupt enabled when
	cbi EECR,EERIE           //only eereq == $FF and enb else disabled
	lds GR0H,eereq
	subi GR0H,(1<<4)-1-(@0<<4) //if hi.byte(group bits) is $F then not stray 
	lds8io GR0H,sreg
	sbrc GR0H,SREG_Z        
	sbi EECR,EERIE
.ENDMACRO

//don't using GR0L in nnn_ir
.MACRO usart_ir 
	lds GR0H,UCSR1B
	.IF @0 == 1
		ori GR0H,(1<<TXCIE1)|(1<<RXCIE1)
	.ELIF @0 == 0
		andi GR0H,~((1<<TXCIE1)|(1<<RXCIE1))
	.ENDIF
	sts UCSR1B,GR0H	
.ENDMACRO

//END DEVICES

.MACRO goto ; goto arg1;
	rjmp @0
.ENDMACRO

.MACRO if ; if ipl arg1 arg2; if pl == ipl then goto arg1 else goto arg2
	cpi GR0L,@0
	breq @1
	rjmp @2
.ENDMACRO

.MACRO sipl ;set ipl; example sipl ipl6; (pl<-ipl6)
	push16 GR0
	ldi GR0L,@0
	sts pl,GR0L
.ENDMACRO

.MACRO siplgoto ;siplgoto ipl arg1 //set pl then goto agr1 
	sipl @0
	goto @1
.ENDMACRO


//spl7->dis5->dis6->dis7->splret
//spl6->dis5->dis6->enb7->splret
//spl5->dis5->enb7->enb6->splret
//spl0->enb7->enb6->enb5->splret
//splx restore previous spln

spl7:	;{OK}
	siplgoto ipl7,dis5
spl6:	;{OK}
	siplgoto ipl6,dis5
spl5:	;{OK}
	siplgoto ipl5,dis5
spl0:	;{OK}
	siplgoto ipl0,enb7

//INPUT: 
//OUTPUT: None
//USED: GR0L - temp
//ACTION:
//NOTE: Damaging register GR0L; splx used only in iret

splx:	

	lds GR0L,pl
	cpi GR0L,5
	brlt spl0
	breq spl5
	cpi GR0L,7
	brlt spl6
	brge spl7
	
splret:	;{OK}
	pop16 GR0
	ret
	
	//disable devices' interrupt request level(category) n
	//don't using GR0L in nnn_ir
dis7:	;{OK}

	timer_ir dis	

	goto splret

dis6:	;{OK}
	
	flashdisk_ir dis
//	eeprom_ir dis

	if ipl6,enb7,dis7

dis5:	;{OK}

	usart_ir dis

	if ipl5,enb7,dis6

	
	//enable devices' interrupt request level(category) n
	//don't using GR0L in nnn_ir
enb7:	;{OK}

	timer_ir enb

	if ipl6,splret,enb6

enb6:	;{OK}

	flashdisk_ir enb
//	eeprom_ir enb

	if ipl5,splret,enb5

enb5:	;{OK}
	
	usart_ir enb

	goto splret
//INPUT: None
//OUTPUT: None
//USED: X - arg of putc,wakeup; GR1 - input char; GR3L - value of cc+0
        // Y - addr. of tty buffer
//ACTION:
//NOTE:
ttyi: // console tty input interrupt routine
	//jsr     r0,setisp / save reg r1, r2, r3
	//mov     *$tkb,r1 / r1 = char in tty reader buffer
	;inc     *$tks / set the reader enable bit
	//bic     $!177,r1 / clear upper 9 bits of the character (strip off
                        // 8th bit of char)
	//cmp     r1,$'a-40 / is character upper case A,..., upper case Z.
                         // note that
	//blt     ttyi_1 / lower case a is represented by 141, upper case by
	//cmp     r1,$'z-40 / 101; and lower case z by 172, upper
                         // case Z by 132.
	//bgt     ttyi_1 / if not upper case, branch
	//add     $40,r1 / if upper case, calculate the representation of its
                      // lower case counter part

	isPSWspl ipl5

		//emulate retisp
	push16 Z           //setisp
	push16 Y           //setisp
	push16 GR3         //setisp
	push16 GR2         //setisp
	push16 GR1         //setisp
	lds16 GR1,clockp   //setisp
	push16 GR1         //setisp
	//end emulate retisp
	lds GR1L,UDR1
	andi GR1L,~$80
	cpi GR1L,'a'-32
	brlt ttyi_1
	cpi GR1L,'z'-32	
	breq ttyi_0
	brge ttyi_1
ttyi_0:
	subi GR1L,(-32)

ttyi_1:
	//cmp     r1,$175 / char = "}"? Note: may be quit char (fs)
	//beq     ttyi_3 / yes 2f
	//cmp     r1,$177 / char = "del" ?
	//beq     ttyi_3 / yes, 2f
	//jsr     r0,putc; 0 / put char in r1 on clist entry
	//br      ttyi_2
	//movb    r1,ttyoch / put char in ttyoch
	//jsr     r0,startty / load char in tty output data buffer
	//cmp     r1,$4 / r1 = "eot"
	//beq     ttyi_2 / yes, 1f
	//cmp     r1,$12 / r1 = "lf"
	//beq     ttyi_2 / yes 1f
	//cmpb    cc+0,$15. / are there less than 15 chars on the input list
	//blo     retisp / yes, return

	cpi GR1L,$7D
	breq ttyi_3
	cpi GR1L,$7F
	breq ttyi_3
	push16 X
	clr16 X
	call putc
	rjmp putc_1_ttyi
	rjmp putc_2_ttyi
putc_1_ttyi:
	pop16 X
	rjmp ttyi_2
putc_2_ttyi:
	pop16 X
	sts ttyoch,GR1L
	call startty
	cpi GR1L,$4
	breq ttyi_2
	cpi GR1L,$0A
	breq ttyi_2
	lds GR3L,cc+0
	cpi GR3L,$0F
	brlo ttyi_retisp

ttyi_2:
	//jsr     r0,wakeup; runq; 0 / no, wakeup the input process
	//br      retisp / return

	push16 X
	clr16 X
	call wakeup
	pop16 X
	rjmp ttyi_retisp
ttyi_3: // r1 = "}" or "delete" to get here
	//mov     tty+[ntty*8]-8+6,r2 / move console tty buffer address to r2
	//beq     ttyi_4 / if 0, wakeall
	//movb    r1,6(r2) /move "}" or del into "interrupt char" byte of buffer

	lds16 Y,tty+[ntty*8]-8+6
	tst16 Y
	breq ttyi_4
	std Y+6,GR1L
ttyi_4:
	//jsr     r0,wakeall / wakeup all sleeping processes
	//br      retisp / return

	rcall wakeall
	rjmp ttyi_retisp
ttyi_retisp:

	//emulate retisp
	pop16 GR1        //retisp; 
	sts16 clockp,GR1 //retisp; 
	pop16 GR1        //retisp; 
	pop16 GR2        //retisp;
	pop16 GR3        //retisp;
	pop16 Y          //retisp;
	pop16 Z          //retisp;
	iret             //retisp;
	//end emulate retisp
       
//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:       
wakeall:
	//mov     $39.,0f / flll arg2 of wakeup call wlth 39

	push16 X
	ldi XL,4 //runq+4
	ldi XH,39
wakeall_1:
	//jsr     r0,wakeup; runq+4; 0:.. / wakeup the processes in the
	//dec     0b                      / wait list; decrement arg2
	//bge     wakeall_1 / if not done, go back
	//rts     r0
	
	call wakeup
	dec XH
	brge wakeall_1
	pop16 X
	ret

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
ttyo: // console typewriter output interrupt routine
	//jsr     r0,setisp / save registers
	//jsr     r0,startty / put a char on the console tty output buffer
	//br      retisp / restore registers

	isPSWspl ipl5

		//emulate retisp
	push16 Z           //setisp
	push16 Y           //setisp
	push16 GR3         //setisp
	push16 GR2         //setisp
	push16 GR1         //setisp
	lds16 GR1,clockp   //setisp
	push16 GR1         //setisp
	//end emulate retisp

	call startty

	//emulate retisp
	pop16 GR1        //retisp; 
	sts16 clockp,GR1 //retisp; 
	pop16 GR1        //retisp; 
	pop16 GR2        //retisp;
	pop16 GR3        //retisp;
	pop16 Y          //retisp;
	pop16 Z          //retisp;
	iret             //retisp;
	//end emulate retisp

//INPUT: 
//OUTPUT: None
//USED: GR0,Z,X,Y - temp
//ACTION:
//NOTE: 

clock:
	isPSWspl ipl7
	//mov     r0,-(sp) / save r0
	//tst     *$lks / restart clock?
	//mov     $s.time+2,r0 / increment the time of day
	//inc     (r0)
	//bne     clk_1
	//inc     -(r0)

	push16 GR0
	push16 Z

	lds16 GR0,s_time+2
	inc16 GR0
	sts16 s_time+2,GR0
	brne clk_1
	lds16 GR0,s_time
	inc16 GR0
	sts16 s_time,GR0
clk_1:
	//mov     clockp,r0 / increment appropriate time category
	//inc     (r0)
	//bne     clk_2
	//inc     -(r0)

	lds16 Z,clockp
	ld16 GR0,Z
	inc16 GR0
	st16 Z,GR0
	brne clk_2
	subi16 Z,$0002
	ld16 GR0,Z
	inc16 GR0
	st16 Z,GR0
clk_2:
	//mov     $uquant,r0 / decrement user time quantum
	//decb    (r0)
	//bge     clk_3 / if less than 0
	//clrb    (r0) / make it 0

	lds GR0L,uquant
	dec GR0L
	sts uquant,GR0L
	brge clk_3
	clr GR0L
	sts uquant,GR0L
clk_3:
	//cmp     4(sp),$200 / ps greater than or equal to 200
	//bge     clk_4 / yes, check time outs
	//tstb    (r0) / no, user timed out?
	//bne     clk_7 / no
	//cmpb    sysflg,$-1 / yes, are we outside the system?
	//bne     clk_7 / no, 1f
	//mov     (sp)+,r0 / yes, put users r0 in r0
	//sys     0 / sysrele
	//rti
	lds16io Z,sp	
	ldd GR0H,Z+6+1+1 //high byte
	
	cpi GR0H,ipl4
	brge clk_4
	tst GR0L	//test uquant
	brne clk_7
	lds GR0H,sysflg
	cpi GR0H,OUTSIDE_S
	brne clk_7
	pop16 Z	
	ldi GR0L,n_sysrele
	rcall sysent
	pop16 GR0
	iret

clk_4://high priority
	//mov     $toutt,r0 / r0 points to beginning of time out table

	ldi16 Z,toutt
	clr GR0H
clk_5:
	//tstb    (r0) / is the time out?
	//beq     clk_6 / yes, 3f (get next entry)
	//decb    (r0) / no, decrement the time
	//bne     clk_6 / isit zero now?
	//incb    (r0) / yes, increment the time

	ld GR0L,Z
	tst GR0L
	breq clk_6
	dec GR0L
	brne clk_6
	inc GR0L
clk_6:
	//inc     r0 / next entry
	//cmp     r0,$touts / end of toutt table?
	//blo     clk_5 / no, check this entry
	//mov     (sp)+,r0 / yes, restore r0
	//rti / return from interrupt


	inc GR0H
	st Z+,GR0L
	cpi GR0H,16 //16 elements toutt
	brlo clk_5
	pop16 Z
	pop16 GR0
	iret
clk_7: //dec values of table and calling routines//no high priority
	//mov     (sp)+,r0 / restore r0
	//mov     $240,*$ps / set processor priority to 5
	//jsr     r0,setisp / save registers
	//mov     $touts-toutt-1,r0 /set up r0 as index to dec thru the table

	pop16 Z
	pop16 GR0
	setpl ipl5
	;rcall setisp /*save X,Z,GR0,clockp*/
	//emulate retisp
	push16 Z           //setisp
	push16 Y           //setisp
	push16 X           //setisp
	push16 GR3         //setisp
	push16 GR2         //setisp
	push16 GR1         //setisp
	lds16 GR1,clockp   //setisp
	push16 GR1         //setisp
	//end emulate retisp

	ldi16 X,toutt
	clr GR1H
clk_8:
	//tstb    toutt(r0) / is the time out for this entry
	//beq     clk_9 / yes
	//decb    toutt(r0) / no, decrement the time
	//bne     clk_9 / is the time 0, now
	;asl     r0 / yes, 2 x r0 to get word index for tout entry
	//jsr     r0,*touts(r0) / go to appropriate routine specified in this
	;asr     r0 / touts entry; set r0 back to toutt index

	ld GR1L,X
	tst GR1L
	breq clk_9
	dec GR1L
	st X+,GR1L  //don't care about saving ;X is not changed after call routine
	brne clk_9
	push GR1H //save counter
	ldi16 Z,touts 
	add168 Z,GR1H
	lsl16 Z
	lpm16i Y,Z+
	mov16 Z,Y
	icall
	pop GR1H //restore counter

clk_9:
	//dec     r0 / set up r0 for next entry
	//bge     clk_8 / finished? , no, go back
	//br      retisp / yes, restore registers and do a rti

	inc GR1H
	cpi GR1H,16
	brlo clk_8	
	;rjmp retisp
	//emulate retisp
	pop16 GR1        //retisp; restore
	sts16 clockp,GR1 //retisp; clockp
	pop16 GR1        //retisp; restore GR1
	pop16 GR2        //retisp;
	pop16 GR3        //retisp;
	pop16 X          //retisp;
	pop16 Y          //retisp;
	pop16 Z          //retisp;
	iret             //retisp;
	//end emulate retisp

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE: Only save clockp and change clockp on s.syst+2;save GR0
        //Возможно сохранение некоторых регистров изменится.Существование
		//подпрограммы вообще под сомнением.
setisp:
	push16 Z
	push16 X
	push16 GR0
	lds16 GR0,clockp
	push16 GR0

//INPUT: 
//OUTPUT: GR0,X,Z
//USED: GR0
//ACTION:
//NOTE: restore clockp and iret
retisp:
	pop16 GR0        //restore
	sts16 clockp,GR0 //clockp
	pop16 GR0        //restore GR0(формально,фактически в iret)
	pop16 X
	pop16 Z
	iret

//INPUT: 
//OUTPUT: GR0,X,Z
//USED: GR0
//ACTION:
//NOTE: 
touts_test:
	push GR0L
	pop GR0L
	ret


//INPUT: GR1L - process number ;GR2 - runq
//OUTPUT: None
//USED: GR3H,Z,Y
//ACTION: 
putlu:
	//tstb    (r2)+ / is queue empty?
	//beq     pu_1 / yes, branch
	//movb    (r2),r3 / no, save the "last user" process number in r3
	//movb    r1,p.link-1(r3) / put pointer to user on "last users" link
	//br      pu_2 /
	mov16 Z,GR2
	ldd16 GR3,Z+0
	tst GR3L
	breq pu_1
	ldi16 Y,p_link-1
	add168 Y,GR3H
	st Y,GR1L
	rjmp pu_2
pu_1:
	//movb    r1,-1(r2)
	st Z,GR1L 
pu_2:
	//movb    r1,(r2) 
	//dec     r2 / restore r2
	//rts     r0
	std Z+1,GR1L
	ret

//INPUT: X - time category(категория времени)  
//OUTPUT: None
//USED: GR3 - temp
//ACTION:
//NOTE: Настроить микроконтроллер в инициализации Unix на режим idle 
idle://ok
	//mov	*$ps,-(sp) / save ps on stack
	//clr	*$ps / clear ps
	//mov	clockp,-(sp) / save clockp on stack
	//mov	(r0)+,clockp / arg to idle in clockp
	//1 / wait for interrupt
	//mov      (sp)+,clockp / restore clockp, ps
	//mov	(sp)+,*$ps
	//rts	r0
	isPSWspl ipl0
	lds16 GR3,clockp
	push16 GR3
	sts16 clockp,X
	sleep	
	pop16 GR3
	sts16 clockp,GR3
	iret


//INPUT: u_uno
//OUTPUT: None
//USED: GR2,GR3 - temp,Y - адрес выгрузки,Z - адрес данных процесса или user
//ACTION:
//NOTE: ВАЖНО!!! emt,ilgins
wswap://ok
	setpl ipl7
	ldi16 Y,p_swpa-2
	lds GR3L,u_uno
	lsl GR3L
	add168 Y,GR3L
	ld16 GR3,Y
	mov16 Y,GR3   //получаем адрес выгрузки для процесса u_uno

	ldi16 GR2,core+procdata
	ldi16 Z,core
ws_1://выгружаем данные процесса
	ld GR3L,Z+
	st Y+,GR3L
	cp16 Z,GR2
	brlo ws_1

	ldi16 GR2,euser
	ldi16 Z,user
ws_2://выгружаем структуру user
	ld GR3L,Z+
	st Y+,GR3L
	cp16 Z,GR2
	brlo ws_2

	setpl ipl0
	ret

//INPUT: GR1 - process number
//OUTPUT: None
//USED: GR2,GR3 - temp,Y - адрес загрузки,Z - адрес выгруженных
        //данных процесса или user
//ACTION:
//NOTE:
rswap://ok
	setpl ipl7
	clr GR1H         //для надежности
	mov16 Y,GR1
	lsl16 Y
	addi16 Y,(p_swpa-2)  
	ld16 GR3,Y
	mov16 Y,GR3   //получаем адрес выгрузки для процесса u_uno

	ldi16 GR2,core+procdata
	ldi16 Z,core
rs_1:
	ld GR3L,Y+
	st Z+,GR3L
	cp16 Z,GR2
	brlo rs_1

	ldi16 GR2,euser
	ldi16 Z,user
rs_2:
	ld GR3L,Y+
	st Z+,GR3L
	cp16 Z,GR2
	brlo rs_2

	setpl ipl0
	ret

//INPUT: u.uno
//OUTPUT: None
//USED: GR1 - process number; GR2 - address process query(3 variants)
//ACTION:
//NOTE:
tswap:
	//movb    u.uno,r1 / move users process number to r1
	//mov     $runq+4,r2 / move lowest priority queue address to r2
	//jsr     r0,putlu / create link from last user on Q to u.uno's user

	lds GR1L,u_uno
	clr GR1H
	ldi16 GR2,runq+4
	rcall putlu

//INPUT: None
//OUTPUT: None
//USED: GR1,GR2,GR3,Y,Z, X - arg of idle
//ACTION:
//NOTE:
swap_:
	//mov	$300,*$ps 
	//mov	$runq,r2 
	setpl ipl6         //Поиск первой непустой очереди процессов из трех
	ldi16 Z,runq
	ldi16 GR2,runq+6
sw_1:                  //начиная с самой приоритетной
	//tst     (r2)+ / are there any processes to run in this Q entry
	//bne     sw_2 / yes, process 1f
	//cmp     r2,$runq+6 / if zero compare address to end of table
	//bne     sw_1 / if not at end, go back
	//jsr     r0,idle; s.idlet+2 
	//br      swap_
	ld16i GR1,Z+
	tst16 GR1
	brne sw_2
	cp16 Z,GR2
	brne sw_1
	push16 X
	ldi16 X,s_idlet+2
	rcall idle
	pop16 X
	rjmp swap_
sw_2:	
	//tst     -(r2) / restore pointer to right Q entry
	//mov     r2,u.pri / set present user to this run queue
	//movb    (r2)+,r1 / move 1st process in queue to r1
	//cmpb    r1,(r2)+ / is there only 1 process in this Q to be run
	//beq     sw_3 / yes
	//tst     -(r2) / no, pt r2 back to this Q entry
	//movb    p.link-1(r1),(r2) 
	//br      sw_4
	subi16 Z,$0002
	sts16 u_pri,Z
	ld16i GR1,Z+ 
	cp GR1L,GR1H
	breq sw_3
	subi16 Z,$0002
	ldi16 Y,p_link-1
	add168 Y,GR1L
	ld GR1H,Y
	st Z,GR1H     //установка предстарого процесса
	rjmp sw_4
sw_3:
	//clr     -(r2)
	clr16 GR3
	st16d -Z,GR3 
sw_4:
	//clr     *$ps / clear processor status
	//cmpb    r1,u.uno / is this process the same as the process in core?
	//beq     sw_6 / yes, don't have to swap
	//mov     r0,-(sp) 
	//mov     sp,u.usp / save stack pointer
	//mov     $sstack,sp / move swap stack pointer to the stack pointer
	;mov     r1,-(sp) / GR1 не задействуется в wswap
	//tstb    u.uno / is the process # = 0
	//beq     sw_5 / yes, kill process by overwriting
	//jsr     r0,wswap / write out core to disk
	setpl ipl0
	lds GR1H,u_uno
	cp GR1L,GR1H     //GR1L = fp(first process)
	breq sw_6
	lds16io GR3,sp
	sts16 u_usp,GR3
	ldi16 GR3,sstack
	sts16io sp,GR3
	clr GR1H
	push16 GR1
	lds GR1H,u_uno
	tst GR1H        //GR1H = u.uno
	breq sw_5
	rcall wswap
sw_5:
	//mov     (sp)+,r1 / restore r1 to new process number
	//jsr     r0,rswap / read new process into core
	;jsr     r0,unpack /мы ничего не упаковываем в wswap 
	//mov     u.usp,sp 
	;mov     (sp)+,r0 /в стеке лежит адрес возврата в отличие pdp-11
	pop16 GR1 //восстанавливаем GR1; самый старый процесс или текущий,если
	rcall rswap //нет других
	lds16 GR3,u_usp
	sts16io sp,GR3
sw_6:
	//movb    $30.,uquant / initialize process time quantum
	//rts     r0 / return
	ldi GR3L,30
	sts uquant,GR3L
	ret


touts: .dw startty,$0000,$0000,touts_test,$0000,$0000,$0000,$0000 //ВАЖНО!!!
touts2:.dw $0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000 //заполнить


//INPUT: GR1(r1) - i-number(negative or positive)
//OUTPUT: None
//USED:  GR3, Z - temp; X- arg. of access
//ACTION:
//NOTE: //ВАЖНО!!!незакончена. Заполнить таблицу!
iopen: //ok
	//tst    r1 / write or read access?
	//blt    iop_2 / write, go to 2f
	//jsr    r0,access; 2 / get inode into core with read access
	//cmp    r1,$40. / is it a special file
	//bgt    iop_4 / no. 3f
	//mov    r1,-(sp) / yes, figure out
	//asl    r1
	//jmp    *1f-2(r1) /iop_1-2+(Z)/ which one and transfer to it

	tst16 GR1
	brlt iop_2
	ldi16 X,$0002
	rcall access
	ldi16 GR3,40
	cp16 GR1,GR3
	breq iop_1_0
	brge iop_4
iop_1_0: 
	push16 GR1
	lsl16 GR1
	addi16 GR1,((iop_1-1)<<1)
	mov16 Z,GR1
	lpm16i GR1,Z+
	mov16 Z,GR1
	ijmp
iop_1:
	.dw otty //ВАЖНО!!! заполнить таблицу!
	//.dw touts_test //del
iop_2:
	//neg   r1 / make inode number positive
	//jsr    r0,access; 1 / get inode in 0 core
	//bit    $40000,i.flgs / is it a directory?
	//bne    error_x / yes, transfer (error)
	//cmp    r1,$40. / no, is it a special file?
	//bgt    iop_4 / no, return
	//mov    r1,-(sp) / yes
	//asl    r1                     / figure out which special file
	//jmp    *1f-2(r1) / iop_3-1+(Z)/ it is and transfer

	neg16 GR1 
	ldi16 X,$0001
	rcall access
	lds16 GR3,i_flgs
	andi GR3H,$40
	brne error_x
	ldi16 GR3,40
	cp16 GR1,GR3
	breq iop_2_0
	brge iop_4
iop_2_0:
	push16 GR1
	lsl16 GR1
	addi16 GR1,((iop_3-1)<<1)
	mov16 Z,GR1
	lpm16i GR1,Z+
	mov16 Z,GR1
	ijmp
iop_3:
	.dw otty 
	//.dw touts_test //del
iop_4:
	ret

error_x: //2:
	rjmp error
otty://ok // open console tty for reading or writing
	//mov    $100,*$tks / set interrupt enable bit 
	//mov    $100,*$tps / set interrupt enable bit 
	//mov    tty+[ntty*8]-8+6,r5 / r5 pnt. to the header of the c. tty buf
	//incb   (r5) / inc. the count of processes that opened the console tty
	//tst    u.ttyp / is there a process control tty (i.e., has a tty
                      // buffer header
	//bne    sret / address been loaded into u.ttyp yet)?  yes, branch
	//mov    r5,u.ttyp / no, make the console tty the process control tty
	//br     sret / ?
	lds GR3L,UCSR1B
	ori GR3L,(1<<TXEN1)|(1<<RXEN1)
	sts UCSR1B,GR3L	
	lds16 Y,tty+[ntty*8]-8+6
	ld GR3L,Y
	inc GR3L
	st Y,GR3L
	lds16 GR3,u_ttyp
	tst16 GR3
	brne ret2
	sts16 u_ttyp,Y
	rjmp ret1

ret2:
	rjmp ret1

//INPUT: GR1(r1) - i-number(negative or positive)
//OUTPUT: None
//USED:  GR3, Z - temp;
//ACTION:
//NOTE: //ВАЖНО!!! заполнить таблицу!
iclose: //ok
	//tst    r1 / test i-number
	//blt    icl_2 / if neg., branch
	//cmp    r1,$40. / is it a special file
	//bgt    icl_4 / no, return
	//mov    r1,-(sp) / yes, save r1 on stack
	//asl    r1
	//jmp    *1f-2(r1) /icl_1-2+(Z)/ compute jump address and transfer

	tst16 GR1
	brlt icl_2
	ldi16 GR3,40
	cp16 GR1,GR3
	breq icl_1_0
	brge icl_4
icl_1_0: 
	push16 GR1
	lsl16 GR1
	addi16 GR1,((icl_1-1)<<1)
	mov16 Z,GR1
	lpm16i GR1,Z+
	mov16 Z,GR1
	ijmp
icl_1:
	.dw ctty //ВАЖНО!!! заполнить таблицу!

icl_2:
	//neg    r1 / make it positive
	//cmp    r1,$40. / is it a special file
	//bgt    icl_4 / no. return
	//mov    r1,-(sp)
	//asl    r1 / yes. compute jump address and transfer
	//jmp    *1f-2(r1) /icl_3-1+(Z)/

	neg16 GR1 
	ldi16 GR3,40
	cp16 GR1,GR3
	breq icl_2_0
	brge icl_4
icl_2_0:
	push16 GR1
	lsl16 GR1
	addi16 GR1,((icl_3-1)<<1)
	mov16 Z,GR1
	lpm16i GR1,Z+
	mov16 Z,GR1
	ijmp
icl_3:
	.dw ctty //есть некоторые отличия от варианта чтения.Заполнить таблицу!
icl_4:
	ret

ctty:// close console tty
	//mov    tty+[ntty*8]-8+6,r5 / point r5 to the console tty buffer
	//decb   (r5) / dec number of processes using console tty
	//br     sret / return via sret
	ldi16 Y,tty+[ntty*8]-8+6
	ld GR3L,Y
	dec GR3L
	st Y,GR3L
	rjmp ret1

//INPUT: GR1(r1) - block number (номер блока)
//OUTPUT: Y(r5) - record of I/O query (запись очереди в/в)
//USED: Z(r2) - начало очереди;GR3 - temp;GR4 - temp(vacant buf); X - save
//ACTION:
//NOTE: Первый адрес возврата,когда уже работает с выбранным блоком
bufaloc://ok
	;mov    r2,-(sp) / save r2 on stack
	//mov    $340,*$ps / set processor priority to 7 
	setpl ipl7

ba_1:
	//clr    -(sp) / vacant buffer
	//mov    $bufp,r2
	clr16 GR4
	ldi16 Z,bufp 
ba_2:
	//mov    (r2)+,r5
	//bit    $173000,(r5) / lock+keep+active+outstanding
	//bne    ba_3 / branch when any of bits 9,10,12,13,14,15 are set
	//mov    r2,(sp) 
	ld16i	Y,Z+
	ld16 GR3,Y
	andi GR3H,$F6
	brne ba_3
	mov16 GR4,Z
ba_3:
	//cmpb   (r5),cdev 
	//bne    ba_4
	//cmp    2(r5),r1 
	//bne    ba_4
	//tst    (sp)+ / bump stack pointer
	//br     ba_6 / use this buffer
	lds GR3H,cdev
	cp GR3L,GR3H
	brne ba_4
	ldd16 GR3,Y+2
	cp16 GR3,GR1
	brne ba_4
	rjmp ba_6

ba_4:
	//cmp    r2,$bufp+nbuf+nbuf
	//blo    ba_2 / (all buffers not checked)
	//mov    (sp)+,r2 
	//bne    ba_5 / if a free buffer is found branch to 2f
	//jsr    r0,idle; s.wait+2 / idle if no free buffers
	//br     ba_1
	ldi16 GR3,bufp+nbuf+nbuf
	cp16 Z,GR3
	brlo ba_2
	mov16 Z,GR4
	tst16 Z
	brne ba_5
	push16 X
	ldi16 X,s_wait+2
	rcall idle
	pop16 X
	rjmp ba_1
ba_5: //ok
	//tst    (r0)+ / skip if warmed over buffer
	skip GR3,0	;address return on stack +0
ba_6: 
	//mov    -(r2),r5 / put pointer to word 1 of I/O queue entry in r5
	//movb   cdev,(r5) / put current device number in I/O queue entry
	//mov    r1,2(r5) / move block number into word 2 of I/O queue entry
	ld16d Y,-Z
	lds GR3L,cdev
	st Y,GR3L
	std16 Y+2,GR1
ba_7:
	//cmp     r2,$bufp / bump all entrys in bufp and put latest assigned
	//blos    ba_8 / buffer on the top (this makes if the lowest priority)
	//mov     -(r2),2(r2) / job for a particular device
	//br      ba_7
	ldi16 GR3,bufp
	cp16 Z,GR3
	breq ba_8	; blos ba_8 not correct
	brlo ba_8
	ld16d GR3,-Z
	std16 Z+2,GR3
	rjmp ba_7
ba_8:
	//mov     r5,(r2)
	;mov     (sp)+,r2 / restore r2
	//rts     r0
	st16 Z,Y
	ret

//INPUT: 
//OUTPUT: None
//USED: GR3, Z - temp
//ACTION:
//NOTE: ВАЖНО!!!Неправерено - нет пока записи на устройства
dskwr: //nok
	//bic	$100000,*bufp / clear bit 15 of I/O entry at bottom of queue
	lds16 Z,bufp
	ld16 GR3,Z
	andi GR3H,$7F
	st16 Z,GR3

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE: 
ppoke:
	setpl ipl7
	rcall poke
	setpl ipl0
	ret

//INPUT: None
//OUTPUT: None
//USED: X(r2) - adr. elem. bufp; Y(r1) - adr. record I/O; GR3,GR4 -temp 
        //Z - deverr
//ACTION:
//NOTE: 
poke: //незаконченая
	//mov     r1,-(sp)
	//mov     r2,-(sp)
	//mov     r3,-(sp)
	//mov     $bufp+nbuf+nbuf+6,r2 / r2 points to hi. priority I/O q.pointer
	push16 GR3
	push16 GR4
	push16 X
	push16 Y
	push16 Z
	ldi16 X,bufp+nbuf+nbuf+4 //+4 - sb0 and sb1 except swp
pke_1:
	//mov     -(r2),r1 / r1 points to an I/O queue entry
	//bit     $3000,(r1) / test bits 9 and 10 of word 1 of I/O queue entry
	//beq     pke_3 / branch to 2f if both are clear
	//bit     $130000,(r1) / test bits 12, 13, and 15
	//bne     pke_3 / branch if any are set
	//movb    (r1),r3 / get device id
	//tstb    deverr(r3) / test for errors on this device
	//beq     pke_2 / branch if no errors
	//mov     $-1,2(r1) / destroy associativity
	//clrb    1(r1) / do not do I/O
	//br      pke_3
	ld16d Y,-X
	ld16 GR3,Y
	andi GR3H,$06 //test bits 9 and 10
	ld16 GR3,Y    //GR3H - damaged; restore GR3
	breq pke_wb_3
	andi GR3H,$B0 //test bits 12, 13, and 15
	ld16 GR3,Y    //GR3H - damaged; restore GR3
	brne pke_wb_3
	ldi16 Z,deverr
	add168 Z,GR3L
	ld GR3L,Z
	tst GR3L
	ld16 GR3,Y
	breq pke_2
	ldi16 GR3,$FFFF
	std16 Y+2,GR3
	clr GR3L
	std Y+1,GR3L
	rjmp pke_3

	rjmp pke_we_3 //wrap relative jump
pke_wb_3:         //wb - wrap begin
	rjmp pke_3   //wrap relative jump
pke_we_3:          //we -wrap end

pke_2:
	//cmpb    r3,$1 / device id = 1; device is disk
	;blt     prf / device id = 0; device is drum
	;bgt     ptc / device id greater than or equal to 1; device is dec tape
	
	cpi GR3L,$01
	brlt pke_2_flash_m //device id = 0;device is flash memory
	breq eep_m
	;brge sd_m
	rjmp pke_2_1 //wrap
pke_2_flash_m:   //wrap
	rjmp flash_m //wrap
pke_2_1:         //wrap

eep_m:
;prk: //disk
	//bit     $2,active / test disk busy bit
	//bne     2f / branch if bit is set
	//bis     $2,active / set disk busy bit
	//mov     r1,rkap / rkap points to current I/O queue entry for disk
	//mov     2(r1),mq / put physical block number in mq
	//mov     $12.,div / divide physical block number by 12.
	//mov     $rkda+2,r3 /
	//mov     ac,-(sp) / put remainder from divide on stack;gives sector num
	//mov     $4,lsh / shift quotient 4 bits 
	//bis     mq,(sp) / or mq with sector; gives total disk address
	//br      3f
	lds GR3L,active
	andi GR3L,$02
	brne eep_m_0_pke_3

	rjmp eep_m_0 //wrap
eep_m_0_pke_3:   //wrap
	rjmp pke_3   //wrap
eep_m_0:         //wrap

	lds GR3L,active
	ori GR3L,$02
	sts active,GR3L
	ser GR3L
	sts eereq,GR3L
	sbic EECR,EEPE
	rjmp pke_3
	ldi GR3L,64
	sts eecnt,GR3L
	sts16 eeap,Y
	ldd16 GR4,Y+2
	tst16 GR4
	breq eep_m_0_0
	subi16 GR4,$0014
eep_m_0_0:
	lsl16 GR4      //num block * 64
	lsl16 GR4
	lsl16 GR4
	lsl16 GR4
	lsl16 GR4
	lsl16 GR4
	ldd16 Z,Y+6  //bus address
	andi GR3H,$04 //is read operation?
	breq eep_m_6 //no,branch
	add168 Z,GR3L
	add168 GR4,GR3L
eep_m_1://read operation
    sbi EECR,EERIE //del
	dec16 GR4
    sts16io eear,GR4
	sbi EECR,EERE
	lds8io GR3H,EEDR
	st -Z,GR3H
	dec GR3L
	brne eep_m_1 //if >0 then reading
	clr GR3L
	sts eecnt,GR3L

	
	std Y+1,GR3L //temp bdel
	lds GR3L,active
	andi GR3L,~$02
	sts active,GR3L //edel
	rjmp seta
eep_m_6://write operation
	dec16 GR4
	sts16io eear,GR4
	ld GR3H,-Z
	sts8io eedr,GR3H
	sbi EECR,EEMPE
	sbi EECR,EEPE
	rjmp seta
flash_m:
;prf: / drum
	//bit     $1,active / test drum busy bit
	//bne     pke_3 / branch if bit is set
	//bis     $1,active / set drum busy bit
	//mov     r1,rfap / rfap points to current I/O queue entry for drum
	//mov     $dae+2,r3
	//clr     -(sp)
	//movb    2(r1),1(sp) /move low byte of physical block number into stack
	//clr     -(sp) / word
	//movb    3(r1),(sp) /move high byte of physical block number into stack
	//mov     (sp)+,-(r3) / load dae with high byte of physical block number
;3:
	//mov     (sp)+,-(r3) / load rkda register; load dar register
	//mov     6(r1),-(r3) / load bus address register
	//mov     4(r1),-(r3) / load word count register
	//mov     $103,-(sp) / 103 indicates write operation when loaded in csr
	//bit     $2000,(r1) / if bit 10 of word 1 of I/O queue entry is a one
	//beq     3f / then read operation is indicated
	//mov     $105,(sp) / 105 indicates read operation
;3:
	//mov     (sp)+,-(r3) / load csr with interrupt enabled, command, go
	//br      seta

	lds GR3L,active
	andi GR3L,$01
	brne pke_3
	lds GR3L,active
	ori GR3L,$01
	sts active,GR3L
	sts16 fmap,Y

	//transfer data (only read)
//	andi GR3H,$04//if bit 10 is not set//запись через буф-ию может приводить
//	breq seta     //skip operation read //к некорр. работе.В любом счит-ет.
	mov16 GR4,Y  //save Y
	ldd16 Z,Y+2
	lsl16 Z      //num block * 64
	lsl16 Z
	lsl16 Z
	lsl16 Z
	lsl16 Z
	lsl16 Z
	addi16 Z,(free_storage<<1)
	ldd16 GR3,Y+6  //bus address
	mov16 Y,GR3
	ldi GR3L,63  //read byte num. 63 .. 0 
flash_m_1:
    lpm GR3H,Z+
	st Y+,GR3H
	dec GR3L
	brpl flash_m_1 //if >=0 then reading
	mov16 Y,GR4
	//call interrupt PCINT
	ldi GR3L,1<<PB0
	out PINB,GR3L  //change bit of port

	rjmp seta
;ptc: / tape I/O
	//bit     $4,active
	//bne     2f
	//mov     tccm,r3
	//swab    r3
	//bic     $!7,r3
	//add     $2,r3
	//cmpb    r3,(r1)
	//beq     3f
	//movb    $1,tccm / stop transport if not same unit
;3:
	//bis     $4,active
	//mov     r1,tcap
	//mov     $20.,tcerrc
	//mov     $tape1,tcstate
	//movb    (r1),r3 / device
	//sub     $2,r3 / now unit
	//swab    r3
	//bis     $103,r3 / now rbn,for,unit,ie
	//mov     r3,tccm
seta: // I/O queue bookkeeping; set read/write waiting bits.
	//mov     (r1),r3 / move word 1 of I/O queue entry into r3
	//bic     $!3000,r3 / clear all bits except 9 and 10
	//bic     $3000,(r1) / clear only bits 9 and 10
	//rol     r3
	//rol     r3
	//rol     r3
	//bis     r3,(r1) / or old value of bits 9 and 10 with bits 12 and 13
	ldd GR3H,Y+1
	mov GR3L,GR3H
	andi GR3H,$06
	andi GR3L,~$06
	lsl GR3H
	lsl GR3H
	lsl GR3H
	or GR3L,GR3H
	std Y+1,GR3L
pke_3:
	//cmp     r2,$bufp / test to see if entire I/O queue has been scanned
	//bhi    pke_1
	//mov    (sp)+,r3
	//mov    (sp)+,r2
	//mov    (sp)+,r1
	//rts    r0

	ldi16 GR3,bufp
	cp16 X,GR3
	brlo pke_3_1
	breq pke_3_1
	//wrap
	rjmp pke_1

pke_3_1:

	pop16 Z
	pop16 Y
	pop16 X
	pop16 GR4
	pop16 GR3
	ret

//INPUT: GR1(r1) - block number (номер блока)
//OUTPUT: Y(r5) - указывает на данные буфера ассоц. с записью очереди в/в)
//USED: GR3H - temp
//ACTION:
//NOTE: ВАЖНО!!!Неправерено - нет пока записи на устройства
wslot: //nok
	//jsr     r0,bufaloc / get a free I/O buffer; pointer to first
	//br wsl_1 / word in buffer in r5
	rcall bufaloc
	rjmp wsl_1
wsl_1:
	//bit     $22000,(r5) / check bits 10, 13 (read, waiting to read) 
	//beq     wsl_2 / branch if 10, 13 zero 
	//jsr     r0,idle; s.wait+2 
	//br      wsl_1 / till finished
	ldd GR3H,Y+1
	cpi GR3H,$24
	breq wsl_2
	push16 X
	ldi16 X,s_wait+2
	rcall idle
	pop16 X
	rjmp wsl_1
wsl_2:
	//bis     $101000,(r5) / set bits 9, 15  (write, inhibit bits)
	//clr     *$ps / clear processor status
	//add     $8,r5 / r5 points to first word in data area for this block
	//rts     r0
	ldd GR3H,Y+1
	ori GR3H,$82
	std Y+1,GR3H
	setpl ipl0
	addi16 Y,$0008
	ret

//INPUT: GR1(r1) - block number (номер блока)
//OUTPUT: Y(r5) - указывает на данные буфера ассоц. с записью очереди в/в)
//USED: GR3H - temp; X - arg of idle
//ACTION:
//NOTE: ВАЖНО!!! Поставить nop'ы после setpl ipl0 = кол-ву обработчиков.
   //Хотя это не будет ошибкой,если не ставить.Влияет на производительность.
dskrd://ok
	//jsr     r0,bufaloc / shuffle off to bufaloc; get a free I/O buffer
	//br drd_1
	//bis     $2000,(r5) / set bit 10 
	//jsr     r0,poke / just assigned in bufaloc, bit 10=1 says read
	rcall bufaloc
	rjmp drd_1
	ldd GR3H,Y+1
	ori GR3H,$04
	std Y+1,GR3H
	rcall poke
drd_1:
	//clr     *$ps
	//bit     $22000,(r5) / if either bits 10, or 13 are 1; jump to idle
	//beq     drd_2
	//jsr     r0,idle; s.wait+2
	//br      drd_1
	setpl ipl0
	nop      //Можно удалить.Конвеер.При срабатывании обраб. прерывания
	ldd GR3H,Y+1  //данная инструкция как бы получает неактуальные данные.
	andi GR3H,$24
	breq drd_2
	push16 X
	ldi16 X,s_wait+2
	rcall idle
	pop16 X
	rjmp drd_1
drd_2:
	//add     $8,r5 / r5 points to first word of data in block just read in
	//rts     r0
	addi16 Y,$0008
	ret

//INPUT: GR1(r1) - block number (номер блока)
//OUTPUT: None
//USED: Y(r5) - указывает на данные буфера ассоц. с записью очереди в/в)
//		GR3 - temp GR4L
//ACTION:
//NOTE: 
clear:
	//jsr       r0,wslot 
	//mov     $256.,r3
	push16 GR3
	push GR4L
	clr GR4L
	rcall wslot
	ldi16 GR3,$0100
clr_1: 
	//clr     (r5)+ / zero data word in buffer
	//dec     r3
	//bgt     clr_1 / branch until all data words in buffer are zero
	//jsr	r0,dskwr / write zeroed buffer area out onto physical block 
	//rts     r0
	st Y+,GR4L
	dec16 GR3
	breq clr_1_1
	brge clr_1
clr_1_1:

	rcall dskwr
	pop GR4L
	pop16 GR3
	ret 

//INPUT: X(arg.) - rwmode; GR1(r1) - i-number(портится)
//OUTPUT: None
//USED: Y(r5) - адрес данных буфера;
		//GR2 - смещение записи струк. i-node в блоке(предназначенного как
		//для запис,так и для чтения)
		//Z (r1) - адрес структуры inode;GR3 - temp
//ACTION:
//NOTE:Исходя из малого объема памяти - блок уменьшен с 512 до 64 байт,что
       //влечет за собой изменение некоторых подпрограмм.В их числе icalc.
icalc: //nok-ok
	//add	$31.,r1 / add 31. to i-number
	//mov	r1,-(sp) / save i+31. on stack
	//asr	r1 / divide by 16.
	//asr	r1
	//asr	r1
	//asr	r1 / r1 contains block number of block in which i-node exists
	//jsr	r0,dskrd / read in block containing i-node i.
	//tst	(r0)
	//beq	ic_1 / branch to wslot when argument in icalc call = 1
	//jsr	r0,wslot / set up data buffer for write 

	;mov16 GR2,X //далее X будет использован,так что переназначаем регистр
	addi16 GR1,$0001//пропускаем 1(superblock) блок(в каждом блоке 2 записи)
	mov16 GR2,GR1 //сохраняем GR1 
	asr16 GR1 //делим только на 2,т.к. в блоке у нас 2 записи,а не 16
	rcall dskrd
	tst16 X
	breq ic_1
	rcall wslot //nok
	
ic_1:
	//bic	$!17,(sp) / zero all but last 4 bits; gives (i+31.) mod 16
	//mov	(sp)+,mq / calculate offset in data buffer; 32.*(i+31.)mod16
	//mov	$5,lsh / for i-node i.
	//add	mq,r5 / r5 points to first word in i-node i.
	//mov	$inode,r1 / inode is address of first word of current i-node
	//mov	$16.,r3
	//tst	(r0)+ / branch to 2f when argument in icalc call = 0
	//beq	ic_3 / r0 now contains proper return address for rts r0

	//значение GR1 в GR2, GR1 гарантировано сохраняется в GR2
	andi16 GR2,$0001 //(i+1) mod 2
	lsl16 GR2        //32.*((i+1)mod 2) 
	lsl16 GR2        //Может быть два случая:либо 32.*0 = 0,либо 32.*1 = 32 
	lsl16 GR2        
	lsl16 GR2        
	lsl16 GR2
	add16 Y,GR2
	ldi16 Z,inode     //Z = (r1)
	ldi GR3H,$20      //equalent mov $32.,r3
	tst16 X
	breq ic_3
ic_2: //nok
	//mov	(r1)+,(r5)+ / over write old i-node
	//dec	r3
	//bgt	ic_2
	//jsr	r0,dskwr / write inode out on device
	//rts	r0
	ld GR3L,Z+
	st Y+,GR3L
	dec GR3H
	breq ic_2_1
	brge ic_2
ic_2_1://nok
	rcall dskwr
	ret

ic_3:
	//mov	(r5)+,(r1)+ / read new i-node into "inode" area of core
	//dec	r3
	//bgt	ic_3
	//rts	r0
	ld GR3L,Y+
	st Z+,GR3L
	dec GR3H
	breq ic_3_1
	brge ic_3
ic_3_1:
	ret

//INPUT: GR1(r1) - i-number new i-node 
//OUTPUT: GR1(r1) - i-number ii 
//USED: GR3,GR4 - temp; X - arg1 of icalc
//ACTION:
//NOTE: 
iget://nok-ok
	//cmp	r1,ii / r1 = i-number of current flle
	//bne	ig_1
	//cmp	idev,cdev / is device number of i-node = current device
	//beq	ig_4

	push16 X 		//X used as arg1 of calc
	lds16 GR3,ii
	cp16 GR1,GR3
	brne ig_1
	lds16 GR3,idev
	lds16 GR4,cdev
	cp16 GR3,GR4
	breq iget_ig_4 

	rjmp ig_1 //wrap
iget_ig_4:    //wrap
	rjmp ig_4 //wrap
ig_1:
	//tstb	imod / has i-node of current file been modified i.e.,imod set
	//beq	ig_2
	//clrb	imod / if it has, we must write the new i-node out on disk
	//mov	r1,-(sp)
	//mov	cdev,-(sp)
	//mov	ii,r1
	//mov	idev,cdev
	//jsr	r0,icalc; 1
	//mov	(sp)+,cdev
	//mov	(sp)+,r1
	
	lds GR3L,imod
	tst GR3L
	breq ig_2
	clr GR3L	//begin nok
	sts imod,GR3L
	push16 GR1 
	lds16 GR4,cdev
	push16 GR4
	lds16 GR1,ii
	lds16 GR3,idev
	sts16 cdev,GR3

	ldi16 X,$0001
	rcall icalc

	pop16 GR4
	sts16 cdev,GR4
	pop16 GR1	//end nok
ig_2:
	//tst	r1 / is new i-number non zero
	//beq	ig_4 / branch if r1=0
	//tst	cdev / is the current device number non zero 
	//bne	ig_3 / branch 1f cdev =! 0
	//cmp	r1,mnti / mnti is the i-number of the cross devlce file 
	//bne	ig_3
	//mov	mntd,cdev / make mounted device the current device
	//mov	rootdir,r1

	tst16 GR1
	breq ig_4
	lds16 GR4,cdev
	tst16 GR4
	brne ig_3
	lds16 GR3,mnti 
	cp16 GR1,GR3
	brne ig_3         
	lds16 GR4,mntd    
	sts16 cdev,GR4
	lds16 GR1,rootdir
ig_3:
	//mov	r1,ii
	//mov	cdev,idev
	//jsr	r0,icalc; 0 / read in i-node ii

	sts16 ii,GR1
	sts16 idev,GR4

	clr16 X
	rcall icalc
ig_4:
	//mov	ii,r1
	//rts	r0

	lds16 GR1,ii
	pop16 X
	ret

//INPUT: 
//OUTPUT: GR1(r1) - block number (номер блока)
//USED: X - address of byte with block bit in it; GR3L - mask;
		//GR3H- byte of f.s. with block bit in it; X - temp
		//GR4 - number of blocks
//ACTION:
//NOTE: 
alloc_blk:
	//mov	r2,-(sp) / save r2, r3 on stack
	//mov	r3,-(sp)
	//mov	$systm,r2 / start of inode and free storage map for drum
	//tst	cdev
	//beq	al_b_1 / drum is device
	//mov	$mount,r2 / disk or tape is device
	push16 X
	push16 GR3
	ldi16 X,systm
	lds16 GR3,cdev
	tst16 GR3
	breq al_b_1
	ldi16 X,mount
al_b_1:
	//mov	(r2)+,r1 / first word contains number of bytes in f.s. map
	//asl	r1 / multiply r1 by eight gives, number of blocks in device
	//asl	r1
	//asl	r1
	//mov	r1,-(sp) / save # of blocks in device on stack
	//clr	r1 / r1 contains bit count of free storage map
	ld16i GR1,X+
	lsl16 GR1
	lsl16 GR1
	lsl16 GR1
	lsl16 GR1
	push16 GR4
	mov16 GR4,GR1
	clr16 GR1
al_b_2:
	//mov	(r2)+,r3 / word of free storage map in r3
	//bne	al_b_3 / branch if any free blocks in this word
	//add	$16.,r1
	//cmp	r1 ,(sp) / have we examined all free storage bytes
	//blo	al_b_2
	//jmp	panic / found no free storage
	ld GR3H,X+
	tst GR3H
	brne al_b_3
	addi16 GR1,$0008 //not 16(!)
	cp16 GR1,GR4
	brlo al_b_2
	rjmp panic
al_b_3:
	//asr	r3 / find a free block
	//bcs	al_b_4 / branch when free block found 
	//inc	r1 / increment bit count in bit k (mod8)
	//br	al_b_3
	asr GR3H
	brcs al_b_4
	inc16 GR1
	rjmp al_b_3
al_b_4:
	//tst	(sp)+ / bump sp
	//jsr	r0,noname_1 / have found a free block
	//bic	r3,(r2) / set bit for this block i.e. assign block
	//br	al_b_5
	pop16 GR4
	rcall noname_1
	com GR3L //invert mask
	ld GR3H,X
	and GR3H,GR3L
	st X,GR3H
	rjmp al_b_5	


//INPUT: GR1(r1) - block number (номер блока)
//OUTPUT: None
//USED: X - address of byte with blocks bits in it; GR3L - mask;
		//GR3H- byte of f.s. with blocks bits in it; X - temp
//ACTION:
//NOTE:
free_blk:
	//mov	r2,-(sp) / save r2, r3
	//mov	r3,-(sp)
	//jsr	r0,3f / set up bit mask and word no. in f. s. map for block
	//bis	r3,(r2) / set free storage block bit; indicates free block
//2:
	//mov	(sp)+,r3 / restore r2, r3
	//mov	(sp)+,r2
	//tst	cdev /cdev = 0, drum; cdev = 1 (mountable device)
	//bne	1f
	//incb	smod / set super block modified for drum
	//rts	r0
//1:
	//incb	mmod / set super block modified for mountable device
	//rts	r0
	push16 X
	push16 GR3
	rcall noname_1
	ld GR3H,X
	or GR3H,GR3L
	st X,GR3H
al_b_5:
	lds16 X,cdev
	tst16 X
	brne fr_b_1
	lds GR3H,smod
	inc GR3H
	sts smod,GR3H
	rjmp fr_b_2
fr_b_1:
	lds GR3H,mmod
	inc GR3H
	sts mmod,GR3H
fr_b_2:
	pop16 GR3
	pop16 X
	ret

//INPUT: GR1(r1) - block number (номер блока)
//OUTPUT: X(r2) - address of byte of free storage map for device
	    		  // with block bit in it(адрес байта с битом блока внутри)
				  //GR3L(r3) - mask(number bit in byte)
//USED: Z - used for mask(используется для определения бита через маску)		
//ACTION:
//NOTE: реализация для ATMEGA не требует возврата адреса слова,вместо этого
        //возвращается адрес байта бита блока
noname_1:
	//mov	r1,r2 / block number, k, = 1
	//bic	$!7,r2 / clear all bits but 0,1,2; r2 = (k) mod (8)
	//clr	r3
	//bisb	bitmask(r2),r3 / use mask to set bit in r3 ((k) mod 8)
	//mov	r1,r2 / divide block number by 16
	//asr	r2
	//asr	r2
	//asr	r2
	;asr	r2
	;bcc	nn_1 / bit for block is in lower half of word
	;swab	r3 / swap bytes in r3
;nn_1:
	//asl	r2 / multiply block number by 2; r2 = k/8
	//add	$systm+2,r2 / addr of word of f.s.map for drum with block bit in it
	//tst	cdev
	//beq	nn_2 / cdev = 0 indicates device is drum
	//add	$mount-systm,r2 / for mountable device
	push16 Z
	mov16 X,GR1
	andi16 X,$0007
	ldi16 Z,bitmask
	add16 Z,X
	lpm GR3L,Z
	mov16 X,GR1
	asr16 X	;divide block number by 8
	asr16 X
	asr16 X
	asr16 X
	
	addi16 X,(systm+2)
	lds16 Z,cdev
	tst16 Z
	breq nn_2
	addi16 X,(mount-systm)
nn_2:
	//rts	r0 / return to 'free'
	pop16 Z
	ret

bitmask: .db $01,$02,$04,$08,$10,$20,$40,$80

//INPUT: 
//OUTPUT: GR1(r1) - block number (номер блока)
//USED: GR2 (r2),Z - internal; GR3L - counter ; GR3H - value 0;GR3 - temp
        // X - first word(byte) of data buffer 
//ACTION:
//NOTE: 
mget://nok-ok
	//mov	*u.fofp,mq / file offset in mq
	//clr	ac / later to be high sig
	//mov	$-8,lsh   / divide ac/mq by 256.
	//mov	mq,r2
	//bit	$10000,i.flgs / lg/sm is this a large or small file
	//bne	mg_5 / branch for large file
	//bit	$!17,r2
	//bne	mg_2 / branch if r2 greater than or equal to 16
	//bic	$!16,r2 / clear all bits but bits 1,2,3
	//mov	i.dskp(r2),r1 / r1 has physical block number
	//bne	mg_1 / if phys. block num is zero then need a new block for file
	//jsr	r0,alloc / allocate a new block
	//mov	r1,i.dskp(r2) / physical block number stored in i-node
	//jsr	r0,setimod / set inode modified byte (imod)
	//jsr	r0,clear / zero out disk/drum block just allocated

	lds16 Z,u_fofp
	ld16 GR2,Z
	lsr16 GR2	//divide by 32
	lsr16 GR2
	lsr16 GR2
	lsr16 GR2
	lsr16 GR2
	lds16 Z,i_flgs
	andi ZH,$10
	brne mg_5
	ldi16 Z,$0010 
	cp16 GR2,Z    //(off div 32) >= 16?
	brsh mg_2  //yes,branch;transform into Large file; off >=512(>=8 blocks) 
	andi16 GR2,$000E //mod 16 and (not odd)-1 = odd //хотя mod 16 лишнее
	ldi16 Z,i_dskp   //если он меньше 512,то GR2 всегда будет меньше 16
	add16 Z,GR2      //только для порождения четного нужно
	ld16 GR1,Z	     //находим по смещению GR2 блок в котором указ-ет смещ.
	tst16 GR1
	brne mg_1
	rcall alloc_blk //begin nok
	st16 Z,GR1 //ВАЖНО!!! Хотя Z не изменяется всеравно нужно проверить
	rcall setimod
	rcall clear  //end nok
mg_1://ok
	//rts	r0
	ret
mg_2: // adding on block which changes small file to a large file //nok
	//jsr	r0,alloc / alloc. a new block for this file; block number in r1
	//jsr	r0,wslot / set up for write, r5 points to first word in buffer
	//mov	$8.,r3 / next 6 inst-s transfer old physical block pointers
	//mov	$i.dskp,r2 / into new indirect block for the new large file
	call alloc
	rcall wslot
	ldi16 GR3,$0008
	ldi16 Z,i_dskp //r2 = Z

mg_3://nok
	//mov	(r2),(r5)+
	//clr	(r2)+
	//dec	r3
	//bgt	mg_3
	//mov	$256.-8.,r3 / clear rest of data buffer
	ld16 GR2,Z
	st16i X+,GR2
	st Z+,GR3H  //clr (r2)+
	st Z+,GR3H
	dec GR3L
	breq mg_3_1
	brge mg_3
mg_3_1://nok
	ldi GR3L,32-8 //block size = 64 = 32 words
mg_4://nok
	//clr	(r5)+
	//dec 	r3
	//bgt	mg_4
	//jsr	r0,dskwr / write new indirect block on disk
	//mov	r1,i.dskp / put pointer to indirect block in i-node
	//bis	$10000,i.flgs / set large file bit in i.flgs word of i-node
	//jsr	r0,setimod / set i-node modified flag
	//br	mget
	st X+,GR3H  //clr (r5)+
	st X+,GR3H
	dec GR3L
	breq mg_4_1
	brge mg_4
mg_4_1://nok
	rcall dskwr
	sts16 i_dskp,GR1
	lds GR3L,i_flgs+1
	ori GR3L,$10
	sts i_flgs+1,GR3L
	rcall setimod
	rjmp mget

mg_5: // large file  //nok
	//mov	$-8,lsh / divide byte number by 256.
	//bic	$!776,r2 / zero all bits but 1-8; gives offset in ind. block
	//mov	r2,-(sp) / save on stack
	//mov	mq,r2 / calc. off. in i-node for pointer to proper ind. block
	//bic	$!16,r2
	//mov	i.dskp(r2),r1
	//bne	mg_6 / if no indirect block exists
	//jsr	r0,alloc / allocate a new block
	//mov	r1,i.dskp(r2) / put block number of new block in i-node
	//jsr	r0,setimod / set i-node modified byte
	//jsr	r0,clear / clear new block
	mov16 GR3,GR2 
	lsr16 GR2	//divide by 32
	lsr16 GR2
	lsr16 GR2
	lsr16 GR2
	lsr16 GR2
	andi16 GR3,~$0041 //bic $!776,r2 (mod 512); mod 64 and(not odd)-1 = odd
	push16 GR3
	andi16 GR2,$000E
	ldi16 Z,i_dskp
	add16 Z,GR2
	ldd16 GR1,Z+0
	tst16 GR1
	brne mg_6
	rcall alloc_blk
	st16 Z,GR1  //Z not changed
	rcall setimod
	rcall clear
mg_6:
	//jsr	r0,dskrd / read in indirect block
	//mov	(sp)+,r2 / get offset
	//mov	r1,-(sp) / save block number of indirect block on stack
	//add	r5,r2 / r5 - first word in ind. block, r2 - location of inter
	//mov	(r2),r1 / put physical block no of block in file sought in r1
	//bne	mg_7 / if no block exists
	//jsr	r0,alloc / allocate a new block
	//mov	r1,(r2) / put new block num. into proper location in ind. block
	//mov	(sp)+,r1 / get block number of indirect block
	//mov	(r2),-(sp) / save block number of new block
	//jsr	r0,wslot
	//jsr	r0,dskwr / write newly modified indirect block back out on disk
	//mov	(sp),r1 / restore block number of new block
	//jsr	r0,clear / clear new block
	rcall dskrd
	pop16 Z
	push16 GR1 
	add16 Z,Y //ok
	ldd16 GR1,Z+0
	tst16 GR1
	brne mg_7
	rcall alloc_blk //bnok
	st16 Z,GR1 //Z not changed(?)
	pop16 GR1
	ld16 GR3,Z
	rcall wslot
	rcall dskwr
	mov16 GR1,GR3
	rcall clear      //enok
mg_7:
	//tst	(sp)+ / bump stack pointer
	//rts	r0
	pop16 GR3
	ret

//INPUT: 
//OUTPUT: 
//USED: X - interal
//ACTION:
//NOTE: 
setimod:
	//movb	$1,imod / set current i-node modified bytes
	//mov	s.time,i.mtim / put present time into file modified time
	//mov	s.time+2,i.mtim+2
	//rts	r0
	push16 X
	ldi XL,$01
	sts imod,XL
	lds16 X,s_time
	sts16 i_mtim,X
	lds16 X,s_time+2
	sts16 i_mtim+2,X
	pop16 X
	ret

//INPUT: X - flags mask of access (read - 2; write - 1) GR1 - i-number 
//OUTPUT: None
//USED: GR2 - i.flgs; GR3L - u.uid; GR3H - i.uid  
//ACTION:
//NOTE:
access://ok
	//jsr	r0,iget / read in i-node for current directory (i-number in r1)
	//mov	i.flgs,r2
	//cmpb	i.uid,u.uid / is user same as owner of file
	//bne	acs_1 / no, then branch
	//asrb	r2 / shift owner read write bits into non owner read/write bits
	//asrb	r2
	rcall iget
	lds16 GR2,i_flgs
	lds GR3L,u_uid
	lds GR3H,i_uid
	cp GR3H,GR3L
	brne acs_1
	asr GR2L
	asr GR2L
acs_1:
	//bit	r2,(r0)+ / test read-write flags against argument in access call
	//bne	acs_2
	//tstb	u.uid
	//beq	acs_2
	//jmp	error
	and GR2L,XL
	brne acs_2
	tst GR3L
	breq acs_2
	jmp error 
acs_2:
	//rts	r0
	ret

//INPUT: Y - address of lowest location in core to be cleared;
       //Z - address of highest location In core to be cleared
//OUTPUT: None
//USED: GR3L - zero;
//ACTION:
//NOTE: 
copyz:
	clr GR3L
cpz_1:
	st Y+,GR3L
	cp16 Y,Z
	brlo cpz_1
	ret

error3:
	jmp error

//INPUT: None
//OUTPUT: u_dirp; GR1 - i-node number
//USED: GR3,GR4 - temp; Z - *u_namep;access param
        //Z(r2) - u.namep ;Y(r3) - $u.dirbuf+2; X - arg. of access
//ACTION:
//NOTE: 
namei://ok
	//mov	u.cdir,r1 / put the i-number of current directory in r1
	//mov	u.cdev,cdev / device number for users directory into cdev
	//cmpb	*u.namep,$'/ / is first char in file name a /
	//bne	nami_1
	//inc	u.namep / go to next char
	//mov	rootdir,r1 / put i-number of rootdirectory in r1
	//clr	cdev / clear device number

	lds16 GR1,u_cdir
	lds16 GR3,u_cdev
	sts16 cdev,GR3
	lds16 Z,u_namep
	ld GR3L,Z
	cpi GR3L,'/' 
	brne nami_1
	inc16 Z
	sts16 u_namep,Z
	lds16 GR1,rootdir
	clr16 GR4
	sts16 cdev,GR4
nami_1:
	//tstb	*u.namep / is the character in file name a nul
	//beq	nig / yes, end of file name reached; branch to "nig"
	ld GR3L,Z
	tst GR3L
	breq nami_1_nig
	rjmp nami_2
nami_1_nig:
	rjmp nig
nami_2:
	//jsr	r0,access; 2 / get i-node with i-number r1
	//bit	$40000,i.flgs / directory i-node?
	//beq	error3 / no, got an error
	//mov	i.size,u.dirp / put size of directory in u.dirp
	//clr	u.off / u.off is file offset used by user
	//mov	$u.off,u.fofp / u.fofp is a pointer to the offset 

	ldi16 X,$0002
	rcall access
	lds GR3H,i_flgs+1
	andi GR3H,$40
	breq error3
	lds16 GR3,i_size
	sts16 u_dirp,GR3
	clr16 GR3
	sts16 u_off,GR3
	ldi16 GR3,u_off
	sts16 u_fofp,GR3
nami_3:
	//mov	$u.dirbuf,u.base / u.dirbuf holds a directory entry
	//mov	$10.,u.count / u.count is byte count for reads and writes
	//jsr	r0,readi / read 10. bytes of file with i-number (r1);read entry
	//tst	u.nread
	//ble	nib / gives error return
	//tst	u.dirbuf /
	//bne	nami_4 / branch when active directory entry (i-node non zero)
	//mov	u.off,u.dirp
	//sub	$10.,u.dirp
	//br	nami_3

	ldi16 GR3,u_dirbuf
	sts16 u_base,GR3
	ldi16 GR3,10  //количество считываемы байт от начала файла(u.off)
	sts16 u_count,GR3
	rcall readi
	lds16 GR3,u_nread
	tst16 GR3
	brlt nami_3_1_nib
	breq nami_3_1_nib
	rjmp nami_3_1   //wrap
nami_3_1_nib:       //wrap
	rjmp nib        //wrap

nami_3_1:
	lds16 GR3,u_dirbuf
	tst16 GR3
	brne nami_4         
	lds16 GR3,u_off
	subi16 GR3,10
	sts16 u_dirp,GR3
	rjmp nami_3        
nami_4:
	//mov	u.namep,r2 / u.namep points into a file name string
	//mov	$u.dirbuf+2,r3 / points to file name of directory entry

	lds16 Z,u_namep
	ldi16 Y,u_dirbuf+2
nami_5:
	//movb	(r2)+,r4 / move a character from u.namep string into r4
	//beq	nami_6 /if char is \0 then the last chr in string has been moved
	//cmp	r4,$'/ / is char a </>
	//beq	nami_6
	//cmp	r3,$u.dirbuf+10. / have I checked all 8 bytes of file name
	//beq	nami_5
	//cmpb	(r3)+,r4 / compare char in u.namep string to file name char read 
	//beq	nami_5 / from directory; branch if chars match
	//br	nami_3 / file names do not match go to next directory entry

	ldi16 GR3,u_dirbuf+10

	ld GR4L,Z+
	tst GR4L
	breq nami_6
	cpi GR4L,'/'
	breq nami_6

	cp16 Y,GR3
	breq nami_5
	ld GR4H,Y+
	cp GR4H,GR4L
	breq nami_5
	rjmp nami_3
nami_6:
	//cmp	r3,$u.dirbuf+10. / if equal all 8 bytes were matched
	//beq	nami_7
	//tstb	(r3)+ /
	//bne	nami_3
	;ldi16 GR3,u_dirbuf+10
	cp16 Y,GR3
	breq nami_7
	ld GR3L,Y+
	tst GR3L
	brne nami_6_1_nami_3
	rjmp nami_7
nami_6_1_nami_3:
	rjmp nami_3
nami_7:
	//mov	r2,u.namep / u.namep points to char following a / or nul
	//mov	u.dirbuf,r1 / move i-node number in directory entry to r1
	//tst	r4 / if r4 = 0 the end of file name reached, if r4 = </>
	//bne	nami_2 / then go to next directory

	sts16 u_namep,Z
	lds16 GR1,u_dirbuf
	tst GR4L
	brne nami_7_1_nami_2
	rjmp nig     //wrap
nami_7_1_nami_2: //wrap
	rjmp nami_2  //wrap
nig:
	//tst	(r0)+ / gives non-error return
	skip GR3,0
nib:
	//rts	r0
	ret

//INPUT: GR1 - i-number
//OUTPUT: None
//USED: GR3,Z - temp
//ACTION:
//NOTE: //ВАЖНО!!! заполнить таблицу!
readi: //ok
	//clr	u.nread / accumulates number of bytes transmitted
	//tst	u.count / is number of bytes to be read greater than 0
	//bgt	rdi_1 / yes, branch
	//rts	r0 / no, nothing to read; return to caller

	clr16 GR3
	sts16 u_nread,GR3
	lds16 GR3,u_count
	tst16 GR3
	breq rdi_0
	brge rdi_1
rdi_0:
	ret
rdi_1:
	//mov	r1,-(sp) / save i-number on stack
	//cmp	r1,$40. / want to read a special file (i-nodes 1,...,40 )
	//ble	rdi_2 / yes, branch
	//jmp	dskr / no, jmp to dskr; read file with i-node number (r1)
		     // starting at byte ((u.fofp)), read in u.count bytes
	push16 GR1
	ldi16 GR3,40
	cp16 GR1,GR3
	brlt rdi_2
	breq rdi_2
	rjmp dskr
rdi_2:
	//asl	r1 / multiply inode number by 2
	//jmp	*1f-2(r1)
	lsl16 GR1
	addi16 GR1,((rdi_3-1)<<1)
	mov16 Z,GR1
	lpm16i GR1,Z+
	mov16 Z,GR1
	ijmp
rdi_3:
	.dw rtty //ВАЖНО!!! заполнить таблицу!
	.dw touts_test //del

//INPUT: None
//OUTPUT: None
//USED: GR1(r1) - i-number,GR2(r2),GR3(r3);Y,Z,GR4 - temp
//ACTION:
//NOTE:
dskr://ok
	//mov	(sp),r1 / i-number in r1
	//jsr	r0,iget / get i-node (r1) into i-node section of core
	//mov	i.size,r2 / file size in bytes in r2
	//sub	*u.fofp,r2 / subtract file offset
	//blos	ret
	//cmp	r2,u.count / are enough bytes left in file to carry out read
	//bhis	dskr_1
	//mov	r2,u.count / no, just read to end of file
	pop16 GR1
	push16 GR1
	rcall iget
	lds16 GR2,i_size
	lds16 Z,u_fofp
	ld16 GR4,Z
	sub16 GR2,GR4
	brlo ret1
	breq ret1
	lds16 GR4,u_count
	cp16 GR2,GR4
	brsh dskr_1
	sts16 u_count,GR2
dskr_1:
	//jsr	r0,mget /returns block № of block in file where offset points
	//jsr	r0,dskrd / read block, r5 points to 1st word of data in buffer
	//jsr	r0,sioreg
	rcall mget
	rcall dskrd
	rcall sioreg

	mov16 Y,GR1 
	mov16 Z,GR2

dskr_2:
	//movb	(r2)+,(r1)+ / move data from buffer into user memory(at u.base)
	//dec	r3
	//bne	dskr_2 / branch until proper number of bytes are transferred
	//tst	u.count / all bytes read off disk
	//bne	dskr
	//br	ret

	ld GR4L,Z+
	st Y+,GR4L
	dec16 GR3
	brne dskr_2
	lds16 GR4,u_count
	tst16 GR4
	brne dskr
	rjmp ret1

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
ret1:
	pop16 GR1
ret1_1:
	setpl ipl0
	ret

//INPUT: Y(r5) - point to data of buffer 
//OUTPUT: GR1(r1) -usrdata(dest.) ,GR2 - buffer(source), GR3 - qnt
//USED: Z,GR4- temp; GR2(r2),GR3(r3) 
//ACTION:
//NOTE:
sioreg://ok
	//mov	*u.fofp,r2 / file offset (in bytes) is moved to r2
	//mov	r2,r3 / and also to r3
	//bis	$177000,r3 / set bits 9,...,15. of file offset in r3
	//bic	$!777,r2 / calculate file offset mod 512.
	//add	r5,r2 /r2 points to 1st byte in buf. where data is to be placed
	//mov	u.base,r1 / address of data is in r1
	//neg	r3 / 512 - file offset (mod512.) in r3 (number of free bytes 
	//cmp	r3,u.count 
	//blos	sioreg_1 
	//mov	u.count,r3 
	lds16 Z,u_fofp
	ld16 GR2,Z
	mov16 GR3,GR2
	ori16 GR3,$FFC0
	andi16 GR2,$003F
	add16 GR2,Y
	lds16 GR1,u_base
	neg16 GR3
	lds16 Z,u_count
	cp16 GR3,Z
	brlo sioreg_1
	breq sioreg_1
	lds16 GR3,u_count
sioreg_1:

	//add	r3,u.nread 
	//sub	r3,u.count 
	//add	r3,u.base 
	//add	r3,*u.fofp / new offset = number of bytes done + old file offset
	//rts	r0
	lds16 Z,u_nread
	add16 Z,GR3
	sts16 u_nread,Z
	lds16 Z,u_count
	sub16 Z,GR3
	sts16 u_count,Z
	lds16 Z,u_base
	add16 Z,GR3
	sts16 u_base,Z
	lds16 Z,u_fofp
	ld16 GR4,Z
	add16 GR4,GR3
	st16 Z,GR4
	ret

//INPUT: u_namep
//OUTPUT: GR1 - device number of special file
//USED: GR1; GR3 - temp
//ACTION:
//NOTE: ВАЖНО!!!Поменять диапазон!
getspl:
	//jsr    r0,namei / get the i-number of the special file
	//br errora / no such file
	//sub    $4,r1 / i-number-4 rk=1,tap=2+n
	//ble    errora / less than 0?  yes, error
	//cmp    r1,$9. / greater than 9  tap 7
	//bgt    errora / yes, error
	//rts    r0 / return with device number in r1

	rcall namei
	rjmp errora
	subi16 GR1,4
	brlt errora
	breq errora
	ldi16 GR3,10
	cp16 GR1,GR3
	brge errora
	ret

errora:
	jmp error

//INPUT: GR1 - special file; GR2 - mount point
//OUTPUT: None
//USED: GR1,GR2; GR3 -temp; GR0 - emul stack
//ACTION:
//NOTE:
sysmount:
	//jsr    r0,arg2 / get arguments special and name
	//tst    mnti / is the i-number of the cross device file zero?
	//bne    errora / no, error
	//jsr    r0,getspl / get special files device number in r1
	//mov    (sp)+,u.namep / put the name of file to be placed on the device
	//mov    r1,-(sp) / save the device number
	//jsr    r0,namei / get the i-number of the file
	//br errora
	//mov    r1,mnti / put it in mnti

	sts16 u_namep,GR1 //arg2
	sts16 u_off,GR2   //arg2
	mov16 GR0,GR2     //arg2
	rcall getspl
	sts16 u_namep,GR0 //GR0 для хранения больше не нужен
	mov16 GR0,GR1     //сохраняем в него номер устройства
	rcall namei
	rjmp errora
	sts16 mnti,GR1    //сохраняем i-number точки монтирования(с устр. 0)
sm_1:
	//tstb   sb1+1 / is 15th bit set?
	//bne    sm_1 / (inhibit bit) yes, skip writing
	//mov    (sp),mntd / no, put the device number in mntd
	//movb   (sp),sb1 / put the device number in the lower byte of the entry
	//mov    (sp)+,cdev / put device number in cdev
	//bis    $2000,sb1 / set the read bit
	//jsr    r0,ppoke / read in entire file system

	lds GR3L,sb1+1
	tst GR3L   //ждем освобождение буфера связанного с суперблоком
	brne sm_1  //присоединяемого устройства
	sts16 mntd,GR0
	sts sb1,GR0L
	sts16 cdev,GR0
	lds GR3L,sb1+1
	ori GR3L,$04    //ставим буфер на чтение суперблока присоед. устройства
	sts sb1+1,GR3L
	rcall ppoke
sm_2:
	//tstb   sb1+1 / done reading?
	//bne    sm_2 / no, wait
	//br     sysreta / yes

	lds GR3L,sb1+1
	tst GR3L
	brne sm_2  //ждем пока суперблок не считается во внут. структуры unix
	jmp sysret

errora1:
	jmp error

//INPUT: GR1 - special file
//OUTPUT: None
//USED: GR3 - temp
//ACTION:
//NOTE:
sysumount:
	//jsr    r0,arg; u.namep / point u.namep to special
	//jsr    r0,getspl / get the device number in r1
	//cmp    r1,mntd / is it equal to the last device mounted?
	//bne    errora / no error

	sts16 u_namep,GR1 //arg
	rcall getspl
	lds16 GR3,mntd
	cp16 GR1,GR3
	brne errora1
sum_1:
	//tstb   sb1+1 / yes, is the device still doing I/O?
	//bne    sum_1 / yes, wait
	//clr    mntd / no, clear these
	//clr    mnti
	//br     sysreta / return
	lds GR3L,sb1+1
	tst GR3L
	brne sum_1  //ждем окончания операций с суперблоком
	clr16 GR3
	sts16 mntd,GR3
	sts16 mnti,GR3
	jmp sysret

//INPUT: GR1 - namep; GR2 - mode
//OUTPUT: GR0 - descriptor
//USED: 
//ACTION:
//NOTE:
sysopen://nok-ok
	//jsr    r0,arg2 / get sys args into u.namep and on stack
	//jsr    r0,namei / i-number of file in r1
	//br     error2 / file not found
	//tst    (sp) / is mode = 0 (2nd arg of call; 0 means, open for read)
	//beq    sopn_1 / yes, leave i-number positive
	//neg    r1 / open for writing so make i-number negative

	sts16 u_namep,GR1 //arg2
	sts16 u_off,GR2   //arg2
	mov16 GR0,GR2     //arg2
	rcall namei
	rjmp errora1
	tst16 GR0
	breq sopn_1
	neg16 GR1
sopn_1:
	//jsr    r0,iopen / open file whose i-number is in r1
	//tst    (sp)+ / pop the stack and test the mode
	//beq    op1 / is open for read op1
	
	rcall iopen
	tst16 GR0
	breq op1
op0:
	//neg    r1 / make i-number positive if open for writing

	neg16 GR1
op1:
	//clr    r2 / clear registers
	//clr    r3

	clr16 GR2
	clr16 GR3
sopn_2: // scan the list of entries in fsp table
	//tstb   u.fp(r2) / test the entry in the u.fp list
	//beq    sopn_3 / if byte in list is 0 branch
	//inc    r2 / bump r2 so next byte can be checked
	//cmp    r2,$10. / reached end of list?
	//blt    sopn_2 / no, go back
	//br     error2 / yes, error (no files open)

	ldi16 Y,u_fp
	add16 Y,GR2
	ld GR4L,Y
	tst GR4L
	breq sopn_3_0
	inc GR2L
	cpi GR2L,10
	brlt sopn_2
	rjmp errora1 //nok
sopn_3_0:

	ldi16 Y,(nfiles*8)
sopn_3:
	//tst    fsp(r3) / scan fsp entries
	//beq    sopn_4 / if 0 branch
	//add    $8.,r3 / add 8 to r3 to bump it to next entry mfsp table
	//cmp    r3,$[nfiles*8.] / done scanning
	//blt    sopn_3 / no, back
	//br     error2 / yes, error

	ldi16 Z,fsp
	add16 Z,GR3
	ld16 GR4,Z
	tst16 GR4
	breq sopn_4
	addi16 GR3,8
	cp16 GR3,Y
	brlt sopn_3
	rjmp errora1 //nok
sopn_4: // r2 has index to u.fp list; r3, has index to fsp table
	//mov    r1,fsp(r3) / put i-number of open file into next available
	//mov    cdev,fsp+2(r3) / entry in fsp table, put # of dev. in next word
	//clr    fsp+4(r3)
	//clr    fsp+6(r3) / clear the next two words
	//asr    r3
	//asr    r3 / divide by 8 to get number of the fsp entry-1
	//asr    r3
	//inc    r3 / add 1 to get fsp entry number
	//movb   r3,u.fp(r2) /move entry num. into next avail. slot in u.fp list
	//mov    r2,*u.r0 / move index to u.fp list into r0 loc on stack
	//br     sysret2

	std16 Z+0,GR1
	lds16 GR4,cdev
	std16 Z+2,GR4
	clr16 GR4
	std16 Z+4,GR4
	std16 Z+6,GR4
	lsr16 GR3
	lsr16 GR3
	lsr16 GR3
	inc16 GR3
	ldi16 Y,u_fp
	add16 Y,GR2
	st Y,GR3L
	lds16 Y,u_r0
	std16 Y+0,GR2
	rjmp sysret4

//INPUT: GR1 - descriptor of file; GR2 - buffer; GR3 - nchar
//OUTPUT: None
//USED: GR1,GR2,GR3
//ACTION:
//NOTE:
sysread:
	//jsr	r0,rw1 / get i-number of file to be read into r1
	//tst	r1 / negative i-number?
	//ble	error1 / yes, error 1 to read it should be positive
	//jsr	r0,readi / read data into core
	//br	swrt_1

	rcall rw1
	tst16 GR1
	brlt error5
	breq error5
	rcall readi
	rjmp swrt_1

//INPUT: GR1 - descriptor of file; GR2 - buffer; GR3 - nchar
//OUTPUT: None
//USED: GR1,GR2,GR3; GR3 -temp; Z - *u_r0
//ACTION:
//NOTE:
syswrite:
	//jsr	 r0,rw1 / get i-number in r1 of file to write
	//tst    r1 / positive i-number ?
	//bge    error1 / yes, error 1 negative i-number means write
	//neg    r1 / make it positive
	//jsr    r0,writei / write data

	rcall rw1
	tst16 GR1
	brge error5
	neg16 GR1
	rcall writei
swrt_1:
	//mov    u.nread,*u.r0 / put no. of bytes transferred into (u.r0)
	//br     sysret1

	lds16 Z,u_r0
	lds16 GR3,u_nread
	std16 Z+0,GR3
	rjmp sysret4

error5:
	jmp error

//INPUT: GR1 - descriptor of file; GR2 - buffer; GR3 - nchar
//OUTPUT: None
//USED: GR1,GR2,GR3
//ACTION:
//NOTE:
rw1:
	//jsr    r0,arg; u.base / get buffer pointer
	//jsr    r0,arg; u.count / get no. of characters
	//mov    *u.r0,r1 / put file descriptor (index to u.fp table) in r1
	//jsr    r0,getf / get i-number of the file in r1
	//rts    r0

	sts16 u_base,GR2
	sts16 u_count,GR3
	rcall getf
	ret


//INPUT: GR1 - descriptor of file
//OUTPUT: None
//USED: GR3 -temp; Z - address of fsp entry 
//ACTION:
//NOTE:
getf://ok
	//cmp	r1,$10. / user limited to 10 open files
	//bhis	error3 / u.fp is table of users open files, index in fsp table
	//movb	u.fp(r1),r1 / r1 contains number of entry in fsp table
	//beq	getf_1 / if its zero return
	//asl	r1
	//asl	r1 / multiply by 8 to get index into fsp table entry
	//asl	r1
	//add	$fsp-4,r1 / r1 is pointing at the 3rd word in the fsp entry
	//mov	r1,u.fofp / save address of 3rd word in fsp entry in u.fofp
	//mov	-(r1),cdev / remove the device number  cdev
	//mov	-(r1),r1 / and the i-number  r1

	cpi GR1L,10
	brsh error4
	ldi16 Z,u_fp
	add168 Z,GR1L
	ld GR1L,Z
	tst GR1L
	breq getf_1
	clr GR1H  
	lsl16 GR1
	lsl16 GR1
	lsl16 GR1
	ldi16 Z,fsp-8
	add16 Z,GR1
	mov16 GR3,Z
	addi16 GR3,4
	sts16 u_fofp,GR3
	ldd16 GR3,Z+2
	sts16 cdev,GR3
	ldd16 GR1,Z+0
getf_1:
	//rts	r0
	ret

error4:
	jmp error
sysret4:
	jmp sysret

//INPUT: GR1 - descriptor of file; GR2 - offset; GR3 - ptrname
//OUTPUT: None
//USED: GR1,GR2,GR3; Z,GR4 - temp
//ACTION:
//NOTE:
sysseek:
	//jsr	r0,seektell / get proper value in u.count
	//add	u.base,u.count / add u.base to it
	//mov	u.count,*u.fofp / put result into r/w pointer
	//br	sysret4
	rcall seektell
	lds16 Z,u_base
	lds16 GR4,u_count
	add16 GR4,Z
	sts16 u_count,GR4
	lds16 Z,u_fofp
	std16 Z+0,GR4


//INPUT: GR1 - descriptor of file; GR2 - offset; GR3 - ptrname
//OUTPUT: None
//USED: GR2; GR3,GR4 - temp;GR0 - instead stack; Z - *u_fofp
//ACTION:
//NOTE: если ptrname 0, то указатель установливается на смещение. 
      //если ptrname 1, то указатель устанавливается на его текущее 
	  //место плюс смещение. 
      //если ptrname 2, указатель установливается на размер файла плюс 
	  //смещение. 
      //Бит ошибки (е-бит) установливается для неопределенного 
	  //файлового дескриптора.
seektell:
	//jsr	r0,arg; u.base / puts offset in u.base
	//jsr	r0,arg; u.count / put ptr name in u.count
	;mov	*u.r0,r1 / file descriptor in r1 (index in u.fp list)
	//jsr	r0,getf / u.fofp points to 3rd word in fsp entry
	//mov	r1,-(sp) / r1 has i-number of file, put it on the stack
	//beq	error4 / if i-number is 0, not active so error
	//bgt	setl_0 / if its positive jump
	//neg	r1 / if not make it positive
	sts16 u_base,GR2
	sts16 u_count,GR3
	rcall getf
	mov16 GR0,GR1 //save GR1
	tst16 GR1
	breq error4
	brge setl_0
	neg16 GR1
setl_0:
	//jsr	r0,iget / get its i-node into core
	//cmp	u.count,$1 / is ptr name =1
	//blt	setl_2 / no its zero
	//beq	setl_1 / yes its 1
	//mov	i.size,u.count /  put number of bytes in file in u.count
	//br	setl_2

	rcall iget
	lds16 GR3,u_count
	clr16 GR4
	inc GR4L
	cp16 GR3,GR4
	brlt setl_2
	breq setl_1
	lds16 GR3,i_size
	sts16 u_count,GR3
setl_1: // ptr name =1
	//mov	*u.fofp,u.count / put offset in u.count

	lds16 Z,u_fofp
	ldd16 GR3,Z+0
	sts16 u_count,GR3
setl_2: // ptrname =0
	//mov	(sp)+,r1 / i-number on stack  r1
	//rts	r0

	mov16 GR1,GR0
	ret

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:  ВАЖНО!!! заполнить таблицу!
writei:
	//clr	u.nread / clear the number of bytes transmitted during r/w calls
	//tst	u.count / test the byte count specified by the user
	//bgt	wrti_1 / any bytes to output; yes, branch
	//rts	r0 / no, return - no writing to do

	clr16 GR3
	sts16 u_nread,GR3
	lds16 GR3,u_count
	tst16 GR3
	breq wrti_0
	brge wrti_1
wrti_0:
	ret
wrti_1:
	//mov	r1 ,-(sp) / save the i-node number on the stack
	//cmp	r1,$40. / does the i-node number indicate a special file?
	//bgt	dskw / no, branch to standard file output
	//asl	r1 / yes, calculate the index into the special file
	//jmp	*wrti_2-2(r1) / jump table and jump to the appropriate routine

	push16 GR1
	ldi16 GR3,40
	cp16 GR1,GR3
	breq wrti_1_0
	brge dskw
wrti_1_0:
	lsl16 GR1
	addi16 GR1,((wrti_2-1)<<1)
	mov16 Z,GR1
	lpm16i GR1,Z+
	mov16 Z,GR1
	ijmp
wrti_2:
	.dw wtty //ВАЖНО!!! заполнить таблицу!
	.dw touts_test //del


//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
dskw:
	//mov	(sp),r1 / get an i-node number from the stack into r1
	//jsr	r0,iget / write out (if mod), read i-node 'r1' into i-node struc
	//mov	 *u.fofp,r2 / put [(u.off) or the offset in the fsp entry] in r2
	//add	 u.count,r2 /no. of bytes to be written+file offset is put in r2
	//cmp	 r2,i.size / is this greater than the present size of the file?
	//blos	 dskw_1 / no, branch
	//mov	r2,i.size/yes,inc the fi1e size to file offset+no. of data bytes
	//jsr	r0,setimod / set imod=1, stuff tlme of modification into i-node

	pop16 GR1
	push16 GR1
	rcall iget
	lds16 Z,u_fofp
	ldd16 GR2,Z+0
	lds16 GR3,u_count
	add16 GR2,GR3
	lds16 GR3,i_size
	cp16 GR2,GR3
	brlo dskw_1
	breq dskw_1
	sts16 i_size,GR2
	rcall setimod
dskw_1:
	//jsr	r0,mget / get the block no. in which to write the next data byte
	//bit	*u.fofp,$777 / test the lower 9 bits of the file offset
	//bne	dskw_2 / if its non-zero, branch; if zero, file offset = 0,
		   // 512, 1024,...(i.e., start of new block)
	//cmp	u.count,$512. / if zero, is there enough data to fill an
		              // entire block? (i.e., no. of
	//bhis	dskw_3 / bytes to be written greater than 512.? Yes, branch.

	rcall mget
	lds16 Z,u_fofp
	ldd16 GR3,Z+0
	andi GR3L,63 //test the lower 6 bits (block = 64 bytes)
	brne dskw_2
	ldi16 GR3,64
	lds16 GR4,u_count
	cp16 GR4,GR3
	brsh dskw_3
		   // Don't have to read block
dskw_2:  //in as no past info. is to be saved (entire block overwrtn).
	//jsr	r0,dskrd / no, must retain old info.. Hence, read block 'r1'
		         // into an I/O buffer
	rcall dskrd
dskw_3:
	//jsr	r0,wslot / set write and inhibit bits in I/O queue, proc.
		         // status=0, r5 points to 1st word of data
	//jsr	r0,sioreg / r3 = no. of bytes of data, r1 = address of data,
		          // r2 points to location in buffer in which to
		          // start writing data

	rcall wslot
	rcall sioreg

	mov16 Y,GR1 
	mov16 Z,GR2
dskw_4:
	//movb	(r1 )+,(r2)+ / transfer a byte of data to the I/O buffer
	//dec	r3 / decrement no. of bytes to be written
	//bne	dskw_4 / have all bytes been transferred? No, branch
	//jsr	r0,dskwr / yes, write the block and the i-node
	//tst	u.count / any more data to write?
	//bne	dskw_1 / yes, branch
	//jmp	ret1 / no, return to the caller via 'ret'

	ld GR4L,Y+
	st Z+,GR4L
	dec16 GR3
	brne dskw_4
	rcall dskwr
	lds16 GR3,u_count
	tst16 GR3
	brne dskw_1
	rjmp ret1

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
sysfstat: // set status of open file
	//jsr	r0,arg; u.off / put buffer address in u.off
	//mov	u.off,-(sp) / put buffer address on the stack
	//mov	*u.r0,r1 / put file descriptor in r1
	//jsr	r0,getf / get the files i-number
	//tst	r1 / is it 0?
	//beq	error3 / yes, error
	//bgt	sst_1 / if i-number is negative (open for writing)
	//neg	r1 / make it positive, then branch
	//br	sst_1 / to 1f

	sts16 u_off,GR2
	mov16 GR0,GR2
	lds16 Z,u_r0
	ldd16 GR1,Z+0
	rcall getf
	tst16 GR1
	breq error6
	brge sst_1
	neg16 GR1
	rjmp sst_1

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
sysstat:
	//jsr	r0,arg2 / get the 2 arguments
	//jsr	r0,namei / get the i-number for the file
	//br error3 / no such file, error

	sts16 u_namep,GR1 //arg2
	sts16 u_off,GR2   //arg2
	mov16 GR0,GR2     //arg2
	rcall namei
	rjmp error4
sst_1:
	//jsr	r0,iget / get the i-node into core
	//mov	(sp)+,r3 / move u.off to r3 (points to buffer)
	//mov	r1,(r3)+ / put i-number in 1st word of buffer
	//mov	$inode,r2 / r2 points to i-node

	rcall iget
	mov16 Z,GR0
	st16i Z+,GR1
	ldi16 Y,inode

	ldi16 GR4,(inode+32)
sst_2:
	//mov	(r2)+,(r3)+ / move rest of i-node to buffer
	//cmp	r2,$inode+32 / done?
	//bne	sst_2 / no, go back
	//br	sysret3 / return through sysret

	ld GR3L,Y+
	st Z+,GR3L
	cp16 Y,GR4
	brne sst_2
	rjmp sysret4

error6:
	jmp error
sysret6:
	jmp sysret

//INPUT: GR1 - i-number 
//OUTPUT: GR1 - i-number; Z - offset of byte block; GR4 - mask(mq)
//USED: GR2,GR3 - temp 
//ACTION:
//NOTE:
imap: // get the byte that has the allocation bit for the i-number contained
      // in r1
	;mov	$1,mq / put 1 in the mq
	//mov	r1,r2 / r2 now has i-number whose byte in the map we  must find
	//sub	$41.,r2 / r2 has i-41
	//mov	r2,r3 / r3 has i-41
	//bic	$!7,r3 / r3 has (i-41) mod 8 to get the bit position
	;mov	r3,lsh / move the 1 over (i-41) mod 8 positions to the left
		       // to mask the correct bit
	//asr	r2
	//asr	r2
	//asr	r2 / r2 has (i-41) base 8 of the byte no. from the start 
	           //of the map
	//mov	r2,-(sp) / put (i-41) base 8 on the stack
	//mov	$systm,r2 / r2 points to the in-core image of the super
			  // block for drum
	//tst	cdev / is the device the disk
	//beq	imap_1 / yes
	//add	$mount-systm,r2 / for mounted device, r2 points to 1st word
				// of its super block
	mov16 GR2,GR1
	subi16 GR2,41
	mov16 GR3,GR2
	andi16 GR3,$0007
	ldi16 Z,(bitmask<<1)
	add16 Z,GR3
	lpm GR4L,Z
	clr GR4H
	lsr16 GR2
	lsr16 GR2
	lsr16 GR2 //GR2 = (sp)
	ldi16 Z,systm //Z = r2
	lds16 GR3,cdev
	tst16 GR3
	breq imap_1
	addi16 Z,(mount-systm)

imap_1:
	//add	(r2)+,(sp) / get byte address of allocation bit
	//add	(sp)+,r2 / ?
	//add	$2,r2 / ?
	//rts	r0

	ld16i GR3,Z+
	add16 GR2,GR3
	add16 Z,GR3
	addi16 Z,$0002
	ret

//INPUT: GR1 - i-number 
//OUTPUT: None
//USED: GR1; Z - i.dskp(r2),param copyz; Y - buffer,par copyz,temp;GR3 -temp
//ACTION:
//NOTE:
itrunc:
	//jsr	r0,iget
	//mov	$i.dskp,r2 / address of block pointers in r2

	rcall iget
	ldi16 Z,i_dskp
itrnc_1:
	//mov	(r2)+,r1 / move physical block number into r1
	//beq	itrnc_5
	//mov	r2,-(sp)
	//bit	$10000,i.flgs / test large file bit?
	//beq	itrnc_4 / if clear, branch
	//mov	r1,-(sp) / save block number of indirect block
	//jsr	r0,dskrd / read in block, 1st data word pointed to by r5
	//mov	$256.,r3 / move word count into r3

	ld16i GR1,Z+
	tst16 GR1
	breq itrnc_5
	push16 Z
	lds16 GR3,i_flgs
	andi GR3H,$10
	breq itrnc_4
	push16 GR1
	rcall dskrd
	ldi GR3L,32  //32 points on one block
itrnc_2:
	//mov	(r5)+,r1 / put 1st data word in r1; physical block number
	//beq	itrnc_3 / branch if zero
	//mov	r3,-(sp) / save r3, r5 on stack
	//mov	r5,-(sp)
	//jsr	r0,free / free block in free storage map
	//mov	(sp)+,r5
	//mov	(sp)+,r3

	ld16i GR1,Y+
	tst16 GR1
	breq itrnc_3
	push GR3L
	push16 Y
	rcall free_blk
	pop16 Y
	pop GR3L
itrnc_3:
	//dec	r3 / decrement word count
	//bgt	itrnc_2 / branch if positive
	//mov	(sp)+,r1 / put physical block number of indirect block

	dec GR3L
	tst GR3L
	breq itrnc_3_1
	brge itrnc_2
itrnc_3_1:
	pop16 GR1
itrnc_4:
	//jsr	r0,free / free indirect block
	//mov	(sp)+,r2

	rcall free_blk
	pop16 Z
itrnc_5:
	//cmp	r2,$i.dskp+16.
	//bne	itrnc_1 / branch until all i.dskp entries check
	//bic	$10000,i.flgs / clear large file bit
	//clr	i.size / zero file size
	//jsr	r0,copyz; i.dskp; i.dskp+16. / zero block pointers
	//jsr	r0,setimod / set i-node modified flag
	//mov	ii,r1
	//rts	r0

	ldi16 GR3,(i_dskp+16)
	cp16 Z,GR3
	lds16 GR3,i_flgs
	andi GR3H,~$10
	sts16 i_flgs,GR3
	clr16 GR3
	sts16 i_size,GR3
	ldi16 Y,i_dskp
	ldi16 Z,(i_dskp+16)
	rcall copyz
	rcall setimod
	lds16 GR1,ii
	ret

//INPUT: GR1 - i-number
//OUTPUT: None
//USED: Y - fsp;
//ACTION:
//NOTE:
anyi: // r1 contains an i-number
	//mov	$fsp,r2 / move start of fsp table to r2

	ldi16 Y,fsp
anyi_1:
	//cmp	r1,(r2) / do i-numbers match?
	//beq	anyi_3 / yes, 1f
	//neg	r1 / no complement r1
	//cmp	r1,(r2) / do they match now?
	//beq	anyi_3 / yes, transfer i-numbers do not match
	//add	$8,r2 / no, bump to next entry in fsp table
	//cmp	r2,$fsp+[nfiles*8] / are we at last entry in the table
	//blt	anyi_1 / no, check next entries i-number
	//tst	r1 / yes, no match
	//bge	anyi_2
	//neg	r1 / make i-number positive

	ldd16 GR3,Y+0
	cp16 GR1,GR3
	breq anyi_3
	neg16 GR1
	cp16 GR1,GR3
	breq anyi_3
	addi16 Y,$0008
	ldi16 GR3,(fsp+nfiles*8)
	cp16 Y,GR3
	brlo anyi_1 //ВАЖНО!!! brlt?
	tst16 GR1
	brge anyi_2
	neg16 GR1
anyi_2:
	//jsr	r0,imap / get address of allocation bit in the i-map in r2
	//bicb	mq,(r2) / clear bit for i-node in the imap
	//jsr	r0,itrunc / free all blocks related to i-node
	//clr	i.flgs / clear all flags in the i-node
	//rts	r0 / return

	rcall imap
	ldd16 GR3,Z+0
	com16 GR4      //invert value
	and16 GR3,GR4
	std16 Z+0,GR3
	rcall itrunc
	clr16 GR3
	sts16 i_flgs,GR3
	ret

anyi_3: // i-numbers match
	//incb	7(r2) / increment upper byte of the 4th word
	//rts	r0 / in that fsp entry (deleted flag of fsp entry)

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
fclose://nok-ok
	//mov	r1,-(sp) / r1 (it contains the index to u.fp list)
	//jsr	r0,getf / r1 contains i-number, cdev has device =, u.fofp 
		            // points to 3rd word of fsp entry
	//tst	r1 / is inumber 0?
	//beq	fcls_2 / yes, i-node not active so return
	//tst	(r0)+ / no, jump over error return
	//mov	r1,r2 / move i-number to r2
	//mov	(sp),r1/restore val. of r1 from the stack which is index to u.fp
	//clrb	u.fp(r1) / clear that entry in the u.fp list
	//mov	u.fofp,r1 / r1 points to 3rd word in fsp entry
	//decb	2(r1) / dec. the number of processes that have opened the file
	//bge	fcls_2 / if all processes haven't closed the file, return
	//mov	r2,-(sp) / put r2 on the stack (i-number)
	//clr	-4(r1) / clear 1st word of fsp entry
	//tstb	3(r1) / has this file been deleted
	//beq	fcls_1 / no, branch
	//mov	r2,r1 / yes, put i-number back into r1
	//jsr	r0,anyi / free all blocks related to i-number
			// check if file appears in fsp again

	push16 GR1
	rcall getf
	tst16 GR1
	breq fcls_2
	skip GR3,2  //stack + 2
	mov16 GR2,GR1
	pop16 GR1
	push16 GR1
	clr GR3L
	mov16 Z,GR1   //Z instead r1
	addi16 Z,u_fp
	std Z+0,GR3L
	lds16 Z,u_fofp //Z instead r1
	ldd GR3L,Z+2
	dec GR3L
	std Z+2,GR3L
	brge fcls_2
	push16 GR2       //+/- bnok
	subi16 Z,$0004
	clr16 GR3
	std16 Z+0,GR3
	ldd GR3L,Z+7
	tst GR3L
	breq fcls_1
	mov16 GR1,GR2   //enok
	rcall anyi      //nok
fcls_1:
	//mov	(sp)+,r1 / put i-number back into r1
	//jsr	r0,iclose / check to see if its a special file

	pop16 GR1
	rcall iclose //enok
fcls_2:
	//mov	(sp)+,r1 / put index to u.fp back into r1
	//rts	r0

	pop16 GR1
	ret

//INPUT: GR1 - descriptor of file
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
sysclose: // close the file
	//mov    *u.r0,r1 / move index to u.fp list into r1
	//jsr    r0,fclose / close the file
	//br 	 error2 / unknown file descriptor
	//br     sysret2

	rcall fclose
	rjmp error6
	rjmp sysret6

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
syscreat: // name; mode
	//jsr    r0,arg2 / put file name in u.namep put mode on stack
	//jsr    r0,namei / get the i-number
	//br  scrt_1 / if file doesn't exist 2f
	//neg    r1 / if file already exists make i-number negative
                  // (open for writing)
	//jsr    r0,iopen /
	//jsr    r0,itrunc / truncate to 0 length
	//br     op0

	sts16 u_namep,GR1 //arg2
	sts16 u_off,GR2   //arg2
	mov16 GR0,GR2        //arg2
	rcall namei
	rjmp scrt_1
	neg16 GR1
	rcall iopen
	rcall itrunc
	rjmp op0
scrt_1: // file doesn't exist
	//mov    (sp)+,r1 / put the mode in r1
	//bic    $!377,r1 / clear upper byte
	//jsr    r0,maknod / make an i-node for this file
	//mov    u.dirbuf,r1 / put i-number for this new file in r1
	//br     op0 / open the file

	mov16 GR1,GR0
	clr GR1H
	rcall maknod
	lds16 GR1,u_dirbuf
	rjmp op0

//INPUT: None
//OUTPUT: None
//USED: Y,Z - param. copyz,u_namep,u_dirbuf+10 ;GR4 - u_dirbuf+10,GR3 - temp
//ACTION:
//NOTE:
mkdir:
	//jsr	r0,copyz; u.dirbuf+2; u.dirbuf+10. / clear this
	//mov	u.namep,r2 / r2 points to name of directory entry
	//mov	$u.dirbuf+2,r3 / r3 points to u.dirbuf+2

	ldi16 Y,(u_dirbuf+2)
	ldi16 Z,(u_dirbuf+10)
	rcall copyz
	ldi16 Y,u_namep
	ldi16 Z,u_dirbuf+2
mkd_1: // put characters in the directory name in u.dirbuf+2 - u.dirbuf+10
	//movb	 (r2)+,r1 / move character in name to r1
	//beq	mkd_2 / if null, done
	//cmp	r1,$'/ / is it a "/"?
	//beq	error9 / yes, error
	//cmp	r3,$u.dirbuf+10. / have we reached the last slot for a char?
	//beq	mkd_1 / yes, go back
	//movb	r1,(r3)+ / no, put the char in the u.dirbuf
	//br	mkd_1 / get next char

	ld GR3L,Y+
    tst GR3L
	breq mkd_2
	cpi GR3L,'/'
	breq error7
	ldi16 GR4,u_dirbuf+10
	cp16 Z,GR4
	breq mkd_1
	st Z+,GR3L
	rjmp mkd_1
mkd_2:
	//mov	u.dirp,u.off / pointer to empty current directory slot to u.off
	lds16 GR3,u_dirp
	sts16 u_off,GR3

//INPUT: None
//OUTPUT: None
//USED: GR3 - temp ;X -param. of access; GR1 - i-number of directory
//ACTION:
//NOTE:			     
wdir:
	//mov	$u.dirbuf,u.base / u.base points to created file name
	//mov	$10.,u.count / u.count = 10
	//mov	ii,r1 / r1 has i-number of current directory
	//jsr	r0,access; 1 / get i-node and set its file up for writing
	//jsr	r0,writei / write into directory
	//rts	r0

	lds16 GR3,u_dirbuf
	sts16 u_base,GR3
	ldi16 GR3,10
	sts16 u_count,GR3
	lds16 GR1,ii
	push16 X
	ldi16 X,$0001
	rcall access
	pop16 X
	rcall writei
	ret

error7:
	jmp error
sysret7:
	jmp sysret

//INPUT: GR1 - mode(flags of file)
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
maknod: // r1 contains the mode
	//bis	$100000,r1 / allocate flag set
	//mov	r1,-(sp) / put mode on stack
	//mov	ii,r1 / move current i-number to r1
	//jsr	r0,access; 1 / get its i-node into core
	//mov	r1,-(sp) / put i-number on stack
	//mov	$40.,r1 / r1 = 40

	ori GR1H,$80
	push16 GR1
	lds16 GR1,ii
	push16 X
	ldi16 X,$0001
	rcall access
	pop16 X
	push16 GR1
maknod_1:  // scan for a free i-node (next 4 instructions)
	//inc	r1 / r1 = r1 + 1
	//jsr	r0,imap / get byte addr. and bit position in inode map in r2 & m
	//bitb	mq,(r2) / is the i-node active
	//bne	maknod_1 / yes, try the next one
	//bisb	mq,(r2) / no, make it active (put a 1 in the bit map)
	//jsr	r0,iget / get i-node into core
	//tst	i.flgs / is i-node already allocated
	//blt	maknod_1 / yes, look for another one
	//mov	r1,u.dirbuf / no, put i-number in u.dirbuf
	//mov	(sp)+,r1 / get current i-number back
	//jsr	r0,iget / get i-node in core
	//jsr	r0,mkdir / make a directory entry in current directory
	//mov	u.dirbuf,r1 / r1 = new inode number
	//jsr	r0,iget / get it into core
	//jsr	r0,copyz; inode; inode+32. / 0 it out
	//mov	(sp)+,i.flgs / fill flags
	//movb	u.uid,i.uid / user id
	//movb	$1,i.nlks / 1 link
	//mov	s.time,i.ctim / time created
	//mov	s.time+2,i.ctim+2 / time modified
	//jsr	r0,setimod / set modified flag
	//rts	r0 / return

	inc16 GR1
	rcall imap
	ld GR3L,Z
	mov GR3H,GR3L
	and GR3L,GR4L
	brne maknod_1
	and GR3H,GR4L
	st Z,GR3H
	rcall iget
	lds16 GR3,i_flgs
	tst16 GR3
	brlt maknod_1
	sts16 u_dirbuf,GR1
	pop16 GR1
	rcall iget
	rcall mkdir
	lds16 GR1,u_dirbuf
	rcall iget
	ldi16 Y,inode
	ldi16 Z,(inode+32)
	rcall copyz
	pop16 GR3
	sts16 i_flgs,GR3
	lds16 GR3,u_uid
	sts16 i_uid,GR3
	ldi GR3L,1
	sts i_nlks,GR3L
	lds16 GR3,s_time
	sts16 i_ctim,GR3
	lds16 GR3,(s_time+2)
	sts16 (i_ctim+2),GR3
	rcall setimod
	ret

//INPUT: None
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
sysexit: // terminate process
	//clr	u.intr / clear interrupt control word
	//clr	r1 / clear r1

	clr16 GR3
	sts16 u_intr,GR3
	clr16 GR1
sext_1:// r1 has file descriptor (index to u.fp list)  Search the whole list
	//jsr	r0,fclose / close all files the process opened
	;br .+2 / ignore error return

	rcall fclose
	nop
sext_2:
	//inc	r1 / increment file descriptor
	//cmp	r1,$10. / end of u.fp list?
	//blt	sext_1 / no, go back
	//movb	u.uno,r1 / yes, move dying process's number to r1
	//clrb	 p.stat-1(r1) / free the process
	//asl	r1 / use r1 for index into the below tables
	//mov	p.pid-2(r1),r3 / move dying process's name to r3
	//mov	p.ppid-2(r1),r4 / move its parents name to r4
	//clr	r2
	//clr	r5 / initialize reg

	inc GR1L
	cpi GR1L,10
	brlt sext_1
	lds GR1L,u_uno
	clr GR1H
	mov16 Z,GR1
	addi16 Z,(p_stat-1)
	clr GR3L
	std Z+0,GR3L
	lsl16 GR1
	mov16 Z,GR1
	addi16 Z,(p_pid-2)
	ldd16 Y,Z+0  //Y - r3
	mov16 Z,GR1
	addi16 Z,(p_ppid-2)
	ldd16 GR4,Z+0
	clr16 GR2
	clr16 GR0 // GR0 - r5

sext_3://find children of this dying process, if they are zombies, free them
	//add	$2,r2 / search parent process table for dying process's name
	//cmp	p.ppid-2(r2),r3 / found it?
	//bne	sext_5 / no
	//asr	r2 / yes, it is a parent
	//cmpb	p.stat-1(r2),$3 / is the child of this dying process a zombie
	//bne	sext_4 / no
	//clrb	p.stat-1(r2) / yes, free the child process

	addi16 GR2,$0002
	mov16 Z,GR2
	addi16 Z,(p_ppid-2)
	ldd16 GR3,Z+0
	cp16 GR3,Y
	brne sext_5
	lsr16 GR2              //bnok
	mov16 Z,GR2
	addi16 Z,(p_stat-1)
	ld GR3L,Z
	cpi GR3L,3
	brne sext_4
	clr GR3L
	st Z,GR3L              //enok
sext_4:
	//asl	r2

	lsl16 GR2              //nok
sext_5:  // search the process name table for the dying process's parent
	//cmp	p.pid-2(r2),r4 / found it?
	//bne	sext_6 / no
	//mov	r2,r5 /yes,put index to p.pid table (parents process # x2) in r5

	mov16 Z,GR2
	addi16 Z,(p_pid-2)
	ldd16 GR3,Z+0
	cp16 GR3,GR4
	brne sext_6
	mov16 GR0,GR2
sext_6:
	//cmp	r2,$nproc+nproc / has whole table been searched?
	//blt	sext_3 / no, go back
	//mov	r5,r1 / yes, r1 now has parents process # x2
	//beq	sext_7 / no parent has been found. The process just dies
	//asr	r1 / set up index to p.stat
	//movb	p.stat-1(r1),r2 / move status of parent to r2
	//beq	sext_7 / if its been freed, 2f
	//cmp	r2,$3 / is parent a zombie?
	//beq	sext_7 / yes, 2f
	//movb	u.uno,r3 / move dying process's number to r3
	//movb	$3,p.stat-1(r3) / make the process a zombie
	//cmp	r2,$2 / is the parent waiting for this child to die
	//bne	sext_7 / yes, notify parent not to wait any more
	//decb	p.stat-1(r1) / awaken it by putting it (parent)
	//mov	$runq+4,r2 / on the runq
	//jsr	r0, putlu

	ldi16 GR3,(nproc+nproc)
	cp16 GR2,GR3
	brlt sext_3
	mov16 GR1,GR0
	tst16 GR1
	breq sext_7
	lsr16 GR1
	mov16 Z,GR1
	addi16 Z,(p_stat-1)
	clr GR2H
	ld GR2L,Z
	tst GR2L
	breq sext_7
	cpi GR2L,3
	breq sext_7
	clr YH
	lds YL,u_uno
	mov16 Z,Y
	addi16 Z,(p_stat-1)
	ldi16 GR3,3
	st Z,GR3L
	dec GR3L
	cp16 GR2,GR3
	brne sext_7
	mov16 Z,GR1
	addi16 Z,(p_stat-1)
	ld GR3L,Z
	dec GR3L
	st Z,GR3L
	ldi16 GR2,runq+4
	call putlu

sext_7:  // the process dies
	//clrb	u.uno / put zero as the process number, so "swap" will
	//jsr	r0,swap / overwrite process with another process
	//0	/ and thereby kill it; halt?

	clr GR3L
	sts u_uno,GR3L
	call swap_
	break

//INPUT: None
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
syswait://ok // wait for a process to die
	//movb	u.uno,r1 / put parents process number in r1
	//asl	r1 / x2 to get index into p.pid table
	//mov	p.pid-2(r1),r1 / get the name of this process
	//clr	r2
	//clr	r3 / initialize reg 3
	
	clr GR1H
	lds GR1L,u_uno
	lsl16 GR1
	mov16 Z,GR1
	addi16 Z,(p_pid-2)
	ldd16 GR1,Z+0
	clr16 GR2
	clr16 GR3
swait_1:
	//add	$2,r2 / use r2 for index into p.ppid table / search table
		      // of parent processes for this process name
	//cmp	p.ppid-2(r2),r1 / r2 will contain the childs process number
	//bne	swait_3 / branch if no match of parent process name
	//inc	r3 / yes, a match, r3 indicates number of children
	//asr	r2 / r2/2 to get index to p.stat table
	//cmpb	p.stat-1(r2),$3 / is the child process a zombie?
	//bne	swait_2 / no, skip it
	//clrb	p.stat-1(r2) / yes, free it
	//asl	r2 / r2x2 to get index into p.pid table
	//mov	p.pid-2(r2),*u.r0 / put childs process name in (u.r0)
	//br	sysret1 / return cause child is dead

	addi16 GR2,$0002
	mov16 Z,GR2
	addi16 Z,(p_ppid-2)
	ldd16 GR4,Z+0
	cp16 GR4,GR1
	brne swait_3
	inc16 GR3
	lsr16 GR2
	mov16 Z,GR2
	addi16 Z,(p_stat-1)
	ld GR4L,Z
	cpi GR4L,3
	brne swait_2
	clr GR4L            
	st Z,GR4L
	lsl16 GR2
	mov16 Z,GR2
	addi16 Z,(p_pid-2)
	ldd16 GR4,Z+0
	lds16 Z,u_r0
	std16 Z+0,GR4
	rjmp sysret8        

swait_2:
	//asl	r2 / r2x2 to get index into p.ppid table

	lsl16 GR2
swait_3:
	//cmp	r2,$nproc+nproc / have all processes been checked?
	//blt	swait_1 / no, continue search
	//tst	r3 / one gets here if there are no children or children
		   // that are still active
	//beq	error1 / there are no children, error
	//movb	u.uno,r1 / there are children so put parent process number in r1
	//incb	p.stat-1(r1) / it is waiting for other children to die
	//jsr	r0,swap / swap it out, because it's waiting
	//br	syswait / wait on next process

	ldi16 GR4,(nproc+nproc)
	cp16 GR2,GR4
	brlt swait_1
	tst16 GR3
	breq error8
	clr GR1H
	lds GR1L,u_uno
	mov16 Z,GR1
	addi16 Z,(p_stat-1)
	ldd GR4L,Z+0
	inc GR4L
	std Z+0,GR4L
	call swap_
	rjmp syswait

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
isown:
	//jsr	r0,arg2 / u.namep points to file name
	//jsr	r0,namei / get its i-number
	//br error3
	//jsr	r0,iget / get i-node into core
	//tstb	u.uid / super user?
	//beq	isown_1 / yes, branch
	//cmpb	i.uid,u.uid / no, is this the owner of the file
	//beq	isown_1 / yes
	//jmp	error3 / no, error

	sts16 u_namep,GR1 //arg2
	sts16 u_off,GR2   //arg2
	mov16 GR0,GR2        //arg2
	rcall namei
	rjmp error8
	rcall iget
	lds GR3L,u_uid
	tst GR3L
	breq isown_1
	lds GR3H,i_uid
	cp GR3H,GR3L
	breq isown_1
	rjmp error8
isown_1:
	//jsr	r0,setimod / indicates i-node has been modified
	//mov	(sp)+,r2 / mode is put in r2 (u.off put on stack with 2nd arg)
	//rts	r0

	rcall setimod
	mov16 GR2,GR0
	ret

error8:
	jmp error
sysret8:
	jmp sysret
//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
syschmod: // name; mode
	//jsr	r0,isown / get the i-node and check user status
	//bit	$40000,i.flgs / directory?
	//beq	schmd_1 / no
	//bic	$60,r2 / su & ex / yes, clear set user id and executable modes

	rcall isown
	lds16 GR3,i_flgs
	andi GR3H,$40
	breq schmd_1
	andi GR2L,~$30
schmd_1:
	//movb	r2,i.flgs / move remaining mode to i.flgs
	//br	schwn_2

	sts16 i_flgs,GR2
	rjmp schwn_2

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
syschown: // name; owner
	//jsr	r0,isown / get the i-node and check user status
	//tstb	u.uid / super user
	//beq	schwn_1 / yes, 2f
	//bit	$40,i.flgs / no, set userid on execution?
	//bne	schwn_3 / yes error, could create Trojan Horses

	rcall isown
	lds GR3L,u_uid
	tst GR3L
	breq schwn_1
	andi GR3L,$20
	brne schwn_3

schwn_1:
	//movb	r2,i.uid / no, put the new owners id in the i-node

	sts i_uid,GR2L
schwn_2:
	//jmp	sysret4

	rjmp sysret8
schwn_3:
	//jmp	error

	rjmp error8

//INPUT: GR1 - pointer of time cells
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
systime: // get time of year
	//mov	s.time,4(sp)
	//mov	s.time+2,2(sp) / put the present time on the stack
	//br	sysret4
	lds16 Z,u_r0
	std16 Z+0,GR1	

	mov16 Z,GR1
	ldd16 GR3,Z+0
	mov16 Z,GR3
	lds16 GR3,s_time
	std16 Z+0,GR3
	lds16 GR3,(s_time+2)
	std16 Z+2,GR3

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
sysgetuid:
	//movb	u.ruid,*u.r0 / move the real user id to (u.r0)
	//br	sysret4 / systerm return, sysret

	lds16 Z,u_r0
	lds GR3L,u_ruid
	clr GR3H
	std16 Z+0,GR3
	rjmp sysret8

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
syschdir: // makes the directory specified in the argument the current 
	      // directory
	//jsr	r0,arg; u.namep / u.namep points to path name
	//jsr	r0,namei / find its i-number
	//br error3
	//jsr	r0,access; 2 / get i-node into core
	//bit	$40000,i.flgs / is it a directory?
	//beq	error3 / no error
	//mov	r1,u.cdir / move i-number to users current directory
	//mov	cdev,u.cdev / move its device to users current device
	//br	sysret3

	sts16 u_namep,GR1
	rcall namei
	rjmp error8
	push16 X
	ldi16 X,$0002
	rcall access
	pop16 X
	lds16 GR3,i_flgs
	andi GR3H,$40
	breq error9
	sts16 u_cdir,GR1
	lds16 GR3,cdev
	sts16 u_cdev,GR3
	rjmp sysret8

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
sysintr://nok-ok // set interrupt handling
	//jsr	r0,arg; u.intr / put the argument in u.intr
	//br	squit_1 / go into quit routine

	sts16 u_intr,GR1
	rjmp squit_1

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
sysquit://nok-ok
	//jsr	r0,arg; u.quit / put argument in u.quit

	sts16 u_quit,GR1
squit_1:
	//mov	u.ttyp,r1 / move pointer to control tty buffer to r1
	//beq	sysret4 / return to user
	//clrb	6(r1) / clear the interrupt character in the tty buffer
	//br	sysret4 / return to user

	lds16 GR1,u_ttyp
	tst16 GR1
	breq sysret9
	mov16 Z,GR1 //bnok
	clr GR3L
	std Z+6,GR3L
	rjmp sysret8 //enok


error9:
	jmp error
sysret9:
	jmp sysret
	
//INPUT: XL - offset(not address!) run queue (0;2;4)
      //XH - is the number of the wait channel of the process to be awakened
//OUTPUT: None
//USED: X; GR1 - value channel;Y - runq(r2);Z - wait channel(r3);GR3 -temp 
//ACTION:
//NOTE:
wakeup: //wakeup processes waiting for an event by linking them to the queue
	//mov     r1,-(sp) / put char on stack
	//mov     (r0)+,r2 / r2 points to a queue
	//mov     (r0)+,r3 / r3 = wait channel number
	//movb    wlist(r3),r1 / r1 contains process number in that wait
                            // channel that was sleeping
	//beq     wakeup_2 / if 0 return, nothing to wakeup
	//cmp     r2,u.pri / is runq greater than or equal to users process
       	                // priority
	//bhis    wakeup_1 / yes, don't set time quantum to zero
	//clrb    uquant / time quantum = 0
	push16 GR1
	clr YH
	mov YL,XL
	addi16 Y,runq
	clr ZH
	mov ZL,XH
	addi16 Z,wlist
	clr GR1H
	ld GR1L,Z
	tst GR1L
	breq wakeup_2
	lds16 GR3,u_pri
	cp16 Y,GR3
	brsh wakeup_1
	clr GR3L
	sts uquant,GR3L
wakeup_1:
	//clrb    wlist(r3) / zero wait channel entry
	//jsr     r0,putlu / create a link from the last user on the Q
                        // to this process number that got woken
	clr GR3L
	st Z,GR3L
	mov16 GR2,Y
	clr GR1H
	call putlu

wakeup_2:
	//mov     (sp)+,r1 / restore r1
	//rts     r0
	
	pop16 GR1
	ret

//INPUT: None
//OUTPUT: None
//USED: X - arg. of wakeup,getc; GR1 - output char; GR3 - temp
//ACTION:
//NOTE:
startty: // start or restart console tty output
	//cmpb    cc+1,$5.
	//bhi     strtty_1 / branch to 1f when character count on tty (
                  // output) list is greater than 5.
	//jsr     r0,wakeup; runq+2; 1

	lds GR3L,cc+1
	cpi GR3L,$5
	breq strtty_0
	brsh strtty_1
strtty_0:
	push16 X
	ldi XL,2 //runq+2
	ldi XH,1
	rcall wakeup
	pop16 X

strtty_1:
	//tstb    *$tps / test console output ready bit
	//bge     strtty_5 / branch if ready bit is clear
	//tstb    toutt+0 / is toutt for console a zero
	//bne     strtty_5 / if not; branch to 2f
	//movb    ttyoch,r1 / put character to be output in r1
	//bne     strtty_2
	//jsr     r0,getc; 1/if char is nul, get a char from console output list
	//br strtty_5 / if console output list is empty, branch to 2f

	lds GR3L,UCSR1A
	andi GR3L,1<<UDRE1
	breq strtty_5
	lds GR3L,toutt+0
	tst GR3L
	brne strtty_5
	lds GR1L,ttyoch
	tst GR1L
	brne strtty_2
	push16 X
	ldi16 X,$0001
	rcall getc
	rjmp getc_1_startty
	rjmp getc_2_startty
getc_1_startty:
	pop16 X
	rjmp strtty_5
getc_2_startty:
	pop16 X

strtty_2:
	//clrb    ttyoch
	//mov     r1,*$tpb / put character in console output register
	//cmp     r1,$12 / is char a line feed
	//bne     strtty_3
	//movb    $15,ttyoch / put a cr in ttyoch

	clr GR3L
	sts ttyoch,GR3L
	sts UDR1,GR1L
	cpi GR1L,$0A
	brne strtty_3
	ldi GR3L,$0D
	sts ttyoch,GR3L
strtty_3:
	//cmp     r1,$11 / char = ht
	//bne     strtty_4
	//movb    $15.,toutt+0 / set time out to 15 clock tics

	cpi GR1L,$09
	brne strtty_4
	ldi GR3L,1     //$0D(in original)
	sts toutt+0,GR3L
strtty_4:
	//cmp     r1,$15 / char = cr
	//bne     strtty_5
	//movb    $15.,toutt+0 / set time out to 15 clock ticks

	cpi GR1L,$0D
	brne strtty_5
	ldi GR3L,1     // $0D(in original)
	sts toutt+0,GR3L
strtty_5:
	//rts     r0

	ret

//INPUT: GR1(r1) - list identifier; GR2 - clist offset
//OUTPUT: None
//USED: Z(r1),GR3 -temp
//ACTION:
//NOTE: 
put:
	//asr    r2 / divide r2 by 2; r2 is offset in clist
	//mov    r2,-(sp) / save r2 on stack
	//movb   cl+1(r1),r2 / move offset of last char in list (r1) into r2
	//beq    put_1 / offset = 0 then go to 1f (i.e., start a new list)
	//bic    $!377,r2 / zero top half of r2
	//asl    r2 / multiply offset by 2, r2 now has offset in clist
	//movb   (sp),clist-1(r2) / link new list entry to current last entry 
	//br     put_2

	asr16 GR2
	mov16 GR3,GR2  //instead save to stack
	ldi16 Z,(cl+1)
	add16 Z,GR1
	clr GR2H
	ld GR2L,Z
	tst GR2L
	breq put_1
	clr GR2H       //andi16 Y,$00FF
	lsl16 GR2
	ldi16 Z,(clist-1)
	add16 Z,GR2
	st Z,GR3L
	rjmp put_2
put_1:
	//movb   (sp),cf+1(r1) /put new list entry offset into first char offset
	
	ldi16 Z,(cf+1)
	add16 Z,GR1
	st Z,GR3L 
put_2:
	//mov    (sp)+,r2 / pop stack into r2; offset of new list entry in r2
	//movb   r2,cl+1(r1) / make new list entry the last entry in list (r1)
	//asl    r2 / multiply r2 by 2; r2 has clist offset for new list entry
	//rts    r0

	mov16 GR2,GR3
	ldi16 Z,(cl+1)
	add16 Z,GR1
	st Z,GR2L
	lsl16 GR2

	ret

//INPUT: GR1(r1) - list identifier; GR2 - clist offset
//OUTPUT: None
//USED: GR3,Z - temp
//ACTION:
//NOTE:
get:
	//movb   cf+1(r1),r2 / move current first char offset to r2
	//beq    get_2 / no characters in char list
	//tst    (r0)+ / bump r0, second return
	//cmpb   r2,cl+1(r1) / r2 equal to last char offset
	//beq    get_1 / yes, (i.e., entire char list scanned), branch to 1f
	//bic    $!377,r2 / clear bits 8-15 in r2
	//asl    r2 / multiply r2 by 2 to get offset in clist
	//movb   clist-1(r2),cf+1(r1) / move next char in list pointer to
                                    // first char offset ptr
	//br     get_2
	
	ldi16 Z,(cf+1)
	add16 Z,GR1
	clr GR2H
	ld GR2L,Z
	tst GR2L
	breq get_2
	skip GR3,0	;address return on stack +0
	ldi16 Z,(cl+1)
	add16 Z,GR1
	ld GR3L,Z
	cp GR2L,GR3L
	breq get_1
	andi16 GR2,$00FF
	lsl16 GR2
	ldi16 Z,(clist-1)
	add16 Z,GR2
	ld GR3L,Z
	ldi16 Z,(cf+1)
	add16 Z,GR1
	st Z,GR3L
	rjmp get_2
get_1:
	//clrb   cf+1(r1) / clear first char clist offset
	//clrb   cl+1(r1) / clear last char clist offset
	//bic    $!377,r2 / zero top half of r2
	//asl    r2 / multiply r2 by 2

	clr GR3L
	ldi16 Z,(cf+1)
	add16 Z,GR1
	st Z,GR3L
	ldi16 Z,(cl+1)
	add16 Z,GR1
	st Z,GR3L
	andi16 GR2,$00FF
	lsl16 GR2

get_2:
	//rts    r0

	ret

//INPUT: X - list identifier
//OUTPUT: GR2(r2) - clist offset where character stored
//USED: GR1,GR2,GR3,X
//ACTION:
//NOTE:
putc:
	//mov    r1,-(sp) / save char on stack
	//mov    $-1,r1 / put free list list id in r1
	//jsr    r0,get / take char off free list / clist slot taken
                      // identified by r2
	//br 1f / branch when no chars in free list
	//mov    (r0)+,r1 / put putc call arg in r1 (i.e., list identifier)
	//incb   cc(r1) / increment character count for list (r1)
	//jsr    r0,put / put clist entry on list
	//movb   (sp),clist-2(r2) / put character in new entry

	push16 GR1
	ldi16 GR1,-1
	rcall get
	rjmp putc_1
	skip GR3,2	;address return on stack +2
	mov16 GR1,X
	addi16 X,cc
	ld GR3L,X
	inc GR3L
	st X,GR3L
	rcall put
	pop16 GR3
	push16 GR3
	mov16 GR1,GR3
	ldi16 X,clist-2
	add16 X,GR2
	st X,GR3L
putc_1:
	;tst    (r0)+
	;mov    (sp)+,r1
	//rts    r0
	
	pop16 GR1
	ret
	

//INPUT: X - list identifier
//OUTPUT:GR1(r1) - character on top of list; GR2(r2) - clist offset from put
//USED: GR1,GR2,GR3,X
//ACTION:
//NOTE:
getc:// get a character off character list
	//mov    (r0)+,r1 / put argument in getc call in r1 (char list id)
	//jsr    r0,get
	//br getc_1 / empty char list return
	//decb   cc(r1) / decrement number of char in char list
	//mov    $-1,r1 / load minus 1 in r1
	//jsr    r0,put / put char back on free list
	//movb   clist-2(r2),r1 / put char in r1
	//tst    (r0)+ / bump r0 for non blank char list return

	mov16 GR1,X
	rcall get
	rjmp getc_1
	addi16 X,cc
	ld GR3L,X
	dec GR3L
	st X,GR3L
	ldi16 GR1,-1
	rcall put
	ldi16 X,clist-2
	add16 X,GR2
	clr GR1H
	ld GR1L,X
	skip GR3,0	;address return on stack +0
getc_1:
	//rts    r0

	ret

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
cpass: // get next character from user area of core and put it in r1
	//tst	u.count / have all the characters been transferred (i.e.,
		        // u.count, # of chars. left
	//beq	cpass_1 / to be transferred = 0?) yes, branch
	//dec	u.count / no, decrement u.count
	//movb	*u.base,r1 / take the character pointed to by u.base and
		           // put it in r1
	//inc	u.nread / increment no. of bytes transferred
	//inc	u.base / increment the buffer address to point to the
	//rts	r0 / next byte

	lds16 GR3,u_count
	tst16 GR3
	breq cpass_1
	dec16 GR3
	sts16 u_count,GR3
	lds16 Z,u_base
	clr GR1H
	ldd GR1L,Z+0
	inc16 Z
	sts16 u_base,Z
	lds16 GR3,u_nread
	inc16 GR3
	sts16 u_nread,GR3
	ret

cpass_1: 
	//mov	(sp)+,r0 / put return address of calling routine into r0
	//mov	(sp)+,r1 / i-number in r1
	//rts	r0 / non-local return

	pop16 GR3 //return address -> null
	pop16 GR1 //restore i-number
	ret       //non-local return

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
passc:
	//movb	r1,*u.base /move a char. to the next byte of the users buffer
	//inc	u.base / increment the pointer to point to the next byte
		       // in users buffer
	//inc	u.nread / increment the number of bytes read
	//dec	u.count / decrement the number of bytes to be read
	//bne	passc_1 / any more bytes to read?; yes, branch
	//mov	(sp)+,r0 /no, do a non-local return to the caller of 'readi' by:
	//mov	(sp)+,r1 / (2) pop the i-number off the stack into r1

	lds16 Z,u_base
	std Z+0,GR1L
	inc16 Z
	sts16 u_base,Z
	lds16 GR3,u_nread
	inc16 GR3
	sts16 u_nread,GR3
	lds16 GR3,u_count
	dec16 GR3
	sts16 u_count,GR3
	brne passc_1
	pop16 GR3 //return address -> null
	pop16 GR1 //restore i-number
passc_1:
	//clr	*$ps / clear processor status
	//rts	r0 / return to address currently on top of stack

	setpl ipl0
	ret

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
wtty:
	//jsr	r0,cpass / get next character from user buffer area; if
		         // none go to return address in syswrite
	//tst	r1 / is character = null
	//beq	wtty / yes, get next character

	rcall cpass
	tst16 GR1
	breq wtty
wtty_1:
	//mov	$240,*$ps / no, set processor priority to five
	//cmpb	cc+1,$20. / is character count for console tty greater than 20
	//bhis	wtty_2 / yes; branch to put process to sleep
	//jsr	r0,putc; 1 / find place in freelist to assign to console tty and
	//br wtty_2 / place character in list; if none available
		      // branch to put process to sleep
	//jsr	r0,startty / attempt to output character on tty
	//br	wtty

	setpl ipl5
	lds GR3L,cc+1
	cpi GR3L,20
	brsh wtty_2
	push16 X
	ldi16 X,$0001
	rcall putc
	rjmp putc_1_wtty
	rjmp putc_2_wtty
putc_1_wtty:
	pop16 X
	rjmp wtty_2
putc_2_wtty:
	pop16 X
	rcall startty
	rjmp wtty
wtty_2:
	//mov	r1,-(sp) / place character on stack
	//jsr	r0,sleep; 1 / put process to sleep
	//mov	(sp)+,r1 / remove character from stack
	//br	wtty_1 / try again to place character in clist and output

	push16 GR1
	push16 X
	ldi16 X,$0001
	rcall sleep_
	pop16 X
	pop16 GR1
	rjmp wtty_1


//INPUT: 
//OUTPUT: None
//USED: Y(r5) - pointer to address of buffer 
//ACTION:
//NOTE:
rtty: // read from console tty
	//mov	tty+[8*ntty]-8+6,r5 / r5 is the address of the 4th word of
				    // of the control and status block
	//tst	2(r5) / for the console tty; this word points to the console
		      // tty buffer
	//bne	rtty_1 / 2nd word of console tty buffer contains number
	           // of chars. Is this number non-zero?
	//jsr	r0,canon; ttych / if 0, call 'canon' to get a line (120 chars.)

	lds16 Y,tty+(ntty*8)-8+6
	ldd16 GR3,Y+2
	tst16 GR3
	brne rtty_1
	push16 X
	ldi16 X,ttych
	rcall canon
	pop16 X
rtty_1:
	//tst	2(r5) / is the number of characters zero
	//beq	ret1 / yes, return to caller via 'ret1'
	//movb	*4(r5),r1 / no, put character in r1
	//inc	4(r5) / 3rd word of console tty buffer points to byte which
		      // contains the next char.
	//dec	2(r5) / decrement the character count
	//jsr	r0,passc / move the character to core (user)
	//br	rtty_1 / get next character

	ldd16 GR3,Y+2
	tst16 GR3
	breq ret3
	ldd16 Z,Y+4
	ldd GR1L,Z+0
	inc16 Z
	std16 Y+4,Z
	ldd16 GR3,Y+2
	dec16 GR3
	std16 Y+2,GR3
	rcall passc
	rjmp rtty_1
	
ret3:
	rjmp ret1		

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
sleep_: // wait for event
	//jsr     r0,isintr / check to see if interrupt or quit from user
	//br slp_2 / something happened / yes, his interrupt so return to user
	//mov     (r0)+,r1 / put number of wait channel in r1
	//movb    wlist(r1),-(sp) /put old process number in there, on the stack
	//movb    u.uno,wlist(r1) / put process number of process to put
                              // to sleep in there
	//mov     cdev,-(sp) / nothing happened in isintr so
	//jsr     r0,swap_ / swap out process that needs to sleep
	//mov     (sp)+,cdev / restore device
	//jsr     r0,isintr / check for interrupt of new process
	//br      slp_2 / yes, return to new user
	//movb    (sp)+,r1 / no, r1 = old process number that was originally
                        // on the wait channel
	//beq     slp_1 / if 0 branch
	//mov     $runq+4,r2 / r2 points to lowest priority queue
	//mov     $300,*$ps / processor priority = 6
	//jsr     r0,putlu / create link to old process number
	//clr     *$ps / clear the status; process priority = 0

	rcall isintr
	rjmp slp_2
	mov16 Z,X
	addi16 Z,wlist
	ldd GR3L,Z+0
	push GR3L
	lds GR3L,u_uno
	std Z+0,GR3L
	lds16 GR3,cdev
	push16 GR3
	call swap_
	pop16 GR3
	sts16 cdev,GR3
	rcall isintr 
	rjmp slp_2
	pop GR1L
	clr GR1H
	tst GR1L
	breq slp_1
	ldi16 GR2,runq+4
	setpl ipl6
	call putlu
	setpl ipl0

slp_1:
	//rts     r0 / return

	ret
slp_2:
	//jmp     sysret / return to user

	jmp sysret


//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
isintr:
	//mov     r1,-(sp) / put number of wait channel on the stack
	//mov     r2,-(sp) / save r2
	//mov     u.ttyp,r1 /r1 =pointer to buffer of process control typewriter
	//beq     isintr_1 / if 0, do nothing except skip return
	//movb    6(r1),r1 / put interrupt char in the tty buffer in r1
	//beq     isintr_1 / if its 0 do nothing except skip return
	//cmp     r1,$177 / is interrupt char = delete?
	//bne     isintr_3 / no, so it must be a quit (fs)
	//tst     u.intr /yes, value of u.intr determines handling of interrupts
	//bne     isintr_4 / if not 0, 2f. If zero do nothing.

	push16 GR1 //?
	push16 GR2 //?

	lds16 Z,u_ttyp
	tst16 Z
	breq isintr_1
	clr GR1H
	ldd GR1L,Z+6
	breq isintr_1
	cpi GR1L,$7F
	brne isintr_3
	lds16 GR3,u_intr
	tst16 GR3
	brne isintr_4

isintr_1:
	//tst     (r0)+ / bump r0 past system return (skip)

	skip GR3,4	;address return on stack +4
isintr_2:
	//mov     (sp)+,r2 / restore r1 and r2
	//mov     (sp)+,r1
	//rts     r0

	pop16 GR2
	pop16 GR1
	ret
isintr_3: 	//interrupt char = quit (fs)
	//tst     u.quit / value of u.quit determines handling of quits
	//beq     isintr_1 / u.quit = 0 means do nothing

	lds16 GR3,u_quit
	tst16 GR3
	breq isintr_1
isintr_4: 	//get here because either u.intr <> 0 or u.qult <> O
	//mov     $tty+6,r1 / move pointer to tty block into r1

	ldi16 Z,tty+6
isintr_5: 	//find process control tty entry in tty block
	//cmp     (r1),u.ttyp / is this the process control tty buffer?
	//beq     isintr_6 / block found go to 1f
	//add     $8,r1 / look at next tty block
	//cmp     r1,$tty+[ntty*8]+6 / are we at end of tty blocks
	//blo     isintr_5 / no
	//br      isintr_2 / no process control tty found so go to 4b

	lds16 GR3,u_ttyp
	ldd16 GR1,Z+0
	cp16 GR1,GR3
	breq isintr_6
	addi16 Z,$0008
	ldi16 GR1,tty+(ntty*8)+6
	cp16 Z,GR3
	brlo isintr_5
	rjmp isintr_2
isintr_6:
	//mov     $240,*$ps / set processor priority to 5
	//movb    -3(r1),0f / load getc call argument; character llst identifier
	//inc     0f / increment

	setpl ipl5
	subi16 Z,$0003
	push16 X
	clr XH
	ldd XL,Z+0
	inc XL
isintr_7:
	//jsr     r0,getc; 0:.. / erase output char list for control
	//br isintr_2 / process tty. This prevents a line of stuff
                     // being typed out after you hit the interrupt key
	//br      isintr_7

	rcall getc
	rjmp getc_1_isintr
	rjmp isintr_7
getc_1_isintr:
	pop16 X
	rjmp isintr_2

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
canon:
	//mov    r5,r1 / move tty buffer address to r1
	//add    $10.,r1 / add 10 to get start of data
	//mov    r1,4(r5) / canp = 10(r5) / move buffer addr + 10 to 3rd
                        // word in buffer (char. pointer)
	//clr    2(r5) / ncan / clear 2nd word in buffer, 0 char. count

	mov16 GR1,Y
	addi16 GR1,10
	std16 Y+4,GR1
	clr16 GR3
	std16 Y+2,GR3
can_1:
	//jsr    r0,*(r0) / jump to arg get char off Q of characters, sleep
                        // if none
	//jsr    r0,cesc; 100 / test for @ (kill line)
	//br canon / character was @ so start over
	//jsr    r0,cesc; 43 / test for # (erase last char. typed)
	//br can_1 / character was #, go back
	//cmp    r1,$4 / is char eot?
	//beq    can_2 / yes, reset and return
	//movb   r1,*4(r5) / no, move char to address in 3rd word of buffer
                         // (char. pointer)
	//inc    2(r5) / increment 2nd word (char. count)
	//inc    4(r5) / increment 3rd word (char. pointer)
	//cmp    r1,$'\n / is char = newline
	//beq    can_2 / yes, 1f
	//cmp    2(r5),$120. / is byte count greater than or equal to 120
	//bhis   can_2 / yes, 1f
	//br     can_1 / no, get another char off the Q

	mov16 Z,X
	icall
	push16 X
	ldi16 X,$0040 // 100(octal)  = @
	rcall cesc
	rjmp cesc_1_canon_1
	rjmp cesc_2_canon_2
cesc_1_canon_1:
	pop16 X
	rjmp canon
cesc_2_canon_2:
	ldi16 X,$0023 //43(octal)  = #
	rcall cesc
	rjmp cesc_1_canon_3
	rjmp cesc_2_canon_4
cesc_1_canon_3:
	pop16 X
	rjmp can_1
cesc_2_canon_4:
	pop16 X
	cpi GR1L,$04
	breq can_2
	ldd16 Z,Y+4
	std Z+0,GR1L
	inc16 Z
	std16 Y+4,Z
	ldd16 GR3,Y+2
	inc16 GR3
	std16 Y+2,GR3
	cpi GR1L,'\n'
	breq can_2
	ldi16 Z,120
	cp16 GR3,Z
	brsh can_2
	rjmp can_1

can_2://get here if line is full, a new line has been received or an eot
   	//has been received
	//mov    r5,r1 / move buffer address to r1
	//add    $10.,r1 / add 10
	//mov    r1,4(r5) / canp = 10(r5) / reset char pointer
	//tst    (r0)+ / skip over argument
	//rts    r0 / return

	mov16 GR1,Y
	addi16 GR1,10
	std16 Y+4,GR1
	ret

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
cesc: //test for erase or kill char
	//cmp    r1,(r0)+ / char in r1 = erase or kill character?
	//bne    cesc_1 / no, skip return
	//tst    2(r5) / yes, is char. count = 0
	//beq    cesc_2 / yes, don't skip return
	//dec    2(r5) / no, decrement char count
	//dec    4(r5) / decrement character pointer
	//cmpb   *4(r5),$'\\/ was previous character a "\"
	//bne    cesc_2 / no, don't skip

	cp16 GR1,X
	brne cesc_1
	ldd16 GR3,Y+2
	tst16 GR3
	breq cesc_2
	dec16 GR3
	std16 Y+2,GR3
	ldd16 GR3,Y+4
	dec16 GR3
	std16 Y+4,GR3
	ldd16 Z,Y+4
	ldd GR3L,Z+0
	cpi GR3L,'\\'
	brne cesc_2
cesc_1:
	//tst    (r0)+ / yes, skip

	skip GR3,0	;address return on stack +0
cesc_2:
	//rts    r0 / return

	ret

//INPUT: 
//OUTPUT: None
//USED: 
//ACTION:
//NOTE:
ttych: //get characters from Q of characters inputted to tty
	//mov    $240,*$ps / set processor priority to 5
	//jsr    r0,getc; 0 / takes char. off clist and puts it in r1
	//br ttych_1 / list is empty, go to sleep
	//clr    *$ps / clear process priority
	//rts    r0 / return

	setpl ipl5
	push16 X
	clr16 X
	rcall getc
	rjmp getc_1_ttych
	rjmp getc_2_ttych
getc_1_ttych:
	pop16 X
	rjmp ttych_1
getc_2_ttych:
	pop16 X
	setpl ipl0
	ret

ttych_1: 	//list is empty
	//mov    r5,-(sp) / save r5
	//jsr    r0,sleep; 0 / put process to sleep in input wait channel
	//mov    (sp)+,r5 / restore r5
	//br     ttych / try again

	push16 Y
	push16 X
	clr16 X
	rcall sleep_
	pop16 X
	pop16 Y
	rjmp ttych

garbage:
	nop
	nop
	//.db rol_2(4)
	//xxx6
//	.db ""
//	.db "sss"

.org 0x2000 
free_storage:

fs_map_size:	.dw 30//30 //размер в байтах fs_map
fs_map:          //Отображение хранилища свободного хранения
.overlap

	loop_fwrd 0,6,fillmem_w,$0000,fs_map //reserved blocks for i-nodes(240)
	fillmem_w 7,$7F00,fs_map //plus super block=15 bytes(120 blocks) +1 bit

.set ffb_fs_map = 15*8+1//first free bit in fs_map(changing)

	loop_fwrd 8,14,fillmem_w,$FFFF,fs_map //rest free

.nooverlap

	//rsvblk_map //test
	//rsvblk_map //test

.org fs_map+30/2
inode_map_size:	.dw 30  //размер в байтах inode_map
inode_map:           //Отображение инодов
.overlap

	loop_fwrd 0,14,fillmem_w,$0000,inode_map //all node free

.nooverlap

	//rsv_inode_map 48 //test
	//rsv_inode_map 56 //test
	//rsv_inode_map 57 //test

.org inode_map+30/2
free_storage_inodes: //область хранения самих инодов(не информации в них!)
	
	//init inode 1 to 40
	loop_fwrd 1,14,s_inode_rec,$800F,1,1
	loop_fwrd 15,28,s_inode_rec,$800F,1,1
	loop_fwrd 29,40,s_inode_rec,$800F,1,1

.org free_storage_inodes+(0x1E00/2)//1E00 - 240 i-nodes(or 120 blk)
free_storage_data:  //область хранения данных инодов(информация,вкл.директ.)

// root
bicont_root:
	.dw 41
	addstr8pddng0 ".."
	.dw 41
	addstr8pddng0 "."
	.dw 42 
	addstr8pddng0 "dev"
	.dw 43 
	addstr8pddng0 "bin"
	.dw 44
	addstr8pddng0 "etc"
	.dw 45
	addstr8pddng0 "usr"
	.dw 46
	addstr8pddng0 "tmp" 
//	.dw 49                     //del
//	addstr8pddng0 "testxxxx"   //del
eicont_root:
	zrest_blk (eicont_root - bicont_root)

// dev
bicont_dev:
	.dw 41
	addstr8pddng0 ".."
	.dw 42
	addstr8pddng0 "." 
	.dw 1
	addstr8pddng0 "tty"
	.dw 5
	addstr8pddng0 "eeprom0"
eicont_dev:
	zrest_blk (eicont_dev - bicont_dev)

// bin
bicont_bin:
	.dw 41
	addstr8pddng0 ".."
	.dw 43
	addstr8pddng0 "." 
	.dw 50
	addstr8pddng0 "cat" 
	.dw 88
	addstr8pddng0 "sh" 
	.dw 64
	addstr8pddng0 "echo"
	.dw 75
	addstr8pddng0 "ls"
//	.dw 109				//del
//	addstr8pddng0 "msh" //del
eicont_bin:
	zrest_blk (eicont_bin - bicont_bin)

// etc
bicont_etc:
	.dw 41
	addstr8pddng0 ".."
	.dw 44
	addstr8pddng0 "."
	.dw 47
	addstr8pddng0 "init" 
//	.dw 109
//	addstr8pddng0 "msh" 
//	.dw 48                     //del
//	addstr8pddng0 "testtest"   //del
eicont_etc:
	zrest_blk (eicont_etc - bicont_etc)

// usr
bicont_usr:
	.dw 41
	addstr8pddng0 ".."
	.dw 45
	addstr8pddng0 "."
eicont_usr:
	zrest_blk (eicont_usr - bicont_usr)

// tmp
bicont_tmp:
	.dw 41
	addstr8pddng0 ".."
	.dw 46
	addstr8pddng0 "."
eicont_tmp:
	zrest_blk (eicont_tmp - bicont_tmp)

// init
bicont_init:

//copy data from flash to SRAM
	ldi16 GR4,endt_initdata<<1
	ldi16 Y,begin_initdata
	ldi16 Z,transfer_initdata<<1
cpy_initdata:
	lpm GR3L,Z+
	st Y+,GR3L
	cp16 Z,GR4
	brlo cpy_initdata

	//sys	intr; 0 / turn off interrupts
	//sys quit; 0

	ldi16 GR1,$0000
	ldi GR0L,n_sysintr
	call sysent
	ldi GR0L,n_sysquit
	call sysent

	//sys	mount; eeprom0; usr 
	ldi16 GR1,deeprom0
	ldi16 GR2,dusr
	ldi GR0L,n_sysmount
	call sysent

help:
	//clr	r0 / yes
	//sys	close / close current read
	//mov	$1,r0 / and write
	//sys	close / files
	//sys	open; ctty; 0 / open control tty
	//sys	open; ctty; 1 / for read and write
	//sys	exec; shell; shellp / execute shell

	clr16 GR1
	ldi GR0L,n_sysclose
	call sysent
	ldi16 GR1,$0001
	ldi GR0L,n_sysclose
	call sysent

	ldi16 GR1,dinitctty
	clr16 GR2
	ldi GR0L,n_sysopen
	call sysent
	ldi16 GR1,dinitctty
	ldi16 GR2,$0001
	ldi GR0L,n_sysopen
	call sysent
/*
wait:
	nop
	nop
	rjmp wait
*/
	ldi16 GR1,dshell
	ldi16 GR2,dshellp
	ldi GR0L,n_sysexec
	call sysent

transfer_initdata:
initctty:	.db	"/dev/tty",0
shell:		.db	"/bin/sh",0   //shell:	.db	"/bin/msh",0
shellm:		.db	"-",0
eeprom0:	.db "/dev/eeprom0",0
usr:		.db "/usr",0
shellp:		.dw (shellm-transfer_initdata)*2+0x102 
			.dw $0000
endt_initdata:

.dseg
.org core+2
.overlap

begin_initdata:
dinitctty:	.byte (shell-transfer_initdata)*2
dshell:		.byte (shellm-shell)*2
dshellm:	.byte (eeprom0-shellm)*2
deeprom0:	.byte (usr-eeprom0)*2
dusr:		.byte (shellp-usr)*2
dshellp:	.byte (endt_initdata-shellp)*2

.nooverlap
.cseg
eicont_init:
	zrest_blk (eicont_init - bicont_init)
/*
//msh
bicont_msh:

//copy data from flash to SRAM
	ldi16 GR4,end_mshdata<<1
	ldi16 Y,dmshat
	ldi16 Z,mshat<<1
cpy_mshdata:
	lpm GR3L,Z+
	st Y+,GR3L
	cp16 Z,GR4
	brlo cpy_mshdata

	//mov	$1,r0	
	//sys	write; dmshat; 2.

	//mov	$parbuf,r1
	//mov	$parp,r2
	//jsr	pc,mshblank
	//cmp	r0,$'\n  /
	//bne	msh_1
	//sys	exit

	ldi16 GR1,$0001
	ldi16 GR2,dmshat
	ldi16 GR3,$0002
	ldi GR0L,n_syswrite
	call sysent

	ldi16 X,mshparbuf
	ldi16 Y,mshparp
	rcall mshblank
	cpi GR0L,'\n'
	brne msh_1
	ldi GR0L,n_sysexit
	call sysent
msh_1:
	//mov	r1,(r2)+

	st16i Y+,X
msh_2:
	//movb	r0,(r1)+
	//jsr	pc,mshgetc
	//cmp	r0,$' / space
	//beq	msh_3
	//cmp	r0,$'\n  /
	//beq	msh_4
	//br	msh_2

	st X+,GR0L
	rcall mshgetc
	cpi GR0L,' '
	breq msh_3
	cpi GR0L,'\n'
	breq msh_4
	rjmp msh_2
msh_3:
	//clrb	(r1)+
	//jsr	pc,mshblank
	//cmp	r0,$'\n  /
	//beq	msh_4
	//br	msh_1

	clr GR0L
	st X+,GR0L
	rcall mshblank
	cpi GR0L,'\n'
	breq msh_4
	rjmp msh_1
msh_4:
	//clr	(r2)+
	//sys	exec; parbuf; parp /

	clr16 GR0
	st16i Y+,GR0
	ldi16 GR1,mshparbuf
	ldi16 GR2,mshparp
	ldi GR0L,n_sysexec
	call sysent
msh_6:
	//mov	$1,r0	
	//sys	write; quest; 2. 
	//sys	exit
	
	ldi16 GR1,$0001
	ldi16 GR2,dquest
	ldi16 GR3,$0003
	ldi GR0L,n_syswrite
	call sysent
	ldi GR0L,n_sysexit
	call sysent

mshblank:
	//jsr	pc,mshgetc
	//cmp	r0,$' / space
	//beq	mshblank
	//rts	pc

	rcall mshgetc
	cpi GR0L,' '
	breq mshblank
	ret
mshgetc:
	//clr	r0
	//sys	read; chr; 1 / read char
	//bcs	mshxit1
	//tst	r0
	//beq	mshxit1
	//movb	chr,r0
	//rts	pc

	clr16 GR1
	ldi16 GR2,chr
	ldi16 GR3,$0001
	ldi GR0L,n_sysread
	call sysent
	brcs mshxit1
	tst16 GR0
	breq mshxit1
	lds GR0L,chr
	ret
mshxit1:
	//sys	exit

	ldi GR0L,n_sysexit
	call sysent

mshat:	.db "# "
quest:	.db	"? "
end_mshdata:

.dseg
.org core+2

.overlap

dmshat:		.db "# "
dquest:		.db	"? "

chr:		.db $00
mshparbuf:	.byte 100
mshparp:	.byte 10  


.nooverlap

.cseg
eicont_msh:
	zrest_blk (eicont_msh - bicont_msh)
*/

//cat
bicont_cat:
// cat -- concatinate files

	//mov	(sp)+,r5
	//tst	(sp)+
	//mov	$obuf,r2
	//cmp	r5,$1
	//beq	cat_2

	pop16 Z          //Z(r5) - argc
	pop16 GR3        //GR3 - temp; (sp+1)argv[2] - [file1 file2 ...]
	ldi16 X,obuf     //X - r2
	ldi16 GR3,$0001
	cp16 Z,GR3
	breq cat_2
catloop:
	//dec	r5
	//ble	catdone
	//mov	(sp)+,0f
	//sys	open; 0:..; 0
	//bes	catloop
	//mov	r0,fin
	
	dec16 Z
	breq catdone
	brlt catdone
	pop16 GR1
	ldi16 GR2,$0000
	ldi GR0L,n_sysopen
	call sysent
	brcs catloop
	sts16 catfin,GR0
cat_2:
	//mov	fin,r0
	//sys	read; ibuf; 512.
	//bes	cat_4
	//mov	r0,r4
	//beq	cat_4
	//mov	$ibuf,r3

	lds16 GR1,catfin
	ldi16 GR2,ibuf
	ldi16 GR3,64
	ldi GR0L,n_sysread
	call sysent
	brcs cat_4
	mov16 GR4,GR0
	tst16 GR4
	breq cat_4
	ldi16 Y,ibuf
cat_3:
	//movb	(r3)+,r0
	//jsr	pc,catputc
	//dec	r4
	//bne	cat_3
	//br	cat_2

	ld GR0L,Y+
	rcall catputc
	dec16 GR4
	brne cat_3
	rjmp cat_2
cat_4:
	//mov	fin,r0
	//beq	catloop
	//sys	close
	//br	catloop

	lds16 GR1,catfin
	tst16 GR1
	breq catloop
	ldi GR0L,n_sysclose
	call sysent
	rjmp catloop

catdone:
	//sub	$obuf,r2
	//beq	cadn_1
	//mov	r2,0f
	//mov	$1,r0
	//sys	write; obuf; 0:..

	subi16 X,obuf
	breq cadn_1
	mov16 GR3,X
	ldi16 GR1,1
	ldi16 GR2,obuf
	ldi GR0L,n_syswrite
	call sysent
cadn_1:
	//sys	exit

	ldi GR0L,n_sysexit
	call sysent

catputc:
	//movb	r0,(r2)+
	//cmp	r2,$obuf+512.
	//blo	caput_1
	//mov	$1,r0
	//sys	write; obuf; 512.
	//mov	$obuf,r2

	st X+,GR0L
	ldi16 GR3,(obuf+64)
	cp16 X,GR3
	brlo caput_1
	ldi16 GR1,1
	ldi16 GR2,obuf
	ldi16 GR3,64
	ldi GR0L,n_syswrite
	call sysent
	ldi16 X,obuf
caput_1:
	//rts	pc

	ret

.db		'1' //del

.dseg
.org core+2

.overlap

catfin:		.dw $0000
ibuf: 		.byte 64
obuf: 		.byte 64


.nooverlap

.cseg
eicont_cat:
	zrest_blk (eicont_cat - bicont_cat)

// testtest del
/*bicont_testtest:

//	nop
testloop:
//	nop
//	nop
//	rjmp testloop


	ldi GR0L,n_sysfork
	call sysent
	rjmp child_p
	rjmp parent_p
	nop
	nop
parent_p:
	nop
	nop
	rjmp parent_p
child_p:
	nop
	nop
	rjmp child_p

	rjmp bicont_init
eicont_testtest:
	zrest_blk (eicont_testtest - bicont_testtest)

// testxxxx
bicont_testxxxx:
	.dw 41
	addstr8pddng0 ".."
	.dw 49
	addstr8pddng0 "."
	.dw 48                     //del
	addstr8pddng0 "testtest"   //del
eicont_testxxxx:
	zrest_blk (eicont_testxxxx - bicont_testxxxx)
*/

//Thompson shell (modified)
bicont_sh:

//copy data from flash to SRAM
	ldi16 GR4,end_shdata<<1
	ldi16 Y,dat
	ldi16 Z,at<<1
cpy_shdata:
	lpm GR3L,Z+
	st Y+,GR3L
	cp16 Z,GR4
	brlo cpy_shdata

//sh -- command interpreter
	//mov	sp,r5
	//mov	r5,shellarg / save orig sp in shellarg
	//cmpb	*2(r5),$'- / was this sh calleZd by init or loginx~
	//bne	sh_1 / no
	//sys	intr; 0 / yes, turn off interrupts
	//sys	quit; 0

	lds16io Y,sp
	inc16 Y
	sts16 shellarg,Y
	ldd16 X,Y+2
	ld GR3L,X
	cpi GR3L,'-'
	brne sh_1
	ldi16 GR1,$0000
	ldi GR0L,n_sysintr
	call sysent
	ldi GR0L,n_sysquit
	call sysent

sh_1:
	//sys	getuid / who is user
	//tst	r0 / is it superuser
	//bne	sh_2 / no
	//movb	$'#,at / yes, set new prompt symbol

	ldi GR0L,n_sysgetuid
	call sysent
	tst16 GR0
	brne sh_2
	ldi GR3L,'#'
	sts dat,GR3L
sh_2:
	//cmp	(r5),$1 / tty input?
	//ble	newline / yes, call with '-(or with no command file name)
	//clr	r0 / no, set ttv
	//sys	close / close it
	//mov	4(r5),0f / get new file name
	//sys	open; 0:..; 0 / open it
	//bec	sh_3 / branch if no error
	//jsr	r5,error / error in file name
	//<Input not found\n\0>; .even
	//sys	exit

	ldd16 GR3,Y+0
	ldi16 GR4,$0001
	cp16 GR3,GR4
	breq newline
	brlt newline
	clr16 GR1
	ldi GR0L,n_sysclose
	call sysent
	ldd16 GR1,Y+4
	clr16 GR2
	ldi GR0L,n_sysopen
	call sysent
	brcc sh_3

	ldi16 Z,errmsg1<<1
	rcall sherror
	ldi GR0L,n_sysexit
	call sysent

sh_3:
	//clr	at / clear prompt character, if reading non-tty input file

	clr16 GR3
	sts16 dat,GR3

newline:
	//tst	at / is there a prompt symbol
	//beq	newcom / no
	//mov	$1,r0 / yes
	//sys	write; at; 2. / print prompt

	lds16 GR3,dat
	tst16 GR3
	breq newcom
	ldi16 GR1,$0001
	ldi16 GR2,dat
	ldi16 GR3,2
	ldi GR0L,n_syswrite
	call sysent

newcom:
	//mov	shellarg,sp /
	//mov	$parbuf,r3 / initialize command list area
	//mov	$parp,r4 / initialize command list pointers
	//clr	infile / initialize alternate input
	//clr	outfile / initialize alternate output
	//clr	glflag / initialize global flag
	//clr	countbuf
	
	lds16 GR3,shellarg
	dec16 GR3
	sts16io sp,GR3
	ldi16 Y,parbuf  // Y = r3
	ldi16 X,parp    // X = r4
	clr16 GR3
	sts16 infile,GR3
	sts16 outfile,GR3
	sts16 glflag,GR3
	sts16 countbuf,GR3
newarg:
	//jsr	pc,blank / squeeze out leading blanks
	//jsr	r5,delim / is new character a ; \n or &
	//br nch_4 / yes
	//mov	r3,-(sp) / no, push arg pointer onto stack
	//cmp	r0,$'< / new input file?
	//bne	narg_1 / no
	//mov	(sp),infile / yes, save arg pointer
	//clr	(sp) / clear pointer
	//br	nch_1

	rcall blank
	rcall delim
	rjmp nch_4
	push16 Y
	ldi16 GR3,'<'
	cp16 GR0,GR3
	brne narg_1
	pop16 GR3
	sts16 infile,GR3
	clr16 GR3
	push16 GR3
	rjmp nch_1
narg_1:
	//cmp	r0,$'> / new output file?
	//bne	newchar / no
	//mov	(sp),outfile / yes, save arg pointer
	//clr	(sp) / clear pointer
	//br	nch_1

	ldi16 GR3,'>'
	cp16 GR0,GR3
	brne newchar
	pop16 GR3
	sts16 outfile,GR3
	clr16 GR3
	push16 GR3
	rjmp nch_1
newchar:
	//cmp	$' ,r0 / is character a blank
	//beq	nch_2 / branch if it is (blank as arg separator)
	//cmp	$'\n+200,r0 / treat \n preceded by \
	//beq	nch_2 / as blank
	//jsr	pc,shputc / put this character in parbuf list

	ldi16 GR3,' '
	cp16 GR0,GR3
	breq nch_2
	ldi16 GR3,('\n'+$80)
	cp16 GR0,GR3
	breq nch_2
	rcall shputc
nch_1:
	//jsr	pc,shgetc / get next character
	//jsr	r5,delim / is char a ; \n or &,
	//br nch_2 / yes
	//br	newchar / no, start new character tests

	rcall shgetc
	rcall delim
	rjmp nch_2
	rjmp newchar
nch_2:
	//clrb	(r3)+ / end name with \0 when read blank, or delim
	//mov	(sp)+,(r4)+ / move arg ptr to parp location
	//bne	nch_3 / if (sp)=0, in file or out file points to arg
	//tst	-(r4) / so ignore dummy (0), in pointer list

	clr GR3L
	st Y+,GR3L
	pop16 GR3
	st16i X+,GR3
	tst16 GR3
	brne nch_3
	subi16 X,2
nch_3:
	//jsr	r5,delim / is char a ; \n or &.
	//br nch_4 / yes
	//br	newarg / no, start newarg processing

	rcall delim
	rjmp nch_4
	rjmp newarg
nch_4:
	//clr	(r4) / \n, &, or ; takes to here (end of arg list) after 'delim'
	//mov	r0,-(sp) / save delimter in stack
	//jsr	pc,docom / go to exec command in parbuf
	//cmpb	(sp),$'& / get a new command without wait?
	//beq	newcom / yes
	//tst	r1 / was chdir just executed or line ended with ampersand?
	//beq	nch_6 / yes

	clr16 GR3
	st16 X,GR3
	push16 GR0
	rcall docom
	pop16 GR3
	push16 GR3
	cpi GR3L,'&'
	brne nch_41   //changed
	rjmp newcom
nch_41:
	tst16 GR1
	breq nch_6
nch_5:
	//sys	wait / no, wait for new process to terminate command executed)
	//bcs	nch_6 / no, children not previously waited for
	//cmp	r0,r1 / is this my child
	//bne	nch_5

	ldi GR0L,n_syswait
	call sysent
	brcs nch_6
	cp16 GR0,GR1
	brne nch_5
nch_6:
	//cmp	(sp),$'\n / was delimiter a new line
	//beq	newline / yes
	//br	newcom / no, pick up next command
	pop16 GR3
	push16 GR3
	ldi16 GR4,'\n'
	cp16 GR3,GR4
	brne nch_61     //changed
	rjmp newline
nch_61:
	rjmp newcom

docom:
	//sub	$parp,r4 / out arg count in r4
	//bne	dc_1 / any arguments?
	//clr	r1 / no, line ended with ampersand
	//rts	pc / return from call
	
	subi16 X,parp
	brne dc_1
	clr16 GR1
	ret

dc_1:
	//jsr	r5,chcom; qchdir / is command chdir?
	//br dc_4 / command not chdir
	//cmp	r4,$4 / prepare to exec chdir, 4=arg count x 2
	//beq	dc_2
	//jsr	r5,error / go to print error
	//<Arg count\n\0>; .even
	//br	dc_3

	ldi16 Z,(qchdir<<1)
	rcall chcom
	rjmp dc_4
	ldi16 GR3,4
	cp16 X,GR3
	breq dc_2
	ldi16 Z,(errmsg2<<1)
	rcall sherror
	rjmp dc_3

dc_2:
	//mov	parp+2,0f / more directory name to sys coll
	//sys	chdir; 0:0 / exec chdir
	//bec	dc_3 / no error exit
	//jsr	r5,error / go to print error
	//<Bad directory\n\0>; .even / this diagnostic

	ldi16 Z,(parp+2)
	ldd16 GR1,Z+0
	ldi GR0L,n_syschdir
	call sysent
	brcc dc_3
	ldi16 Z,(errmsg3<<1)
	rcall sherror

dc_3:
	//clr	r1 / set r1 to zero to dkip wait
	//rts	pc / and return

	clr16 GR1
	ret
dc_4:
	//jsr	r5,chcom; glogin / is command login?
	//br dc_5 / not loqin, go to fork
	//sys	exec; parbuf; parp / exec login
	//sys	exec; binpb; parp / or /bin/login

	ldi16 Z,(glogin<<1)
	rcall chcom
	rjmp dc_5
	ldi16 GR1,parbuf
	ldi16 GR2,parp
	ldi GR0L,n_sysexec
	call sysent
	ldi16 GR1,binpb
	ldi16 GR2,parp //del
	ldi GR0L,n_sysexec
	call sysent

dc_5: // no error return??
	//sys	fork / generate sh child process for command
	//br newproc / exec command with new process
	//bec	dc_6 / no error exit, old orocess
	//jsr	r5,error / go to print error
	//<Try again\n\0>; .even / this diaonostic
	//jmp	newline / and return for next try

	ldi GR0L,n_sysfork
	call sysent
	rjmp newproc
	brcc dc_6
	ldi16 Z,errmsg4<<1
	rcall sherror
	rjmp newline
dc_6:
	//mov	r0,r1 / save id of child sh
	//rts	pc / return to "jsr pc, docom" call in parent sh

	mov16 GR1,GR0
	ret

sherror:
	//movb	(r5)+,och / pick up diagnostic character
	//beq	sher_1 / 0 is end of line
	//mov	$1,r0 / set for tty output
	//sys	write; och; 1 / print it
	//br	sherror / continue to get characters

	lpm GR3L,Z+
	clr GR3H
	sts16 och,GR3
	tst16 GR3
	breq sher_1
	ldi16 GR1,$0001
	ldi16 GR2,och
	ldi16 GR3,$0001
	ldi GR0L,n_syswrite
	call sysent
	rjmp sherror
sher_1:
	;inc	r5 / inc r5 to point to return
	;bic	$1,r5 / make it even
	//clr	r0 / set for input
	//sys	seek; 0; 2 / exit from runcom. skip to end of input file
	//rts	r5

	clr16 GR1
	mov16 GR2,GR1
	ldi16 GR3,$0002
	ldi GR0L,n_sysseek
	call sysent
	ret

chcom: // has no effect if tty input
	//mov	(r5)+,r1 / glogin gchdir r1, bump r5
	//mov	$parbuf,r2 / command address  r2 'login'

	mov16 GR2,Y

	ldi16 Y,parbuf
chcm_1:
	//movb	 (r1)+,r0 / is this command 'chdir'
	//cmpb	(r2)+,r0 / compare command name byte with 'login' or 'chdir'
	//bne	chcm_2 / doesn't compare
	//tst	r0 / is this
	//bne	chcm_1 / end of names
	//tst	(r5)+ / yes, bump r5 again to execute login chdir

	lpm GR0L,Z+
	clr GR0H
	ld GR3L,Y+
	cp GR3L,GR0L
	brne chcm_2
	tst16 GR0
	brne chcm_1
	skip GR3,0
chcm_2:
	//rts	r5 / no, return to exec command

	mov16 Y,GR2

	ret

shputc:
	//cmp	r0,$'' / single quote?
	//beq	shputc_1 / yes
	//cmp	r0,$'" / double quote
	//beq	shputc_1 / yes
	//bic	$!177,r0 / no, remove 200, if present
	//movb	r0,(r3)+ / store character in parbuf
	//rts	pc

	ldi16 GR3,$0027
	cp16 GR0,GR3
	breq shputc_1
	ldi16 GR3,$0022
	cp16 GR0,GR3
	breq shputc_1
	andi16 GR0,$007F
	st Y+,GR0L
	ret
shputc_1:
	//mov	r0,-(sp) / push quote mark onto stack

	push16 GR0
shputc_2:
	//jsr	pc,shgetc / get a quoted character
	//cmp	r0,$'\n / is it end or line
	//bne	shputc_3 / no
	//jsr	r5,sherror / yes, indicate missing quote mark
	//<"' imbalance\n\0>; .even
	//jmp	newline / ask for new line

	rcall shgetc
	ldi16 GR3,'\n'
	cp16 GR0,GR3
	brne shputc_3
	ldi16 Z,errmsg5<<1
	rcall sherror
	rjmp newline

shputc_3:
	//cmp	r0,(sp) / is this closing quote mark
	//beq	shputc_4 / yes
	//bic	$!177,r0 / no, strip off 200 if present
	//movb	r0,(r3)+ / store quoted character in parbuf
	//br	shputc_2 / continue

	pop16 GR3
	push16 GR3
	cp16 GR0,GR3
	breq shputc_4
	andi16 GR0,$007F
	st Y+,GR0L
	rjmp shputc_2
shputc_4:
	//tst	(sp)+ / pop quote mark off stack
	//rts	pc / return

	pop16 GR3
	ret

//thp`e new process

newproc:
	//mov	infile,0f / move pointer to new file name
	//beq	np_2 / branch if no alternate read file given
	//tstb	*0f
	//beq	np_1 / branch if no file name miven
	//clr	r0 / set tty input file name
	//sys	close / close it
	//sys	open; 0:..; 0 / open new input file for reading
	//bcc	np_2 / branch if input file ok

	lds16 GR2,infile
	tst16 GR2
	breq np_2
	mov16 Z,GR2
	ld GR3L,Z
	tst GR3L
	breq np_1
	clr16 GR1
	ldi GR0L,n_sysclose
	call sysent
	mov16 GR1,GR2
	clr16 GR2
	ldi GR0L,n_sysopen
	call sysent
	brcc np_2
np_1:
	//jsr	r5,error / file not ok, print error
	//<Input file\n\0>; .even / this diagnostic
	//sys	exit / terminate this process and make parent sh

	ldi16 Z,errmsg6<<1
	rcall sherror
	ldi GR0L,n_sysexit
	call sysent

np_2:
	//mov	outfile,r2 / more pointer to new file name
	//beq	np_6 / branch if no alternate write file
	//cmpb	(r2),$'> / is > at beqinninrg of file name?
	//bne	np_3 / branch if it isn't
	//inc	r2 / yes, increment pointer
	//mov	r2,0f
	//sys	open; 0:..; 1 / open file for writing
	//bec	np_5 / if no error

	lds16 Z,outfile
	tst16 Z
	breq np_6
	ld16 GR3,Z
	cpi GR3L,'>'
	brne np_3
	inc16 Z
	mov16 GR1,Z
	ldi16 GR2,1
	ldi GR0L,n_sysopen
	call sysent
	brcc np_5
np_3:
	//mov	r2,0f
	//sys	creat; 0:..; 17 / create new file with this name
	//bec	np_5 / branch if no error

	mov16 GR1,Z
	ldi16 GR2,$000F
	ldi GR0L,n_syscreat
	call sysent
	brcc np_5
np_4:
	//jsr	r5,sherror
	//<Output file\n\0>; .even
	//sys	exit

	ldi16 Z,errmsg7<<1
	rcall sherror
	ldi GR0L,n_sysexit
	call sysent
np_5:
	//sys	close / close the new write file
	//mov	r2,0f / move new name to open
	//mov	$1,r0 / set ttv file name
	//sys	close / close it
	//sys	open; 0:..; 1 / open new output file, it now has file desc. 1
	//sys	seek; 0; 2 / set pointer to current end of file

	mov16 GR1,GR0
	ldi GR0L,n_sysclose
	call sysent
	mov16 GR2,Z
	ldi16 GR1,$0001
	ldi GR0L,n_sysclose
	call sysent
	mov16 GR1,GR2
	ldi16 GR2,$0001
	ldi GR0L,n_sysopen
	call sysent
	clr16 GR1
	ldi16 GR2,$0002
	ldi GR0L,n_sysseek
	call sysent
np_6:
	//tst	glflag / was *, ? or [ encountered?
	//bne	np_8 / yes
	//sys	exec; parbuf; parp / no, execute this commend
	//sys	exec; binpb; parp / or /bin/this command

	lds16 GR3,glflag
	tst16 GR3
	brne np_8
	ldi16 GR1,parbuf
	ldi16 GR2,parp
	ldi GR0L,n_sysexec
	call sysent
	ldi16 GR1,dbinpb
	ldi16 GR2,parp  //del
	ldi GR0L,n_sysexec
	call sysent

np_7:
	//jsr	r5,sherror / a return for exec is the diagnostic
	//<No command\n\0>; .even
	//sys	exit

	ldi16 Z,errmsg8<<1
	rcall sherror
	ldi GR0L,n_sysexit
	call sysent
np_8:
	//mov	$glob,parp-2 / prepare to process *,?
	//sys	exec; glob; parp-2 / execute modified command
	//br	np_7

	ldi16 GR3,dglob
	ldi16 Z,(parp-2)
	st16 Z,GR3
	mov16 GR1,GR3
	mov16 GR2,Z
	ldi GR0L,n_sysexec
	call sysent
	rjmp np_7

delim:
	//cmp	r0,$'\n / is character a newline
	//beq	dlim_3
	//cmp	r0,$'& / is it &
	//beq	dlim_3 / yes
	//cmp	r0,$'; / is it ;
	//beq	dlim_3 / yes
	//cmp	r0,$'? / is it ?
	//beq	dlim_1
	//cmp	r0,$'* / is it *
	//beq	dlim_1
	//cmp	r0,$'[ / is it beginning of character string (for glob)
	//bne	dlim_2

	ldi16 GR3,'\n'
	cp16 GR0,GR3
	breq dlim_3
	ldi16 GR3,'&'
	cp16 GR0,GR3
	breq dlim_3
	ldi16 GR3,';'
	cp16 GR0,GR3
	breq dlim_3
	ldi16 GR3,'?'
	cp16 GR0,GR3
	breq dlim_1
	ldi16 GR3,'*'
	cp16 GR0,GR3
	breq dlim_1
	ldi16 GR3,'['
	cp16 GR0,GR3
	brne dlim_2
dlim_1:
	//inc	glflag / ? or * or [ set flag

	lds16 GR3,glflag
	inc16 GR3
	sts16 glflag,GR3

dlim_2:
	//tst	(r5)+ / bump to process all except \n,;,&

	skip GR3,0

dlim_3:
	//rts	r5

	ret

blank:
	//jsr	pc,shgetc / get next character
	//cmp	$' ,r0 / leading blanks
	//beq	blank / yes, 'squeeze out'
	//cmp	r0,$200+'\n / new-line preceded by \ is translated
	//beq	blank / into blank
	//rts	pc

	rcall shgetc
	ldi16 GR3,' '
	cp16 GR0,GR3
	breq blank
	ldi16 GR3,('\n'+$80)
	cp16 GR0,GR3
	breq blank
	ret

shgetc:
	//tst	param / are we substituting for $n
	//bne	shgetc_3/ yes
	//mov	inbufp,r1 / no, move normal input pointer to r1
	//cmp	r1,einbuf / end of input line?
	//bne	shgetc_1 / no
	//jsr	pc,getbuf / yes, put next console line in buffer
	//br	shgetc

	lds16 GR3,param
	tst16 GR3
	brne shgetc_3
	lds16 Z,inbufp
	lds16 GR3,einbuf
	cp16 Z,GR3
	brne shgetc_1
	rcall getbuf
	rjmp shgetc
shgetc_1:
	//movb	(r1)+,r0 / move byte from input buffer to r0
	//mov	r1,inbufp / increment routine
	//bis	escap,r0 /if last character was \ this adds 200 to current char.
	//clr	escap / clear, so escap normally zero
	//cmp	r0,$'\\ / note that \\ is equal \ in as
	//beq	shgetc_2
	//cmp	r0,$'$ / is it $
	//beq	shgetc_5 / yes
	//rts	pc / no

	ld GR0L,Z+
	clr GR0H
	sts16 inbufp,Z
	lds16 GR3,escap
	or16 GR0,GR3
	clr16 GR3
	sts16 escap,GR3
	ldi16 GR3,'\\'
	cp16 GR0,GR3
	breq shgetc_2
	ldi16 GR3,'$'
	cp16 GR0,GR3
	breq shgetc_5
	ret 

shgetc_2:
	//mov	$200,escap / mark presence of \ in command line
	//br	shgetc / get next character

	ldi16 GR3,$0080
	sts16 escap,GR3
	rjmp shgetc

shgetc_3:
	//movb	*param,r0 / pick up substitution character put in r0
	//beq	shgetc_4 / if end of substitution arg, branch
	//inc	param / if not end, set for next character
	//rts	pc / return as though character in ro is normal input

	lds16 Z,param
	ld GR0L,Z
	clr GR0H
	tst16 GR0   //?
	breq shgetc_4
	lds16 GR3,param
	inc16 GR3
	sts16 param,GR3
	ret

shgetc_4:
	//clr	param / unset substitution pointer
	//br	shgetc / get next char in normal input

	clr16 GR3
	sts16 param,GR3
	rjmp shgetc

shgetc_5:
	//jsr	pc,shgetc / get digit after $
	//sub	$'0,r0 / strip off zone bits
	//cmp	r0,$9. / compare with digit 9 
	//blos	shgetc_6 / less than or equal 9
	//mov	$9.,r0 / if larger than 9, force 9

	rcall shgetc
	subi16 GR0,'0'
	ldi16 GR3,9
	cp16 GR0,GR3
	brlo shgetc_6
	breq shgetc_6
	ldi16 GR0,9
shgetc_6:
	//mov	shellarg,r1 / get pointer to stack for this call of shell
	//inc	r0 / digit +1
	//cmp	r0,(r1) / is it less than # of args in this call
	//bge	shgetc / no, ignore it. so this $n is not replaced
	//asl	r0 / yes, multiply by 2 (to skip words)
	//add	r1,r0 / form pointer to arg pointer (-2)
	//mov	2(r0),param / move arg pointer to param
	//br	shgetc / go to get substitution arg for $n

	lds16 Z,shellarg
	inc16 GR0
	ld16 GR3,Z
	cp16 GR0,GR3
	brlt shgetc_61    //changed
	rjmp shgetc
shgetc_61:
	lsl16 GR0
	add16 GR0,Z
	mov16 Z,GR0
	addi16 Z,$0002
	ld16 GR3,Z
	sts16 param,GR3
	rjmp shgetc
getbuf:
	//mov	$inbuf,r0 / move input buffer address
	//mov	r0,inbufp / to input buffer pointer
	//mov	r0,einbuf / and initialize pointer to end of character string
	//clr	r0 / set for tty input
	//sys	read; inbuf; 1 / read next char into inbuf
	//bcs	shxit1 / error exit
	//tst	r0 / a zero input is end of file
	//beq	shxit1 / exit
	//inc	einbuf / eventually einbuf points to \n (+1) of this line
	//inc	countbuf 
	//cmp	countbuf,101. / have we exceeded input buffer size
	//bhis	shxit1 / if so, exit assume some sort of binary
	//rts	pc / yes, return

	ldi16 GR0,inbuf
	sts16 inbufp,GR0
	sts16 einbuf,GR0
	clr16 GR1
	ldi16 GR2,inbuf
	ldi16 GR3,1
	ldi GR0L,n_sysread
	call sysent
	brcs shxit1
	tst16 GR0
	breq shxit1
	lds16 GR3,einbuf
	inc16 GR3
	sts16 einbuf,GR3
	lds16 GR3,countbuf
	inc16 GR3
	sts16 countbuf,GR3
	ldi16 GR4,101
	cp16 GR3,GR4
	brsh shxit1
	ret

shxit1:
	//sys	exit

	ldi GR0L,n_sysexit
	call sysent

errmsg1:	.db	"Input not found",'\n',0
errmsg2:	.db "Arg count",'\n',0
errmsg3:	.db "Bad directory",'\n',0
errmsg4:	.db "Try again",'\n',0
errmsg5:	.db '\x22','\x27'," imbalance",'\n',0
errmsg6:	.db "Input file",'\n',0
errmsg7:	.db "Output file",'\n',0
errmsg8:	.db "No command",'\n',0

qchdir:
	.db	"chdir",0
glogin:
	.db	"login",0
at:
	.db	"@ "
glob:
	.db	"/etc/glob",0
binpb:
	.db	"/bin/"
end_shdata:

.dseg
.org core+2

.overlap

param:		.dw $0000
glflag:		.dw $0000
infile: 	.dw $0000 
outfile:	.dw $0000
			.dw $0000 // room for glob
parp:		.byte 10
inbuf:		.db $00
escap:		.dw $0000
inbufp: 	.dw $0000
einbuf:		.dw $0000
och:		.dw $0000
shellarg:	.dw $0000
countbuf:	.dw $0000

dat:
	.db	"@ "
dglob:
	.db	"/etc/glob",0
dbinpb:
	.db	"/bin/"

parbuf: 	.byte 100

.nooverlap

.cseg

eicont_sh:
	zrest_blk2 (eicont_sh - bicont_sh)

// echo
bicont_echo:
	//mov	(sp)+,r5
	//tst	(sp)+

	pop16 Z          //Z(r5) - argc
	pop16 GR3        //GR3 - temp; (sp+1)argv[2] - [file1 file2 ...]

echoloop:
	//dec	r5
	//ble	echodone
	//mov	(sp)+,r4

	dec16 Z
	breq echodone
	brlt echodone
	pop16 Y
echo_1:
	//movb	*(r4),0f
	//bne	echo_2
	//movb	$' ,0f

	ld GR3L,Y
	sts echooch,GR3L
	tst GR3L
	brne echo_2
	ldi GR3L,$20
	sts echooch,GR3L
echo_2:
	//mov	$1,r0
	//sys	write; 0:..; 1 /write char
	//tstb *(r4)+
	//bne echo_1
	//br echoloop

	ldi16 GR1,1
	ldi16 GR2,echooch
	ldi16 GR3,1
	ldi GR0L,n_syswrite
	call sysent
	ld GR3L,Y+
	tst GR3L
	brne echo_1
	rjmp echoloop

echodone:

	//movb	$'\n,0f
	//mov	$1,r0
	//sys	write; 0:..; 1 /write char
	//sys	exit

	ldi GR3L,'\n'
	sts echooch,GR3L
	ldi16 GR1,1
	ldi16 GR2,echooch
	ldi16 GR3,1
	ldi GR0L,n_syswrite
	call sysent
	ldi GR0L,n_sysexit
	call sysent

.dseg
.org core+2

.overlap

echooch:		.dw $0000

.nooverlap

.cseg
eicont_echo:
	zrest_blk (eicont_echo - bicont_echo)


// ls
bicont_ls:

//copy data from flash to SRAM
	ldi16 GR4,endt_lsdata<<1
	ldi16 Y,begin_lsdata
	ldi16 Z,transfer_lsdata<<1
cpy_lsdata:
	lpm GR3L,Z+
	st Y+,GR3L
	cp16 Z,GR4
	brlo cpy_lsdata

	//mov	$1,obuf
	//mov	sp,r5
	//mov	(r5)+,count
	//tst	(r5)+
	//dec	count
	//mov	count,ocount
	//bgt	lsloop
	//mov	$dotp,r5

	ldi16 GR3,1
	ldi16 Y,lsobuf
	std16 Y+0,GR3
	lds16io X,sp
	inc16 X
	ld16i GR3,X+
	addi16 X,2
	dec16 GR3
	sts16 count,GR3
	sts16 ocount,GR3
	tst16 GR3
	breq ls_0
	brge lsloop
ls_0:
	ldi16 X,ddotp
lsloop:
	//mov	(r5)+,r4
	//cmpb	(r4)+,$'-
	//bne	ls_3
	//dec	ocount

	ld16i Y,X+
	ld GR3L,Y+
	cpi GR3L,'-'
	brne ls_3
	lds16 GR3,ocount
	dec16 GR3
	sts16 ocount,GR3
ls_1:
	//movb	(r4)+,r0
	//beq	lseloop

	//cmpb	r0,$'a
	//bne	ls_2
	//inc	allflg
	//br	ls_1

	ld GR3L,Y+
	tst GR3L
	breq lseloop

	cpi GR3L,'a'
	brne ls_2
	lds16 GR4,allflg
	inc16 GR4
	sts16 allflg,GR4
	rjmp ls_1
ls_2:
	//cmpb	r0,$'d
	//bne	ls_1
	//inc	dirflg
	//br	ls_1

	cpi GR3L,'d'
	brne ls_1
	lds16 GR4,dirflg
	inc16 GR4
	sts16 dirflg,GR4
	rjmp ls_1
ls_3:
	//dec	r4
	//jsr	pc,lsdo

	dec16 Y
	rcall lsdo
lseloop:
	//dec	count
	//bgt	lsloop
	//tst	dnp
	//bne	ls_4
	//mov	$dotp,r5
	//br	lsloop

	lds16 GR3,count
	dec16 GR3
	sts16 count,GR3
	breq ls_3_1
	brge lsloop
ls_3_1:
	lds16 GR3,dnp
	tst16 GR3
	brne ls_4
	ldi16 X,ddotp
	rjmp lsloop
ls_4:
	//jsr	r5,flush; lsobuf
	//sys	exit

	ldi16 Y,lsobuf
	rcall fl_lib
	ldi GR0L,n_sysexit
	call sysent

lsdo:
	//mov	$namef,r1
	//mov	$filnam,r3
	//mov	r4,dnp
	//clr	isadir
	//tst	dirflg
	//bne	nondir
	//sys	stat; dnp: 0; statb
	//bec	lsdo_1
	//jsr	r5,questf; < nonexistent\n\0>; .even
	//rts	pc

	ldi16 GR3,filnam
	sts16 dnp,Y
	clr16 GR1 //GR1 temp
	sts16 isadir,GR1
	lds16 GR1,dirflg
	tst16 GR1
	breq lsdo_0
	rjmp nondir
lsdo_0:
	lds16 GR1,dnp
	ldi16 GR2,statb
	ldi GR0L,n_sysstat
	call sysent
	brcc lsdo_1
	mov16 Z,Y
	rcall pstring_sr
	ldi16 Z,lsnonexistent<<1
	rcall pstring_fl
	ret
lsdo_1:
	//bit	$40000,statb+2	/test directory
	//beq	nondir
	//inc	isadir
	//mov	r4,r0
	//jsr	r5,fopen; lsdbuf
	//bcc	lsdo_2
	//jsr	r5,questf; < unreadable\n\0>; .even
	//rts	pc

	lds16 GR2,statb+2
	andi GR2H,$40 
	brne lsdo_1_0
	rjmp nondir
lsdo_1_0:
	lds16 GR2,isadir
	inc16 GR2
	sts16 isadir,GR2
	mov16 GR1,Y
	ldi16 Y,lsdbuf
	rcall fopen_lib
	brcc lsdo_2
	mov16 Z,GR1
	rcall pstring_sr
	ldi16 Z,lsunreadable<<1
	rcall pstring_fl
	ret
lsdo_2:
	//cmp	ocount,$1
	//ble	lsdo_3
	//tst	isadir
	//beq	lsdo_3
	//jsr	r5,pstring; dnp
	//jsr	r5,pstring; colon

	lds16 GR2,ocount
	ldi16 GR0,1
	cp16 GR2,GR0
	brlt lsdo_3
	breq lsdo_3
	lds16 GR2,isadir
	tst16 GR2
	breq lsdo_3
	lds16 Z,dnp
	rcall pstring_sr
	ldi16 Z,colon<<1
	rcall pstring_fl
lsdo_3:
	//jsr	r5,getw; lsdbuf
	//bec	lsdo_5
	//mov	lsdbuf,r0
	//sys	close
	//cmp	ocount,$1
	//ble	lsdo_4
	//tst	isadir
	//beq	lsdo_4
	//jsr	r5,pstring; nl

	ldi16 Y,lsdbuf
	rcall getw_lib
	brcc lsdo_5
	lds16 GR1,lsdbuf
	ldi GR0L,n_sysclose
	call sysent
	lds16 GR2,ocount
	ldi16 GR0,1
	cp16 GR2,GR0
	brlt lsdo_4
	breq lsdo_4
	lds16 GR2,isadir
	tst16 GR2
	breq lsdo_4
	ldi16 Z,nl<<1
	rcall pstring_fl
lsdo_4:
	//rts	pc

	ret
lsdo_5:
	//mov	$4,-(sp)
	//tst	r0
	//bne	lsdo_7

	ldi GR1L,4
	tst16 GR0
	brne lsdo_7
lsdo_6:                      
	//jsr	r5,getw; lsdbuf
	//dec	(sp)
	//bne	lsdo_6
	//tst	(sp)+
	//br	lsdo_3

	ldi16 Y,lsdbuf
	rcall getw_lib
	dec GR1L
	brne lsdo_6
	rjmp lsdo_3             
lsdo_7:
	//mov	r3,r2
	//mov	$namef,r1

	mov16 Z,GR3
lsdo_8:
	//jsr	r5,getw; lsdbuf
	//movb	r0,(r2)+
	//swab	r0
	//movb	r0,(r2)+
	//dec	(sp)
	//bne	lsdo_8
	//tst	(sp)+
	//clrb	(r2)+
	//tst	allflg
	//bne	lsdo_9
	//cmpb	(r3),$'.
	//bne	lsdo_9
	//br	lsdo_3

	ldi16 Y,lsdbuf
	rcall getw_lib
	st16i Z+,GR0
	dec GR1L
	brne lsdo_8
	clr GR0L
	st Z+,GR0L
	lds16 GR0,allflg
	tst16 GR0
	brne lsdo_9
	mov16 Z,GR3
	ld GR0L,Z
	cpi GR0L,'.'
	brne lsdo_9
	rjmp lsdo_3
lsdo_9:
	//mov	$8.,-(sp)
	//mov	$namef,r1

	ldi GR1L,8
	mov16 Z,GR3
lsdo10:
	//movb	(r1)+,r0
	//beq	lsdo_11
	//jsr	r5,putc; lsobuf
	//dec	(sp)
	//bne	lsdo10

	ld GR0L,Z+
	tst GR0L
	breq lsdo_11
	ldi16 Y,lsobuf
	rcall putc_lib
	dec GR1L
	brne lsdo10
lsdo_11:
	//tst	(sp)+
	//jsr	r5,pstring; nl
	//br	lsdo_3

	ldi16 Z,nl<<1
	rcall pstring_fl
	rjmp lsdo_3

nondir:
	//mov	r3,r2

	mov16 Z,GR3
nondir_1:
	//movb	(r4)+,(r2)+
	//bne	nondir_1

	ld GR0L,Y+
	st Z+,GR0L
	tst GR0L
	brne nondir_1
nondir_2:
	//cmp	r2,r3
	//blos	nondir_3
	//cmpb	-(r2),$'/
	//bne	nondir_2
	//inc	r2

	cp16 Z,GR3
	brlo nondir_3
	breq nondir_3
	ld GR0L,-Z
	cpi GR0L,'/'
	brne nondir_2
	inc16 Z
nondir_3:
	//mov	$8.,-(sp)

	ldi GR1L,8
nondir_4:
	//movb	(r2)+,r0
	//beq	nondir_5
	//jsr	r5,putc; obuf
	//dec	(sp)
	//bne	nondir_4

	ld GR0L,Z+
	tst GR0L
	breq nondir_5
	ldi16 Y,lsobuf
	rcall putc_lib
	dec GR1L
	brne nondir_4

nondir_5:
	//tst	(sp)+
	//jsr	r5,pstring; nl
	//rts	pc

	ldi16 Z,nl<<1
	rcall pstring_fl
	ret

//INPUT: Z(damage) - hold address of message   
//OUTPUT: None
//USED: GR0,Y
//ACTION:
//NOTE:
pstring_fl: //ok
	//mov	r5,-(sp)
	//mov	(r5),r5

	push16 Y
	push GR0L
pstring_fl_1:
	//movb	(r5)+,r0
	//beq	pstring_2
	//jsr	r5,putc; obuf
	//br	pstring_1

	lpm GR0L,Z+
	tst GR0L
	breq pstring_fl_2
	ldi16 Y,lsobuf
	rcall putc_lib
	rjmp pstring_fl_1
pstring_fl_2:
	//mov	(sp)+,r5
	//tst	(r5)+
	//rts	r5
	
	pop GR0L
	pop16 Y
	ret

//INPUT: Z(damage) - hold address of message   
//OUTPUT: None
//USED: GR0,Y
//ACTION:
//NOTE:
pstring_sr: //ok
	//mov	r5,-(sp)
	//mov	(r5),r5

	push16 Y
	push GR0L
pstring_sr_1:
	//movb	(r5)+,r0
	//beq	pstring_2
	//jsr	r5,putc; obuf
	//br	pstring_1

	ld GR0L,Z+
	tst GR0L
	breq pstring_sr_2
	ldi16 Y,lsobuf
	rcall putc_lib
	rjmp pstring_sr_1
pstring_sr_2:
	//mov	(sp)+,r5
	//tst	(r5)+
	//rts	r5
	
	pop GR0L
	pop16 Y
	ret

//INPUT: Y - hold address of ioptr  
//OUTPUT: None
//USED: GR1,GR2,GR3
//ACTION:
//NOTE:

flush_lib: //ok
	//mov	r0,-(sp)
	//mov	r1,-(sp)
	//mov	(r5)+,r1
	//jsr	pc,fl
	//mov	(sp)+,r1
	//mov	(sp)+,r0
	//rts	r5

	push16 GR3
	push16 GR2
	push16 GR1
	push16 GR0
	rcall fl_lib
	pop16 GR0
	pop16 GR1
	pop16 GR2
	pop16 GR3
	ret

//INPUT:  Y - hold address of ioptr 
//OUTPUT: None
//USED: GR0(damage),GR1(damage),GR2(damage),GR3(damage)
//ACTION:
//NOTE:

fl_lib: //ok
	//mov	r1,r0
	//add	$6,r0
	//mov	r0,-(sp)
	//mov	r0,0f
	//mov	4(r1),0f+2
	//beq	fl_lib_1
	//sub	(sp),0f+2
	//mov	(r1),r0
	//sys	0; 9f
	//.data
	//9:
	//sys	write; 0:..; ..
	//.text

	ldi16 GR2,$0006
	add16 GR2,Y      //GR2 - also (sp)
	ldd16 GR3,Y+4
	tst16 GR3
	breq fl_lib_1
	sub16 GR3,GR2
	ldd16 GR1,Y+0
	ldi GR0L,n_syswrite
	call sysent
fl_lib_1:
	//mov	(sp)+,4(r1)
	//mov	$512.,2(r1)
	//rts	pc

	ldi16 GR3,$0006  //GR3 - temp
	add16 GR3,Y
	std16 Y+4,GR3
	ldi16 GR3,16
	std16 Y+2,GR3
	ret

//INPUT: Y - hold address of ioptr; GR1 - namep
//OUTPUT: GR0 - descriptor
//USED: GR0,GR1,GR2,Y
//ACTION:
//NOTE:
fopen_lib:
	//mov	r1,-(sp)
	//mov	(r5)+,r1
	//mov	r0,0f
	//sys	0; 9f
	//.data
	//9:
	//sys	open; 0:..; 0
	//.text
	//bes	1f
	//mov	r0,(r1)+
	//clr	(r1)+
	//mov	(sp)+,r1
	//rts	r5
	push16 GR2
	clr16 GR2
	ldi GR0L,n_sysopen
	call sysent
	brcs fopen_lib_1
	st16i Y+,GR0
	clr16 GR2
	st16i Y+,GR2
	pop16 GR2
	ret
fopen_lib_1:
	//mov	$-1,(r1)
	//mov	(sp)+,r1
	//sec
	//rts	r5

	ldi16 GR2,$FFFF //GR2 - temp
	std16 Y+0,GR2
	pop16 GR2
	sec
	ret


//INPUT: Y - hold address of ioptr
//OUTPUT: GR0 - character
//USED: GR1,GR2,GR3
//ACTION:
//NOTE:
getw_lib:
	//mov	(r5),9f
	//mov	(r5)+,8f
	//jsr	r5,getc; 8:..
	//bec	getw_lib_1
	//rts	r5

	rcall getc_lib
	brcc getw_lib_1
	ret
getw_lib_1:
	//mov	r0,-(sp)
	//jsr	r5,getc; 9:..
	//swab	r0
	//bis	(sp)+,r0
	//rts	r5

	push GR0L
	rcall getc_lib
	mov GR0H,GR0L
	pop GR0L
	ret

//INPUT: Y - hold address of ioptr
//OUTPUT: GR0 - character(GR0L only)
//USED: GR1,GR2,GR3 
//ACTION:
//NOTE:
getc_lib:
	//mov	r1,-(sp)
	//mov	(r5)+,r1
	//dec	2(r1)
	//bge	getc_lib_3
	//mov	r1,r0
	//add	$6,r0
	//mov	r0,0f
	//mov	r0,4(r1)
	//mov	(r1),r0
	//sys	0; 9f
	//.data
	//9:
	//sys	read; 0:..; 512.
	//.text
	//bes	getc_lib_1
	//tst	r0
	//bne	getc_lib_2

	push16 GR3
	push16 GR2
	push16 GR1
	ldd16 GR3,Y+2
	dec16 GR3
	std16 Y+2,GR3
	brge getc_lib_3
	ldi16 GR2,$0006
	add16 GR2,Y
	std16 Y+4,GR2
	ldd16 GR1,Y+0	
	ldi16 GR3,16
	ldi GR0L,n_sysread
	call sysent
	brcs getc_lib_1
	tst16 GR0
	brne getc_lib_2

getc_lib_1:
	//mov	(sp)+,r1
	//sec
	//rts	r5

	pop16 GR1
	pop16 GR2
	pop16 GR3
	sec
	ret
getc_lib_2:
	//dec	r0
	//mov	r0,2(r1)

	dec16 GR0
	std16 Y+2,GR0
getc_lib_3:
	//clr	r0
	//bisb	*4(r1),r0
	//inc	4(r1)
	//mov	(sp)+,r1
	//rts	r5

	clr16 GR0
	mov16 GR2,Y //save
	ldd16 GR3,Y+4
	mov16 Y,GR3
	ld GR1L,Y
	or GR0L,GR1L
	inc16 Y
	mov16 GR3,Y
	mov16 Y,GR2 //restore
	std16 Y+4,GR3
	pop16 GR1
	pop16 GR2
	pop16 GR3
	clc
	ret

//INPUT: Y - hold address of ioptr; GR0 - character
//OUTPUT: None
//USED: GR0,GR1,GR2,GR3
//ACTION:
//NOTE:
putc_lib: //ok
	//mov	r1,-(sp)
	//mov	(r5)+,r1

	push16 GR3
	push16 GR2
	push16 GR1
putc_lib_1:
	//dec	2(r1)
	//bge	putc_lib_2
	//mov	r0,-(sp)
	//jsr	pc,fl
	//mov	(sp)+,r0
	//br	putc_lib_1

	ldd16 GR3,Y+2
	dec16 GR3
	std16 Y+2,GR3
	brge putc_lib_2
	push16 GR0
	rcall fl_lib
	pop16 GR0
	rjmp putc_lib_1
putc_lib_2:
	//movb	r0,*4(r1)
	//inc	4(r1)
	//mov	(sp)+,r1
	//rts	r5

	mov16 GR2,Y //save
	ldd16 GR3,Y+4
	mov16 Y,GR3
	st Y,GR0L
	inc16 Y
	mov16 GR3,Y
	mov16 Y,GR2 //restore
	std16 Y+4,GR3
	pop16 GR1
	pop16 GR2
	pop16 GR3
	ret


lsnonexistent:	.db " nonexistent",'\n',0
lsunreadable:	.db " unreadable",'\n',0
nl:				.db '\n',0
colon:			.db ':','\n',0

transfer_lsdata:
dot:	.db '.',0
dotp:	.dw (dot-transfer_lsdata)*2+0x102
endt_lsdata:


.dw $0000

.dseg
.org core+2

.overlap

begin_lsdata:
ddot:	.db '.',0
ddotp:	.dw 0

count:	.byte 2
ocount:	.byte 2
allflg:	.byte 2
dirflg:	.byte 2
isadir:	.byte 2
filnam:	.byte 32
namef:	.byte 8
dnp:	.byte 2
lsdbuf:	.byte 22
lsobuf:	.byte 22
statb:	.byte 34

.nooverlap

.cseg
eicont_ls:
	zrest_blk2 (eicont_ls - bicont_ls)
/*
// dsw
bicont_dsw:

//copy data from flash to SRAM
	ldi16 GR4,endt_dswdata<<1
	ldi16 Y,begin_dswdata
	ldi16 Z,transfer_dswdata<<1
cpy_dswdata:
	lpm GR3L,Z+
	st Y+,GR3L
	cp16 Z,GR4
	brlo cpy_dswdata

	//cmp	(sp)+,$2
	//blt	1f
	//tst	(sp)+
	//mov	(sp)+,0f
1:
	//sys	stat; 0:dot; stbuf
	//bes	error
	//mov	stbuf+4,r0
	//bic	$!60000,r0
	//cmp	r0,$40000
	//bne	error
	//mov	0b,0f
	//sys	open; 0:..; 0
	//bes	error
	//mov	r0,r1
1:
	//clrb	buf+17.
	//mov	r1,r0
	//sys	read; buf; 16.
	//bes	done
	//tst	r0
	//beq	done
	//tst	buf
	//beq	1b
	//mov	0b,r2
	//mov	$obuf,r3
2:
	//movb	(r2)+,(r3)+
	//bne	2b
	//mov	$buf+2,r2
	//dec	r3
	//cmpb	-1(r3),$'/
	//beq	2f
	//movb	$'/,(r3)+
2:
	//movb	(r2)+,(r3)+
	//bne	2b
	//sys	stat; obuf; stbuf
	//bes	error
	//bit	$60000,stbuf+4
	//bne	1b
	//mov	$buf+2,r2
2:
	//tstb	(r2)+
	//bne	2b
	//movb	$' ,-(r2)
	//sub	$buf+1,r2
	//mov	r2,0f

2:
	//mov	$1,r0
	//sys	write; buf+2; 0:..
	//clr	r0
	//sys	read; ch; 1
	//cmpb	ch,$'\n
	//beq	1b
	//clr	r0
	//sys	read; ch1; 1
	//cmpb	ch1,$'\n
	//beq	3f
4:
	//clr	r0
	//sys	read; ch; 1
	//cmpb	ch,$'\n
	//beq	2b
	//br	4b
3:
	//cmpb	ch,$'x
	//beq	done
	//cmpb	ch,$'y
	//bne	2b
	//sys	unlink; obuf
	//bes	error
	//br	1b

done:
	//sys	exit

error:
	//mov	$1,r0
	//sys	write; mes; 2
	//sys	exit

dot:	<.\0>
mes:	<?\n>
transfer_dswdata:


endt_dswdata:

.dseg
.org core+2

.overlap

begin_dswdata:

obuf:	.=.+100.
stbuf:	.=.+40.
buf:	.=.+18.
ch:	.=.+1
ch1:	.=.+1

.nooverlap
.cseg
eicont_dsw:
	zrest_blk (eicont_dsw - bicont_dsw)

// clri
bicont_clri:

//copy data from flash to SRAM
	ldi16 GR4,endt_clridata<<1
	ldi16 Y,begin_clridata
	ldi16 Z,transfer_clridata<<1
cpy_clridata:
	lpm GR3L,Z+
	st Y+,GR3L
	cp16 Z,GR4
	brlo cpy_clridata

	//cmp	(sp)+,$2
	//blt	error
	//beq	1f
	//mov	4(sp),fs1
	//mov	4(sp),fs2
1:
	//tst	(sp)+
	//mov	(sp)+,r0
	//clr	r5
1:
	//movb	(r0)+,r1
	//beq	1f
	//mpy	$10.,r5
	//sub	$'0,r1
	//cmp	r1,$10.
	//bhis	error
	//add	r1,r5
	//br	1b
1:
	//add	$31.,r5
	//mov	r5,r0
	//als	$-4,r0
	//mov	r0,0f
	//sys	open; fs1: filsys; 0
	//bes	error
	//mov	r0,-(sp)
	//sys	seek; 0:..; 3
	//mov	(sp),r0
	//sys	read; buf; 512.
	//mov	(sp)+,r0
	//sys	close
	//mov	r5,r0
	//als	$5,r0
	//bic	$!777,r0
	//add	$buf,r0
	//mov	$16.,r5
1:
	//clr	(r0)+
	//sob	r5,1b
	//sys	open; fs2: filsys; 1
	//bes	error
	//mov	r0,-(sp)
	//mov	0b,0f
	//sys	seek; 0:..; 3
	//mov	(sp)+,r0
	//sys	write; buf; 512.
	//bes	error
	//sys	exit

error:
	//mov	$1,r0
	//sys	write; 1f; 2f-1f
	//sys	exit
1:
//	<error\n>
2:

filsys:	</dev/junk\0>


transfer_clridata:


endt_clridata:

.dseg
.org core+2

.overlap

begin_clridata:

buf:	.=.+512.

.nooverlap
.cseg
eicont_clri:
	zrest_blk (eicont_clri - bicont_clri)


// sum
bicont_sum:

//copy data from flash to SRAM
	ldi16 GR4,endt_sumdata<<1
	ldi16 Y,begin_sumdata
	ldi16 Z,transfer_sumdata<<1
cpy_sumdata:
	lpm GR3L,Z+
	st Y+,GR3L
	cp16 Z,GR4
	brlo cpy_sumdata

	//mov	(sp)+,r3
	//tst	(sp)+
loop:
	//dec	r3
	//bgt	1f
	//sys	exit
1:
	//clr	bcnt
	//mov	(sp)+,0f
	//sys	open; 0:..; 0
	//bec	3f
	//mov	$1,r0
	//sys	write; 1f; 2f-1f
	//br	loop
1:
//	<oprd\n>
2:	.even
3:
	//mov	r0,r1
	//clr	r5
1:
	//mov	r1,r0
	//sys	read; buf; 512.
	//bes	err
	//tst	r0
	//beq	print
	//inc	bcnt
	//mov	$buf,r2
2:
	//movb	(r2)+,r4
	//add	r4,r5
	//adc	r5
	//sob	r0,2b
	//br	1b

err:
	//mov	$1,r0
	//sys	write; 1f; 2

print:
	//jsr	pc,decml
	//mov	$1,r0
	//sys	write; bl; 1
	//mov	bcnt,r5
	//jsr	pc,decml
	//mov	$1,r0
	//sys	write; nl; 1
	//mov	r1,r0
	//sys	close
	//br	loop
1:	<? >
nl:	<\n>
bl:	< >
	.even

decml:
	//mov	r0,-(sp)
	//mov	r1,-(sp)
	//mov	r5,r1
	//jsr	pc,1f
	//mov	(sp)+,r1
	//mov	(sp)+,r0
	//rts	pc

1:
	//clr	r0
	//dvd	$10.,r0
	//mov	r1,-(sp)
	//mov	r0,r1
	//beq	1f
	//jsr	pc,1b
1:
	//mov	(sp)+,ch
	//add	$'0,ch
	//mov	$1,r0
	//sys	write; ch; 1
	//rts	pc


transfer_sumdata:


endt_sumdata:

.dseg
.org core+2

.overlap

begin_sumdata:

ch:	.=.+2
bcnt:	.=.+2
buf: .=.+512.

.nooverlap
.cseg
eicont_sum:
	zrest_blk (eicont_sum - bicont_sum)
*/

testdata: //del

/* Template
// xxx
bicont_xxx:

//copy data from flash to SRAM
	ldi16 GR4,endt_lsdata<<1
	ldi16 Y,begin_lsdata
	ldi16 Z,transfer_lsdata<<1
cpy_lsdata:
	lpm GR3L,Z+
	st Y+,GR3L
	cp16 Z,GR4
	brlo cpy_lsdata

transfer_xxdata:


endt_xxxdata:

.dseg
.org core+2

.overlap

.nooverlap
.cseg
eicont_xxx:
	zrest_blk (eicont_xxx - bicont_xxx)
*/

.dw $1234 //del


	//adding inodes 41..
	//i-number,flags,nlks,uid,size
	addinode 41,$C00E,7,0,(eicont_root - bicont_root)*2 //root
	addinode 42,$C00E,2,0,(eicont_dev - bicont_dev)*2  //dev
	addinode 43,$C00E,2,0,(eicont_bin - bicont_bin)*2  //bin
	addinode 44,$C00E,2,0,(eicont_etc - bicont_etc)*2  //etc
	addinode 45,$C00E,2,0,(eicont_usr - bicont_usr)*2  //usr
	addinode 46,$C00F,2,0,(eicont_tmp - bicont_tmp)*2  //tmp 
	addinode 47,$801E,1,1,(eicont_init - bicont_init)*2  //init
	//addinode 109,$801E,2,1,(eicont_msh - bicont_msh)*2  //msh
	addinode 50,$801E,1,3,(eicont_cat - bicont_cat)*2  //cat
	addinode2 88,$901E,1,1,(eicont_sh - bicont_sh)*2  //sh
	addinode 64,$801E,1,3,(eicont_echo - bicont_echo)*2  //echo
	addinode2 75,$901E,1,3,(eicont_ls - bicont_ls)*2  //ls
//	addinode 48,$801E,2,3,(eicont_testtest - bicont_testtest)*2//testtest del
//	addinode 49,$C00E,2,3,(eicont_testxxxx - bicont_testxxxx)*2//testxxxx del


.dseg
.org core

.overlap

.byte procdata

.nooverlap

.org ecore

swpprocdata:
.byte nproc*(procdata+usize)

systm:
//.org systm+2	;(размер отоб. своб. хран.)ровно один
//.org systm+30	;(отоб. своб. хран.)блок.В дальнейшем возможно изменение.
//.org systm+2	;тоже самое для 
//.org systm+30	;отоб. хран. i-node'ов.
.byte 64 ;изменил с 128 на 64, суперблок укладывается в один блок 
	s_time: .dw $0000,$0000
	s_syst: .dw $0000,$0000
	s_wait: .dw $0000,$0000
	s_idlet:.dw $0000,$0000
	s_chrgt:.dw $0000,$0000

inode:
	i_flgs: .dw $0000
	i_nlks:	.db $00
	i_uid:	.db $00
	i_size:	.dw $0000
	i_dskp:	.byte 16
	i_ctim:	.dw $0000,$0000
	i_mtim:	.dw $0000,$0000
.org inode+32
mount: /*.org mount+64*/.byte 64
proc:
	p_pid:	.byte 2*nproc
	p_swpa:	.byte 2*nproc //эквивалентен disk address(dska)
	p_ppid:	.byte 2*nproc
	//p_break:.byte 2*nproc ;не требуется при фиксированных размерах(ничего не пакуется)
	p_link:	.byte nproc
	p_stat: .byte nproc
tty:		.byte ntty*8
fsp:		.byte nfiles*8
bufp:		.byte (nbuf*2)+4//+4(sb0 +sb1 except swp)
sb0:		.byte 8
sb1:		.byte 8
;swp		.byte 8

ii:		.dw $0000
idev:	.dw $0000
cdev:	.dw $0000
deverr:	.byte 3 //flash - 0 ,eeprom - 1 ,sd/cf card - 2
active:	.dw $0000
fmap:	.dw $0000 //flash memory
eeap:	.dw $0000 //eeprom 
mnti:	.dw $0000
mntd:	.dw $0000
mpid:	.dw $0000
clockp: .dw $0000
rootdir:.dw $0000
toutt:	.byte 16 //touts в постоянной памяти
runq:	.byte 6

wlist:	.byte 40
cc:		.byte 30
cf:		.byte 31
cl:		.byte 31
clist:	.byte clistsize
pl: 	.db $00
imod:	.db $00
smod:	.db $00
mmod:	.db $00
uquant:	.db $00
sysflg: .db $00
ttyoch:	.db $00
eereq:	.db $00 //eeprom device status request; 0 or $FF(or high byte $F)
		//set value $FF because group bits is safe instead $01 or other bit
eecnt:	.db $00 //counter

.byte 100
sstack:
.org sstack+1
buffer: .byte ntty*140
		.byte nbuf*(64+8)
//.org RAMEND-64+1 //structure user = 64?
user:
	u_sp:   	.dw $0000
	u_usp:  	.dw $0000
	u_r0:   	.dw $0000
	u_cdir:		.dw $0000
	u_fp:		.byte 10
	u_fofp: 	.dw $0000
	u_dirp:		.dw $0000
	u_namep:	.dw $0000
	u_off:  	.dw $0000
    u_base: 	.dw $0000
	u_count:	.dw $0000
	u_nread:	.dw $0000
	u_break:	.dw $0000
	u_ttyp:		.dw $0000
	u_dirbuf:	.byte 10
	u_pri:		.dw $0000
    u_intr: 	.dw $0000
	u_quit: 	.dw $0000

	u_cdev:		.dw $0000
	u_uid:		.db $00
	u_ruid:		.db $00
	u_uno:		.db $00
euser:
initargp:	.byte 4
initparam:	.db "/etc/init",'\0','\0','\0','\0' //второй ноль на всякий случай
/*вставить нули для подстраховки */

.eseg
free_storage_ee:

fs_map_size_ee:	.dw 6//2 //размер в байтах fs_map_ee
fs_map_ee:          //Отображение хранилища свободного хранения
.overlap


	ee_map_val(0)	//rsrv. blk for sblock and i-nodes 1..40
	ee_map_val(1)	//i-nodes 1..40
	ee_map_val(2)	//rest 12 rsvd. block (for not using)
	ee_map_val(3)
	ee_map_val(4)
	ee_map_val(5)

//	.db $00,$00   
//	.db $07,$FF   
//	.db $F0,$00   

.set ffb_fs_map_ee = 25//first free bit in fs_map(changing)

.nooverlap

//	rsvblk_map_ee //test
//	rsvblk_map_ee //test

.org fs_map_ee+6
inode_map_size_ee:	.dw 1  //размер в байтах inode_map
inode_map_ee:           //Отображение инодов
.overlap

	fillmem_w 0,$0000,inode_map_ee //all node free

.nooverlap

	//rsv_inode_map_ee 41 //test
	//rsv_inode_map_ee 43 //test
	//rsv_inode_map_ee 48 //test
inode_map_ee_end:
	zrest_blk_ee (inode_map_ee_end-fs_map_size_ee)
	//zrest_blk_ee 0 //test

.org free_storage_ee+64
free_storage_inodes_ee: //область хранения самих инодов(не информации в них!)


.org free_storage_inodes_ee+(0x100)//100 - 8 i-nodes(or 4 blk)
free_storage_data_ee://область хран. данных инодов(информация,вкл.директ.)

// root_ee
bicont_root_ee:
	.dw 41
	addstr8pddng0_ee ".."
	.dw 41
	addstr8pddng0_ee "."
	.dw 42 
	addstr8pddng0_ee "xxx"
	.dw 43 
	addstr8pddng0_ee "testfile"
//	.dw 48                        //del
//	addstr8pddng0_ee "testxxxx"   //del
eicont_root_ee:
	zrest_blk_ee (eicont_root_ee - bicont_root_ee)

bicont_xxx:
	.dw 41
	addstr8pddng0_ee ".."
	.dw 42
	addstr8pddng0_ee "."
	.dw 43 
	addstr8pddng0_ee "testfile"
eicont_xxx:
	zrest_blk_ee (eicont_xxx - bicont_xxx)

bicont_testfile:
	.db $AA
	.byte 64
	.db $DD
eicont_testfile:
	zrest_blk_ee (eicont_testfile - bicont_testfile)


	//adding inodes 41..
	//i-number,flags,nlks,uid,size
//	addinode_ee 41,$C00E,3,1,(eicont_root_ee - bicont_root_ee)  //root
//	addinode_ee 42,$C00E,2,1,(eicont_xxx - bicont_xxx)          //xxx
	addinode_ee 43,$800F,2,3,(eicont_testfile - bicont_testfile)//testfile

