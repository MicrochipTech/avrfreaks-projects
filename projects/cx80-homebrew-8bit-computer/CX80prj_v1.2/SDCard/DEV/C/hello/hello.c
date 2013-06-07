/*
    CX80 - Homebrew 8bit computer
    (c) 2013 Calogiuri Enzo Antonio
    
    hello.c - Simple test that show how, using system calls,
              clear the screen, place the cursor on the screen and print text.
              
    Compiler used: SDCC ver. 3.2.0 #8008
    Compiled on Windows 7 Home Premium
    
    To compile this source, open a Command Prompt (Windows) on Hello folder and
    type: make [return]
    
    To run this example on CX80, put hello.bin on sdcard, power on CX80
    and type: run hello.bin [return]
*/

#include <stdlib.h>
#include <stdio.h>

//Include the CX80 system definition
#include "../CX80.h"

//This point to a memory used for manage system calls
char *CX80_System_FNC_EntryPoint;

void ClearScreen(void)
{
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_CLEAR_SCREEN;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
}

void MoveCursor(char row, char column)
{
    //Move cursor function need to set two memory location:
    //REG1L must contain the new row position
    //REG1H must contain the new column position
    //Valid row value must be between 0..24
    //Valid column value must be between 0..37
    
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = row;
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR) = column;
    
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_MOVE_CURSOR;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
}

int main(void)
{
    //Init the system calls pointer
    //the first 8 bytes of memory are reserved and used to control the system
    CX80_System_FNC_EntryPoint = (char *)0x0000;
    
    ClearScreen();
    
    MoveCursor(2, 13);
    
    //Thanks to the file putchar.rel we can use printf, puts, ...
    printf("Hello World!\n\r");
    
    //Exit to CX80 OS with no errors
    return 0;
}