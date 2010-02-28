/*****************************************************
File                    : OBuffer.c
Version                 : 0.0
Date                    : 27/Dec/2008
Author                  : Ehab Anwar
                          flik.80@gmail.com
                          dhost.info/bdrelectronics
                          
*********************************************************/

#include <OBuffer.h>

eeprom DataType Buffer[BufferLength] = {0};
eeprom unsigned char BufferPtr[BufferLength] = {0};

//////////////////////////////////////////////////////////////////
void WriteToBuffer(DataType c)                                  //
        {                                                       //
        unsigned char Ptr, loop;                                //
        Ptr = GetBufferPtr();                                   //
        if(Ptr == BufferLength)                                 //
                {                                               //
                for(loop = 0; loop < BufferLength; loop++)      //
                        BufferPtr[loop] = 0;                    //
                BufferPtr[0] = 1;                               //
                Buffer[0] = c;                                  //
                }                                               //
        else                                                    //
                {                                               //
                BufferPtr[Ptr] = 1;                             //
                Buffer[Ptr] = c;                                //
                }                                               //
        }                                                       //
//////////////////////////////////////////////////////////////////
        
//////////////////////////////////                                              
DataType ReadFromBuffer(void)   //        
        {                       //         
        char Ptr;               //           
        Ptr = GetBufferPtr();   //          
        return Buffer[Ptr - 1]; //
        }                       //
//////////////////////////////////

//////////////////////////////////////////////////////////////////                              
unsigned char GetBufferPtr(void)                                //
        {                                                       //
        unsigned char loop;                                     //
        for(loop = 0; loop < BufferLength; loop++)              // 
                {                                               //
                if(BufferPtr[loop] == 0)                        //
                        return loop;                            //
                }                                               //
        return BufferLength;                                    //
        }                                                       //
//////////////////////////////////////////////////////////////////