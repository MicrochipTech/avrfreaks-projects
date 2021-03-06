
/****************************************************************************
  ����������� �������� ��� ��������� ������������� ���� ���������
****************************************************************************/
#define nop()				asm("nop"::)		// ��� ��������

#define INT_enable()		asm("sei"::)		// ���������� ���������� ����������
#define INT_disable()		asm("cli"::)		// ���������� ���������� ����������

#define	GREEN_ON()			PORTA |= 0x02		// ��������� �������� ����������
#define	GREEN_OFF()			PORTA &= ~0x02		// ���������� �������� ����������
#define	GREEN_TOG()			PORTA ^= 0x02		// ������������ �������� ����������

#define	RED_ON()			PORTA |= 0x04		// ��������� �������� ����������
#define	RED_OFF()			PORTA &= ~0x04		// ���������� �������� ����������
#define	RED_TOG()			PORTA ^= 0x04		// ������������ ��������� ����������

#define	resetETH()			PORTC = 0x02		//����� ������ Ethernet
#define	releaseETH()		PORTC = 0x01		//������������ �� ������ ������ Ethernet

#define	setTimer0(x)		TCNT0 = (x)			// ������ ���� � ������� ������ ��� ����������� ��������
#define TWI_Start()			TWCR=0xA5			//(1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWSTA) TWI Interface enabled.
