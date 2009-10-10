//
// CRC8 calculation
// Call this fuction with this command:
//
// crc = CRC8cacl(tableOfData, lengthOfData);
//
// Example:
//
// unsigned char crc;	
// unsigned char ramBuffer[31]; //Make a table in RAM with 32 bytes (0-31)
//
// crc = CRC8cacl(ramBuffer, 32); // Make the calculation of this 32 RAM bytes and put the result of the calculation in register "crc"
//
//
//

#include "crc8.h"

//----------------------------------------------------------
unsigned char CRC8calc(unsigned char *input, unsigned int len)
{
    unsigned char i, check;

    check=0;
    for (i=0; i<len; i++)
    {
        check = CRC8(*input, check);
        input++;
    }
   
    return check;
} 


//----------------------------------------------------------
unsigned char CRC8(unsigned char data, unsigned char seed)
{
    unsigned char i, feedback;

    for (i=0; i<8; i++)
    {
        feedback = ((seed ^ data) & 0x01);
        if (!feedback) seed >>= 1;
        else
        {
            seed ^= 0x18;
            seed >>= 1;
            seed |= 0x80;
        }
        data >>= 1;
    }

    return seed;   
}
 


