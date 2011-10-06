#include <includes.h>

// call to print memory to a text buffer as hex data
// option=0, ############
// option=1, ##:##:##:##:##:## (used for MAC)
char* hex2ascii(char* textbuf, void* startadd, unsigned char length, unsigned char option)
{
	void* endadd = (unsigned char*)startadd+length;
	char* retval = textbuf;

	unsigned char tempchar;
	do
	{
		tempchar=*(unsigned char*)startadd++;
		textbuf[0]=0x30+(tempchar>>4);
		textbuf[1]=0x30+(tempchar&0x0F);
		if(textbuf[0]>0x39)
			textbuf[0]+=7;
		if(textbuf[1]>0x39)
			textbuf[1]+=7;
		textbuf+=2;
		if(option==1 && startadd != endadd)
			*textbuf++=':';
	}while(startadd != endadd);
	*textbuf++=0;
	return retval;
}

//write a number to a text buffer, base can be 1-41 (using alpha up to Z)
//number will be right justified within string of length, left padded with char pad
//if length is 0, or padding character is null, string length will be the minimum required
//if fixed length specefied is inadequate for value, buffer will contain '#' characters
//note that specefied length does not include terminator, or sign character, so buffer must be at least length+2
//if sign_pad is non0, positive values will begin with this character
char* ascii_long(long value, char *text, unsigned char base, unsigned char fixed_length, char digit_pad, char sign_pad)
{
	char* retval;
	retval=text;
	if(value < 0)
	{
		*text++='-';
		value *=-1ul;
	}
	else
	{
		if(sign_pad)
			*text++=sign_pad;
	};
	ascii_ulong(value, text, base, fixed_length, digit_pad);
	return retval;
}

//write a number to a text buffer, base can be 1-41 (using alpha up to Z)
//number will be right justified within string of length, left padded with char pad
//if length is 0, or padding character is null, string length will be the minimum required
//if fixed length specefied is inadequate for value, buffer will contain '#' characters
//note that specefied length does not include terminator, so buffer must be at least length+1
char* ascii_ulong(unsigned long value, char *text, unsigned char base, unsigned char fixed_length, char pad)
{
	unsigned long digit_weight;
	unsigned char digit;
	int length;
	char finished=FALSE;
	char *retval;
	
	retval=text;
	
	digit_weight=1;
	length=1;
	
	//determine how many digits we need
	while(value/base >= digit_weight)
	{
		length++;
		digit_weight *= base;
	};
	
	//apply padding?
	if(pad && fixed_length)
	{
		if(fixed_length < length)
		{
			memset(text, '#', length);
			text[length]=0;
			finished=TRUE;
		}
		else if(fixed_length > length)
		{
			memset(text, pad, fixed_length-length);	//add padding characters
			text+=fixed_length-length;
		};
	};
	
	//write digits
	do
	{
		digit = value / digit_weight;	//calculate current digit
		if(digit > 9)					//write ascii
			*text = digit-10+'A';
		else
			*text = digit+'0';
		value -= digit * digit_weight;	//subtract from total
		text++;							//move to next digit
		digit_weight /= base;			//reduce power of digit_weight to current digit units
	}while(digit_weight);
	
	*text=0;							//terminate string
	return retval;
}
