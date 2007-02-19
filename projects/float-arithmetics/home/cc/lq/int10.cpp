#include <heport.h>
#include <helq.h>

// This is the DOS- INT-10 Call :
// Call Example :
//		 union REGS registers
//		 registers.x.ax =
//		 int10( &registers );
//		 if ( registers.x.ax == 0 ) ...
//
// Hint : The DOSGNU code is in source file int10.s
//
#if defined( BORWIN ) || defined ( __TURBOC__ )
int int10(union REGS *inregs)
{
return int86( 0x10, inregs, inregs );
}
/*
short int_10_si_save ;
short int_10_bp_save ;

int10(union REGS *inregs)
{
asm {


		mov		[int_10_bp_save],bp
		mov		ax,4[bp]
		mov		si,ax
		mov		[int_10_si_save],si
// load all registers for int10 call
		mov		ax,[si]
		mov		bx,2[si]
		mov		cx,4[si]
		mov		dx,6[si]
		mov		di,10[si]
		mov		si,8[si]

		int		10H
		push	di
		mov		di,[int_10_si_save]
		mov		[di],ax
		pop		ax
		mov		10[di],ax
		mov		8[di],si
		mov		2[di],bx
		mov		4[di],cx
		mov		6[di],dx

		mov		bp,[int_10_bp_save]

} // endasm
return 0 ;
}
*/
#endif	// BORWIN



#ifdef MDOS40

long int_10_si_save ;
long int_10_bp_save ;

// similar to int10.s, compare source codes !
// int10.s is for DOSGNU (32 Bit native DOS)
//
// This code should be used to display graphics by MDOS40 / MDOS50
// MSVC in Console application
int int10(union REGS *inregs)
{
__asm
		{
		mov		[int_10_bp_save],ebp

		push	ebx
		push	ecx
		push	edx
		push	esi
		push	edi

		mov		edi,8[ebp]	// get start address

		push	[edi]	// edi
		push	8[edi]	// ebp
		push	4[edi]	// esi
//		push	12[edi] // cflags
		push	16[edi] // ebx
		push	20[edi] // edx
		push	24[edi] // ecx
		push	28[edi] // eax
//		push	32[edi] // eflags


//		pop		eflags
		pop		eax
		pop		ecx
		pop		edx
		pop		ebx
//		pop		cflags
		pop		esi
		pop		ebp
		pop		edi


		int		16		// 10H, VIDEO-Interrupt
// push all registers into stack
// at this moment the application tends to crash - why ???

		push	ebp
		push	edi
		push	esi
//		push	cflags
		push	ebx
		push	edx
		push	ecx
		push	eax
//		push	eflags
// all return registers are pushed into stack

		mov		ebp,[int_10_bp_save]	//restore frame pointer
		mov		edi,8[ebp]		// get start adresse of union REGS

// restore them from stack into register area
//		pop		32[edi]		// eflags
		pop		28[edi]		// eax
		pop		24[edi]		// ecx
		pop		20[edi]		// edx
		pop		16[edi]		// ebx
//		pop		12[edi]		// cflags
		pop		4[edi]		// esi
		pop		[edi]		// edi
		pop		8[edi]		// ebp

		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx

} // endasm
return 0 ;
}
#endif	// MDOS40
