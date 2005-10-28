#include "gpr.h"

void print_hexbyte(unsigned char i)
{
    unsigned char h, l;
    
    h = i & 0xF0;               // High nibble
    h = h>>4;
    h = h + '0';
    
    if (h > '9')
        h = h + 7;

    l = (i & 0x0F)+'0';         // Low nibble
    if (l > '9')
        l = l + 7;


    putchar(h);
    putchar(l);
}


void delay(char d)
{
    char i,j,k;
    for(i=0; i<d; i++)
        for(j=0; j<40; j++)
            for(k=0; k<176; k++);
}

