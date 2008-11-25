
#define TX_NEWLINE		{transmitByte(0x0d); transmitByte(0x0a);}

unsigned char receiveByte(void);
void transmitByte(unsigned char);
void transmitString_F(const unsigned char*);
void transmitString(unsigned char*);