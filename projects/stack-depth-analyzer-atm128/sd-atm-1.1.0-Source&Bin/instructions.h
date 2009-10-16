/********************************************************************************/
/*  sd-atm, John Kim, john-k.home@rcn.com,  October 2007                        */
/*   Stack depth analyzer, works for avr atm128.                                */
/*   Free code.                                                                 */
/*   Compiles on Windows, MSYS, MinGW, and "gcc (GCC) 3.4.2 (mingw-special)"    */
/*                                                                              */
/********************************************************************************/


#ifndef __INSTRUCTIONS_H__
#define __INSTRUCTIONS_H__

#define BRANCH_INSTRUCTIONS \
	"breq", "brge", "brne", "brcs", "brcc", "brlt", "brhc", "brhs", "brid", \
	"brie", "brmi", "brpl", "brtc", "brts", "brvc", "brvs", "brbc", "brbs"

#define SKIP_INSTRUCTIONS \
	"sbrs", "sbrc", "cpse", "sbic", "sbis"

#define JUMP_INSTRUCTIONS \
	"jmp", "rjmp"

#define INDIRECT_JUMP_INSTRUCTIONS \
	"ijmp", "eijmp"

#define CALL_INSTRUCTIONS \
	"call", "rcall"

#define INDIRECT_CALL_INSTRUCTIONS \
	"icall", "eicall"

#define LONG_INSTRUCTIONS \
	"call", "jmp", "sts", "lds"

#define INSTRUCTIONS_A \
	"adc", "add", "adiw", "and", "andi", "asr", "bld", "break", "bst", "cbi", "clh", "cli", \
	"cln", "cls", "clt", "clv", "clz", "com", "cp", "cpc", "cpi", "dec", "elpm", "eor"

#define INSTRUCTIONS_F \
	"fmul", "fmuls", "fmulsu", "in", "inc", "ldi", "lpm", "lsr", "mov", "movw", "mul", "muls", \
	"mulsu", "neg", "nop", "or", "ori", "out", "pop", "push", "ret", "reti", "ror"

#define INSTRUCTIONS_S \
	"sbc", "sbci", "sbi", "sbiw", "seh", "sei", "sen", "ses", "set", "sev", "sez", "sleep", \
	"spm", "sub", "subi", "swap", "wdr", "ld", "ldd", "sec", "st", "std", "lds", "sts"

#endif //__INSTRUCTIONS_H__
