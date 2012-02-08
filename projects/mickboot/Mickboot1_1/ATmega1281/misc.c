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

unsigned int read_bige_uint(unsigned int bige)
{
	unsigned int retval;
	retval = ((unsigned char*)&bige)[0];
	retval<<=8;
	retval+= ((unsigned char*)&bige)[1];
	return retval;
}
