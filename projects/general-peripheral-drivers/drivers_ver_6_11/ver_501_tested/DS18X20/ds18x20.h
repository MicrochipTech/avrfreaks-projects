
/*********************************************************************************************************
 Title  :   C header file for the DS1820 library (ds1820.c)
 Author:    Chris efstathiou  
 E-mail:    hendrix@otenet.gr
 Homepage:  ........................
 Date:      2/Dec/2002
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  any AVR MCU device
 Comments:  This software is FREE but without any warranty of any kind.
*********************************************************************************************************/



#ifndef DS18X20_H
#define DS18X20_H   1



/*##############################################################################################*/
/*			       START OF CONFIGURATION BLOCK					*/
/*##############################################################################################*/

#define ONLY_1_DEVICE_ON_THE_BUS     0    /* 1= only one ds18x20 is connected */     
#define DS18X20_CONVERSION_TIME_MS   750  /* Used in parasite mode only */

/*##############################################################################################*/
/*				END OF CONFIGURATION BLOCK					*/
/*##############################################################################################*/

/* DS18X20 TYPES */
#define DS1820     0x10  /* The plain old DS1820 with 9 bit resolution */
#define DS18S20    0x10  /* 9 bit resolution new DS1820 */
#define DS1820B    0x28  /* A very rare case. The package is marked DS1820-0014B5. A DS18B20 actually */
#define DS18B20    0x28  /* DS1820 with 12 bit resolution.  */


/* DS1820, DS18B20, DS18S20 COMMANDS */
#define CONVERT_T                 0x44
#define READ_SCRATCHPAD           0xBE
#define WRITE_SCRATCHPAD          0x4E
#define COPY_SCRATCHPAD           0x48
#define RECALL_EE                 0xB8
#define READ_POWER_SUPPLY         0xB4

/* DS18X20 REGISTERS COMMON TO ALL TYPES */
#define DS18X20_T_HIGH            2
#define DS18X20_T_LOW             3
#define DS18X20_CRC               8

/* DS18X20 TYPE SPECIFIC REGISTERS */
#define DS18X20_CONFIG            4
#define DS18X20_COUNT_REMAIN      6
#define DS18X20_COUNT_PER_C       7


/* DS18X20 keywords */
#ifndef LOCATION_IS_RAM
#define LOCATION_IS_RAM           1
#endif
#ifndef LOCATION_IS_EEPROM
#define LOCATION_IS_EEPROM        2
#endif 
#ifndef LOCATION_IS_FLASH
#define LOCATION_IS_FLASH         3
#endif

#define DS18X20_PARASITE          0
#define DS18X20_POWERED           1
#define DS18X20_DONT_WAIT         0
#define DS18X20_WAIT              1


/* RESOLUTION SETTINGS FOR DS18B20 */
#define DS18X20_9_BIT             0
#define DS18X20_10_BIT            (1<<5)
#define DS18X20_11_BIT            (1<<6)
#define DS18X20_12_BIT            ((1<<6)|(1<<5))






/********************************************************************************************************/
/*                             PUBLIC FUNCTION PROTOTYPES                                               */
/********************************************************************************************************/

/*           NO SPECIAL INITIALIZATION IS NEEDED. YOU CAN USE THE FUNCTIONS IMMEDIATELY.                */ 

/*
   ds18x20_select_device(address, LOCATION_IS_X);  X= RAM, EEPROM, FLASH)
   If only one device is defined in ds18x20.h the arguments can be 0.
   EXAMPLE: ds18x20_select_device(0,0);
*/
#if ONLY_1_DEVICE_ON_THE_BUS == 0
extern void          ds18x20_select_device(unsigned char *mem_array, unsigned char mem_type);
#endif
/* Retuns 1 if DS18X20 is normally powered or 0 if parasite powered */ 
extern unsigned char ds18x20_get_power_status(void);

/* Retuns a signed int containing the temperature */
extern signed int    ds18x20_get_temp(unsigned char wait);

/* Fills a 9 byte char array with the entire scratchpad contents */
extern void          ds18x20_get_scratchpad(unsigned char *mem_array);

/* The below 2 functions read and write selected registers */
extern signed int    ds18x20_get_reg(unsigned char reg);
extern void          ds18x20_set_reg(unsigned char reg, signed int value);



/* Macro definitions */

#define ds18x20_command(cmd)    ow_write_byte(cmd)


#endif	/* #ifndef DS18X20_H */
/*######################################################################################################*/
/*					   T H E   E N D						*/
/*######################################################################################################*/

