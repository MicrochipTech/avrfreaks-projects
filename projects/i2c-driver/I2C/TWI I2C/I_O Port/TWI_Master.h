/*****************************************************************************
*
* File				TWI_Master.h compiled with gcc
* Date              Dec 2, 2008
* Source			Modified from AVR315 - TWI Master Implementation 
* Supported devices : All devices with a TWI module can be used.
*                     The example is written for the ATmega168.
*			 Requires that clock be set at 4MHz.
*
* This is the hardware driver for the TWI module implementing the I2C bus.
* 12/19/08	Added Random (memory) Read function.		-jkl
* 
****************************************************************************/

/****************************************************************************
  TWI Status/Control register definitions
****************************************************************************/
#define TWI_BUFFER_SIZE 18   // Slave Address + Memory start address + 16 data bytes (message size)

#define TWI_TWBR            0x0C        // TWI Bit rate Register setting.
                                        // See Application note for detailed 
                                        // information on setting this value.
// Not used defines!
//#define TWI_TWPS          0x00        // This driver presumes prescaler = 00

/****************************************************************************
  Global definitions
****************************************************************************/

union TWI_statusReg                       // Status byte holding flags.
{
    unsigned char all;
    struct
    {
        unsigned char lastTransOK:1; 
		unsigned char memRead:1;
        unsigned char unusedBits:6;
    };
};

extern union TWI_statusReg TWI_statusReg;

/****************************************************************************
  Function definitions
****************************************************************************/
void TWI_Master_Initialise( void );
unsigned char TWI_Transceiver_Busy( void );
unsigned char TWI_Get_State_Info( void );
void TWI_Start_Random_Read( unsigned char * , unsigned char );
void TWI_Start_Read_Write( unsigned char * , unsigned char );
unsigned char TWI_Read_Data_From_Buffer( unsigned char *, unsigned char );

/****************************************************************************
  Bit and byte definitions
****************************************************************************/
#define TWI_READ_BIT  0       // Bit position for R/W bit in "address byte".
#define TWI_ADR_BITS  1       // Bit position for LSB of the slave address bits in the init byte.

#define TRUE          1
#define FALSE         0

/****************************************************************************
  TWI State codes
  Note: 3 lsbs are for prescale value and should be masked out. (12/2 -jkl)
****************************************************************************/
// General TWI Master staus codes                      
#define TWI_START                  0x08  // START has been transmitted  
#define TWI_REP_START              0x10  // Repeated START has been transmitted
#define TWI_ARB_LOST               0x38  // Arbitration lost

// TWI Master Transmitter staus codes                      
#define TWI_MTX_ADR_ACK            0x18  // SLA+W has been tramsmitted and ACK received
#define TWI_MTX_ADR_NACK           0x20  // SLA+W has been tramsmitted and NACK received 
#define TWI_MTX_DATA_ACK           0x28  // Data byte has been tramsmitted and ACK received
#define TWI_MTX_DATA_NACK          0x30  // Data byte has been tramsmitted and NACK received 

// TWI Master Receiver staus codes  
#define TWI_MRX_ADR_ACK            0x40  // SLA+R has been tramsmitted and ACK received
#define TWI_MRX_ADR_NACK           0x48  // SLA+R has been tramsmitted and NACK received
#define TWI_MRX_DATA_ACK           0x50  // Data byte has been received and ACK tramsmitted
#define TWI_MRX_DATA_NACK          0x58  // Data byte has been received and NACK tramsmitted

// TWI Miscellaneous status codes
#define TWI_NO_STATE               0xF8  // No relevant state information available; TWINT = “0”
#define TWI_BUS_ERROR              0x00  // Bus error due to an illegal START or STOP condition

