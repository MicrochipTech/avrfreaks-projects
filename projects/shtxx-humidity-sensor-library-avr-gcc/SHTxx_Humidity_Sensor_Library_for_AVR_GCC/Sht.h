//----------------------------------------------------------------------------------
//
// Sensirion SHT1x Humidity Sensor Library
//
//----------------------------------------------------------------------------------

#ifndef __sht_h
#define __sht_h

#define XTAL 				8000000		// Processor clock

#define SHT_TEMPERATURE 	0x03		// Measure temp - for ShtMeasure
#define SHT_HUMIDITY 		0x05		// Measure humidity - for ShtMeasure

#define SHT_DDR				DDRB		// Port with clock and data pins
#define SHT_PORT			PORTB		// Port with clock and data pins
#define SHT_PIN				PINB		// Port with clock and data pins
#define SHT_CLOCK			1			// Pin used to output clock to SHT
#define SHT_DATA			2			// Pin used to read/output data from/to SHT

#define SHT_DELAY			25			// uS delay between clock rise/fall

#define STATUS_REG_W 		0x06 		// Command to read status register
#define STATUS_REG_R 		0x07 		// Command to write status register
#define RESET 				0x1e 		// Command for soft reset (not currently used)


void ShtInit(void);
void ShtReset(void);
int  ShtMeasure(unsigned char mode);
void ShtCalculate(int *p_temperature, int *p_humidity);
char ShtReadStatus(unsigned char *p_value);
char ShtWriteStatus(unsigned char value);

#endif
