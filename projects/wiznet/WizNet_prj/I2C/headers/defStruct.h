/****************************************************************************
  �������� �������� �������� ���������
****************************************************************************/

struct Request
{	
	unsigned char		req;		// ������� ������� ��������/������ ������ � ������ ethernet
	unsigned char		leng;		// ����� ������������ � �������� ������
};

struct TWI_status											// Status byte holding flags.
{
	volatile unsigned char	TWI_state;					// State byte. Default set to 0.
	unsigned char				TWI_msgSize;				// Number of bytes to be transmitted.
	unsigned char				TWI_buf[TWI_BUFFER_SIZE];	// Transceiver buffer
};
/*-------------------------------------------------------------------------*/
struct Descript
{	
	unsigned char	phase;			// ������� ���� ������ � ������� ethernet.
	unsigned char	countError;		// ������� ������ ���������� � ������ ethernet ����� TWI
};
