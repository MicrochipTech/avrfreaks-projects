/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * pjv [at] post.com wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 *
 * sd_interface.c
 *
 * Created: 17.3.2014 16:07:21
 *  Author: PjV
 *
 * Version: 1.0 
 */
 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "macros.h"
#include <util/delay.h>
#include "tracker.h"
#include "sd_interface.h"
#include "db_uart.h"
/*filesystem headers*/
#include "fat.h"
#include "fat_config.h"
#include "partition.h"
#include "sd_raw.h"
#include "sd_raw_config.h"


static uint8_t _find_file_in_dir(struct fat_fs_struct* fs, struct fat_dir_struct* dd, const char* name, struct fat_dir_entry_struct* dir_entry);
static unsigned char _init_kml_file(void);

static struct partition_struct* SD_partition;
static struct fat_fs_struct* SD_fs;
static struct fat_dir_struct* SD_dd;
static struct fat_file_struct* SD_fd;
static struct fat_dir_entry_struct SD_file_entry;

/****************************************************************************************//**
 * Description: Initialize and open a memory card.
 * return:	1 success
 *			0 fails
 *******************************************************************************************/
unsigned char SD_Initialize(void)
{
	while (1)
	{
		/* setup sd card slot */
		if(!sd_raw_init()) {
			PCICR &= ~(1<<PCIE0);//disable pin change interrupt (pushbutton int.)
			TOGGLEBIT(YELLOW_LED_PORT,YELLOW_LED_PIN);
			TOGGLEBIT(GREEN_LED_PORT,GREEN_LED_PIN);
			#if DEBUG
			db_uart_puts_p(PSTR("MMC/SD initialization failed\nInsert Card...\n"));
			#endif
			continue;
		}
		
		PCICR |= (1<<PCIE0);//re-enable pushbutton interrupt

		/* open first partition */
		// struct partition_struct* 
		SD_partition = partition_open(	sd_raw_read,
										sd_raw_read_interval,
#if SD_RAW_WRITE_SUPPORT
										sd_raw_write,
										sd_raw_write_interval,
#else
										0,
										0,
#endif
										0 );

		if(!SD_partition)
		{
			/* If the partition did not open, assume the storage device
			* is a "superfloppy", i.e. has no MBR.
			*/
			SD_partition = partition_open(	sd_raw_read,
											sd_raw_read_interval,
#if SD_RAW_WRITE_SUPPORT
											sd_raw_write,
											sd_raw_write_interval,
#else
											0,
											0,
#endif
											-1 );
			if(!SD_partition){
				db_uart_puts_p(PSTR("opening partition failed\n"));
				continue;
			}
		}	

		/* open file system */
		//struct fat_fs_struct* 
		SD_fs = fat_open(SD_partition);
		
		if(!SD_fs) {
			db_uart_puts_p(PSTR("opening filesystem failed\n"));
			continue;
		}

		/* open root directory */
		struct fat_dir_entry_struct directory;
		fat_get_dir_entry_of_path( SD_fs, "/", &directory);

		//struct fat_dir_struct* 
		SD_dd = fat_open_dir( SD_fs, &directory);
	
		if(!SD_dd)
		{
			db_uart_puts_p(PSTR("opening root directory failed\n"));
			continue;
		}
		return(1);
	}//while
}


/************************************************************************************//**
* Create and Open kml-file
 * return:	1 success
 *			0 fails
****************************************************************************************/
unsigned char SD_CreateOpenedFile(void)
{
	char filename[11];//from track00 to track99
	unsigned char SD_file_cntr = 0;
	
	sprintf( filename, "track%02d.kml", SD_file_cntr );
	
	while( _find_file_in_dir( SD_fs, SD_dd, filename, &SD_file_entry) )
	{
		sprintf( filename, "track%02d.kml", ++SD_file_cntr );
		//db_uart_puts(filename);
		
		/* abort -> Next the file sequence number is greater than 99 (track99.kml)*/
		if (SD_file_cntr > 98)
		{
			/*close file system*/
			SD_Close();
			cli();
			
			while (1)
			{
				TOGGLEBIT(YELLOW_LED_PORT,YELLOW_LED_PIN);
				_delay_ms(100);
				#if DEBUG
					db_uart_puts_p(PSTR("remove files from directory\r"));
				#endif
			}
		}
	}

#if DEBUG
	db_uart_puts_p(PSTR("\rtrying create file: "));
	db_uart_puts(filename);
	db_uart_putc('\r');
#endif	

	if(!fat_create_file( SD_dd, filename, &SD_file_entry ))
	{
		db_uart_puts_p(PSTR("error creating file: "));
		db_uart_puts(filename);
		db_uart_putc('\n');
		return(0);
	}

	SD_fd = fat_open_file( SD_fs, &SD_file_entry);
	if(!SD_fd)
	{
		db_uart_puts_p(PSTR("error opening "));
		db_uart_puts(filename);
		db_uart_putc('\n');
		return(0);
	}
	else
	{
#if DEBUG		
		db_uart_puts(filename);
		db_uart_puts_p(PSTR(" open\nPosition...\r"));
#endif			
		_init_kml_file();
	}
	return(1);
}

/****************************************************************************************//**
 * Initialize first lines of kml-file. Start-tags.
 * return:	1 success
 *			0 fails
 *******************************************************************************************/
static unsigned char _init_kml_file(void)
{
	const char* kml_header =	"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
								"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\r\n"
								"<Placemark>\r\n"
								"<name>Tracker</name>\r\n"
								"<description>Route</description>\r\n"
								"<Style id=\"Red\">\r\n"
								"<LineStyle>\r\n"
								"<color>a00f00ff</color>\r\n"
								"<width>3</width>\r\n"
								"</LineStyle>\r\n"
								"</Style>\r\n"
								"<LineString>\r\n"
								"<styleUrl>#Red</styleUrl>\r\n"
								"<tessellate>1</tessellate>\r\n"
								"<coordinates>\r\n";

	//int len = 337;//I counted it
	
	if( fat_write_file( SD_fd, (uint8_t*) kml_header, /*len*/337 ) != /*len*/337 ){
		db_uart_puts_p(PSTR("error writing kmlheader\n"));
		return(0);
	}//else db_uart_puts_p(PSTR("kmlheader OK\r"));	
	
	return(1);
}

/************************************************************************************//**
* Description: End of kml-file. Write end-tags of file and close file system:
* 
* </coordinates>
* </LineString>
* </Placemark>
* </kml>

 * return:	1 success
 *			0 fails
****************************************************************************************/
unsigned char SD_CloseKmlFile(void)
{
	const char* kml_footer = "</coordinates>\r\n</LineString>\r\n</Placemark>\r\n</kml>\r\n";
	unsigned char len = 53;

	if( fat_write_file (SD_fd, (unsigned char*)kml_footer, len) != len )
	{
		#if DEBUG
			db_uart_puts_p(PSTR("file footer error\n"));
			_delay_ms(1);
		#endif
		return(0);
	}
	
	/* close file system */
	SD_Close();
	
	return(1);	
}

/****************************************************************************************//**
 * Description: Close file System
 * return:	1 success
 *			0 fails
 *******************************************************************************************/
unsigned char SD_Close(void)
{
	unsigned char rc = 0;
	
	/* Ensures all buffered data is written to the card */
	if(!sd_raw_sync())
		db_uart_puts_p(PSTR("error syncing disk\n"));

	/* close file */
	fat_close_file(SD_fd);
	
	/* close directory */
	fat_close_dir(SD_dd);
	
	/* close file system */
	fat_close(SD_fs);
	
	/* close partition */
	rc = partition_close(SD_partition);
	
#if DEBUG
	if (!rc){
		db_uart_puts_p(PSTR("error closing partition\r"));
	}
#endif
	
	return(rc);
}

/****************************************************************************************//**
 * Description: Writes data to a file.
 * param [in] The buffer from which to read the data to be written.
 * return:	1 success
 *			0 fails
 *******************************************************************************************/
unsigned char SD_WriteToFile( const char* buf )
{
	unsigned char buf_len = 0;
	buf_len = strlen(buf);
	
	if( fat_write_file( SD_fd, (uint8_t*) buf, buf_len ) != buf_len )
	{
		#if DEBUG
			db_uart_puts_p(PSTR("error writing to file\n"));
		#endif
		return(0);
	}

	/* write buffered data straight to the file */
	if(!sd_raw_sync())
		db_uart_puts_p(PSTR("error syncing disk\n"));
	
	return(1);
}

/****************************************************************************************//**
 * return:	1 success
 *			0 fails
 *******************************************************************************************/
uint8_t _find_file_in_dir(	struct fat_fs_struct* fs, 
							struct fat_dir_struct* dd, 
							const char* name, 
							struct fat_dir_entry_struct* dir_entry)
{
	while(fat_read_dir(dd, dir_entry)){
		if(strcmp(dir_entry->long_name, name) == 0){
			fat_reset_dir(dd);
			return 1;
		}
	}
	return 0;
}

/****************************************************************************************//**
 * Description: Set GPS UTC time for File System.
 *******************************************************************************************/
#if FAT_DATETIME_SUPPORT
void get_datetime(uint16_t* year, uint8_t* month, uint8_t* day, uint8_t* hour, uint8_t* min, uint8_t* sec)
{
	/* GPS UTC time */
	
	unsigned int temp = 0;
	unsigned char digit;

	temp = 0;
	digit = rmc.ddmmyy[0];
	temp = 10*atoi((const char*)&digit);
	digit = rmc.ddmmyy[1];
	temp = temp + (atoi((const char*)&digit));
	*day = (uint8_t)temp;
	
	temp = 0;
	digit = rmc.ddmmyy[2];
	temp = 10*atoi((const char*)&digit);
	digit = rmc.ddmmyy[3];
	temp = temp + (atoi((const char*)&digit));
	*month = (uint8_t)temp;	
	
	//2014
	temp = 0;
	digit = rmc.ddmmyy[4];
	temp = (10*atoi((const char*)&digit));
	digit = rmc.ddmmyy[5];
	temp = temp + (atoi((const char*)&digit));
	temp = temp + 2000;
	*year = temp;

	/*hhmmss*/

	temp = 0;
	digit = rmc.hhmmss[0];
	temp = 10*atoi((const char*)&digit);
	digit = rmc.hhmmss[1];
	temp = temp + (atoi((const char*)&digit));	
	*hour = (uint8_t)temp;

	temp = 0;
	digit = rmc.hhmmss[2];
	temp = 10*atoi((const char*)&digit);
	digit = rmc.hhmmss[3];
	temp = temp + (atoi((const char*)&digit));
	*min = (uint8_t)temp;
	
	*sec = 0;
}
#endif
