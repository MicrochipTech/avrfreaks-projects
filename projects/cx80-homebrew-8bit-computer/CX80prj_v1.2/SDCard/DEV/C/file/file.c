/*
    CX80 - Homebrew 8bit computer
    (c) 2013 Calogiuri Enzo Antonio
    
    file.c - Simple test that show how, using system calls,
             create a text file and write into it.
              
    Compiler used: SDCC ver. 3.2.0 #8008
    Compiled on Windows 7 Home Premium
    
    To compile this source, open a Command Prompt (Windows) on file folder and
    type: make [return]
    
    To run this example on CX80, put file.bin on sdcard, power on CX80
    and type: run file.bin [return]
*/
#include <stdlib.h>
#include <stdio.h>

//Include the CX80 system definition
#include "../CX80.h"

//This point to a memory used for manage system calls
char *CX80_System_FNC_EntryPoint;
//This variable store the file ID (CX80 can handle three files at once) 
unsigned char FileID;

unsigned char CreateFile(char *FileName)
{
    //Obtain the real address of passed pointer
    unsigned short pFileName = (unsigned short)((char *)&FileName[0]);
    
    //REG1L = Access file mode. FA_WRITE | FA_CREATE_ALWAYS means that
    //if file exists, overrite it
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = FA_WRITE | FA_CREATE_ALWAYS;
    //REG2L and  REG2H contain the address of file name string
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2L_ADDR) = CX80_LoByte(pFileName);
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2H_ADDR) = CX80_HiByte(pFileName);
    
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_OPEN_CREATE_FILE;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
    
    //Return value of selected function.
    //If REG1H = 0xEF : error on file
    //If REG1H = 0xFF : no free file slot
    //If REG1H >= 0 and REG1H <= 2 : REG1H contain valid file ID
    return (*(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR));
}

void CloseFile(char fID)
{
    //REG1L = ID of the file to be closed
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = fID;
    
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_CLOSE_FILE;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
}

unsigned char WriteBufferToFile(char fID, char *wBuffer, unsigned short len)
{
    //Obtain the real address of passed pointer
    unsigned short pBuffer = (unsigned short)((char *)&wBuffer[0]);
    
    //REG1L = ID of the file to be written to
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = fID;
    //REG2L and REG2H contain the address of the buffer to write
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2L_ADDR) = CX80_LoByte(pBuffer);
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2H_ADDR) = CX80_HiByte(pBuffer);
    //REG3L and REG3H contain the length (in bytes) of buffer to write
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG3L_ADDR) = CX80_LoByte(len);
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG3H_ADDR) = CX80_HiByte(len);
    
    //Set the type of function desired
    *CX80_System_FNC_EntryPoint = CX80_FNC_WRITE_FILE_BUFFER;
    
    //Asks the system to perform the desired function
    CX80_DO_SYSTEM_CALL;
    
    //Return value of selected function.
    //If REG1H = 0xEF : wrong file slot
    //If REG1H = 0xAA : Error while writing
    //If REG1H = 0x00 : write ok
    return (*(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR));
}

int main(void)
{
    //Init the system calls pointer
    //the first 8 bytes of memory are reserved and used to control the system
    CX80_System_FNC_EntryPoint = (char *)0x0000;
    
    puts("Create ftest.txt ...\n\r");
    FileID = CreateFile("ftest.txt");
    
    if (FileID < 3)
    {
        //Valid file ID, now write data into file.
        
        if (WriteBufferToFile(FileID, "CX80 FILE TEST\n\r", 16) != 0)
            puts("Error while write on file!\n\r");
        else
            puts("Write ok\n\r");
        
        CloseFile(FileID);
    }
    else
        puts("Error on file creation!\n\r");
    
    return 0;
}