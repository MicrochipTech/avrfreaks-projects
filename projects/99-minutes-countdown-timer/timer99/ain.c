//schematic description
//led a pin connect to pb0 through 430ohm resister
//led b pin connect to pb1 through 430ohm resister
//led c pin connect to pb2 through 430ohm resister
//led d pin connect to pb3 through 430ohm resister
//led e pin connect to pb4 through 430ohm resister
//led f pin connect to pb5 through 430ohm resister
//led g pin connect to pb6 through 430ohm resister
//led dp pin connect to pb7 through 430ohm resister
//led col0 connect to pd0(transister driver required)
//led col1 connect to pd1(transister driver required)
//select key connect to pd4(10kohm pull resister required)
//set key connect to pd3(transister driver required)
//run&stop key connect to pd2(transister driver required)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
const uint8_t d_led[] PROGMEM ={~0x3f,~0x06,~0x5b,~0x4f,~0x66,~0x6d,~0x7d,~0x07,~0x7f,~0x6f};
#define KEY_MIN_TIME 5
#define _M_MAX 99
#define LED0_ON() do{ PORTD|=_BV(PORTD1);\
PORTD&=~_BV(PORTD0);}\
while(0)
#define LED1_ON() do{ PORTD|=_BV(PORTD0);\
PORTD&=~_BV(PORTD1);}\
while(0)
#define set_mstatus_bit(temp1) do{ M_STATUS|=_BV(temp1);} while(0)
#define clr_mstatus_bit(temp1) do{ M_STATUS&=~_BV(temp1);} while(0)
#define _LED_F_BIT 7 //toggle bit in every half second
#define _LED_LED01 1 //current scan line,0 means led0,1 means led1
#define _LED_F_LED0 2//led0 flash flag,1 means flash,0 means no flash
#define _LED_F_LED1 3//led1 flash flag,1 means flash,0 means no flash
#define _KEY_PRESSED 6 //have key pressed 1 means a key pressed and no processed yet,0 means no key pressed
#define _TIMER_RUN 0 //timer run or stop,1 means run,0 means stop


static uint8_t temp_use,_8MS,_S,_M;
static uint8_t led0,led1,key_store,key_times;
register unsigned char M_STATUS asm("r3");




void hextobcd(uint8_t temp1)
{
led1=temp1/10;
led0=temp1%10;
}
uint8_t bcdtoled(uint8_t temp1)
{
return pgm_read_byte(d_led+temp1);
}


SIGNAL(SIG_OVERFLOW0)
{//interrupt in every 8ms on xtal 4M
TCNT0=0x100-125;

//flash led signal and seconds minus one

_8MS++;
switch(_8MS)
{
case   1:clr_mstatus_bit(_LED_F_BIT);break;
case  63:set_mstatus_bit(_LED_F_BIT);break;
case 125:
_8MS=0;//reset _8MS
if(M_STATUS&_BV(_TIMER_RUN))
{
if(_S==0)
{
_S=59;//reset _S
//if_M>0 then _M--
if(_M)
_M--;//minutes minus one
}
else
_S--;//second minus one
}
break;
}

PORTB=0xff;//LED OFF
hextobcd(_M);
//LED display flash bit include
if(M_STATUS&_BV(_LED_LED01))
{
clr_mstatus_bit(_LED_LED01);
LED1_ON();//LED1 ON
temp_use=bcdtoled(led1);
if((M_STATUS&_BV(_LED_F_LED1))&&(M_STATUS&_BV(_LED_F_BIT)))
temp_use=0xff;
PORTB=temp_use;
}
else
{
set_mstatus_bit(_LED_LED01);
LED0_ON();//LED0 ON
temp_use=bcdtoled(led0);
if((M_STATUS&_BV(_LED_F_LED0))&&(M_STATUS&_BV(_LED_F_BIT)))
temp_use=0xff;
if((M_STATUS&_BV(_TIMER_RUN))&&(M_STATUS&_BV(_LED_F_BIT)))
temp_use&=0x7f;
PORTB=temp_use;
}

//KEY poll
temp_use=PIND&0x1c;
if(!(M_STATUS&_BV(_KEY_PRESSED))){
if(temp_use==0xc||temp_use==0x14||temp_use==0x18)
{
if(temp_use==key_store)
{key_times++;
if(key_times>=KEY_MIN_TIME)
{set_mstatus_bit(_KEY_PRESSED);
key_times=0;
}
}
else
{key_store=temp_use;
key_times=0;}
}
else
{key_times=0;key_store=0xff;}

}
//KEY PROCESS//////////////////////////////
if(M_STATUS&_BV(_KEY_PRESSED))
{
if(M_STATUS&_BV(_TIMER_RUN))
{//RUNNING
switch(key_store)
{
//SELECT KEY PRESSED
case 0xc:
if(_M<99)
_M++;
break;
//SET KEY PRESSED
case 0x14:
if(_M>=1)
_M--;
else
_S=0;
break;
//RUN PAUSE KEY PRESSED
case 0x18:
M_STATUS=0x8;PORTD|=_BV(PORTD6);M_STATUS&=~_BV(_TIMER_RUN);
break;
}

}

else
{//SET TIMER COUNTER
switch(key_store)
{
//SELECT KEY PRESSED
case 0xc:
if(M_STATUS&_BV(_LED_F_LED0))
{set_mstatus_bit(_LED_F_LED1);clr_mstatus_bit(_LED_F_LED0);}
else
{set_mstatus_bit(_LED_F_LED0);clr_mstatus_bit(_LED_F_LED1);}
break;
//SET KEY PRESSED
case 0x14:
if(M_STATUS&_BV(_LED_F_LED1))
{
if((_M/10)==9)
_M-=90;
else
_M+=10;
}
else
{
if((_M%10)==9)
_M-=9;
else
_M++;
}
break;
//RUN STOP KEY PRESSED
case 0x18:
if(_M)
//goto START;
{M_STATUS|=_BV(_TIMER_RUN);PORTD&=~_BV(PORTD6);M_STATUS&=~_BV(_LED_F_LED1);M_STATUS&=~_BV(_LED_F_LED0);_8MS=0;eeprom_write_byte(0,_M);}
break;
}

}
clr_mstatus_bit(_KEY_PRESSED);
key_store=0xff;


}
if(_M==0&&_S==0)
{M_STATUS=0x8; PORTD|=_BV(PORTD6);_8MS=0;_M=eeprom_read_byte(0);_S=0;}
}
int main(void)
{
DDRD=_BV(PORTD0)|_BV(PORTD1)|_BV(PORTD6);//set portd0 and portd1 outport for led row scanline set portd6 as relay control signal
PORTD=_BV(PORTD6)|_BV(PORTD2)|_BV(PORTD3)|_BV(PORTD4)|_BV(PORTD0);//set portd2 and portd3 to high for key input
DDRB=0xff;
PORTB=0xff;
TCNT0=0x100-125;//set timer0 count
TIMSK=_BV(TOIE0);//enable t0 interrupt
TCCR0=_BV(CS02);//set t0 prescale to ck/64

//read eeprom address 0 then judge it is right
_M=eeprom_read_byte(0);
if(!_M||(_M>=_M_MAX))
_M=8;


//set _M
//START0:
M_STATUS=0x8;
key_times=0;key_store=0xff;
sei();
while(1);
}
