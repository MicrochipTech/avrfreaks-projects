#include <mega32.h>
#include "bin2ascii.h"

// Important: You have to disable the autamtic global register allocation in the compiler settings.
void main()
{
    unsigned int num16=0xffff;
    unsigned long num32=0xffffffff;
    unsigned char buffer[10];

    utoa16(num16,buffer);
    utoa32(num32,buffer);

while(1)
    {

    #asm nop
    #endasm

    }

}