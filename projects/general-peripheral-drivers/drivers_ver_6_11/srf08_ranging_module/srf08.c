
/****************************************************************************
 Title  :   C  file for the SRF08 FUNCTIONS library (srf08.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      13/Jul/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/

#if !defined(AVRGCC_VERSION)
#if ((__GNUC__ * 100) + __GNUC_MINOR__) > 303
#define AVRGCC_VERSION      304        /* AVRGCC version for including the correct files  */
#elif ((__GNUC__ * 100) + __GNUC_MINOR__) >= 300  &&  ((__GNUC__ * 100) + __GNUC_MINOR__) <= 303
#define AVRGCC_VERSION      303        /* AVRGCC version for including the correct files  */
#else 
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

#include "srf08.h"
#include "i2c.h"






/* Global Variables */
static unsigned char address=SRF08_UNIT_0;

/*#################################################################################################*/


/*#################################################################################################*/

void srf08_set_gain(unsigned char gain)
{
    if(gain>31) { gain=31; }
    i2c_start_tx(address);         
    i2c_put_byte(1);                               
    i2c_put_byte(gain);
    i2c_stop();                            

return;
}
/*#################################################################################################*/

void srf08_set_range(unsigned int millimeters)
{
    millimeters= (millimeters/43); 
    if(millimeters > 0xff ) { millimeters=0xff; }
    i2c_start_tx(address);         
    i2c_put_byte(2);                               
    i2c_put_byte(millimeters);
    i2c_stop();                            

return;
}
/*#################################################################################################*/

unsigned int srf08_ping(unsigned char range_unit)
{

     union i2c_union {
                       unsigned int  rx_word; 
                       unsigned char rx_byte[2];
                     } i2c;

       
        i2c_start_tx(address);
        i2c_put_byte(SRF08_COMMAND);                    
        i2c_put_byte(range_unit);
        i2c_stop();
/* INSTEAD OF WAITING USING A DELAY LOOP, POLL SRF08 FOR COMPLETION OF RANGING */             
        do{
            i2c_start_tx(address);
            i2c_put_byte(SRF08_ECHO_1);
            i2c_start_rx(address);
            i2c.rx_byte[1]=i2c_get_byte(I2C_CONTINUE);   /* get high byte msb first  */                             
            i2c.rx_byte[0]=i2c_get_byte(I2C_QUIT);
            i2c_stop();
    
          }while(i2c.rx_byte[0] >= 0xFF);  

          i2c_start_tx(address);
          i2c_put_byte(SRF08_ECHO_1);
          i2c_start_rx(address);
          i2c.rx_byte[1]=i2c_get_byte(I2C_CONTINUE);  /* get high byte msb first */                         
          i2c.rx_byte[0]=i2c_get_byte(I2C_QUIT);      /* get low byte msb first  */                          
          i2c_stop();


return(i2c.rx_word);
}
/*#################################################################################################*/

unsigned int srf08_read_register(unsigned char srf08_register)
{
union i2c_union {
                  unsigned int  rx_word; 
                  unsigned char rx_byte[2];
                } i2c;


        i2c_start_tx(address);
        i2c_put_byte(srf08_register);
        i2c_start_rx(address);
       
        /* get high byte msb first */ 
        if(srf08_register>=2) { i2c.rx_byte[1]=i2c_get_byte(I2C_CONTINUE); }                         
       
        /* get low byte msb first  */ 
        i2c.rx_byte[0]=i2c_get_byte(I2C_QUIT);                          

        i2c_stop();

return(i2c.rx_word);
}
/*#################################################################################################*/

void srf08_change_i2c_address(unsigned char new_i2c_address)
{

    /* New address validity check */
    if( new_i2c_address<SRF08_UNIT_0 || new_i2c_address>SRF08_UNIT_15 )  { return; }
    if(new_i2c_address%2) { return; }

    /* Start the I2C address changing procedure */
    i2c_start_tx(address);         
    i2c_put_byte(SRF08_COMMAND); 
    i2c_put_byte(0XA0); 
    i2c_stop(); 
    
    i2c_start_tx(address);         
    i2c_put_byte(SRF08_COMMAND); 
    i2c_put_byte(0XAA);                                
    i2c_stop();

    i2c_start_tx(address);         
    i2c_put_byte(SRF08_COMMAND); 
    i2c_put_byte(0XA5);
    i2c_stop();

    i2c_start_tx(address);         
    i2c_put_byte(SRF08_COMMAND); 
    i2c_put_byte(new_i2c_address); 
    i2c_stop(); 

    /* Make the new i2c address the active one. */
    address=new_i2c_address;                           

return;
}
/*#################################################################################################*/

void srf08_select_unit(unsigned char srf08_address)
{
/* New address validity check */

if((srf08_address<SRF08_UNIT_0 || srf08_address>SRF08_UNIT_15) && srf08_address!=SRF08_BROADCAST_ADDRESS)
 {
    return;
 }
if(srf08_address%2) { return; }

/* Make the new i2c address the active one. */
address=srf08_address;                           

return;
}


/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

