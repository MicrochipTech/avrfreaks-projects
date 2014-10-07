/*
 * tracker.h
 *
 * Created: 14.3.2014 13:36:14
 *  Author: PjV
 *
 * Version: 1.0 
 */ 


#ifndef TRACKER_H_
#define TRACKER_H_

/* Main Program States */
typedef enum
{
	INIT,
	SEARCH_SIGNALS,
	READY,
	START,
	SEARCH_VALID_RMC,
	WRITE_LINE,
	CLOSE_FILE
}STATE_t;

/***************************************************//**
* Global Variables
*******************************************************/

/**
 * This struct contains last valid NMEA RMC sentence data. 
 */
typedef struct /*NMEA_RMC*/
{
	/**
     * UTC time, used for files timestamp.
     */
	unsigned char hhmmss[6+1];		
	/**
     * north-south position
     */	
	unsigned char latitude[11+1];
	/**
     * N/S indicator, 'N'=North, 'S'=South
     */	
	unsigned char ns_indicator;
	/**
     * east-west position
     */		
	unsigned char longitude[12+1];
	/**
     * E/W indicator, 'E'=east or 'W'=west
     */		
	unsigned char ew_indicator;	
	/**
     * Speed over ground, Knots
	 * Not used in this application.
     */		
	unsigned char speed[5+1];		
	/**
     * date, used for files
     */		
	unsigned char ddmmyy[6+1];
	
}NMEA_RMC_t;

/* nmea_rmc struct */
NMEA_RMC_t rmc;

/*Maximum length is limited to 82 including the start and end sequences*/
/*Current receiving gps nmea-rmc-sentence*/
volatile unsigned char rmc_rx_data_g[82+1];
/*Last Valid Current data is stored here*/
unsigned char rmc_last_valid_data_g[82+1];

/*Received current data is ready for store*/
volatile unsigned char rmc_ready_flag;

//TODO
char line_to_file[26+1];

#endif /* TRACKER_H_ */