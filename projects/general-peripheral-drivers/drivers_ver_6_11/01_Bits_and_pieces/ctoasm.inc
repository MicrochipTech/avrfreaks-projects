/*
    CtoASM.inc

    Specify the parameter passing from C functions

    Michael Stumpf, (c) 9. August 1999
*/

#ifndef __CTOASM_INC
#define __CTOASM_INC

#define rP0      r25   /* first incoming parameter (MSB) */
#define rP1      r24   /* 2nd incoming parameter         */
#define rP2      r23   /* 3rd incoming parameter         */
#define rP3      r22   /* 4th incoming parameter         */
#define rP4      r21   /* */
#define rP5      r20   /* */
#define rP6      r19   /* */
#define rP7      r18   /* */

#define rByte    r24   /* a function returning a byte passes this as if it was
                          an integer in R25:R24, but R25 is unused */
/* temporary registers, usable without saving,
   maybe clobbered after an external function call */
#ifndef __tmp_reg__
#define __tmp_reg__     r0
#endif
/* r1 MUST be cleared on return */
#ifndef __zero_reg__
#define __zero_reg__    r1
#endif

#define rTI0     r31  /* 1st temporary with immidiate */
#define rTI1     r30  /* 2nd temporary with immidiate */
#define rTI2     r27  /* 3rd temporary with immidiate */
#define rTI3     r26  /* 4th temporary with immidiate */

/* registers wich have to be saved */
#define rSI0     r17   /* first saved (pushed) register with immidiate */
#define rSI1     r16   /* 2nd push register  */
#define rSI2     r29   /* 3rd saved (pushed) register */
#define rSI3     r28   /* 4th saved (pushed) register  */

#define rS0      r15   /* 3rd pushed register  */
#define rS1      r14   /* 4th pushed register  */
#define rS2      r13   /* */
#define rS3      r12   /* */
#define rS4      r11   /* */
#define rS5      r10   /* */
#define rS6      r9    /* */
#define rS7      r8    /* */
#define rS8      r7    /* */
#define rS9      r6    /* */
#define rS10     r5    /* */
#define rS11     r4    /* */
#define rS12     r3    /* */
#define rS13     r2    /* */

#endif /* __CTOASM_INC */
