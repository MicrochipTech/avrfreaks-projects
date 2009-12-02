/**
 * @file   
 * @author Alex Raimondi 
 * @date   8. 6. 2002
 * 
 * @brief  Low level function to interface an SHT11.
 * 
 * The SHT11 is a digital humidity & temperature sensmitter 
 * from Sensirion (www.sensirion.com).
 */
#ifndef SHT11_H
#define SHT11_H


#define PORTA (void*)0xffe02800
#define PORTB (void*)0xffe02c00
#define SCK 0x0A
#define DATA 0x0B


const char crcTable[] = {
        0, 49, 98, 83, 196, 245, 166, 151, 185, 136, 219, 234, 125, 76, 31, 46, 67, 114, 33, 16,
        135, 182, 229, 212, 250, 203, 152, 169, 62, 15, 92, 109, 134, 183, 228, 213, 66, 115, 32, 17,
        63, 14, 93, 108, 251, 202, 153, 168, 197, 244, 167, 150, 1, 48, 99, 82, 124, 77, 30, 47,
        184, 137, 218, 235, 61, 12, 95, 110, 249, 200, 155, 170, 132, 181, 230, 215, 64, 113, 34, 19,
        126, 79, 28, 45, 186, 139, 216, 233, 199, 246, 165, 148, 3, 50, 97, 80, 187, 138, 217, 232,
        127, 78, 29, 44, 2, 51, 96, 81, 198, 247, 164, 149, 248, 201, 154, 171, 60, 13, 94, 111,
        65, 112, 35, 18, 133, 180, 231, 214, 122, 75, 24, 41, 190, 143, 220, 237, 195, 242, 161, 144,
        7, 54, 101, 84, 57, 8, 91, 106, 253, 204, 159, 174, 128, 177, 226, 211, 68, 117, 38, 23,
        252, 205, 158, 175, 56, 9, 90, 107, 69, 116, 39, 22, 129, 176, 227, 210, 191, 142, 221, 236,
        123, 74, 25, 40, 6, 55, 100, 85, 194, 243, 160, 145, 71, 118, 37, 20, 131, 178, 225, 208,
        254, 207, 156, 173, 58, 11, 88, 105, 4, 53, 102, 87, 192, 241, 162, 147, 189, 140, 223, 238,
        121, 72, 27, 42, 193, 240, 163, 146, 5, 52, 103, 86, 120, 73, 26, 43, 188, 141, 222, 239,
        130, 179, 224, 209, 70, 119, 36, 21, 59, 10, 89, 104, 255, 206, 157, 172 };
/** 
 * Macros to toggle port state of SCK line.
 */
//#define SHT11_SCK_LO()		__raw_writel(0x0, PORTB + PIO_CODR) 	 
//#define SHT11_SCK_HI()		__raw_writel(0x0, PORTB + PIO_SODR) 

/** 
 * Macros to toggle port state of DATA line.
 * 
 * Since DATA line has to be externaly pulled up by a resistor on initialisation
 * we have to write a 0 into port bit and toggle line by switching between 
 * input and output.
 */
//#define SHT11_DATA_LO()		__raw_writel(0x1, PORTB + PIO_CODR)	
//#define SHT11_DATA_HI()		__raw_writel(0x1, PORTB + PIO_SODR)

/**
 * Macro to read DATA bit from port.
 */
//#define SHT11_GET_BIT() (bit_is_set(SHT11_DATA_PIN, SHT11_DATA_BIT) > 0 ? 1 : 0) 


/**
 * Variable to hold crc value continously updated while communicating.
 */
//extern uint crcValue;

/**
 * Variable to hold current content of sht11 status register.
 * This value is needed as start value of crc calculation.
 * The initial value (after power on reset) is 0. 
 */
//extern uint sht11StatusReg;

/** 
 * Definitions of all known sht11 commands.
 */
/*@{*/
#define SHT11_MEAS_T	0x03		///< Start measuring of temperature.
#define SHT11_MEAS_RH	0x05		///< Start measuring of humidity.
#define SHT11_STATUS_R	0x07		///< Read status register.
#define SHT11_STATUS_W	0x06		///< Write status register.
#define SHT11_RESET		0x1E		///< Perform a sensor soft reset.
#define TRUE 1
#define FALSE 0
#define BYTE_SWAP2(X) ((X & 0x00FF) << 8) | ((X & 0xFF00) >> 8)
#define SWAP_2(x) ( (((x) & 0xff) << 8) | ((unsigned short)(x) >> 8) )

#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT)) 
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT)) 
#define FLIPBIT(ADDRESS,BIT) (ADDRESS ^= (1<<BIT)) 
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT)) 

#define SETBITMASK(x,y) (x |= (y)) 
#define CLEARBITMASK(x,y) (x &= (~y)) 
#define FLIPBITMASK(x,y) (x ^= (y)) 
#define CHECKBITMASK(x,y) (x & (y)) 

#define VARFROMCOMB(x, y) x 
#define BITFROMCOMB(x, y) y 

#define C_SETBIT(comb) SETBIT(VARFROMCOMB(comb), BITFROMCOMB(comb)) 
#define C_CLEARBIT(comb) CLEARBIT(VARFROMCOMB(comb), BITFROMCOMB(comb)) 
#define C_FLIPBIT(comb) FLIPBIT(VARFROMCOMB(comb), BITFROMCOMB(comb)) 
#define C_CHECKBIT(comb) CHECKBIT(VARFROMCOMB(comb), BITFROMCOMB(comb)) 


/*@}*/

/**
 * Enum to select between temperature and humidity measuring.
 */
typedef enum _sht11MeasureType
{
	sht11MeaT		= SHT11_MEAS_T,	///< Temperature		
	sht11MeaRh		= SHT11_MEAS_RH	///< Humidity
} sht11MeasureType;

/**
 * @brief	Start new transmission
 *
 * Performs a transmission start sequence and resets crc 
 * calculation.
 *
 * @note	Do not use this function directly. 
  */
void halSht11TransmissionStart( void );

/**
 * @brief	Read one byte from sensor
 *
 * Reads one byte form sensor and sends or skips ACK
 * according to sendAck.
 * This function does not perform any crc calculation!
 *
 * @note	Do not use this function directly. 
 *
 * @param	sendAck	if TRUE an ack will be sent to sensor
 *					if FALSE no ack will be sent. This will terminate the communication.
 *
 * @return	the byte read form sensor
 */
uint halSht11ReadByte( bool sendAck );

/**
 * @brief	Send one byte to sensor
 *
 * Sends a byte to the sensor and updates the crc calculation.
 *
 * @note	Do not use this function directly. 
 *
 * @param	value	The value to be sent.
 *
 * @return	TRUE if ACK form sensor received. FALSE otherwise.
 */
bool halSht11SendByte( uint value );

/**
 * @brief	Sets up sht11 
 *
 * Ports are set up to enable communication.
 */
void halSht11Init( void );

/** 
 * @brief	Start measuring
 *
 * Starts a temperature or humidity measurement according
 * to the value in type.
 * This function does not wait for the measurement to complete.
 *
 * @param	type	The type of value that should be measured.
 *
 * @return	TRUE if ACK form sensor received. FALSE otherwise.
 */
bool halSht11StartMeasure( sht11MeasureType type );

/**
 * @brief	Wait for the sensor to complete measurment.
 *
 * This function blocks until the sensor sings completion of current 
 * measurement. 
 *
 * @warning	By now there is no timeout implemented. We have to take care 
 *			the sensor is realy measuring. Otherwise we are trapped in an 
 *			endless loop.
 */
 #define halSht11ValueReady()	(SHT11_GET_BIT() == 0)

/**
 * @brief	Gets the result of measurement.
 *
 * First this function checks for the sensor to be ready. If not it blocks until
 * value is ready.
 * The value is read form sensor, the crc value is updated and compared with crc
 * value supplied by the sensor.
 *
 * @param	value	Measured value is returned in here.
 *
 * @return	TRUE if no crc error, FALSE otherwise.
 */
bool halSht11GetMeaValue( ulong * value );

/**
 * @brief	Write status register
 *
 * The supplied value is written into status register. 
 * sht11StatusReg (needed for crc calculation) is updated, too.
 *
 * @param	value	new status register value.
 *
 * @return	TRUE if ACK form sensor received. FALSE otherwise.
 */
bool halSht11WriteStatus( uint value );

/**
 * @brief	Read status register
 *
 * Reads out the sensor's status register. Crc is updated and compared 
 * to the crc value supplied by the sensor.
 * sht11StatusReg (needed for crc calculation) is updated, too.
 *
 * @param	value	Current status register value is returned in here.
 *
 * @return	TRUE if no crc error, FALSE otherwise.
 */
bool halSht11ReadStatus( uint *value );

bool readBit( u32 *port,uint bit );
bool writeBit( u32 *port,uint bit,uint value );
void showRegs( u32 *port );
char sht11_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);
char sht11_softreset(void);

#endif

