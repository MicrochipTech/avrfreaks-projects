//*****************************************************
//********* HEADER FILE FOR I2C ROUTINES **************
//*****************************************************

#define	 START				0x08
#define  REPEAT_START		0x10
#define  MT_SLA_ACK			0x18
#define  MT_SLA_NACK		0x20
#define  MT_DATA_ACK		0x28
#define  MT_DATA_NACK		0x30
#define  MR_SLA_ACK			0x40
#define  MR_SLA_NACK		0x48
#define  MR_DATA_ACK		0x50
#define  MR_DATA_NACK		0x58
#define  ARB_LOST			0x38

#define  ENABLE_I2C			TWCR= 0x44
#define	 DISABLE_I2C		TWCR= 0x00

#define  ERROR_CODE			0x7e
	
#define  DS1307_W			0xd0
#define  DS1307_R			0xd1

unsigned char i2c_start(void);
unsigned char i2c_repeatStart(void);
unsigned char i2c_sendAddress(unsigned char);
unsigned char i2c_sendData(unsigned char);
unsigned char i2c_receiveData_ACK(void);
unsigned char i2c_receiveData_NACK(void);
void i2c_stop(void);
	