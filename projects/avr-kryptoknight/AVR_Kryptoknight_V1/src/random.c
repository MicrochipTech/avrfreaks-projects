/*!\file random.c
 *File containing routines to generate truly random numbers, based on a
 *hardware random number generator.
 */
#define NEED_TIMER1
#define NEED_HASH
#include "define.h"

/**********************************************************************
Local functions
**********************************************************************/
static byte* plainRandom(void);

/*********************************************************************
Local variables
**********************************************************************/
static dword random=0;			//!<32bit random number, fresh data from RNG
volatile FLAGS flags;	//!<flags (\see define.h)


/*!This routine calculates 128-bit random numbers.  The random number
 *generating engine is seeded with 32-bit random numbers from a 
 *hardware random number generator.  The design of the random number 
 *generator engine is that of the Intel RNG.
 *\param bit128Random a pointer to a 16-byte array.  It will
 *contain the 128-bit randomword when the function returns.
 */
void getRandom(byte bit128Random[16]){
	byte Wt[64];		//64byte message (padded and length added)
	byte Wt_mem[40];			//remembers 10 dwords of Wt
	byte* hash;					//20-byte (160bit) hash value
	
	resetHash();

	//fill 512-datablock with null data
	for(byte i=0;i<64;i++)Wt[i]=0;
	
	/*Every round outputs one 32-bit number.  Five rounds are needed
	 *to fill an 128-bit number, because we don't use the 32-bit number
	 *of the first round.  This number is always the same, so unsafe.
	 */
	for(byte round=0;round<5;round++){
	
		//save 10 first dwords of Wt in remember block
		for(byte i=0;i<40;i++)Wt_mem[i]=Wt[i];
		
		//calculate hash
		hash=calcHash(Wt);

		//wait for random 32-bit value
		byte* p=plainRandom();
		
		//Set Wt in a new state
		for(byte i=0;i<20;i++)Wt[i]=hash[i];
		//copy new random dword to SHA-1 mixer input
		for(byte i=0;i<4;i++)Wt[i+20]=p[i];
		for(byte i=0;i<40;i++)Wt[i+24]=Wt_mem[i];
		
		/*RNG output:
		 *store 32-bit random number of mixer when not in first round, 
		 *otherwise first dword of 128bit random number would always 
		 *be the same.
		 */
		if(round>0)for(byte i=0;i<4;i++)bit128Random[(round-1)*4+i]=hash[16+i];
	}
}//getRandom*/


/*!Routine for calculating random numbers.  It starts a routine 
 *that generates 32-bit random numbers.  When a 32-bit random 
 *number is generated, the COMPLETE flag gets set.  This routine 
 *polls the status of this COMPLETE flag. The 32-bit random number
 *is then sent on the serial line. 
 *This routine waits for 32-bit random numbers of the input 
 *capture.
 */
static byte* plainRandom(){
	//Enables input capture interrupts
	timer1_Activate();
	//loop until WAITTIMER1-bit is set
	while(!(flags.waittimer1));
	flags.waittimer1=0;
	//for(byte i=0;i<4;i++)send_USART(p[i]);
	//disable input capture interrupts
	timer1_DeActivate();

	return (byte *)&random;
}//plainRandom