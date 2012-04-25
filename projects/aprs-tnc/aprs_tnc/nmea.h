//************************************************************************
// nmea.h	defines functions and variables
//
//************************************************************************


uint8_t	Time_Temp[7]; 	// Temp variable here so main can read seconds

extern void nmea_buffer_init(void);
extern uint8_t gps_grab_nmea (void);

extern void MsgSendAck (uint8_t *rxbytes, uint8_t msg_start);

uint8_t time2send;			// 1 => time to send
uint8_t new_position;		// 1 => new gps position computed
uint8_t alt_valid;			// 1 => 3D fix with altitude info
