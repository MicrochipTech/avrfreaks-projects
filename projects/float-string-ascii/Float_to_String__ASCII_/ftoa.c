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

#define SIZE_OF_MANTISSA			23
#define BITS_UNTIL_EXPONENT		23
#define BITS_UNTIL_SIGN				31

#include <stdlib.h>
#include "ftoa.h"  

void												return_string
   (
	char				  			sign,
   unsigned long				long_num,
	char							exp_base,
	char							exp,
	char							ch[]
	);

void												ftoa
	(
   float							fp,
   char							ch[]
   )
   {
   long *						fake_fp_pointer;
   unsigned long				fixed_point;
   char							sign;
   char							n;
   char							exponent;

   #if USE_BASE_2_EXP == 0
   char                    exponent_10x;
   unsigned int				m;
   #if ALLOW_FULL_RANGE == 1
	   float						max_value_10;
      float						max_value_2;
	#else
		unsigned long			max_value_10;
		unsigned long			max_value_2;
   #endif
   char							mult_or_divide;
   #endif

	/*Turn floating point to fixed point number, stored in type LONG
   with the decimal point stored at this point: 1.00000, eg: 2452 is 0.02452

   Algorithm used:
   1] set n to 0
   2] set F to 0
   3] read nth bit counting from LSB of floating point number, if 1, add 1 to F
   4] divide F by 2
   5] Increment n
   6] While still bits left to read, goto step 3
   */
   fake_fp_pointer = &fp;

   //1] set n to 0
   n = 0;

   //2] set F to 0
   fixed_point = 0;

	while (n < SIZE_OF_MANTISSA)
   	{
   	//3] read nth bit counting from LSB of floating point number, if 1,
      //   add 1 to F
		if (((*fake_fp_pointer >> n) & 0x01) == 1)
   		{
      	fixed_point += 100000;
      	}

  	 	//4] divide F by 2
   	fixed_point = fixed_point >> 1;

      //5] Increment n
      n++;
      }

 	//there is an extra 1 that is dropped by floating point
   fixed_point += 100000;

   //now convert the exponent to signed type
   exponent = (unsigned char)(*fake_fp_pointer >> BITS_UNTIL_EXPONENT)  - 127;

   //finally set up sign of mantissa if applicable
   if (((*fake_fp_pointer >> BITS_UNTIL_SIGN) & 0x01) == 1)
   	{
     	sign = '-';
      }
   else
   	{
      sign = '+';
      }

   //convert to a "normal" number if the number is smallish
	#if USE_E_ONLY_WHEN_NEEDED == 1
	if (exponent  >= 0 && exponent <= 10)
	   {
	   fixed_point *= 1 << exponent;
		return_string(sign, fixed_point, 0, 0, ch);
      return;
		}
	if (exponent >= -10 && exponent < 0)
	   {
	   fixed_point /= 1 << (exponent * -1);
      return_string(sign, fixed_point, 0, 0, ch);
      return;
		}
   #endif

   #if USE_BASE_2_EXP == 1
  	return_string(sign, fixed_point, 2, exponent, ch);
   return;
   #endif

   /* Now all thats left is to convert this data:
   	fixed_point X 2^exponent to something like
      fixed_point X 10^exponent, if you don't care about converting the number
		to an easier to use format, don't continue this code, as its just
		SLOW. Also, at this point the data is still accurate to about 6
		significant digits. Past this point you should test as it won't be...
   */

   #if USE_BASE_2_EXP == 0

   /*
      use the following algorithm to convert the two numbers
      1] Find a value of x so 10^x that is as small as possible, BUT still
         bigger than 2^exponent
      2] Divide 2^exponent by 10^x, store in m
      3] Multiply mantissa by m
      4] Add x10^x

   BUT in our case m must be below 0.2 or the type long will overflow, so if m
   is bigger than .2 we just use a bigger x
   */

   /*
   1] Find a value of x so 10^x that is as small as possible, BUT still
   bigger than 2^exponent

   as it turns out, dividing exponent by 3 using integer math is a pretty
   good way to get a rough approximation of step 1 that seems to work*/

   exponent_10x = exponent / 3;

	step2:
   mult_or_divide = 1;
   /*this section can use floating point... HOWEVER if you don't need to be able
   to use large exponents, you could easily get away with non-floating point.
   performs 10 (to the power of) exponent_10x, stored in max_value_10, but when
   exponent_10x is negative it uses some tricks to always use multiplication
   (this lets you use type unsigned long). */
   max_value_10 = 1;
	if (exponent_10x > 0)
   	{
	   n = 0;
      while (n < exponent_10x)
      	{
      	max_value_10 *= 10;
         n++;
         }
      }
   else
   	{
	   n = exponent_10x;
      while (n < 0)
      	{
         max_value_10 *= 10;
         n++;
         }
      mult_or_divide *= -1;
      }
	
   /*this section can use floating point... HOWEVER if you don't need to be able
   to use large exponents, you could easily get away with non-floating point.
   performs 2 (to the power of) exponent, stored in max_value_2, but when th
   exponent is negative it uses some tricks to always use multiplication (this
   lets you use type unsigned long) */
 
	max_value_2 = 1;
	if (exponent > 0)
   	{
	   n = 0;
		while (n < exponent)
      	{
      	max_value_2 *= 2;
         n++;
         }
      }
   else
   	{
	   n = exponent;
      while (n < 0)
      	{
         max_value_2 *= 2;
         n++;
         }
      mult_or_divide *= -1;
      }

   /*
   2] Divide 2^exponent by 10^x, store in m

   again... this is a slow spot due to floating point, but you could
   switch to fixed point to speed everything up if you wanted */
	
   if (mult_or_divide == 1)
   	{
	   m = (max_value_2 / max_value_10) * 10000;
      }
   else
   	{
      m = (max_value_2 * max_value_10) * 10000;
      }
      
	if (m > 20000)
	   {
   	exponent_10x++;
      //if m is too big, we increase the exp and try again, although this should
      //never happen it is a simple error checking
		goto step2;
		}

	//3] Multiply mantissa by m:
   fixed_point *= m;

   //move decimal place around
   exponent_10x = exponent_10x - 1;

   /* Now fixed_point has the number
   exponent_10x has the exponent, so the final
   number is

   sign fixed_point x10 ^ exponent_10x, and be sure to insert a decimal place
   between digit 5 and 6 (counting from the RIGHT) of fixed_point */

   return_string(sign, fixed_point, 10, exponent_10x, ch);
	#endif
   
   return;
   }


   /* This is the function that returns the proper ASCII string. It inserts a
decimal between the 5th and 6th digit (counting from RIGHT) of long_num,  adds
the sign, and when applicable inserts a letter and adds the exponent.

You can probably improve on this routine a bit, especially changing the itoa
call as it is only a char, but it typecasts it into an int

Also in AVR-GCC the call to ulta (unsigned long to ascii) takes about half of the
total time it takes to execute the entire function! So if you need faster speed,
try replacing that function.
*/

void												return_string
   (
	char				  			sign,
   unsigned long				long_num,
	char							exp_base,
	char							exp,
	char							ch[]
	)
   {
   unsigned char				array_count;
   unsigned char				end_of_array = 0;
   unsigned char				n;



   //if you use Imagecraft C Compiler change ultoa to ltoa
   //convert long to char in base 10 (note: this call takes about as much time
   //as the rest of the routine!!
	ultoa(long_num, &ch[1], 10);
	//add in sign
	ch[0] = sign;

   //get size of array
   while (ch[end_of_array] != '\0')
   	{
      end_of_array++;
      }

   array_count = end_of_array - 6;
   n = end_of_array;

   //move 5 digits over one to the right to make room for the '.'
	while (n > array_count)
	  	{
	 	ch[n+1] = ch[n];
	  	n--;
	  	}
	ch[n + 1] = '.';

   if (exp_base == 0)
      {
      return;
      }
   else
   	{
      //get exp value
      itoa((int)exp, &ch[end_of_array + 2], 10);

      #if USE_BASE_2_EXP == 1
			ch[end_of_array+1] = 't';
      #else
	      ch[end_of_array+1] = 'e';
   	#endif

      return;
      }
	}


