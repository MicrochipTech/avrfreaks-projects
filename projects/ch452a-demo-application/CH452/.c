#include <avr/io.h>
#include <avr/interrupt.h>

#include "CH452CMD.H"
#include "BUTTONS.H"
#include "PIN.H"

unsigned char Position2Number(unsigned char);

unsigned char Numbers[8] = {CH452_BCD_TEST,1,2,3,4,5,6,7};

int main(void)
{
unsigned char dat,index,temp;
	CH452_Init();
	CH452_Write(CH452_RESET); 
	CH452_Write(CH452_SYSON2);
	CH452_Write(CH452_BCD);  
	CH452_Write(CH452_TWINKLE | 8); 
	for(dat = 0; dat < 8; dat++)				//Output Numbers on the display.
	{
		CH452_Write((CH452_DIG0 + (dat<<8))| Numbers[dat]);
	}

	index = 8;									//Start position of flashing digit.
	while(1)
	{	
		if(!(CH452_PIN & (1<<CH452_DOUT_PIN)))	// If you don`t want to use an external interrupt 				  
		{					  					// on DOUT pin ,do polling like me.	
			dat = CH452_Read()>>4;
			if(dat == B1thButtonPressed)
			{
				dat = Position2Number(index);	//Get number of position.
				Numbers[dat]++;					//Increment the current value of the flashing digit.
				if(Numbers[dat] == 16) Numbers[dat] = 0; 				
				CH452_Write((CH452_DIG0 + (dat<<8))| Numbers[dat]);
				while((CH452_Read()>>4) != B1thButtonReleased) {}; //Waiting when button will released.
				continue;
			}
			if(dat == B2thButtonPressed)
			{
				dat = Position2Number(index);	
				Numbers[dat]--;
				if(Numbers[dat] == 255) Numbers[dat] = 15; 				
				CH452_Write((CH452_DIG0 + (dat<<8))| Numbers[dat]);
				while((CH452_Read()>>4) != B2thButtonReleased) {};
				continue;
			}
			if(dat == B3thButtonPressed)
			{
				CH452_Write(CH452_LEFTCYC);			//Left shift of the display.
				temp = Numbers[7];					
				for(dat = 7; dat > 0; dat--)		//Left shift of the Numbers.
				{
					Numbers[dat] = Numbers[dat - 1];	
				}
				Numbers[0] = temp;
				while((CH452_Read()>>4) != B3thButtonReleased) {};
				continue;
			}
			if(dat == B4thButtonPressed)
			{
				CH452_Write(CH452_RIGHTCYC);		
				temp = Numbers[0];						
				for(dat = 0; dat < 7; dat++)	
				{
					Numbers[dat] = Numbers[dat + 1];	
				}
				Numbers[7] = temp;
				while((CH452_Read()>>4) != B4thButtonReleased) {};
				continue;
			}
			if(dat == B5thButtonPressed)
			{
				if(index == 128) index = 1;			
				else index = index << 1;			//Left shift flashing digit.
				CH452_Write(CH452_TWINKLE | index);
				while((CH452_Read()>>4) != B5thButtonReleased) {};
				continue;
			}
			if(dat == B6thButtonPressed)
			{
				if(index == 1) index = 128;
				else index = index >> 1;
				CH452_Write(CH452_TWINKLE | index);
				while((CH452_Read()>>4) != B6thButtonReleased) {};
				continue;
			}
			if(dat == BigButtonPressed)
			{
			//	CH452_Write();
				while((CH452_Read()>>4) != BigButtonReleased) {};
				continue;
			}
		}
	} 
return 0;
}
//Get number of position.
unsigned char Position2Number(unsigned char Pos)
{
	unsigned char temp = 0;
	if(!Pos) return 0xFF;
	while((Pos & 1) != 1)
		{
			temp++;
			Pos>>=1;
		}
	return temp;
}
