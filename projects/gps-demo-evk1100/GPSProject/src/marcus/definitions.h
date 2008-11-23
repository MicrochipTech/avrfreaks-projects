#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

// DEFINITIONS
#define EXAMPLE_USART               (&AVR32_USART1)
#define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_PIN
#define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_FUNCTION
#define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_PIN
#define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_FUNCTION

#define gps_buff_size 85


// STRUCTURES
struct currentPositionStruct
{
	char gpsBuff[gps_buff_size];
	int parseGPSBuff;  // true or false, ready to parse gpsBuff
	int recordData;	   // true or false, should we keep recording data in ISR
	int index;		   // keep track of where we are in the gpsBuff
	//int reportUpdate; // true or false, should we send new location back to GUI
	
	double timeStamp;	// time
	double latitude;	// in decimal degrees
	double longitude;	// in decimal degrees
	double speed;		// in km/h
	double heading;		// true heading in degrees
	double altitude;	// in meters

	int fixIndicator;
	int numberOfSatellites;
};

struct gpsStruct
{
	int gotGGA;
	int gotVTG;
	int parseDone;
};

// DECLARATIONS
extern struct currentPositionStruct CurrentPosition;
extern struct gpsStruct gps;


// GLOBALS
extern unsigned char TimeOut;



#endif /*DEFINITIONS_H_*/
