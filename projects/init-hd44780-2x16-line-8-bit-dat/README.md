# init hd44780 2x16 line 8 bit dat

Uploaded by BBh on 2012-08-06 14:11:00 (rating 5 out of 5)

## Summary

Description : SETS LCD POWERTRIP TO 8 BIT DATA, 2 LINE, 5X8 FRONT CHAR, cursor on,blinking of cursor character position  

Input : BUSYGFLAG DB7 PORT "B"  

Output : CTRL OUTPUT = PORT "D". ENABLE=PD0 RW=PD1 RS=PD2  

 : DATA OUTPUT = PORT "B". DB0-DB7 EQU LCD DATA. 


Note  

LCD\_BUSYFLAG LCD\_INIT\_8BIT 


LCD sub calls:  

LCD\_ENABLE LCD\_WRITE\_COMMAND LCD\_FUNKTIONSSET LOAD BYTE LCD\_CLR LCD\_READ\_COMMAND LCD\_SET\_line2\_COMMAND LCD\_OUT  

CUROSER\_HOME LCD\_DELAY LCD\_SET\_line1\_COMMAND(message)( onley to show that lcd)


 hardware connect:  

LCD. MCU  

1 -  

2 + 5,5 dc  

3 contrast (10k pot)


4 rs Pd2  

5 rw Pd1 port D  

6 en Pd0 


7 Db0 Pb0  

8 Db1 Pb1  

9 Db2 Pb2  

10 Db3 Pb3 port B  

11 Db4 Pb4  

12 Db5 Pb5  

13 Db6 Pb6

## Compilers

- AVR Studio 4

## Tags

- General Lib.functions
- AVR Studio 4
