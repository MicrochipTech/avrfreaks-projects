/* A routine that changes a type FLOAT into an ASCII string. These routines
 have a lot of options, and can be fairly big (a normal set-up will take about
 1100 bytes of FLASH, routine can take up to 6000 bytes of FLASH if you need all
 the features of it).

 They operate at a fairly low-level, and i've tried to make them pretty efficent
 and flexible. They work with AVR-GCC, as they need a function that will change
 a type unsigned long into ASCII, and a type int (or char) into ascii.
 AVR-GCC provides these with ultoa and itoa functions, check your header file
 stdlib.h for your compiler to see what you will have to change them to (this
 is in the return_string function).

 Also they depend on the lay-out of the bits inside the floating point type,
 but luckily I suspect most ANSI C compilers will be the same for the AVR, but
 they might not be. So ALWAYS do some tests on these routines

 Also you will get a warning about assignment between incompatible pointer
 types, it is just the way this routine works. It might give errors on some
 compilers even, but on GCC it just warns you, so carefully ignore the warning !

 This routine created by Colin O'Flynn (c_oflynn@yahoo.com or coflynn@newae.com)
 , and is put in the public domain, freely. Feel free to to e-mail me with
 questions and comments, and I'm always interested in knowing about projects
 which found my code useful! */

/* If set to 1, a larger range of numbers is possible
	(bigger exponents, in my tests set to 1 works up until about 19 or 20
   significant digits, ie: 1e19). If set to 0, only small exponents
	are possible (ie: 1.234e4), BUT using small exponents
	prevents a LOT of floating point multiplication, which
	is VERY slow, so 0 is recommened unless otherwise needed (also setting
   this to 0 saves a TON of code space, test to find out just how much!

   Note: this setting has NO effect when using base-2 exponents, as they are
   normally set to maximum exponent range*/
#define ALLOW_FULL_RANGE			0

/* If set to 1, it returns the number in a base 2 exponent, ie:
	it returns 1.245t10, which means 1.245 x2^10 (the t is basically
   like the e, but a t is used to avoid confusion). Saves some time
   and removes ALL floating point calculations, which makes the code
   smaller as well. One other note is that setting this to 1 is the
   best way to get accurate floating point information, as all other
   conversions (converting to e notation especially especially) have error
   in them, so do some tests on the types of numbers you will use.

   When set to 0, the number is returned in this format:
   1.245e5, which means 1.245 x10^5 */
#define USE_BASE_2_EXP 				1

/* If set to 1, it will return the actual number in normal decimal
	notation when the number is small, and doesn't display it in the
	fancy e (or t) notation. Not converting to e notation saves a LOT
	of time, as it avoids a lot of floating point calculations, but
	doesn't remove them from the code. Set USE_BASE_2_EXP to remove
	floating point from code. Note that any number within approx +-200 will
	be returned in normal format with this set to 1. If you will only be using
   small numbers, set this to 1 and USE_BASE_2_EXP to 1 to remove floating point
   calculations*/
#define USE_E_ONLY_WHEN_NEEDED	1

/* Convert type FLOAT to a char, using the format
	specified in the defines */

void												ftoa
	(
   float							fp,
   char							ch[]
   );
