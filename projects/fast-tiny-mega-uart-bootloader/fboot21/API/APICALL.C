/************************************************************************/
/*				API-call				*/
/************************************************************************
Input:  r22		= function number
	r27,r26		= source address in SRAM
	r21,r31,r39	= destination address in Flash

Output: r22		= success

Used:   r0,r1,r21,r22,r23,r26,r27,r30,r31
*/


#include <avr\io.h>
#include "apicall.h"
#include "protocol.h"


static void apicall( void ) __attribute__ ((noinline));
static void apicall( void )
{
  asm volatile("ldi r22, %0" :: "M" ((FLASHEND>>1)&0xFF));	// lo
  asm volatile("push r22");
  asm volatile("ldi r22, %0" :: "M" ((FLASHEND>>9)&0xFF));	// hi
  asm volatile("push r22");
#if( FLASHEND > 0x1FFFF )
  asm volatile("ldi r22, %0" :: "M" ((FLASHEND>>17)&0xFF));	// xhi
  asm volatile("push r22");
#endif
  asm volatile("ldi r22, %0" :: "M" (API_PROG_PAGE));		// function
  return;							// jump to API
}


/**************	copy one page from SRAM to Flash ************************/

unsigned char copy_flash( void *src, void *dst, unsigned char dst_hi )
{
  unsigned char i;

  if( (unsigned int)dst & (SPM_PAGESIZE-1))
    return API_ERR_PAGE;			// not on page limit

  asm volatile("movw r26, %0" :: "r" (src));
  asm volatile("movw r30, %0" :: "r" (dst));
  asm volatile("mov r21, %0" :: "r" (dst_hi));
  apicall();
  asm volatile("clr r1");			// clear zero reg
  asm volatile("mov %0, r22" : "=r" (i));
  return i;					// success
}
