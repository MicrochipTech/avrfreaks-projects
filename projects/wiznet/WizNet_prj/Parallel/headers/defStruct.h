/****************************************************************************
  �������� �������� �������� ���������
****************************************************************************/

struct ETH_status
{	
	unsigned char	phase;			// ��������� ������ � ������� ethernet
	unsigned char	initRequest;	// ������ ������������� �������� ������ ethernet 1-FLASH, 2-USB, 3-USER IP destination
};

struct Request
{	
	unsigned char		req;		// ������� ������� ��������/������ ������ � ������ ethernet
	unsigned char		leng;		// ����� ������������ � �������� ������
};

//��������������� ��������� ��� �������� ������ ������ WIZnet
struct TD
{
	unsigned char* pIData;	//�������� ��������� ������ ������ ������ WIZnet
	unsigned char* pEData;	//�������� ��������� ������ ����������
	unsigned char lengs;		//����� ������������ ������
};
