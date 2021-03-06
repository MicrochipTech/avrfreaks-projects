
/****************************************************************************
  ����������� �������� ��� ��������� ������������� ���� ���������
****************************************************************************/
#define nop()				asm("nop");			// �� ��������
#define enable()			asm("sei");			// ���������� ���������� ����������
#define disable()			asm("cli");			// ���������� ���������� ����������
#define release()			TCCR0A=0; PORTE &= ~0x04; asm("jmp 0");	// ������� �������,������� �� ������ ������


#define	GREEN_ON()			PORTE |= 0x08		// ��������� �������� ����������
#define	GREEN_OFF()			PORTE &= ~0x08		// ���������� �������� ����������
#define	GREEN_TOG()			PORTE ^= 0x08		// ������������ �������� ����������

#define	RED_ON()			PORTE |= 0x04		// ��������� �������� ����������
#define	RED_OFF()			PORTE &= ~0x04		// ���������� �������� ����������
#define	RED_TOG()			PORTE ^= 0x04		// ������������ ��������� ����������

#define	EWDT_reset()		PORTD ^= 0x80		// ����� �������� ����������� �������

#define sendUSBdata(x)		*((volatile unsigned char*)0xfff4) = (x)	//������� ��� ������ ������ � ����� USB
#define getUSBdata()		*((volatile unsigned char*)0xfff4)			//������� ��� ������ ������ �� ����� USB (��������������� ���������)
#define getUSBstat()		*((volatile unsigned char*)0xfff5)			//������� ��� ������ ����� ��������� FTDI USB ��������
#define getEXTdata()		*((volatile unsigned char*)0xfff6)			//������� ��� ������ ��������� �������������� � ��������� ������� �������� (d2=s2 d1=s1 d0=s0)

