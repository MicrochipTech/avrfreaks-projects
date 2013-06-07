/*
    CX80 - Homebrew 8bit computer
    (c) 2013 Calogiuri Enzo Antonio
    
    hello.c - Simple test that show how, using system calls,
              controll the cursor and the screen.
              
    Compiler used: SDCC ver. 3.2.0 #8008
    Compiled on Windows 7 Home Premium
    
    To compile this source, open a Command Prompt (Windows) on Screen folder and
    type: make [return]
    
    To run this example on CX80, put screen.bin on sdcard, power on CX80
    and type: run screen.bin [return]
*/

#include <stdlib.h>
#include <stdio.h>

//Include the CX80 system definition
#include "../CX80.h"

//This point to a memory used for manage system calls
char *CX80_System_FNC_EntryPoint;

///////////////////////////////////////////////////////////////

void WaitKeypressed(void)
{
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_WAIT_FOR_KEYPRESSED;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
}

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

void ScreenDemo(void)
{
    ClearScreen();
    
    puts("Move cursor to 18, 14\n\r");
    puts("Press key...\n\r"); 
    
    MoveCursor(14, 18);
    
    WaitKeypressed();
    
    ClearScreen();
    puts("Move cursor to home\n\r");
    puts("Press key...\n\r");
    
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_CURSOR_HOME;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
    
    WaitKeypressed();
    
    ClearScreen();
    puts("Cursor in block shape\n\r");
    puts("Press key...\n\r");
    
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_BLOCK_CURSOR;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
    
    WaitKeypressed();
    
    ClearScreen();
    puts("Cursor in underline shape\n\r");
    puts("Press key...\n\r");
    
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_UNDERLINE_CURSOR;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
    
    WaitKeypressed();
    
    ClearScreen();
    puts("Hidden the cursor\n\r");
    puts("Press key...\n\r");
    
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_CURSOR_SHOW;
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = 0;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
    
    WaitKeypressed();
    
    ClearScreen();
    puts("Show the cursor\n\r");
    puts("Press key...\n\r");
    
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_CURSOR_SHOW;
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = 1;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
    
    WaitKeypressed();
    
    ClearScreen();
    puts("Change line dimension\n\r");
    puts("Press key...\n\r");
    
    MoveCursor(14, 0);
    
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_SET_LINE_DIMENSION;
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = '1';
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
    
    puts("NEW LINE");
    
    WaitKeypressed();
    
    ClearScreen();
    puts("Invert video signal\n\r");
    puts("Press key...\n\r");
    
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_INVERT_VIDEO_OUTPUT;
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = 1;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
    
    WaitKeypressed();
    
    ClearScreen();
    puts("Restore video signal\n\r");
    puts("Press key...\n\r");
    
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_INVERT_VIDEO_OUTPUT;
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = 0;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
    
    WaitKeypressed();
}

int main(void)
{
    //Init the system calls pointer
    //the first 8 bytes of memory are reserved and used to control the system
    CX80_System_FNC_EntryPoint = (char *)0x0000;
    
    ClearScreen();
    
    puts("CX80 Screen Function Demo\n\r");
    puts("Press key...\n\r");
    
    WaitKeypressed();
    
    ScreenDemo();
    
    return 0;
}