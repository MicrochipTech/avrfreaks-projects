/************************************************************/
/**********File Related to CRC Check*************************/
/************************************************************/

// CRC Polynomial being used by SHT11
#define CRC8POLY	0x18  //0X18 = X^8+X^5+X^4+X^0


// Swap the bits as Sensron has  a  CRC implemented in different way
// Refer Sensirion_Humidity_SHT1x_SHT7x_CRC_Calculation_V1.pdf
unsigned char  swapbits(unsigned char data_in)
{
        unsigned char x,  i, tmp=0;
        for (i = 1; i<=8; i++)
		{ 	
			tmp= tmp << 1;
            
			x =    1 & data_in ;
			if (x == 1 ) 
			{
				tmp=tmp | 1;
            }
		
		data_in = data_in >> 1;
        }
	
	return(tmp);
}

// Calculate CRC of the recived data
// This code is from Colin O'Flynn - Copyright (c) 2002 only minor changes by M.Thomas 9/2004
// Changed as per program requirement 
unsigned char crc8(unsigned char crc, unsigned char data_in)
{
    unsigned char  bit_counter = 8, feedback_bit;
		do 
		{
       		feedback_bit = ((crc ^ data_in) & 0x01);
        	if (feedback_bit == 1)   // IF BIT IS 1
	        {
	            crc  = crc ^  CRC8POLY;
	            crc = crc  >> 1;
	            crc = crc | 0x80;
	        }

        	if (feedback_bit == 0)	// if bit is 0
	        {
	            crc  >>= 1;
	        }
        	data_in = data_in  >> 1;
			bit_counter--; 
		
		} while (bit_counter > 0);
    
	return (crc) ;    
}
