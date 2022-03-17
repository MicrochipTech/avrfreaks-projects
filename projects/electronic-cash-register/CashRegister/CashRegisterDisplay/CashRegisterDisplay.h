//*****************************************************
// File Name  : cashRegisterDisplay.c
// Title      : ������������ ������� ������ (Display)
// Target MCU : Atmel ATmega8 MCU at 4MHz XTAL
//*****************************************************

/*----------------------------------------------------------------
                         HEADER FILES 
-----------------------------------------------------------------*/
#include <avr/io.h>
#include "i2c.h"		// include i2c support
/*----------------------------------------------------------------
           CONNECTION BETWEEN 7 segment AND ATMEGA8 
-----------------------------------------------------------------*/
#define DATA_DDR	 DDRD
#define DATA_PORT 	 PORTD
#define Pin_a     		5
#define Pin_b     		4
#define Pin_c     		0
#define Pin_d     		1
#define Pin_e     		3
#define Pin_f     		6
#define Pin_g     		7
#define Pin_Dec         2

#define LOCAL_ADDR	0x05 // � ��������� ����� ��� I2C ������������ (���������� Slave)
#define TARGET_ADDR	0x01 // � ��������� ��� ��������� ������������ (master)


// local data buffer
unsigned char localBuffer[8] = {'0','0','0'}; // ���� buffer ������������ �� 0.00 ...
                               // ���� ���� ����� ����������� �������. ������ 0.00

unsigned char localBufferLength = 3;//����� � ��������� ������� ��� byte ��� buffer
/* ���� �� localBufferLength=3 ���� �������� ��� � buffer �������� 3 bytes, 
���� � ��������� ���� �����
   address:  byte:
     0         1
     1         2
     2         3
*/

signed char   disp_number;
unsigned char bufferPottition;
unsigned char i;
/*----------------------------------------------------------------
                  FUNCTIONS 
-----------------------------------------------------------------*/
//void Init_Ports(void);
void disp_numberlay(int a);
void delay_ms(u32 count);
void i2cSlaveReceiveService(u08 receiveDataLength, u08* receiveData);
u08 i2cSlaveTransmitService(u08 transmitDataLengthMax, u08* transmitData);
void i2cMasterSendDiag(u08 deviceAddr, u08 length, u08* data);
void showByte(u08 byte);
void hideZeroNumbers (void);
