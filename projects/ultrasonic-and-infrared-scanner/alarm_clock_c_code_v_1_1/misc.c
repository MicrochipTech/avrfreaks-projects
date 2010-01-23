


//This takes the 7-segment LED code from the Eagle counting unit and converts it into a
// number from 0 to 9.
unsigned char segTblReturn(unsigned char uLookMeUp)
{
	u08 x;
	
	for(x=0;x<10;x++)
	{
		if(PRG_RDB(&strSegTbl[x])==uLookMeUp)	
		{
			return(x);	//we found the number in the 7 segment LED lookup table
		}
	}
	return(0);	//we did not find the number in the lookup table
}
