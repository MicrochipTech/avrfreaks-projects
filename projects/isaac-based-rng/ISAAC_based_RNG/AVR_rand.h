/* Use the ISAAC made by Bob Jenkins at
http://burtleburtle.net/bob/rand/isaacafa.html to generate a FLOAT type random
number, although easily adaptable to whatever you want.

Highly advised you check out the above website for more information on this
generator

WARNING: Ram usage could reach very high amounts at times, so try to use this
generator on at least AtMega processor. 144 bytes of RAM will be used as
static var storage, so kiss at LEAST 144 bytes goodbye...

This code written by Colin O'Flynn
*/


/*Initilize random number generator with a seed. As long as the seed is the same
the resultant random number sequence will be the same. Every time you power on
the AVR you need to run this, therfor every time you power on the AVR you will
have the same sequence of numbers. HOWEVER, if you store the ctx variable in
something like EEPROM before powering down, and restore it on power-on, you
only need to run this routine the very first time*/
void												init_rnd_gen
	(
   void
   );

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
   );

