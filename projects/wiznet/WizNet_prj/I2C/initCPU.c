/**********************************************************************************
  �������� ������������ ��������� � ��������� ��� ���������� ������ ������ ethernet
**********************************************************************************/

#include <avr/io.h>
#include <avr/wdt.h>
#include <compat/twi.h>
#include "headers/MACRO.H"
#include "headers/defConstant.h"
#include "headers/defStruct.h"
/****************************************************************/
#define TWI_TWBR	10			// TWI Bit rate Register setting. See Application note for detailed  information on setting this value.
/****************************************************************/
extern struct	TWI_status		TWI_stat;		// ��������� ���������� TWI
extern struct Descript		ETH_dsc;		// ������� ��������� ������� ������������ ������ Ethernet
extern unsigned char time_out[NT];			// ������ ���������� ��� ����������� ��������

void initCPU(void)
{
	DDRA = 0x07;
	DDRC = 0x03; 	
	DDRD = 0x01;
	
	PORTD = 0xFF;
	
	TCCR0A = 0x05;				// ��������� ����������� 1024 ��� ������� 0.
	TIMSK0 = 1;					// ���������� ���������� �� ������������
	TCNT0 = 0;
	
	
	TWAR &= ~(1<<TWGCE);					// ��������� ������� ������� �������� ������ ��� ������ ������ ������
	TWSR &= ~((1<<TWPS0) | (1<<TWPS1));	// ��������� ������������ �������� I2C
	TWBR=TWI_TWBR;							// ��������� ������� �������� 6MHz/(16+2(TWBR)*4^TWPS)=166.666kHz, ��� TWPS=0,TWBR=10
	TWCR |= (1<<TWEN);						// ��������� �������������� ������� I2C, ���������� ���� ���������
	TWI_stat.TWI_state=TWI_FREE;
	ETH_dsc.phase=0;						// �������-��������� �� ������������� ������ Ethernet
	
	time_out[3] = BLOCK_TWI_TIME;			// ������������� ������� �������� ���������� ����� TWI
	resetETH();
	
	wdt_enable(WDTO_1S);		// ��������� WDT �� ����� 1�
	wdt_reset();				// ����� ����������� ����������� �������
}

