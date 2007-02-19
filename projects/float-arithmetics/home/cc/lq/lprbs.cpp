
/* Pseudo Random Binary Sequence

This program generates reproducable random numbers for several acces tests

This program does the same algorithm a a shift register with xor-Feedback


 +--+  +--+	 +--+  +--+	 +--+  +--+	 +--+  +--+	 +--+  +--+	 +--+  +--+
 |QD|<-|QD|<-|QD|<-|QD|<-|QD|<-|QD|<-|QD|<-|QD|<-|QD|<-|QD|<-|QD|<-|QD|<----+
 |	|  |  |	 |	|  |  |	 |	|  |  |	 |	|  |  |	 |	|  |  |	 |	|  |  |		|
 |	|  |  |	 |	|  |  |	 |	|  |  |	 |	|  |  |	 |	|  |  |	 |	|  |  |		|
 +--+  +--+	 +--+  +--+	 +--+  +--+	 +--+  +--+	 +--+  +--+	 +--+  +--+		|
  |		|			|				  |		|			|	  |		|		|
  |		|			+-----------+	  |		+-----+		|	  |		|		|
  v		v						v	  v			  v		v	  v		v		|
 +-------+					   +-------+		 +-------+	 +-------+		|
 | XOR	 |					   | XOR   |		 | XOR	 |	 | XOR	 |		|
 +-------+					   +-------+		 +-------+	 +-------+		|
	 |							   |				 |			 |			|
	 +-----------------------+	   |				 |		+----+			|
							 v	   v				 v		v				|
							+-------+				+--------+				|
							| XOR	|				| XOR	 |				|
							+-------+				+--------+				|
								|						|					|
								+--------+	   +--------+					|
										 v	   v							|
										+-------+							|
										| XOR	|							|
										+-------+							|
											|								|
											+--->---------------------------+
The shift register is a 32 Bit shift register, which is shifted left one bit,
then the xor of some flipflop output are xored together to a parity signal
which is fed back to the data input of the first flipflop.
For start the flopflop must not contain 0, then it cycles randomally throug
2**n - 1 different states in a strange order.
The hardware version might be usead as a noise generator.
In the software version random ist better, if 32 shift clocks are executed to
buid a new number, then the bit combination is not dependend to the bit
combination of the preceding code.

This program is used for hidden information and diary storing.
So if this program is altered, my old diaries can not be read.
*/

#include <heport.h>
#include <helq.h>


uint32_t	lprbs_mask = 0xD3733749 ;	/* May be overwritten by other certified
									   mask constants. See C:\home\JP\Q\qprbs.c
									   and prbs.dat
									*/
uint32_t	lprbs_word = 3 ;			/* Start value and last random number */
int		lprbs_faktor = 32 ;			/* 32 generates very random numbers, while
									   1 would generate just numbers in which
									   successing number is twice its
									   preceding number plus one
									*/

#define PARITY 1		// 0 or 1

#ifndef MSVC40xx
#ifndef MDOS40xx
#ifndef DOSGNUxx
uint32_t	lprbs()
{
int i ;
uint32_t llprbs_word ;					/* local copy of ... */
uint32_t parity ;


llprbs_word = lprbs_word ;
for ( i = 0 ; i < lprbs_faktor ; ++i )
	{
	parity = llprbs_word & lprbs_mask ;		// select some bits
	parity ^= ( parity >>16 ) ;		// bring parity of result to bit 0
	parity ^= ( parity >> 8 ) ;
	parity ^= ( parity >> 4 ) ;
	parity ^= ( parity >> 2 ) ;
	parity ^= ( parity >> 1 ) ;
	llprbs_word += llprbs_word ;			// shift left
	if ( ( parity & 1 ) == PARITY )						  // If odd parity
		{
		llprbs_word++ ;						// set last bit, if parity was ODD
		}
	}		/* make more bit cycles */
lprbs_word = llprbs_word ;
return llprbs_word ;
}
#endif
#endif
#endif


#if defined( MSVC40xx ) || defined( MDOS40xx )
uint32_t	lprbs()
{

__asm
	{

	mov edx,lprbs_word
	mov ecx,lprbs_faktor
	mov ebx,lprbs_mask
lprbs_loop:
	mov eax,edx		// old shift register content
	add edx,edx		// new shift register content
	and eax,ebx
#if PARITY
	jpe lprbs_no_inc	// parity even
#else
	jnp lprbs_no_inc	// parity even
#endif
	inc edx				// parity odd
lprbs_no_inc:
	dec cx				// repeat loop counter
	jnz lprbs_loop
	mov lprbs_word,edx
	}
return lprbs_word ;
}
#endif



#ifdef DOSGNUxx
// GNU Assembler routines must preserve registers!
uint32_t	lprbs()
{
asm( "push %edx" );		// preserve registers
asm( "push %ecx" );		// onto stack
asm( "push %ebx" );

asm( "mov _lprbs_word, %edx" );
asm( "mov _lprbs_faktor, %ecx" );
asm( "mov _lprbs_mask, %ebx" );
asm( "lprbs_loop:" );
asm( "mov %edx, %eax" );

asm( "add %edx, %edx" );
asm( "and %ebx, %eax" );

#if PARITY
asm( "jpe lprbs_no_inc" );
#else
asm( "jnp lprbs_no_inc" );
#endif


asm( "inc %edx" );
asm( "lprbs_no_inc:" );
asm( "dec %cx" );				// repeat loop counter
asm( "jnz lprbs_loop" );
asm( "mov %edx, _lprbs_word" );

asm( "pop %ebx" );
asm( "pop %ecx" );		// must preserve registers
asm( "pop %edx" );		// must preserve registers

return lprbs_word ;
}
#endif
