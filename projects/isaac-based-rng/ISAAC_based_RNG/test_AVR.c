//#include <IO.h>
#include <stdio.h>

#include "AVR_rand.h"



int												main
	(
   void
   )
   {
   float                   rando_the_num[90];
   unsigned int				i;
   init_rnd_gen();
   get_rnd_float(rando_the_num, -100, 100, 90);
   for (i = 0; i < 90; i++)
   	{
  		printf("%f\n", rando_the_num[i]);
      }

   return 1;
   }

