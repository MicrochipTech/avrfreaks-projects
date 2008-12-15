// AVRStudio.h

#include "Global.h"						// Global definitions
#include "IDEs.h"
#include "Familys.h"

// IDE ID
#define IDE_ID IDE_AVRSTUDIO

// IDE Name
#define IDE_NAME "AVR Studio"

// IDE Version
#define IDE_VERSION "1.1"

// Project file extension
#define PROJECTEXTENSION ".apr"

// Project file extension
#define PROJECTFILEFILTER "AVR Studio Project (*.apr)|*.apr||"

// Number of supported familys
#define NUMBERFAMILYS 1

// Supported Familys
int supportedFamilys[] = {
	FAMILY_AVR};

// IDE Family support IDs
int familyIDs[] = {
	FAMILY_AVR, 0x01};

// OpCodes
#define OPCODE_AVR_RCALL 0xD0000000
#define OPCODE_AVR_RCALLMASK 0xF0000000
#define OPCODE_AVR_CALL 0x940E0000
#define OPCODE_AVR_CALLMASK 0xFE0E0000
#define OPCODE_AVR_RJMP 0x00C0
#define OPCODE_AVR_RJMPMASK 0x00F0
#define OPCODE_AVR_JMP 0x0C940000
#define OPCODE_AVR_JMPMASK 0x0EFE0000

// Text formatting info
#define ASMCOMMENT ";"
#define ASMSTRINGCHAR '\"'
#define ASMCHARCHAR '\''
#define ASMESCAPECHAR '\\'
#define ASMDIRECTIVECHAR '.'
#define ASMDIRECTIVES ".byte,.cseg,.csegsize,.db,.def,.device,.dseg,.dw,.endm,.endmacro,.equ,.eseg,.exit,.include,.list,.listmac,.nolist,.org,.set"
#define ASMKEYWORDCHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_"
#define ASMKEYWORDS ",sleep,rcall,icall,swap,bset,bclr,push,adiw,subi,sbci,sbiw,andi,rjmp,ijmp,reti,cpse,sbrc,sbrs,sbic,sbis,brbs,brbc,breq,brne,brcs,brcc,brsh,brlo,brmi,brpl,brge,brlt,brhs,brhc,brts,brtc,brvs,brvc,brie,brid,add,adc,sub,sbc,and,ori,eor,com,neg,sbr,cbr,inc,dec,tst,clr,ser,ret,cpc,cpi,mov,ldi,ldd,lds,std,sts,lpm,out,pop,sbi,cbi,lsl,lsr,rol,ror,asr,bst,bld,sec,clc,sen,cln,sez,clz,sei,cli,ses,cls,sev,clv,set,clt,seh,clh,nop,wdr,jmp,in,st,ld,cp,or,"
#define CCOMMENT "//"
#define CCOMMENTSTART "/*"
#define CCOMMENTEND "*/"
#define CSTRINGCHAR '\"'
#define CCHARCHAR '\''
#define CESCAPECHAR '\\'
#define CDIRECTIVECHAR '#'
#define CDIRECTIVES "#define,#elif,#else,#endif,#ifdef,#ifndef,#include,#pragma,#undef,#if defined,#if"
#define CKEYWORDCHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_"
#define CKEYWORDS ",asm,auto,break,case,char,const,continue,default,do,double,else,enum,extern,float,for,goto,if,int,long,register,return,short,signed,sizeof,static,struct,switch,typedef,union,unsigned,void,volatile,while,"

// Last Error
int error = SPERROR_SUCCESS;
char errorMessage[1000];

