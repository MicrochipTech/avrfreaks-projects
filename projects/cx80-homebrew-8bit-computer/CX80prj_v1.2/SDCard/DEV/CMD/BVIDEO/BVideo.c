#include <stdlib.h>
#include <stdio.h>

//Include the CX80 system definition
#include "../CX80.h"

//This point to a memory used for manage system calls
char *CX80_System_FNC_EntryPoint;

int main(void)
{
    CX80_System_FNC_EntryPoint = (char *)0x0000;
    
    CX80_System_FNC_EntryPoint[CX80_SYSTEM_FNC_REG_ADDR] = CX80_FNC_INVERT_VIDEO_OUTPUT;
    CX80_System_FNC_EntryPoint[CX80_SYSTEM_REG1L_ADDR] = 0;
    
    CX80_DO_SYSTEM_CALL;
    
    return 0;
}