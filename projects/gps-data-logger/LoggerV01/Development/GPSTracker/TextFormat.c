
#include "ProjectCommon.h"
#include "TextFormat.h"
#include "ReceiverIO.h"
#include <string.h>  //memset only

char convBuf[convBufLen];		//enough to hold a long

static void SetOVF(void);

//-----------------------------------------------------------------------------------
// convert the number to ascii. Number is 0 padded from left, if necc.
// number must fit into width (inc any -). If the number cant be expressed
// in the specified width, buffer is set to "Err".
// width excludes null terminator.
// Good for displaying long/lat without the need to convert to double,
// eg if lat is 123456, its value is 0.123456 and LtoZ(123456,9) gives
// 000123456 so adding the . with PutDotInIt(3) gives 000.123456

const char* const LtoaZ(int32_t val, uint8_t width)
{

	char *p =convBuf+width;
	
	if(val < 0)
	{
		convBuf[0]='-';
		val =-val;
		width--;
	}
	*p--='\0';
		
	while (width--)
	{	
		*p--=val%10 +'0';
		val/=10;	
	}
	if (val)		//not enough digits specified
		SetOVF();
	
	return convBuf;
}

//-----------------------------------------------------------------------------------
// my version of lota  - prevents the need to use the stdlib, which isn't
// thread safe, this isn't either, but wont crash, just return crap value.
// start conversion at width as it makes ltoaS work also!

const char* const LtoaN(int32_t val, uint8_t width)
{
	bool sign;
	
	if (val<0)
	{
		val =-val;
		sign=true;
	}
	else 
		sign=false;
	
	
	char *p =convBuf+width;
		
	*p--='\0';
			
	while (width--)
	{	
		*p--=val%10 +'0';
		val/=10;	
		if (val==0) break;
	}
	
	if (val)		//not enough digits specified
	{
		SetOVF();
		return convBuf;
	}	
	
	if (sign)
	{
		if (p==convBuf)
			SetOVF();
		else
			*p='-';
		return p;
	}
	else
		return p+1;
}


//-----------------------------------------------------------------------------------
// convert the number to ascii. Number is space padded from left, if necc.
// number must fit into width (inc any -). If the number cant be expressed
// in the specified width, buffer is set to "Ovf".
// width excludes null terminator

const char* const LtoaS(int32_t val, uint8_t width)
{
	memset(convBuf,' ',width);
	LtoaN( val, width);
	return convBuf;
}


//-----------------------------------------------------------------------------------
//convert lat/long -180.000000 -> +180.000000 to text value
//eg -03.123456
// numbnerto be converted is fixed point (scaled integer). Number of decimal places
// defined by LatLongFractionDigits

const char* const LtoaLL(int32_t val, uint8_t totalChars)
{
	bool sign;
	int i;
	char *p =convBuf+convBufLen-1;				//last char in buffer, buffer always big enough	
	
	memset(convBuf,' ',convBufLen);				//space fill

	*p='\0';
	
	if (val<0)
	{
		val =-val;
		sign=true;
	}
	else 
		sign=false;
		
			
	for(i=0;i<LatLongFractionDigits;i++)		//first few are fractional (fixed point stylee)
	{	
		*--p=val%10 +'0';
		val/=10;	
	}
	*--p='.';									//fixed pt, so always here.
	i=0;
	do
	{
		*--p=val%10 +'0';
		val/=10;
		i++;
	}while (val);
	
	if (i==1)			//laways 2 digits before dot, eg 09.123456
		*--p='0';
		
	if (sign)
		*--p='-';

	if ((convBuf+convBufLen-2)-p+1 < totalChars)
		p=(convBuf+convBufLen-2)-(totalChars-1);
	else
		*(p+totalChars)='\0';
	
	return p;
}
//-----------------------------------------------------------------------------------

static void SetOVF(void)
{
	convBuf[0]='O';
	convBuf[1]='v';
	convBuf[2]='f';
	convBuf[3]='\0';
}

//-----------------------------------------------------------------------------------
// put a dot at the specified position (use to display long/lat)
// Dont use  stdlib as its not thread safe.
// Since this is application specific, there is little range checking etc.
/*
const void PutDotInIt(uint8_t digitsBefore)
{
	int8_t i;
	
	// look for last character
	// inc digits before if its space padded or -ve
	
	for (i=0;i<convBufLen;i++)
	{
		if ((convBuf[i]=='-') || (convBuf[i]==' '))	
			digitsBefore++;
		if (convBuf[i]=='\0')
			break;
	}
	
	i++;
	if (i>=convBufLen)
		i= convBufLen-2;

	while (i>digitsBefore)
	{
		convBuf[i] = convBuf[i-1];
		i--;
	}
	convBuf[i]='.';		
}
*/