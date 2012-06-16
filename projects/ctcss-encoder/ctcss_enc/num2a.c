#include "num2a.h"


void ftoa(char * buf, double f, char pad) 
{ 
	char * p; 

	f = (f*100.0 + 0.5)/100.0;        // round 
	p = ltoa(buf,f,pad);
	*p++ = '.';

	f -= (unsigned int) f;
	f = f * 100.0;
	ltoa(p,f,2);
}

char * ltoa(char * buf, unsigned long val, char pad)
{
	unsigned char i;
	char tmp_buf[10];

	for(i=0; val>0; val/=10)
		tmp_buf[i++] = (val % 10) + '0';
	if(i==0)
		tmp_buf[i++] = '0';

	while(i<pad)
		tmp_buf[i++] = '0';

	while(i>0)
		*buf++ = tmp_buf[--i];

	*buf = 0;

	return buf;
}
