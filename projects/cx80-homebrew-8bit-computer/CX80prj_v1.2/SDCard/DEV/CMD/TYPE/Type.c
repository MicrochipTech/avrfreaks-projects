#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Include the CX80 system definition
#include "../CX80.h"

#define MAX_LINE_ON_SCREEN          24

//This point to a memory used for manage system calls
char *CX80_System_FNC_EntryPoint;

char ReadLine[38];
unsigned char FileID;

///////////////////////////////////////////////////////////

void CX80PrintString(char *Txt)
{
    unsigned short pBuffer = (unsigned short)((char *)&Txt[0]);
    
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = CX80_LoByte(pBuffer);
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR) = CX80_HiByte(pBuffer);
    
    *CX80_System_FNC_EntryPoint = CX80_FNC_PRINT_STRING;
    
    CX80_DO_SYSTEM_CALL;
}

void GetLine(char *Buffer, char len)
{
    unsigned short pBuffer = (unsigned short)((char *)&Buffer[0]);
    
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = len;
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2L_ADDR) = CX80_LoByte(pBuffer);
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2H_ADDR) = CX80_HiByte(pBuffer);
    
    *CX80_System_FNC_EntryPoint = CX80_FNC_GET_INPUT_STRING;
    
    CX80_DO_SYSTEM_CALL;
}

unsigned char OpenFile(char *FName)
{
    unsigned short pFileName = (unsigned short)((char *)&FName[0]);
    
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = FA_READ;
    
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2L_ADDR) = CX80_LoByte(pFileName);
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2H_ADDR) = CX80_HiByte(pFileName);
    
    *CX80_System_FNC_EntryPoint = CX80_FNC_OPEN_CREATE_FILE;
    
    CX80_DO_SYSTEM_CALL;
    
    return (*(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR));
}

unsigned char GetString(char fID, char *buffer, unsigned short len)
{
    unsigned short pBuffer = (unsigned short)((char *)&buffer[0]);
    
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = fID;
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2L_ADDR) = CX80_LoByte(pBuffer);
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2H_ADDR) = CX80_HiByte(pBuffer);
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG3L_ADDR) = CX80_LoByte(len);
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG3H_ADDR) = CX80_HiByte(len);
    
    *CX80_System_FNC_EntryPoint = CX80_FNC_GET_STRING_FROM_FILE;
    
    CX80_DO_SYSTEM_CALL;
    
    return (*(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR));
}

void CloseFile(char fID)
{
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = fID;
    
    *CX80_System_FNC_EntryPoint = CX80_FNC_CLOSE_FILE;
    
    CX80_DO_SYSTEM_CALL;
}

void MoveCursor(char row, char column)
{
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = row;
    *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR) = column;
    
    *CX80_System_FNC_EntryPoint = CX80_FNC_MOVE_CURSOR;
    
    CX80_DO_SYSTEM_CALL;
}

unsigned char GetCh(void)
{
    *CX80_System_FNC_EntryPoint = CX80_FNC_WAIT_FOR_KEYPRESSED;
    
    CX80_DO_SYSTEM_CALL;
    
    return (*(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR)); 
}

int main(void)
{
    unsigned char ret, pCh;
    char cnt, *pntch;
    
    CX80_System_FNC_EntryPoint = (char *)0x0000;
    
    CX80PrintString("CX80 Type Command.\n\r");
    CX80PrintString("Show text file on video.\n\r");
    CX80PrintString("Filename (empty to exit)\n\r");
    CX80PrintString(":");
    
    GetLine(ReadLine, 38);
    
    CX80PrintString("\n\r");
    
    if (strlen(ReadLine) > 0)
    {
        ret = 0;
        cnt = 0;
        
        FileID = OpenFile(ReadLine);
        
        if (FileID < 3)
        {
            while (1)
            {
                ret = GetString(FileID, ReadLine, 38);
                    
                if (cnt == MAX_LINE_ON_SCREEN)
                {
                    MoveCursor(24, 0);
                    CX80PrintString("More? (Y/N)");
                    
                    pCh = GetCh();
                    
                    if ((pCh == 'y') || (pCh == 'Y'))
                    {
                        cnt = 0;
                        
                        MoveCursor(24, 0);
                        
                        CX80PrintString("           ");
                        
                        MoveCursor(24, 0);
                    }
                    else
                        break;
                }
                    
                CX80PrintString(ReadLine);
                
                cnt++;
                    
                pntch = strchr(ReadLine, '\n');
                
                if (pntch == NULL)
                    CX80PrintString("\n\r");
                    
                if (ret != 0)
                    break;
            }
            
            CloseFile(FileID);
            
            if (ret == 0xAA)
                CX80PrintString("Error while read file!\n\r");
        }
        else
           CX80PrintString("Error while open file!\n\r");
    }
    
    return 0;
}