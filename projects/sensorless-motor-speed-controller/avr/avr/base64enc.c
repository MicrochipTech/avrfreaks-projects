/* copyright
   * (c) 2004 Stephen Humble stephenhumble at telstra.com
   *
   *  This file is part of robotx see http://Robotx.sourceforge.net
   *  dual sensorless motor driver firmware.
   *
   *  This is free software; you can redistribute it and/or modify
   *  it under the terms of the GNU General Public License as published by
   *  the Free Software Foundation; either version 2 of the License, or
   *  (at your option) any later version.
*/
/* when transfering binary data across the RS485 or RS232 link
   * some codes are used for end of file end of line or flow control so raw binary 
   * must be encoded somehow these routines do
   * base16 (hexidecimal) or base64 (data efficient) which is the purpose of these 
   routines */

#include <avr/io.h>
#include ATMEGA
#include <inttypes.h>
#include "common.h"
#include "base64enc.h"
/*
   mime base 64 conversion
   0 A   16 Q   32  g  48  w
   1 B   17 R   33 h   49 x
   2 C   18 S   34 i   50 y
   3 D   19 T   35 j   51 z
   4 E   20 U   36 k   52 0
   5 F   21 V   37 l   53 1
   6 G   22 W   38 m   54 2
   7 H   23 X   39 n   55 3
   8 I   24 Y   40 o   56 4
   9 J   25 Z   41 p   57 5
   10 K   26 a   42 q   58 6
   11 L   27 b   43 r   59 7
   12 M   28 c   44 s   60 8
   13 N   29 d   45 t   61 9
   14 O   30 e   46 u   62 +
   15 P   31 f   47 v   63 /
*/
//convert a 6 bit value to a base64 ascii encoded character
#ifdef BASE64_CODE

void hexputc(uint8_t c)
{
   putc(itohex(c>>4));
   putc(itohex(c&15));
}

void hexputw(int16_t c)
{
   hexputc(c>>8);
   hexputc(c);
}

void hexputl(int32_t c)
{
   hexputw(c>>16);
   hexputw(c);
}

void mimeputc(uint8_t c)
{
   putc(itohex(c>>4));
   putc(itohex(c&15));
}

void mimeputw(int16_t c)
{
   char q[3];
   rawtomime((char *)&c,&q,2);
   char y;
   for(y=0;y<3;y++)putc(q[y]);
}

void mimeputl(int32_t c)
{
   char q[6];
   rawtomime((char *)&c,&q,4);
   char y;
   for(y=0;y<6;y++)putc(q[y]);
}

char itomime(unsigned char q)
{
   if( q<26)return q+'A';
   if( q<52)return q-26+'a';
   if( q<62)return q-52+'0';
   if( q==62)return '+';
   if(q==63)return '/';
   return '?';
}
//convert a base64 ascii encoded character to a 6 bit value
unsigned char mimetoi(char q)
{
   if( q>='a')return q-'a'+26;
   if( q>='A')return q-'A';
   if( q>='0')return q-'0'+52;
   if( q=='+')return 62;
   if( q=='/')return 63;
   return 0;
}

//convert a 4 bit value to a base16 ascii encoded character
char itohex(unsigned char q)
{
   if( q<10)return q+'0';
   if( q<16)return q-10+'A';
   return '?';
}

//convert a base16 ascii encoded character to a 6 bit value
unsigned char hextoi(char q)
{
   if( q>='A')return q-'A'+10;
   if( q>='0')return q-'0';
   return 0;
}

uint8_t rawtohex(uint8_t *z,char *q,char x)
{
   char y;
   for(y=0;y<x;y++){
		      *q=itohex((*z)>>4);
		      q++;
		      *q=itohex((*z)&15);
		      q++;
		      z++;
		   }
   return 1;
}

hextoraw(char **z,char *q,char x)
{
   char y=0;
   for(y=0;y<x;y++)
   {
      unsigned char a,b,c,d;
      q=hextoi(**z);
      (*z)++;
      q+=hextoi(**z)<<4;
      q++;
      (*z)++;
   }
}

/* z is binary data in
   *  q is resultant base64 encoded data 
   *  x is length of data must be divisible by 3
   *  z should be a multiple of 3 bytes long 
   * return value is length of encoded data 
*/
/*  this works
   uint8_t rawtomime(uint8_t *z,char **q,char x)
   {
   char y=0;
   char *zz=z;
   char *qq=*q;
   for(y=0;y<x;y+=3){
   *qq=itomime((zz[0]>>2));
   qq++;
   *qq=itomime( zz[0]&3<<4 | (zz[1]>>4) );
   qq++;
   *qq=itomime(zz[1]&15<<2|(zz[2]>>6));
   qq++;
   *qq=itomime(zz[2]&0x3f);
   qq++;
   zz+=3;
   }
   return 1;
   }
*/
uint8_t rawtomime(uint8_t *z,char **q,char x)
{
   char y=0;
   uint8_t *zz=z;
   for(y=0;y<x;y+=3){
		       *(*q)=itomime( ((zz[0])>>2) );
		       (*q)++;
		       *(*q)=itomime(((zz[0]&3)<<4) | (zz[1]>>4) );
		       (*q)++;
		       *(*q)=itomime(((zz[1]&15)<<2)| ((zz[2]>>6)));
		       (*q)++;
		       *(*q)=itomime(zz[2]&0x3f);
		       (*q)++;
		       zz+=3;
		    }
   return 1;
}


/* z is base64 data to decode
   *  q is decoded data 
   *  x is length of data
*/
mimetoraw(char *z,uint8_t *q,char x)
{
   char y=0;
   uint8_t a,b,c,d;
   do{
	a=mimetoi(*z++);
	b=mimetoi(*z++);
	c=mimetoi(*z++);
	d=mimetoi(*z++);
	*q=(a<<2) | (b>>4);
	q++;
	*q=((b&15)<<4) | (c>>2);
	q++;
	*q=((c&3)<<6) | (d);
	q++;
	y+=4;
     }while(y<x);
}

/*
   * tests the hex and base64 routines by doing a series of conversions
   * from one type to another and then back again
*/
void enc_test(void )
{
   char y;
   uint32_t i[3];
   i[0]=0xff000000;
   i[1]=0x0102ffff;
   i[2]=0x03040506;
   char bf[17];

   char *bp;
   bp=&bf[0];
   rawtomime(&i[0],&bp,12);
   for(y=0;y<16;y++) putc(bf[y]);
   uint8_t *bi=&i[0];
   mimetoraw(&bf[0],&i[0],16);
   bi=&i[0];
   for(y=0;y<12;y++){
		       puts(",");
		       put_i16((uint8_t)bi[y]);
		    }
   puts(" = 0x");
   rawtohex(&i[0],buf,12);
   for(y=0;y<24;y++) putc(buf[y]);

   hextoraw(buf,&i[0],24);
   bp=&bf[0];
   rawtomime(&i[0],&bp,12);
   for(y=0;y<16;y++) putc(bf[y]);

}
#endif

