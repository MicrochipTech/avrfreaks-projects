# simple CAN TRANSMITTER & CAN RECEIVER with ATTINY804 , MCP2515 in ASSEMBLY

Uploaded by darth vader on 2022-01-20 21:47:00 (rating 0 out of 5)

## Summary

Chip ATTINY 804


ASSEMBLER


AS7 STUDIO PROJECT


CAN BUS INTERFACE MCP2515 board from ALIEXPRESS (8MHZ CRYSTAL OSC)


Simple CAN message sending node and a CAN message receiving node.  

transmitter sends a 8 byte message with STANDARD ID 0x003 with first byte 0x01.  

After adelay of 1000ms another message with STANDARD ID 0x100 with first byte 0x00  

is transmitted in a loop. The CAN receiver node receives the message as the filters   

and masks in it are enabled only to receive 0x100,0x101,0x102,0x103,0x001,0x002,0x003  

ID's only. Once a matching ID is loaded in the RXB0 the 1st byte of the message is  

compared to 0x00, if true the LED on PB3 is switched off. If the first byte of the   

message is 0x01 the LED on PB3 is switched on. 2 receiver codes available . one in polling   

and another one interrupt driven receiver. The selected bus speed is 125kpbs. the interrupt  

driven code also has a UART routine that will transmit the message id and data to a terminal  

(terra term) using a FTDI dongle. The UART is output on PB2 and is conncted to the rx pin of the   

FTDI dongle.The 120OHMS terminating resistor was enabled by shorting the shorting bars on the   

MCP2515 borad. ATTINY804 has SPI as following , MOSI - PA1  , MISO - PA2  , SCK - PA3 , SS - PA4  

UART - PB2 , RECEIVE INTERRUPT - PA5 , LED - PB3.  

MOSI connceted to SI MCP2515 board  

MISO connected to SO MCP2515 board  

SS connected to CS MCP2515 board  

SCK connected to CLK MCP2515 board  

PA5 conncetd to INT MCP2515 board if code with interrupt is used


 


 


BIT timing


MCP OSC frequency = 8MHZ  

base bit clock runs at 1/2 speed of Fosc = 8/2 = 4Mhz  

we plan to use 125kbps = 125000  

time quanta TQ = bit clock/ bus speed = 4000000/125000 = 32  

As 32 TQ is big we will use prescaler to reduce TQ to some where near 16  

BRP (prescaler) of CNF1 if written 0 is counted as 1  

we prescale by 2 = 4Mhz/2 = 2Mhz (2000000)      (BRP bits written 1 equals prescale 2)  

now TQ = 2000000/125000 = 16


nominal bit time = SyncSeg + PropSeg + PS1 + PS2


sync segment is fixed at 1TQ  

propagation segment is anywhere between 1=8 TQ  

PS1 is between 1-8 TQ  

PS2 is between 2-8 TQ  (minimum 2TQ is must for info process)  

SJW is 1-4 TQ  

we split the 16TQ nicely between the above values taking care about the minimum rules  

1. PropSeg + PS1 ≥ PS2  

2. PropSeg + PS1 ≥ tPROP  

3. PS2 > SJW


I split the 16 TQ into  

SYYNC segment 1TQ  

PROP segment 2 TQ  

PS1 segment 7 TQ  

PS2 segment 6 TQ


so 1+2+7+6 = 16TQ  

SJW,BRP,PS1,PS2 etc are loaded with 1 count less as 0 value is counted as 1 in CNF registers  

details in data sheet.


filter and mask values in the code can be changed to suit message id to be received  

if masks are written 0x00 all messages will be received.

## Compilers

- Atmel Studio 7

## Tags

- Complete code
- Atmel Studio 7
- CAN
- assembly
- ATTINY804
- MCP2515
- Car Networking
