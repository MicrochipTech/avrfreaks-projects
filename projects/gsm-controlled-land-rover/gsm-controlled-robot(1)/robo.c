Source program:
Robit.c
#include <mega16.h>
void main(void)
{
unsigned int k, h;
DDRA=0x00;
DDRD=0XFF;
while (1)
{
k =~PINA;
h=k & 0x0F;
switch (h)
{
case 0x02: //if I/P is 0x02
{
PORTD=0x89;//O/P 0x89 ie Forward
break;
}
case 0x08: //if I/P is 0x08
{
PORTD=0x86; //O/P 0x86 ie Backward
break;
}
case 0x04:
{
PORTD=0x85; // Left turn
break;
}
case 0x06:
{
PORTD=0x8A; // Right turn
break;
}
case 0x05:
{
PORTD=0x00; // Stop
break;
}
}
}
}