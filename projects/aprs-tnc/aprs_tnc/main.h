//************************************************************************
// main.h	Functions and variables
//
//************************************************************************


// Global function prototypes

extern void enable_transmit(void);
extern void enable_receive(void);
extern uint8_t eeprom_rd(uint16_t address);
extern void eeprom_wr(uint16_t address, uint8_t data);

#define LED0		0
#define SIRF_ENABLE	4
#define PTT			7		// PortC bit 7 = PTT

#define BIT_DELAY	191		// Delay for 833uS @ 14.7456 MHz & prescale 64
#define	MARK		95		// For 16 samples at 1200 Hz
#define	SPACE		51 		// For 16 samples at 2200 Hz

#define NUM_FLAGS	10		// number of times to send AX25_FLAG byte
#define CR_CHAR	0xd			// ASCII CR character

#define	RXSIZE	128

uint8_t		tx_tone;				// Contains Counter2 value for tone
uint8_t		tone_change;			// flag a tone change request
uint8_t		tx_mode;				// 1 => packet TX is in progress
uint8_t		tx_delay_cnt;			// pre-preamble length
uint16_t	slow_beacon_rate;		// going slow beacon rate
uint8_t		fast_beacon_rate;		// driving fast beacon rate
uint8_t 	low_speed;				// low speed threshold
uint8_t 	high_speed;				// high speed threshold
uint8_t		min_turn_angle;			// minimum turn angle
uint8_t		turn_slope;				// turn slope
uint8_t		min_turn_time;			// minimum time for a turn
uint16_t	gps_speed;				// gps speed in knots
uint16_t	gps_course;				// gps course in degrees true north

uint8_t flag_mode ;		// bit stuffing & crc not done for Flag byte

volatile 	uint8_t tx_packet_done;	// =1 => packet transmitted
volatile 	uint8_t	gps_eol;		// GPS CR found in nmea string
volatile	uint8_t gps_start;		// Set when initial nmea '$' is found
volatile 	uint8_t	pc_eol;			// GPS CR found in nmea string
volatile	uint8_t pc_start;		// Set when initial nmea '$' is found
volatile	uint8_t	ticker;			// ticker runs at Counter2 speed
volatile	uint8_t	delay_ticker;	// same as ticker

volatile 	uint8_t switcha;	// switch A has been clicked
volatile 	uint8_t switchb;	// switch B has been clicked


uint8_t		rxbytes[RXSIZE];	// Incoming character array
uint8_t 	msg_start;			// Index of message start after header
uint8_t		msg_end;			// Index of message ending character
uint8_t		command;			// mode we are running in
uint8_t		rxtoggled;			// Signals frequency just toggled
uint16_t	crc;				// Current checksum for incoming message
uint16_t	dcd;				// Carrier detect of sorts


volatile uint8_t busy;			// Carrier detect of sorts


