
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>


#define SAVE_CONTEXT() \
asm volatile	\
( 				\
	"push r0 \n\t"		\
	"push r1 \n\t"	\
	"push r2 \n\t"	\
	"push r3 \n\t"	\
	"push r4 \n\t"	\
	"push r5 \n\t" \
	"push r6 \n\t"\
	"push r7 \n\t"	\
	"push r8 \n\t"	\
	"push r9 \n\t"	\
	"push r10 \n\t"\
	"push r11 \n\t"\
	"push r12 \n\t"\
	"push r13 \n\t"\
	"push r14 \n\t"\
	"push r15 \n\t"\
	"push r16 \n\t"\
	"push r17 \n\t"\
	"push r18 \n\t"\
	"push r19 \n\t"\
	"push r20 \n\t"\
	"push r21 \n\t"\
	"push r22 \n\t"\
	"push r23 \n\t"\
	"push r24 \n\t"\
	"push r25 \n\t"\
	"push r26 \n\t"\
	"push r27 \n\t"\
	"push r28 \n\t"\
	"push r29 \n\t"\
	"push r30 \n\t" \
	"push r31 \n\t" \
	"in r1, __SREG__ \n\t"	\
	"push r1 \n\t"	\
	"cli \n\t"\
	"in r0, __SP_L__ \n\t"\
	"sts savedsp, r0 \n\t"\
	"in r0, __SP_H__ \n\t"\
	"sts savedsp+1, r0 \n\t"\
	"out __SREG__,r1 \n\t"\
	"clr r1 \n\t"\
);

#define RESTORE_CONTEXT() \
asm volatile (\
	"cli \n\t"\
	"lds r28, savedsp \n\t" \
	"out __SP_L__, r28 \n\t" \
	"lds r29, savedsp+1 \n\t" \
	"out __SP_H__, r29 \n\t" \
	"pop r0 \n\t" \
	"out __SREG__, r0 \n\t" \
	"pop r31 \n\t" \
	"pop r30 \n\t" \
	"pop r29 \n\t" \
	"pop r28 \n\t" \
	"pop r27 \n\t" \
	"pop r26 \n\t" \
	"pop r25 \n\t" \
	"pop r24 \n\t" \
	"pop r23 \n\t" \
	"pop r22 \n\t" \
	"pop r21 \n\t" \
	"pop r20 \n\t" \
	"pop r19 \n\t" \
	"pop r18 \n\t" \
	"pop r17 \n\t" \
	"pop r16 \n\t" \
	"pop r15 \n\t" \
	"pop r14 \n\t" \
	"pop r13 \n\t" \
	"pop r12 \n\t" \
	"pop r11 \n\t" \
	"pop r10 \n\t" \
	"pop r9 \n\t" \
	"pop r8 \n\t" \
	"pop r7 \n\t" \
	"pop r6 \n\t" \
	"pop r5 \n\t" \
	"pop r4 \n\t" \
	"pop r3 \n\t" \
	"pop r2 \n\t" \
	"pop r1 \n\t" \
	"pop r0 \n\t" \
);

void next_thread(void) __attribute__ ( ( naked ) ) __attribute__ ( ( noinline ) );
void makethreads(void) __attribute__ ( ( naked ) ) __attribute__ ( ( noinline ) );


// **** Global variables ****

	unsigned char* savedsp;

	unsigned char threadstack1[256];
	unsigned char threadstack2[150];

	unsigned char* stackp[3];		//[total number of stacks (including main)]

	unsigned char current_thread=0;	//0=main thread

void next_thread(void)
{
	SAVE_CONTEXT();
	stackp[current_thread]=savedsp;
	current_thread++;
	if(current_thread==3)
		current_thread=0;
	savedsp=stackp[current_thread];
	RESTORE_CONTEXT();
	asm volatile ( "ret" );
}

void makethreads(void)
{
	SAVE_CONTEXT();							//save context (33bytes + 2bytes return address from this routine)

	memcpy(&threadstack1[256-35],savedsp+1,35);	//create thread1 context
	stackp[1]=&threadstack1[256-35-1];

	memcpy(&threadstack2[150-35],savedsp+1,35);	//create thread2 context
	stackp[2]=&threadstack2[150-35-1];

	RESTORE_CONTEXT();
	asm volatile ( "ret" );
}
