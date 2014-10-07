#ifndef IO_H
   #define IO_H

   // constant definitions used for controlling IO bits
   typedef enum {pinA, pinB, pinC, pinD} pins;

   extern void IoBitSet (pins bit);
   extern void IoBitClr (pins bit);
   extern void IoInit (void);

#endif
