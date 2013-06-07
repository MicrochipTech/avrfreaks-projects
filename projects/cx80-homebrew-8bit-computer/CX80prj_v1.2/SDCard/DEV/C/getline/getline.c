/*
    CX80 - Homebrew 8bit computer
    (c) 2013 Calogiuri Enzo Antonio
    
    getline.c - Simple test that show how, using system calls,
                get a string typed by user.
              
    Compiler used: SDCC ver. 3.2.0 #8008
    Compiled on Windows 7 Home Premium
    
    To compile this source, open a Command Prompt (Windows) on getline folder and
    type: make [return]
    
    To run this example on CX80, put getline.bin on sdcard, power on CX80
    and type: run getline.bin [return]
*/
#include <stdlib.h>
#include <stdio.h>

//Include the CX80 system definition
#include "../CX80.h"

//This point to a memory used for manage system calls
char *CX80_System_FNC_EntryPoint;

//Buffer used for user input
char InputBuffer[30];

void GetLine(char *Buffer, char len)
{
    //Obtain the real address of passed pointer
    unsigned short pBuffer = (unsigned short)((char *)&Buffer[0]);
    
    /*
        Get input string function need to set 3 memory locations:
        REG1L = must contain the length of input buffer
        REG2L = must contain the lower byte of addres of input buffer
        REG2H = must contain the upper byte of addres of input buffer
    */
    
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = len;
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2L_ADDR) = CX80_LoByte(pBuffer);
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2H_ADDR) = CX80_HiByte(pBuffer);
    
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_GET_INPUT_STRING;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
}

int main(void)
{
    //Init the system calls pointer
    //the first 8 bytes of memory are reserved and used to control the system
    CX80_System_FNC_EntryPoint = (char *)0x0000;
    
    //Thanks to the file putchar.rel we can use printf, puts, ...
    printf("Example of getline system call\n\r");
    printf("Type a string, confirm with Return\n\r");
    printf("->");
    
    GetLine(InputBuffer, 30);
    
    printf("\n\rTyped:%s\n\r", InputBuffer); 
    
    //Exit to CX80 OS with no errors
    return 0;
}