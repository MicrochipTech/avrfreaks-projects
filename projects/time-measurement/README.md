# time measurement

Uploaded by amu_bgr on 2011-10-28 14:26:00 (rating 0 out of 5)

## Summary

program timeduration  

'cpu used is atmega16  

'clock is 8 mhz  

dim flag as bit  

dim time as byte  

dim LCD\_RS as sbit at PORTD2\_bit  

dim LCD\_EN as sbit at PORTD3\_bit  

dim LCD\_D4 as sbit at PORTD4\_bit  

dim LCD\_D5 as sbit at PORTD5\_bit  

dim LCD\_D6 as sbit at PORTD6\_bit  

dim LCD\_D7 as sbit at PORTD7\_bit


dim LCD\_RS\_Direction as sbit at DDD2\_bit  

dim LCD\_EN\_Direction as sbit at DDD3\_bit  

dim LCD\_D4\_Direction as sbit at DDD4\_bit  

dim LCD\_D5\_Direction as sbit at DDD5\_bit  

dim LCD\_D6\_Direction as sbit at DDD6\_bit  

dim LCD\_D7\_Direction as sbit at DDD7\_bit  

 sub procedure int0handler() org 0x002 'int0 vector  

 TCCR1B=2 'start timer1 with prescaller 2  

 end sub  

sub procedure int1handler() org 0x004 'int1 vector  

 'flag=0  

 TCCR1B=0 'stop timer1  

 flag=0  

end sub


main:  

' Main program  

flag=1  

MCUCR = 10 'interupt on falling edge(both int0 and int1)  

GICR = 192 'start external int0 and int1  

SREG.B7 = 1 'start global interupts  

 while (flag = 1)'loop untill time measurement is not complete  

 wend  

'the rest of main program  

'HERE i will write code to read TCNT1L AND TCNT1H AND TO DISPLAY AT LCD.  

low(time)=tcnt1l  

high(time)=tcnth


 Lcd\_Init() ' Initialize LCD  

 Lcd\_Cmd(\_LCD\_CLEAR) ' Clear display  

 Lcd\_Cmd(\_LCD\_CURSOR\_OFF)  

 lcd\_out(1,1,time)


end.

## Compilers

- mikroBasic for AVR

## Tags

- Complete code
- mikroBasic for AVR
