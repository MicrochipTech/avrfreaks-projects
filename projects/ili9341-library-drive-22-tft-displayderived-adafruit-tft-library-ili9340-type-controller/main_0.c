#include<avr/io.h>
#include<util/delay.h>
#include<stdio.h>
#include<stdbool.h>
#include<inttypes.h>
#include "ili9341.h"
#include "ili9341gfx.h"
#define POINTCOLOUR PINK


extern uint16_t vsetx,vsety,vactualx,vactualy,isetx,isety,iactualx,iactualy;
static FILE mydata = FDEV_SETUP_STREAM(ili9341_putchar_printf, NULL, _FDEV_SETUP_WRITE);


int main(void)
{
stdout = & mydata;
ili9341_init();//initial driver setup to drive ili9341
ili9341_clear(BLACK);//fill screen with black colour
_delay_ms(1000);
ili9341_setRotation(3);//rotate screen
_delay_ms(2);

display_init();//display initial data


while(1)
{
ili9341_settextcolour(CYAN,BLACK);

ili9341_setcursor(vsetx,vsety);
_delay_ms(2);
ili9341_settextsize(3);
ili9341_write('1');
_delay_ms(2);
ili9341_write('0');
_delay_ms(2);
ili9341_write('.');
_delay_ms(2);
ili9341_write('2');
_delay_ms(2);
ili9341_write('3');
_delay_ms(2);



ili9341_setcursor(vactualx,vactualy);
_delay_ms(2);
ili9341_settextsize(5);
ili9341_write('1');
_delay_ms(2);
ili9341_write('0');
_delay_ms(2);
ili9341_write('.');
_delay_ms(2);
ili9341_write('2');
_delay_ms(2);
ili9341_write('3');
_delay_ms(2);

_delay_ms(2000);

ili9341_setcursor(vsetx,vsety);
_delay_ms(2);
ili9341_settextsize(3);
ili9341_write('9');
_delay_ms(2);
ili9341_write('0');
_delay_ms(2);
ili9341_write('.');
_delay_ms(2);
ili9341_write('4');
_delay_ms(2);
ili9341_write('5');
_delay_ms(2);



ili9341_setcursor(vactualx,vactualy);
_delay_ms(2);
ili9341_settextsize(5);
ili9341_write('9');
_delay_ms(2);
ili9341_write('0');
_delay_ms(2);
ili9341_write('.');
_delay_ms(2);
ili9341_write('4');
_delay_ms(2);
ili9341_write('5');
_delay_ms(2);

_delay_ms(2000);


}

}