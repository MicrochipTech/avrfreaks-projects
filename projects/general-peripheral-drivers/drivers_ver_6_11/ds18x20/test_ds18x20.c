
/*********************************************************************************************************
 Title  :   C  main file for the testing of DS18X20 driver (test_ds18x20.c)
 Author:    Chris Efstathiou 
 E-mail:    ........................
 Homepage:  ........................
 Date:      22/1/2003 10:59:49 рм
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  any AVR MCU device
 Comments:  This software is FREE.
*********************************************************************************************************/

/*
   COMMENTS
*/



/********************************************************************************************************/
/*                                   PREPROCESSOR DIRECTIVES                                            */
/********************************************************************************************************/

#if !defined(AVRGCC_VERSION)
#if ((__GNUC__ * 100) + __GNUC_MINOR__) > 303
#warning "compatibility header file included"
#include "compatibility.h"
#define AVRGCC_VERSION      304        /* AVRGCC version for including the correct files  */
#elif ((__GNUC__ * 100) + __GNUC_MINOR__) >= 300  &&  ((__GNUC__ * 100) + __GNUC_MINOR__) <= 303
#warning "compatibility header file not included"
#define AVRGCC_VERSION      303        /* AVRGCC version for including the correct files  */
#else 
#warning "compatibility header file not included"
#define AVRGCC_VERSION   300    
#endif
#endif

#if AVRGCC_VERSION >= 303

#include <avr/io.h>
#include <avr/eeprom.h>

#else

#include <io.h>
#include <eeprom.h>

#endif

#include "ds18x20.h"
#include "lcd_io.h"
#include "one_wire.h"

#ifndef DELAY_L_ACCURATE
/* 6 cpu cycles per loop + 3 overhead when a constant is passed. */
#define DELAY_L_ACCURATE(x)  ({ unsigned long number_of_loops=(unsigned long)x;   \
                                __asm__ volatile ( "L_%=: \n\t"                   \
                                                   "subi %A0,lo8(-(-1)) \n\t"     \
                                                   "sbci %B0,hi8(-(-1)) \n\t"     \
                                                   "sbci %C0,hlo8(-(-1)) \n\t"    \
                                                   "sbci %D0,hhi8(-(-1)) \n\t"    \
                                                   "brne L_%= \n\t"               \
                                                   : /* NO OUTPUT */              \
                                                   : "w" (number_of_loops)        \
                                                 );                               \
                             })                                      




#define TIME_L1_MS      ( 1*(F_CPU/6000) )     /* 1 MILLISECOND  */

#define DELAY_MS(ms)    DELAY_L_ACCURATE( (TIME_L1_MS*(ms)) )

/* Do not use this for delays below 100 microseconds */
#define DELAY_L_US(us)  DELAY_L_ACCURATE( (((us)*(F_CPU/6000))/1000) )

#endif /* #ifndef DELAY_L_ACCURATE */






#define ROM_CODES   8  /* The number of the rom code keys saved in the eeprom */


/********************************************************************************************************/
/*                                   TYPE DEFINITIONS                                                   */
/********************************************************************************************************/





/********************************************************************************************************/
/*                                   GLOBAL VARIABLES                                                   */
/********************************************************************************************************/

unsigned char eeprom_rom_codes[ROM_CODES][OW_TOTAL_R0M_CODE_SIZE]__attribute__((section(".eeprom")));
unsigned char ds18x20_pos_in_array=0;
unsigned char rom_code[OW_TOTAL_R0M_CODE_SIZE];

/********************************************************************************************************/
/*                                   LOCAL FUNCTION PROTOTYPES                                          */
/********************************************************************************************************/

unsigned char scan_1w_bus(void);


/********************************************************************************************************/
/*                                   LOCAL FUNCTION DEFINITIONS                                         */
/********************************************************************************************************/

unsigned char scan_1w_bus(void)
{
unsigned char x=0,temp1=0,temp2=0,temp3=0,match_found=0,match_counter=0,array_stored_roms=0;

 /* After reset we need to know how many DS18X20 are in the eeprom array */
 for(array_stored_roms=0,temp1=0,temp2=0; temp1<(sizeof(eeprom_rom_codes)/8); temp1++)
   {
      temp2=eeprom_rb((unsigned int)eeprom_rom_codes[temp1]);
      if(temp2==0x28 || temp1==0x10) { array_stored_roms++; }
   }


/* 
    reset search rom and perform a new search. Only DS18X20 are taken into account.
    x represents the total number of DS18X20 found.
*/
 ow_search_rom(rom_code, OW_RESET_SEARCH_ROM); 

 while(ow_get_msg()!= OW_SEARCH_ROM_FINISHED)      
     {    
         ow_search_rom(rom_code, OW_SEARCH_ROM);

         if(rom_code[0]==0x28 || rom_code[0]==0x10)
          {
             x++;
             for(temp1=0; temp1<(sizeof(eeprom_rom_codes)/ROM_CODES); temp1++)
               {
                  for(temp2=0; temp2<OW_TOTAL_R0M_CODE_SIZE; temp2++)
                    {
                       temp3=eeprom_rb((unsigned int)&eeprom_rom_codes[temp1][temp2]);
                       if(temp3==rom_code[temp2]) { match_found=1; } else{ match_found=0; break; }  
                    }
                  if(match_found) { match_counter++; break; } 
               }
          }
     }  

/*
   If the DS18X20 devices found in the search dont have their rom codes already stored in the eeprom,
   or the number of DS18X20 rom codes stored in the eeprom are not the same with the total number
   of DS18X20 found in the search, then the eeprom array is cleared completely, the bus is searched
   again and the DS18X20 rom codes that were found in the new search are stored in the eeprom array.
*/
 if(x>match_counter || x!=array_stored_roms)
  {
     lcd_clrscr();
     lcd_gotoxy(0,1); lcd_puts_P("FOUND NEW HARDWARE");
     lcd_gotoxy(0,3); lcd_puts_P("New rom codes = "); lcd_puti((x-match_counter),0);
     DELAY_MS(5000);
     /* Clear the EEPROM array */ 
     for(temp1=0; temp1<sizeof(eeprom_rom_codes); temp1++)
       {
          eeprom_wb((unsigned int)eeprom_rom_codes+temp1, 0);
       }
     /* reset search rom and prerform a new search */
     ow_search_rom(rom_code, OW_RESET_SEARCH_ROM);
     x=0;
     while(ow_get_msg()!= OW_SEARCH_ROM_FINISHED)
         {
             ow_search_rom(rom_code, OW_SEARCH_ROM);
 
            /* If it is a ds18x20 save the key found in the eeprom */
            if(rom_code[0]==0x28 || rom_code[0]==0x10)
             {
                x++;
                for(temp1=0; temp1<OW_TOTAL_R0M_CODE_SIZE; temp1++)
                  {
                     eeprom_wb((unsigned int)(eeprom_rom_codes[x-1]+temp1), rom_code[temp1] );
                  }
             }
         } 	

  }

/* return the number of DS18X20 found */
return(x);
}

/********************************************************************************************************/
/*                                   MAIN FUNCTION                                                      */
/********************************************************************************************************/
void main(void)
{
signed int           temperature=0;
unsigned char        x=0, temp1=0, temp2=0;


/*------------------------------------------------------------------------------------------------------*/
lcd_init();
/*
   BEWARE! CHANGE OF PORT!!!!  if you dont use the ow_config_io() function or it is not enabled
                               then the settings in one_wire.h apply. 
*/
#if ONE_WIRE_DYNAMIC_IO_CHANGE == 1
ow_configure_io(PORTB,0);
#endif
/* First let's check for any error. */
ow_reset();
if(ow_get_msg()==OW_BUS_ERROR) 
 {
   lcd_clrscr();
   lcd_puts_P("1 WIRE BUS ERROR ");  
   return;
 } 
else if(ow_get_msg()==OW_NO_DEVICE_PRESENT) 
      {
        lcd_clrscr();
        lcd_puts_P("NO 1W DEVICE FOUND ");  
        return;
      }
/*
    QUESTION: why i am including this message since the ds18x20 dont issue an alarm this way?
    ANSWER  : because some other 1 wire devices do so i included the below lines as an example.
*/
else if(ow_get_msg()==OW_ALARM_DETECTED) 
      {
        lcd_clrscr();
        lcd_puts_P("ALARM DETECTED");  
        return;
      } 

/*------------------------------------------------------------------------------------------------------*/
/*
   Scan the 1 wire bus for any changes from the previus state and find out how many DS18X20
   are on the bus. Only DS18X20's are taken into account.
   If the DS18X20 devices found in the search dont have their rom codes already stored in the eeprom,
   or the number of DS18X20 rom codes stored in the eeprom are not the same with the total number
   of DS18X20 found in the search, then the eeprom array is cleared completely, the bus is searched
   again and the DS18X20 rom codes that were found in the new search are stored in the eeprom array.
*/
    x=scan_1w_bus();

    lcd_clrscr();
    lcd_clrline(0); lcd_puts_P("Rom Codes found= "); lcd_puti(x,0);
    lcd_clrline(1); lcd_puts_P("Last Rom Code is: ");
    lcd_gotoxy(0,2);
    for(temp1=0; temp1<(OW_TOTAL_R0M_CODE_SIZE-((OW_TOTAL_R0M_CODE_SIZE/2)-1)); temp1++)
      {
         lcd_puti(eeprom_rb((unsigned int)(eeprom_rom_codes[x-1]+temp1)),0);
         if( temp1<(OW_TOTAL_R0M_CODE_SIZE-(OW_TOTAL_R0M_CODE_SIZE/2)) ) { lcd_putc('-'); }
      }
    lcd_gotoxy(0,3);
    for(; temp1<OW_TOTAL_R0M_CODE_SIZE; temp1++)
      {
         lcd_puti(eeprom_rb((unsigned int)(eeprom_rom_codes[x-1]+temp1)),0);
         if(temp1<7) { lcd_putc('-'); }
      }    
#if OW_CALCULATE_CRC8_NEEDED == 1
    lcd_gotoxy(12,3);
    lcd_puts_P("CRC8=");
    lcd_puti(calculate_crc8(rom_code, 7),0);
#endif
    DELAY_MS(5000);  /* delay 5 seconds so you can read the SEARCH_ROM screen */
        
/*------------------------------------------------------------------------------------------------------*/

/* Now lets set some DS18X20 registers that set varius parameters */
/* Set high, low thresholds and the resolution if applicable */

#if ONLY_1_DEVICE_ON_THE_BUS == 0
for(temp1=0; temp1<x; temp1++)
  {
     /* ds18x20_select_device(address, LOCATION_IS_X);  X= RAM, EEPROM, FLASH) */
     ds18x20_select_device(eeprom_rom_codes[temp1], LOCATION_IS_EEPROM);
     /* time to start cooling your room */
     ds18x20_set_reg(DS18X20_T_HIGH, 30);       
     /* time to start heating your room */
     ds18x20_set_reg(DS18X20_T_LOW,  14);
     ds18x20_set_reg(DS18X20_CONFIG, DS18X20_12_BIT);
  } 
#elif ONLY_1_DEVICE_ON_THE_BUS == 1

     ds18x20_set_reg(DS18X20_T_HIGH, 30);       
     ds18x20_set_reg(DS18X20_T_LOW,  14);
     ds18x20_set_reg(DS18X20_CONFIG, DS18X20_12_BIT);

#endif   

/*------------------------------------------------------------------------------------------------------*/

/* It is time to configure the visual human interface (yes i mean the LCD display) */

lcd_clrscr();
lcd_clrline(0);
lcd_puts_P("Temperature Celsius");

/* Display the number of the rom code selected. */
lcd_clrline(3);
#if ONLY_1_DEVICE_ON_THE_BUS == 0
lcd_put_i(x,0,2);
lcd_puts_P("devices are used");
#else
lcd_puts_P("1 device mode !    ");
#endif

/*------------------------------------------------------------------------------------------------------*/
/* 
   Display the power supply type of the devices found. If even one of them is on parasite power a "YES"
   is displayed else a "NO".
*/
lcd_clrline(2);
lcd_puts_P("Parasite power=");

#if ONLY_1_DEVICE_ON_THE_BUS == 0

for(temp1=0,temp2=0; temp1<x; temp1++)
    {
       ds18x20_select_device(eeprom_rom_codes[temp1], LOCATION_IS_EEPROM);
       if(ds18x20_get_power_status()) { temp2++; } 
    }
if(temp2<x) { lcd_puts_P("YES"); } else{ lcd_puts_P("NO"); }

#elif ONLY_1_DEVICE_ON_THE_BUS == 1

if(ds18x20_get_power_status) { lcd_puts_P("NO"); } else{ lcd_puts_P("YES"); } 

#endif 

/*------------------------------------------------------------------------------------------------------*/  
/* 
   Finally we can make a loop that takes temperature measurements for ever.
   But first i will use a technic to avoid erroneus readings after power on.
   I will do this by waiting for every ds18x20 to complete one measurement 
   and then read the temperature.
   ds18x20_get_temp(DS18X20_WAIT) waits for the temperature conversion to finish and then returns.
   ds18x20_get_temp(DS18X20_DONT_WAIT) check if the temperature conversion is finished and if the
   conversion is finished it returns the measurement else it returns the previus measurement.
   This trick avoids erroneus readings at power up.
*/
temp2=DS18X20_WAIT;
while(1)
    {

#if ONLY_1_DEVICE_ON_THE_BUS == 0

       for(temp1=0, temperature=0; temp1<x; temp1++)
         {
            ds18x20_select_device(eeprom_rom_codes[temp1], LOCATION_IS_EEPROM);
            temperature+=ds18x20_get_temp(temp2); 
         }
       temperature=temperature/x;       
       lcd_gotoxy(8,1); 
       lcd_put_i(temperature,1,6);
       /* now lets set the measurements to full speed by using polling and not delay loop */
       temp2=DS18X20_DONT_WAIT;

#elif ONLY_1_DEVICE_ON_THE_BUS == 1

       temperature=ds18x20_get_temp(temp2); 
       lcd_gotoxy(8,1); 
       lcd_put_i(temperature,1,6);
       /* now lets set the measurements to full speed by using polling and not delay loop */
       temp2=DS18X20_DONT_WAIT;

#endif
    }


return;
}


/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

