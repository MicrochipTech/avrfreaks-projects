/*
This is a set of random number routines based on the ISAAC algorithm at
http://burtleburtle.net/bob/rand/isaacafa.html
It is based on some heavy copy-and paste of free C code provided by Bob Jenkins.
The first 3/4 of this code is his, and it is marked as such. It is highly
recommened you check out the website for more info on the ISAAC... very nice.

This code created by Colin O'Flynn.

WARNING: Heavy useage of RAM and FLASH - about 6% FLASH and 3% SRAM of an
AtMega128! You can move some if it into EEPROM or something probably. Also note
that every time the generator is seeded it will create the same sequences, and
as written it has to be seeded every time at power-up. To avoid this, store the
ctx variable in EEPROM at power-down and restore it at power-up */

/*
------------------------------------------------------------------------------
Standard definitions and types, Bob Jenkins
------------------------------------------------------------------------------
*/


#ifndef STANDARD
# define STANDARD
/*
# ifndef STDIO
#  include <stdio.h>
#  define STDIO
# endif
# ifndef STDDEF
#  include <stddef.h>
#  define STDDEF
# endif
*/


//typedef  unsigned long long 	ub8;
//#define UB8MAXVAL 0xffffffffffffffffLL
//#define UB8BITS 64
//typedef    signed long long 	sb8;
//#define SB8MAXVAL 0x7fffffffffffffffLL
typedef  unsigned long  int  	ub4;   /* unsigned 4-byte quantities */
#define UB4MAXVAL 0xffffffff
typedef    signed long  int  	sb4;
#define UB4BITS 32
#define SB4MAXVAL 0x7fffffff
typedef  unsigned short int  	ub2;
#define UB2MAXVAL 0xffff
#define UB2BITS 16
typedef    signed short int  	sb2;
#define SB2MAXVAL 0x7fff
typedef  unsigned       char 	ub1;
#define UB1MAXVAL 0xff
#define UB1BITS 8
typedef    signed       char 	sb1;   /* signed 1-byte quantities */
#define SB1MAXVAL 0x7f
typedef                 int  	word;  /* fastest type available */

#define bis(target,mask)  ((target) |=  (mask))
#define bic(target,mask)  ((target) &= ~(mask))
#define bit(target,mask)  ((target) &   (mask))
#ifndef min
# define min(a,b) (((a)<(b)) ? (a) : (b))
#endif /* min */
#ifndef max
# define max(a,b) (((a)<(b)) ? (b) : (a))
#endif /* max */
#ifndef align
# define align(a) (((ub4)a+(sizeof(void *)-1))&(~(sizeof(void *)-1)))
#endif /* align */
#ifndef abs
# define abs(a)   (((a)>0) ? (a) : -(a))
#endif
#define TRUE  1
#define FALSE 0
#define SUCCESS 0  /* 1 on VAX */

#endif /* STANDARD */
/*
------------------------------------------------------------------------------
rand.h: definitions for a random number generator
By Bob Jenkins, 1996, Public Domain
MODIFIED:
  960327: Creation (addition of randinit, really)
  970719: use context, not global variables, for internal state
  980324: renamed seed to flag
  980605: recommend RANDSIZL=4 for noncryptography.
  010626: note this is public domain
------------------------------------------------------------------------------
*/

#ifndef RAND
#define RAND
#define RANDSIZL   (4)
#define RANDSIZ    (1<<RANDSIZL)

/* context of random number generator */
struct randctx
{
  ub4 randcnt;
  ub4 randrsl[RANDSIZ];
  ub4 randmem[RANDSIZ];
  ub4 randa;
  ub4 randb;
  ub4 randc;
};
typedef  struct randctx  randctx;

/*
------------------------------------------------------------------------------
 If (flag==TRUE), then use the contents of randrsl[0..RANDSIZ-1] as the seed.
------------------------------------------------------------------------------
*/
void randinit(randctx *ctx, word flag);

void isaac(randctx *ctx);


/*
------------------------------------------------------------------------------
 Call rand(/o_ randctx *r _o/) to retrieve a single 32-bit random value
------------------------------------------------------------------------------
*/
#define rand(r) \
   (!(r)->randcnt-- ? \
     (isaac(r), (r)->randcnt=RANDSIZ-1, (r)->randrsl[(r)->randcnt]) : \
     (r)->randrsl[(r)->randcnt])

#endif  /* RAND */
/*
------------------------------------------------------------------------------
rand.c: By Bob Jenkins.  My random number generator, ISAAC.  Public Domain.
MODIFIED:
  960327: Creation (addition of randinit, really)
  970719: use context, not global variables, for internal state
  980324: added main (ifdef'ed out), also rearranged randinit()
  010626: Note that this is public domain
------------------------------------------------------------------------------
*/
//NOTE: This file slightly modified by Colin O'Flynn November 2002
//http://burtleburtle.net/bob/rand/isaacafa.html is the website for info on this
//great generator RNG tested on DIEHARD by Colin
//http://stat.fsu.edu/~geo/diehard.html for info on DIEHARD

#ifndef STANDARD
#include "std_rnd.h"
#endif
#ifndef RAND
#include "rand.h"
#endif

#define ind(mm,x)  (*(ub4 *)((ub1 *)(mm) + ((x) & ((RANDSIZ-1)<<2))))
#define rngstep(mix,a,b,mm,m,m2,r,x) \
{ \
  x = *m;  \
  a = (a^(mix)) + *(m2++); \
  *(m++) = y = ind(mm,x) + a + b; \
  *(r++) = b = ind(mm,y>>RANDSIZL) + x; \
}

void     isaac(randctx *ctx)
{
   register ub4 a,b,x,y,*m,*mm,*m2,*r,*mend;
   mm=ctx->randmem; r=ctx->randrsl;
   a = ctx->randa; b = ctx->randb + (++ctx->randc);
   for (m = mm, mend = m2 = m+(RANDSIZ/2); m<mend; )
   {
      rngstep( a<<13, a, b, mm, m, m2, r, x);
      rngstep( a>>6 , a, b, mm, m, m2, r, x);
      rngstep( a<<2 , a, b, mm, m, m2, r, x);
      rngstep( a>>16, a, b, mm, m, m2, r, x);
   }
   for (m2 = mm; m2<mend; )
   {
      rngstep( a<<13, a, b, mm, m, m2, r, x);
      rngstep( a>>6 , a, b, mm, m, m2, r, x);
      rngstep( a<<2 , a, b, mm, m, m2, r, x);
      rngstep( a>>16, a, b, mm, m, m2, r, x);
   }
   ctx->randb = b; ctx->randa = a;
}


#define mix(a,b,c,d,e,f,g,h) \
{ \
   a^=b<<11; d+=a; b+=c; \
   b^=c>>2;  e+=b; c+=d; \
   c^=d<<8;  f+=c; d+=e; \
   d^=e>>16; g+=d; e+=f; \
   e^=f<<10; h+=e; f+=g; \
   f^=g>>4;  a+=f; g+=h; \
   g^=h<<8;  b+=g; h+=a; \
   h^=a>>9;  c+=h; a+=b; \
}

/* if (flag==TRUE), then use the contents of randrsl[] to initialize mm[]. */
void randinit(randctx *ctx, word flag)
{
   word i;
   ub4 a,b,c,d,e,f,g,h;
   ub4 *m,*r;
   ctx->randa = ctx->randb = ctx->randc = 0;
   m=ctx->randmem;
   r=ctx->randrsl;
   a=b=c=d=e=f=g=h=0x9e3779b9;  /* the golden ratio */

   for (i=0; i<4; ++i)          /* scramble it */
   {
     mix(a,b,c,d,e,f,g,h);
   }

   if (flag) 
   {
     /* initialize using the contents of r[] as the seed */
     for (i=0; i<RANDSIZ; i+=8)
     {
       a+=r[i  ]; b+=r[i+1]; c+=r[i+2]; d+=r[i+3];
       e+=r[i+4]; f+=r[i+5]; g+=r[i+6]; h+=r[i+7];
       mix(a,b,c,d,e,f,g,h);
       m[i  ]=a; m[i+1]=b; m[i+2]=c; m[i+3]=d;
       m[i+4]=e; m[i+5]=f; m[i+6]=g; m[i+7]=h;
     }
     /* do a second pass to make all of the seed affect all of m */
     for (i=0; i<RANDSIZ; i+=8)
     {
       a+=m[i  ]; b+=m[i+1]; c+=m[i+2]; d+=m[i+3];
       e+=m[i+4]; f+=m[i+5]; g+=m[i+6]; h+=m[i+7];
       mix(a,b,c,d,e,f,g,h);
       m[i  ]=a; m[i+1]=b; m[i+2]=c; m[i+3]=d;
       m[i+4]=e; m[i+5]=f; m[i+6]=g; m[i+7]=h;
     }
   }
   else
   {
     /* fill in mm[] with messy stuff */
     for (i=0; i<RANDSIZ; i+=8)
     {
       mix(a,b,c,d,e,f,g,h);
       m[i  ]=a; m[i+1]=b; m[i+2]=c; m[i+3]=d;
       m[i+4]=e; m[i+5]=f; m[i+6]=g; m[i+7]=h;
     }
   }

   isaac(ctx);            /* fill in the first set of results */
   ctx->randcnt=RANDSIZ;  /* prepare to use the first set of results */
}


/*==========================================================================
The following code is mine, the above is Bob Jenkins
============================================================================*/

#include "AVR_rand.h"
//stores the random number between accesses, takes 144 bytes of SRAM right
//there!!!, could reduce that by storing in EEPROM or something
static randctx					ctx;

/*Initilize random number generator with a seed. As long as the seed is the same
the resultant random number sequence will be the same. Every time you power on
the AVR you need to run this, therfor every time you power on the AVR you will
have the same sequence of numbers. HOWEVER, if you store the ctx variable in
something like EEPROM before powering down, and restore it on power-on, you
only need to run this routine the very first time*/
void												init_rnd_gen
	(
   void
   )
   {
   unsigned int				i;
   ctx.randa = ctx.randb = ctx.randc = (ub4)0;
   for (i=0; i<256; ++i) ctx.randrsl[i]=(ub4)0;
 	randinit(&ctx,FALSE);        
   return;
   }


/* Generate as many random floats as you say in the number_of_rnds_to_get
variable, and stores them in ptr_to_rndfloat[]. lower and upper limit set the
lower and upper limit of the resulting rumbers, anything between -127 and +127
should work fine. Note that lower_limit MUST be higher than upper_limit, and
upper_limit-lower_limit != 0 */
unsigned char									get_rnd_float
	(
   float 						ptr_to_rndfloat[],
   char							lower_limit,
   char							upper_limit,
   unsigned char				number_of_rnds_to_get
   )
   {
   unsigned char				counter;
   unsigned char				rnd_num_counter;


   //test that things look good
   if (lower_limit >= upper_limit)
   	return 1;
   
   //get some random numbers (16 to be exact)
   isaac(&ctx);

   rnd_num_counter = 0;
   for (counter = 0; counter < number_of_rnds_to_get; counter++)
   	{
      if (rnd_num_counter >= 16)
      	{
         rnd_num_counter = 0;
         isaac(&ctx);
         }
      //ctx.randrsl[ARRAY] is an array of type LONG that goes from 0 to 15, it
      //holds new random numbers on each call to the isaac routine
      ptr_to_rndfloat[counter] = (float)
      		    ((double)ctx.randrsl[rnd_num_counter] / (double)(UB4MAXVAL /
      			 (int)(upper_limit - lower_limit))) + lower_limit;
      rnd_num_counter++;
      }

   return 0;
   }

