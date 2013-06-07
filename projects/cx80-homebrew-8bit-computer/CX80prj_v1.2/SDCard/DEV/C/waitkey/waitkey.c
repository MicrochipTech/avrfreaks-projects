/*
    CX80 - Homebrew 8bit computer
    (c) 2013 Calogiuri Enzo Antonio
    
    waitkey.c - Simple test that show how, using system calls,
                program waits a keypress and print it on screen.
              
    Compiler used: SDCC ver. 3.2.0 #8008
    Compiled on Windows 7 Home Premium
    
    To compile this source, open a Command Prompt (Windows) on waitkey folder and
    type: make [return]
    
    To run this example on CX80, put waitkey.bin on sdcard, power on CX80
    and type: run waitkey.bin [return]
*/
#include <stdlib.h>
#include <stdio.h>

//Include the CX80 system definition
#include "../CX80.h"

//This point to a memory used for manage system calls
char *CX80_System_FNC_EntryPoint;

int main(void)
{
    //Init the system calls pointer
    //the first 8 bytes of memory are reserved and used to control the system
    CX80_System_FNC_EntryPoint = (char *)0x0000;
    
    printf("Press a key...");
    
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_WAIT_FOR_KEYPRESSED;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
    
    printf("\n\rKey pressed = %c\n\r", *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR));
    
    return 0;
}