//************************************************************************
// ax25.h	defines functions and variables
//
//************************************************************************


#define AX25_UI		0x3			// UI frame
#define AX25_NL3	0xf0		// no layer 3
#define X25_FLAG	0x7e		// Flag byte

// global variables

uint8_t mic_dst_addr[7];	// Mic-E destination field = Latitude + TO_SSID
uint8_t mic_src_addr[7]; 
uint8_t mic_path1[6];
uint8_t mic_long[4];
uint8_t mic_sp_crs[3];
uint8_t mic_symbol[2];
uint8_t mic_altitude[4];


uint16_t crc;
uint8_t	beacon_interval;
uint8_t	tx_delay;			// Number of 6.7ms delay cycles (send flags)
uint8_t	sym_table;
uint8_t	sym_id;
uint8_t	comment_txt[40];
uint8_t	min_thresh;
uint8_t	slope_thresh;
uint8_t	delta_beacon;
uint8_t	fast_beacon;
uint8_t	slow_beacon;
uint8_t	low_speed;
uint8_t	high_speed;

uint8_t mic_dst_addr[7];	// Mic-E destination field = Latitude + SSID

uint8_t 	tx_pack[50];		// bit stuffed output buffer.
uint8_t 	tx_ptr;				// point to buffer
uint8_t 	tx_bit_cnt;			// bit # within current byte
int16_t		tx_bit_count;		// total # of bits to transmit

// global function prototypes

extern void ax25_mice_packet(void);
extern void ax25_txbyte(uint8_t inbyte);




