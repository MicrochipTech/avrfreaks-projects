#include "crc.h"

unsigned int hitungCRC16(unsigned char *data, unsigned char len)
{
	unsigned int sum= 0;
	unsigned char j;
	  
	for(j= 0; j <= len-1; ++j)
	{
		unsigned char i;
			
		sum^= *data++;
			
		for(i= 0; i < 8; ++i)
		{
			if(sum & 1)
				sum= (sum >> 1) ^ POLYNOM;
			else
				sum= (sum >> 1);
		};
	};
	  
	return sum;
}
