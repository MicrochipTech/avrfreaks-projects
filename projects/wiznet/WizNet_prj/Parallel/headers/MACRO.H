
/****************************************************************************
  ����������� �������� ��� ��������� ������������� ���� ���������
****************************************************************************/
#define nop()				asm("nop"::)		// ��� ��������
#define INT_enable()		asm("sei"::)		// ���������� ���������� ����������
#define INT_disable()		asm("cli"::)		// ���������� ���������� ����������

#define	GREEN_ON()			PORTE |= 0x08		// ��������� �������� ����������
#define	GREEN_OFF()			PORTE &= ~0x08		// ���������� �������� ����������
#define	GREEN_TOG()			PORTE ^= 0x08		// ������������ �������� ����������
#define	RED_ON()			PORTE |= 0x04		// ��������� �������� ����������
#define	RED_OFF()			PORTE &= ~0x04		// ���������� �������� ����������
#define	RED_TOG()			PORTE ^= 0x04		// ������������ ��������� ����������

#define	WDText()			PORTD ^= 0x80		// ����� �������� ����������� �������

#define	getSens()			*((volatile unsigned char*)0xfff6)			//������ d6=reset_ethernet
#define	resEthernet()		*((volatile unsigned char*)0xfff6)=0x40		//��������� ������� ����� �� ������ Ethernet
#define	setEthernet()		*((volatile unsigned char*)0xfff6)=0x00		//������ ������� ����� �� ������ Ethernet
