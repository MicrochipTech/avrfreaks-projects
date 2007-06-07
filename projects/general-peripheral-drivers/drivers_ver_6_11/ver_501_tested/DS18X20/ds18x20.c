
/*********************************************************************************************************
 Title  :   C include file for the DS1820 library (ds1820.c)
 Author:    Chris efstathiou  
 E-mail:    hendrix@otenet.gr
 Homepage:  ........................
 Date:      2/Dec/2002
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  any AVR MCU device
 Comments:  This software is FREE but without any warranty of any kind.
*********************************************************************************************************/

/* 
    Although i could had written it in the new style, i prefer the old one for compatibility sake.
*/

/********************************************************************************************************/
/*                                   PREPROCESSOR DIRECTIVES                                            */
/********************************************************************************************************/

#ifndef _IO_REG_MACRO_MODE_
#define _IO_REG_MACRO_MODE_  1	   /* In case you have the new assignment mode io headers */
#endif

#ifndef  _SFR_ASM_COMPAT
#define  _SFR_ASM_COMPAT     1	   /* This is for GCC 3.2 */
#endif



#if AVRGCC_VERSION == 330

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#else

#include <io.h>
#include <eeprom.h>
#include <pgmspace.h>

#endif

#include "one_wire.h"
#include "ds18x20.h" 
#include "lcd_io.h"



/********************************************************************************************************/
/*                                   TYPE DEFINITIONS                                                   */
/********************************************************************************************************/





/********************************************************************************************************/
/*                                   LOCAL FUNCTION PROTOTYPES                                          */
/********************************************************************************************************/
static void send_selected_rom(void);





/********************************************************************************************************/
/*                                   GLOBAL VARIABLES                                                   */
/********************************************************************************************************/
unsigned char conversion_in_progress=0;
unsigned char power_status=0;
unsigned char ds18x20_rom_code[8];
signed int    temperature=0;


/********************************************************************************************************/
/*                                   LOCAL FUNCTIONS                                                    */
/********************************************************************************************************/
static void send_selected_rom(void)
{
#if ONLY_1_DEVICE_ON_THE_BUS == 1
  ow_read_rom(ds18x20_rom_code);
  ow_command(OW_SKIP_ROM);
#elif ONLY_1_DEVICE_ON_THE_BUS == 0
unsigned char x=0;

  ow_command(OW_MATCH_ROM);  
  for(x=0; x<8; x++) { ow_put_byte(ds18x20_rom_code[x]); }
#endif

return;
}

/********************************************************************************************************/
/*                                   PUBLIC FUNCTIONS                                                   */
/********************************************************************************************************/


/*######################################################################################################*/
/*                                 DS1820 - DS18S20 FUNCTIONS                                           */
/*######################################################################################################*/
 
#if ONLY_1_DEVICE_ON_THE_BUS == 0
void ds18x20_select_device(unsigned char *mem_array, unsigned char mem_type)
{

unsigned char x=0;

if(mem_type == LOCATION_IS_RAM)
 { 
    for(x=0; x<8; x++) { *(ds18x20_rom_code+x)=(*(mem_array+x)); } 
 }
else if(mem_type == LOCATION_IS_FLASH)
      { 
         for(x=0; x<8; x++) { *(ds18x20_rom_code+x)=PRG_RDB((const unsigned char*)(mem_array+x)); }
      }
else if(mem_type == LOCATION_IS_EEPROM)
      {
         for(x=0; x<8; x++) { ds18x20_rom_code[x]=eeprom_rb((unsigned int)(mem_array+x)); }
      }

return;
}
#endif
/*######################################################################################################*/

unsigned char ds18x20_get_power_status(void)
{
    ow_reset();
    send_selected_rom();
    ow_put_byte(READ_POWER_SUPPLY);
    power_status=ow_get_bit();
    ow_reset();


return(power_status);
}
/*######################################################################################################*/

signed int ds18x20_get_temp(unsigned char wait)
{
signed char       bit=0;
unsigned char     t_resolution=0, skip_convert_command=0;



/* First we must see if the DS18X20 is busy converting and if yes return the previus value. */
ow_reset();
send_selected_rom();
if(ow_get_bit()==0) { return(temperature); }

/* Now we need to know the DS18B20 resolution if there is one of course */
if(ds18x20_rom_code[0]==DS18B20)
 {
    t_resolution=ds18x20_get_reg(DS18X20_CONFIG);
    t_resolution &= DS18X20_12_BIT;
    if(t_resolution==DS18X20_12_BIT) { t_resolution=12; }
    else if(t_resolution==DS18X20_11_BIT) { t_resolution=11; }
    else if(t_resolution==DS18X20_10_BIT) { t_resolution=10; }
    else{ t_resolution=9; }
 }

if(ds18x20_get_power_status() == 0 || wait) 
 {
    ow_reset();
    send_selected_rom();
    ow_put_byte(CONVERT_T);
    ow_pull_hard_dq(1);
    if(t_resolution) { DELAY_MS( (DS18X20_CONVERSION_TIME_MS/(1<<(12-t_resolution))) ); }
    else{DELAY_MS(DS18X20_CONVERSION_TIME_MS); }
    ow_pull_hard_dq(0);
    skip_convert_command=1;
 }

/* If we reached here conversion is complete so lets update the temperature value. */
temperature=0;
ow_reset();
send_selected_rom();
ow_put_byte(READ_SCRATCHPAD);

/* read the Temperature registers, LSBit of LSByte first */
for(bit=0; bit<16; bit++)
  {
     if(ow_get_bit()) { temperature |= (1<<bit); }
  }

if(ds18x20_rom_code[0]==DS18B20) 
 {
     bit=(temperature&0x0F);
     if(temperature < 0) { bit|=0xF0; }
     bit=(bit*10)/16;
     temperature=(((temperature/16)*10)+bit);
 }
else{  
        bit=(temperature&0x01);
        if(temperature < 0) { bit|=0xFE; }
        bit=(bit*10)/2;
        temperature=(((temperature/2)*10)+bit);
    }

/* Send a new CONVERT_T command */
if(skip_convert_command==0)
 {
    ow_reset();
    send_selected_rom();
    ow_put_byte(CONVERT_T);
 }
ow_reset();



return(temperature);
}
/*######################################################################################################*/

void ds18x20_get_scratchpad(unsigned char *mem_array)
{
unsigned char x=0;

    ow_reset();
    send_selected_rom();
    ow_put_byte(READ_SCRATCHPAD);
    for(x=0; x<9; x++) {*(mem_array+x)=ow_get_byte(); }
    ow_reset();

return;
}
/*######################################################################################################*/

void ds18x20_set_reg(unsigned char reg, signed int value)
{
unsigned char scratchpad[9];
unsigned char x=0, write_end_pos=3;

if(ds18x20_rom_code[0]==DS18B20) { write_end_pos=4; } 

   ds18x20_get_scratchpad(scratchpad);
   scratchpad[reg]=value;
   ow_reset();
   send_selected_rom();
   ow_put_byte(WRITE_SCRATCHPAD);

   /* write the byte, lsb first */
   for(x=2; x<=write_end_pos; x++)
     {
        ow_put_byte(scratchpad[x]);
     }

   ow_reset();


return;
}
/*######################################################################################################*/
signed int ds18x20_get_reg(unsigned char reg)
{
unsigned char scratchpad[9];

    ds18x20_get_scratchpad(scratchpad);


return(scratchpad[reg]);
}

/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/













