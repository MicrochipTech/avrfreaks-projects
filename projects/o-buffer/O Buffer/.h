/*****************************************************
File                    : OBuffer.h
Version                 : 0.0
Date                    : 27/Dec/2008
Author                  : Ehab Anwar
                          flik.80@gmail.com
                          dhost.info/bdrelectronics                            
*********************************************************/

#define DataType        unsigned char
#define BufferLength    10      //not more than 255

void WriteToBuffer(DataType c);
DataType ReadFromBuffer(void);
unsigned char GetBufferPtr(void);