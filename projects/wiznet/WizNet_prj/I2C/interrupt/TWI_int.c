/*****************************************************************************
*
	���������� TWI ����������. ���������� �������������� ��� �������� ������� �� ����.
	TWSR - ������� ��������� ���������� ����������, ������������ ��� ������� ������� ����������.
	TWCR - ������ ��������� ������ ������ ����������, ������������ ��� ������� ��������� ������� ����������� ����������.
	������� ������� ������ ������� ���������: 
	1. ��������� ������� ��������� ���������� ���������� �� ���� TWI � TWI_stat.TWI_state.
	   ���� �������� 0xFF, �� ��������� �������� ����� � ������ ������ � ��� ������.
	2. ���� ��������� ��������, ��  TWI_stat.TWI_state=oxff - ������ ���������, TWI_stat.TWI_msgSize = ���������� ���� ��� ��������,
	   TWI_buf[TWI_BUFFER_SIZE] - ��������� ������� ��� ��������.
	3. ��������� ���������� �� TWI �����������, �������� start �������.
	
	���������� ���������� �������� ��� ������ ���������. ��������� ������� ��� ���������� ������� �� ���� TWSR
	� ������� ������� � ������ TWI_buf.
	
	����� ��������� ����������, ���������� TWI ��������� TWI_stat.TWI_state, ��������, ��� �������� ���������.
	��� �������� ������ ���������� ���������� �� ������ �� ��������� �����, ������ ��� ������ Ethernet
	3� ������ ����� ����� �������� ���������: ����� ������� �� ����� � ����� �������� ������, �������� ����� ������ ����������,
	������� ����� ������ ����������, ����� ���� ������, �������������� ����� ������. ����������� ����� ��� ��������
	- 3� ����� ��� ��������� ������.
	��� �������� ������ ������ ���� ������ ����� ��������� ��� ������ ������� � ����� ��������, ��������� ����� ������,
	�������������� �����, ����� ��������� ����������� ������ �� ��������� ����������.
	��� ��������� ����� ������� ����� ���������� ���������� � �������� ����������� WizNet � ����� ������ � I2C �����������.
*
****************************************************************************/

#include <avr/io.h>
#include <compat/twi.h>
#include <avr/interrupt.h>            
#include "headers/defConstant.h"
#include "headers/defStruct.h"
#include "headers/MACRO.H"

#define TWI_TR			0x85	//�������� ���������� ����� � ���������.
#define TWI_STOP		0x94	//��������� ���� ���������, ������ ���������� �� TWI.
#define TWI_RC_ACK		0xC5	//����� ���������� ����� � ����������, ��������� ACK.
#define TWI_RC_NACK		0x85	//����� ���������� ����� � ����������. �� ��������� ACK.
#define TWI_ARB			0xA5	//��������� ����� ���������.
/*
	TWI_BUSY		0xFF	// TWI Bus busy
	TWI_FREE		0x00	// TWI Bus free
	TWI_WRITE		0x01	// TWI Bus write compleat and free
	TWI_READ		0x02	// TWI Bus read compleat and free
	TWI_ERROR		0x03	// TWI Bus error and free
*/
extern struct TWI_status	 TWI_stat;
extern unsigned char time_out[NT];	// ������ ���������� ��� ����������� ��������
/*
struct TWI_status		// ��������� �������� ��������� TWI ����������.
{
	volatile unsigned char		TWI_state;					// ��� ���������. Default set to TWI_NO_STATE.
	unsigned char				TWI_msgSize;				// Number of bytes to be transmitted. ���������� ���� � ������ ��� �������� ��� ������.
	unsigned char				TWI_buf[TWI_BUFFER_SIZE];	// Transceiver buffer. ����� ������ ��� �������� ��� ������.
}
*/

SIGNAL(SIG_2WIRE_SERIAL)
{
	static unsigned char indexTWIbuf;			//������� ������ � ������ ������ TWI ����������

	time_out[3] = BLOCK_TWI_TIME;					// ��������� ������� �������� ������ ���������� � �������� ���������
	switch (TWSR)
	{
		case TW_START:								// START has been transmitted 
		case TW_REP_START:							// Repeated START has been transmitted
		indexTWIbuf = 0;							// Set buffer pointer to the TWI Address location
		case TW_MT_SLA_ACK:						// SLA+W has been tramsmitted and ACK received
		case TW_MT_DATA_ACK:						// Data byte has been tramsmitted and ACK received
		if (indexTWIbuf < TWI_stat.TWI_msgSize)
		{
		/*
		TWI Interface enabled (1<<TWEN)|(1<<TWIE)|(1<<TWINT)
		Enable TWI Interupt and clear the flag to send byte
		*/
			TWDR = TWI_stat.TWI_buf[indexTWIbuf++];//����� ��������� ���� �� ������ TWI.
			TWCR = TWI_TR;							//�������� ���������� ����� � ���������.
		}
		else										// Send STOP after last byte
		{
		/*
		Set status bits to completed successfully.
		TWI Interface enabled (1<<TWEN)|(1<<TWINT)|(1<<TWSTO)
		Disable TWI Interrupt and clear the flag
		Initiate a STOP condition.
		*/
			TWCR = TWI_STOP;						//��������� ���� ���������, ������ ���������� �� TWI.
			TWI_stat.TWI_state = TWI_WRITE;			//������ ��������
		}
		break;
		case TW_MR_DATA_ACK:						// Data byte has been received and ACK tramsmitted
		TWI_stat.TWI_buf[indexTWIbuf++] = TWDR;
		case TW_MR_SLA_ACK:						// SLA+R has been tramsmitted and ACK received. ����� ���������� ����� � ����������, ��������� ACK.
		if (indexTWIbuf < (TWI_stat.TWI_msgSize-1))// Detect the last byte to NACK it. �������� ������� �������� ������
		/*
		TWI Interface enabled (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)
		Enable TWI Interupt and clear the flag to read next byte
		Send ACK after reception
		*/
		TWCR = TWI_RC_ACK;							//����� ���������� ����� � ����������, ��������� ACK.
		else										// Send NACK after next reception
		/*
		TWI Interface enabled (1<<TWEN)|(1<<TWIE)|(1<<TWINT)
		Enable TWI Interupt and clear the flag to read next byte
		Send NACK after reception
		*/
		TWCR = TWI_RC_NACK;							//����� ���������� ����� � ����������.
		break; 
		case TW_MR_DATA_NACK:						// Data byte has been received and NACK tramsmitted
		TWI_stat.TWI_buf[indexTWIbuf] = TWDR;
		/*
		Set status bits to completed successfully. 
		TWI Interface enabled
		Disable TWI Interrupt and clear the flag
		Initiate a STOP condition.
		*/
		TWCR = TWI_STOP;							//��������� ���� ���������, ������ ���������� �� TWI.
		TWI_stat.TWI_state = TWI_READ;				//������ ���������	
		break;      
		case TW_MT_ARB_LOST:						// Arbitration lost
		/*
		TWI Interface enabled (1<<TWEN)|(1<<TWSTA)|(1<<TWIE)|(1<<TWINT)
		Enable TWI Interupt and clear the flag
		Initiate a (RE)START condition.
		*/
		TWCR = TWI_ARB;								//��������� ����� ���������.
		break;
		case TW_MT_SLA_NACK:						// SLA+W has been tramsmitted and NACK received
		case TW_MR_SLA_NACK:    					// SLA+R has been tramsmitted and NACK received    
		case TW_MT_DATA_NACK:						// Data byte has been tramsmitted and NACK received
		case TW_BUS_ERROR:							// Bus error due to an illegal START or STOP condition
		default:
		/*
		Reset TWI Interface
		Enable TWI-interface and release TWI pins
		Store TWSR and automatically sets clears noErrors bit.
		Disable Interupt
		No Signal requests
		*/     
		TWCR = TWI_STOP;							//��������� ���� ���������, ������ ���������� �� TWI.
		TWI_stat.TWI_state = TWI_ERROR;				//������ ����
	}
}

