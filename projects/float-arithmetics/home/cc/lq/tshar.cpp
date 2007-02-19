/*
Here is a collection of programs which, under different operating
systems make possible to keep track on parallel programming.

  Each main program
*/

#include <heport.h>
#include <helq.h>
#include <tshar.h>


#define TS_DEBUG



#if !defined AVR
int progno ;
const char *tsnext_p[NPRGPATH] ;	// tsnext string pointer
const char *proces_p[NPRGPATH] ;	// process name string pointer
int *spsave[NPRGPATH] ;
int *bpsave[NPRGPATH] ;

bool flag_defined[NPRGPATH] = { true } ;
#endif


#if defined AVR
uint8_t progno = 0 ;					// program index, 0=the main program
uint16_t frsave[NPRGPATH];				//
#endif



#if defined( MDOS50 ) || defined( MDOS40 )
FUNCT void tsopen(void)		// MDOS40
{
return ;
}

FUNCT int tseinri(int index, void (*funaddr)(), char *pname,
void *stackbase, int stacksize)
{
int *tos ;
int rv ;

if ( index >= 0 && index < NPRGPATH )
	{
	proces_p[index] = pname ;
	tos = ((int *)stackbase) + ( ( stacksize / sizeof(int) ) & 0xfffffffc ) ;
	*--tos = (int)funaddr ; // retaddr
	tos -= 7 ;			// 28 Bytes, 8 registers ; (ebp,ebx,edi,esi) eax ecx edx
	spsave[index] = tos ;
	bpsave[index] = tos+6 ;
	flag_defined[index] = true ;
	rv = 0 ;
	}
else
	{
	rv = 1 ;	// error number
	}
return rv ;
}
#endif


#if defined( MDOS50 ) || defined( MDOS40 )
void tsnext(const char *pt )
{
tsnext_p[ progno ] = pt ;
// Save context of actual function
__asm
	{
	push eax
	push ecx
	push edx
	mov ebx, progno
	mov spsave[ebx*4], esp
	mov bpsave[ebx*4], ebp
	}

	for(;;)
		{
		if ( ++progno >= NPRGPATH ) progno = 0 ;
		if ( flag_defined[ progno ] ) break ;
		}

// get context of next function
__asm
	{
	mov ebx, progno
	mov esp, spsave[ebx*4]
	mov ebp, bpsave[ebx*4]
	pop edx
	pop ecx
	pop eax
	}
return ;
}
#endif






#if defined __TURBOC__
FUNCT void tsopen(void)		// __TURBOC__
{
return ;
}


FUNCT int tseinri(int index, void (*funaddr)(), char *pname,
void *stackbase, int stacksize)
{
int *tos ;
int rv ;

if ( index >= 0 && index < NPRGPATH )
	{
	proces_p[index] = pname ;
	tos = ((int *)stackbase) + ( ( stacksize / sizeof(int) ) & 0xfffffffc ) ;
	*--tos = (int)funaddr ; // retaddr
	tos -= 3+1 ;			// 8 Bytes, 3 registers ; ax cx dx
	spsave[index] = tos ;
	bpsave[index] = tos+4 ;
	flag_defined[index] = true ;
	rv = 0 ;
	}
else
	{
	rv = 1 ;	// error number
	}
return rv ;
}


FUNCT void tsnext(const char *pt )		// __TURBOC__
{
tsnext_p[ progno ] = pt ;
// Save context of actual function
asm {
	push ax
	push cx
	push dx
	mov bx, progno
	add bx,bx
	mov spsave[bx], sp
	mov bpsave[bx], bp
	}

	for(;;)
		{
		if ( ++progno >= NPRGPATH ) progno = 0 ;
		if ( flag_defined[ progno ] ) break ;
		}

// get context of next function
asm {
	mov bx, progno
	add bx,bx
	mov sp, spsave[bx]
	mov bp, bpsave[bx]
	pop dx
	pop cx
	pop ax
	}
return ;
}
#endif	// __TURBOC__



// ******************************************************************* AVR
#if defined AVR
FUNCT void tsopen(void)		// AVR
{
return ;
}

FUNCT void tseinri_P(
	int index,
	void (*funaddr)(),
	void *stackbase,
	int stacksize)	// Bytes in the stack
{
uint16_t *pf ;
uint8_t *pfc ;
// pf[0] = 0 ;		// unused

// frsave[0] = (uint16_t)pf ;
pf = (uint16_t *) ((char *)stackbase + (stacksize - (2*2) ));
pf[0] = 0x3344 ; // (uint16_t)stackbase + stacksize ; // preset caller frame

pfc = (uint8_t *)(pf + 1);			// Higher address = return addresse
*pfc = (uint8_t)((uint16_t)funaddr>>8) ;
++pfc ;
*pfc = (uint8_t)(uint16_t)funaddr ;

pf = (uint16_t *) ((char *)stackbase + (stacksize - (2*2) -1 -20));
frsave[index] = ((uint16_t) pf) ;

return ;
}


// 2006-05-23
// three emtpy loops of one main program and two functions, NPRGPATH = 3
// takes 278 Cycles, At 16 MHz = 17.375 us


FUNCT void dummy_tsnext_P(void)		// AVR
{

// Save old Frame pointer (callers caller stack)
//		PUSH	R28				 Push register on stack
//		PUSH	R29				 Push register on stack
//
//		IN		R28,0x3D		 In from SPL
//		IN		R29,0x3E		 In from SPH
//		SBIW	R28,0x02		 Subtract immediate from word, needs 2 bytes in tsnext
//		IN		R0,0x3F			 In from I/O location
//		CLI						 Global Interrupt Disable
//		OUT		0x3E,R29		 Out to I/O location
//		OUT		0x3F,R0			 Out to I/O location interrup enable after next statement
//		OUT		0x3D,R28		 Out to I/O location
//		STD		Y+2,R25			 Store indirect with displacement , save *pt
//		STD		Y+1,R24			 Store indirect with displacement

// here
asm("	.text                      \n" );
asm("	.global tsnext_P                      \n" );
asm("tsnext_P:                       \n" );
asm("	.global _Z8tsnext_Pv                      \n" );
asm("_Z8tsnext_Pv:                       \n" );
asm("	push	r2                      \n" );	// 1
asm("	push	r3                      \n" );	// 2
asm("	push	r4                      \n" );	// 3
asm("	push	r5                      \n" );	// 4
asm("	push	r6                      \n" );	// 5
asm("	push	r7                      \n" );	// 6
asm("	push	r8                      \n" );	// 7
asm("	push	r9                      \n" );	// 8
asm("	push	r10                     \n" );	// 9
asm("	push	r11                     \n" );	// 10
asm("	push	r12                     \n" );	// 11
asm("	push	r13                     \n" );	// 12
asm("	push	r14                     \n" );	// 13
asm("	push	r15                     \n" );	// 14
asm("	push	r16                     \n" );	// 15
asm("	push	r17                     \n" );	// 16
asm("	push	r24                     \n" );	// 17
asm("	push	r25                     \n" );	// 18
asm("	push	r28                     \n" );	// 19
asm("	push	r29                     \n" );	// 20
asm("	push	r30                     \n" );	// 21
asm("	push	r31                     \n" );	// 22

asm("	in	r28,0x3D                     \n" );		// SPL
asm("	in	r29,0x3E                     \n" );		// SPH
asm("	lds	r24,progno                   \n" );
asm("	add	r24,r24                      \n" );
asm("	sub	r25,r25                      \n" );
asm("	ldi	r30,lo8(frsave)              \n" );
asm("	ldi	r31,hi8(frsave)              \n" );	// The Stack pointer arraz
asm("	add	r30,r24                      \n" );
asm("	adc	r31,r25                      \n" ); // Z = &frsave[progno]
asm("	std	Z+0,r28                      \n" );
asm("	std	Z+1,r29                      \n" );		// The frame is saved
asm("tsnext_P_1:                         \n" );
asm("	lds	r24,progno                   \n" );
asm("	inc	r24                          \n" );
asm("	sts	progno,r24                   \n" );
asm("	cpi	r24, %0                      \n" : : "M" (NPRGPATH) : );		// NPRGPATH
//asm("	cpi	r24,4                        \n" );		// NPRGPATH
asm("	brlo	tsnext_P_2               \n" );
asm("	clr	r24                          \n" );
asm("	sts	progno,r24                   \n" );
asm("tsnext_P_2:                         \n" );
//asm("	lds	r24,progno                   \n" );
asm("	add	r24,r24                      \n" );
asm("	sub	r25,r25                      \n" );
asm("	ldi	r30,lo8(frsave)                      \n" );
asm("	ldi	r31,hi8(frsave)                      \n" );
asm("	add	r30,r24                      \n" );
asm("	adc	r31,r25                      \n" );
asm("	ldd	r28,Z+0                      \n" );
asm("	ldd	r29,Z+1                      \n" );		// The frame is restored
asm("	or	r28,r29                      \n" );
asm("	breq	tsnext_P_1                      \n" );
// frsave[progno] = pf ; // Pointer to frame
// SP = pf
// *pf	   = pf[0] = local
// *(pf+1) = pf[1] = local
// *(pf+2) = pf[2] = Callers r29 YH
// *(pf+3) = pf[3] = Callers r28 YL
// *(pf+4) = pf[4] = Callers PC
// *(pf+5) = pf[5] = Callers PC

asm("	lds	r24,progno                      \n" );
asm("	add	r24,r24                      \n" );
asm("	sub	r25,r25                      \n" );
asm("	ldi	r30,lo8(frsave)                      \n" );
asm("	ldi	r31,hi8(frsave)                      \n" );
asm("	add	r30,r24                      \n" );
asm("	adc	r31,r25                      \n" );

asm("	ldd	r28,Z+0                      \n" );
asm("	ldd	r29,Z+1                      \n" );		// The stack pointer is restored
// asm("	adiw	r28,2                      \n" );

// Now SP points to some place (the stack of this function)
// SP+1=L SP+2=H : the caller frame pointer
// SP+3=L SP+4=H : the return addresse (start addresse in first loop)
asm("	in	r0,0x3f                      \n" );
asm("	cli                      \n" );
asm("	out	0x3E,r29                      \n" );
asm("	out	0x3F,r0                      \n" );
asm("	out	0x3D,r28                      \n" );
asm("	pop	r31                      \n" );		// ZL
asm("	pop	r30                      \n" );		// ZH
asm("	pop	r29                      \n" );		// YL
asm("	pop	r28                      \n" );		// YH Y = Frame Pointer
asm("	pop	r25                      \n" );
asm("	pop	r24                      \n" );
asm(" 	pop r17                      \n" );
asm(" 	pop r16                      \n" );
asm(" 	pop r15                      \n" );
asm(" 	pop r14                      \n" );
asm(" 	pop r13                      \n" );
asm(" 	pop r12                      \n" );
asm(" 	pop r11                      \n" );
asm(" 	pop r10                      \n" );
asm(" 	pop r9                      \n" );
asm(" 	pop r8                      \n" );
asm(" 	pop r7                      \n" );
asm(" 	pop r6                      \n" );
asm(" 	pop r5                      \n" );
asm(" 	pop r4                      \n" );
asm(" 	pop r3                      \n" );
asm(" 	pop r2                      \n" );
asm("	ret                      \n" );

//		   ADIW	   R28,0x02			Add immediate to word
//		   IN	   R0,0x3F			In from I/O location
//		   CLI						Global Interrupt Disable
//		   OUT	   0x3E,R29			Out to I/O location // restore SPH
//		   OUT	   0x3F,R0			Out to I/O location
//		   OUT	   0x3D,R28			Out to I/O location // restore SPL
//		   POP	   R29				Pop register from stack // REstore callers frame
//		   POP	   R28				Pop register from stack
//		   RET						Subroutine return

}

#endif
