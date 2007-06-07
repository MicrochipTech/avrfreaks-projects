/************************************************************************************************
 Title  :   C Test program for the HEX KEYBOARD FUNCTIONS library (test_hex_keyb.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      15/2/2003 7:28:14 μμ
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

************************************************************************************************/





#ifndef AVRGCC_VERSION
#define AVRGCC_VERSION    330        /* AVRGCC version for including the correct files  */
#endif

#if AVRGCC_VERSION == 330

#include <avr/io.h>
#include <avr/eeprom.h>

#else

#include <io.h>
#include <eeprom.h>

#endif
/* THE LCD DRIVER IS INCLUDED AUTOMATICALLY WHEN INCLUDING "hex_keyb.h" */
#include "hkb.h"









/********************************************************************************************************/
/*                                   GLOBAL VARIABLES                                                   */
/********************************************************************************************************/
volatile unsigned char int_mode=0;



/********************************************************************************************************/
/*                                   LOCAL FUNCTION PROTOTYPES                                          */
/********************************************************************************************************/
#if HEX_KEYB_WILL_USE_INTERRUPT == 1
void isr_execute_this_function(unsigned char key_pressed);



/********************************************************************************************************/
/*                                   LOCAL FUNCTION DEFINITIONS                                         */
/********************************************************************************************************/
/*
    Below is the function that is called when the keyboard ISR is executed.
    It is not needed to write the actual ISR. Also note that Interrupts are enabled 
    inside the keyboard ISR and inside this function unless you forbid them by disabling
    the global interrupts bit.
    All keyboard functions can be used as the keyboard interrupt and only the keyboard interrupt
    is disabled by the keyboard ISR.
    After returning from this function the keyboard ISR  waits for all keys to be released
    even if you have selected another keyboard (in I2C mode), reenables the keyboard interrupt
    and then returns.
    If you have configured this driver for multiple keyboards the keyboard that generated the
    interrupt is automatically selected by polling to see which keyboard has a key pressed.
    Of course you can select another keyboard if you wish. The ISR will handle this.  
*/
void isr_execute_this_function(unsigned char key_pressed)
{

/* If "ESCAPE" has been pressed signal that you want to proceed with the polled mode tests. */
if(key_pressed==ESCAPE) { int_mode=0; return; } 

LCD_GOTOXY(0,0);  LCD_PUTS_P( "   INTERRUPT MODE" );
LCD_GOTOXY(0,1);  LCD_PUTS_P( "KEY PRESSED = " ); LCD_PUTC(key_pressed);
#if NUMBER_OF_HEX_KEYBOARDS >= 2 && HEX_KEYB_WILL_USE_I2C == 1
LCD_GOTOXY(0,2); LCD_PUTS_P( "KEYBOARD ID = " ); LCD_PUTI(hkb_get_id(),0);
#endif
LCD_GOTOXY(0,3);  LCD_PUTS_P( "hit ESCAPE to exit" );


return;
}
#endif

/********************************************************************************************************/
/*                                   MAIN FUNCTION                                                      */
/********************************************************************************************************/

void main(void)
{


#if HKEYB_SCAN_FUNCTION_ONLY == 0 

static struct {
              	 char battery_name[11];
                 unsigned char lib_battery_voltage;
                 unsigned char lib_battery_capacity;
                 unsigned char lib_charging_time;

              } battery_lib[9] __attribute__((section(".eeprom")));    

unsigned char key=0, c=0, x=0, test=0, key_buffer[11];
unsigned int  number_buf=0, number=0;



while(1)
    {

#if HEX_KEYB_WILL_USE_INTERRUPT == 1
/* 
    Initialize the keyboard port and pins and turn on or off (as needed) the keyboard assigned
    external interrupt.
    The first argument passed to the "hex_keyb_init(isr_execute_this_function,1)" function
    is a pointer to a "void function_name(unsigned char key_code)"
    function type and the second is turning the keyboard interrupt on or off.
    1=ON, 0=OFF. 
*/
              LCD_CLRSCR();
              /* A flag in order to break from the while loop used below. */
              int_mode=1;      
              hkb_init(isr_execute_this_function,1); 
              LCD_GOTOXY(0,0);  LCD_PUTS_P( "Press a key " );
              /* while the "int_mode" flag is 1 wait here. */
              while(int_mode);
              /* Since you pressed "ESCAPE" turn off the keyboard interrupt and continue the tests. */
              hkb_init(isr_execute_this_function,0); 
#endif

#if NUMBER_OF_HEX_KEYBOARDS >= 2 && HEX_KEYB_WILL_USE_I2C == 1
/* 
   hkb(get_id() is a macro (hkb_select(0xFF). Any keyboard id that does not exists will do the same job.
   The hkb_get_id() function does not change anything. HKB_0 is the selected keyboard by default. 
*/
              x=hkb_get_id();         /* The default selected keyboard is HKB_0 so x==0. */
              x=hkb_select(HKB_1);    /* x now holds the selected keyboard id (x==1).    */
              x=hkb_select(HKB_0);    /* x now holds the selected keyboard id (x==0).    */

#endif  





      while(1)
          {   

            LCD_CLRSCR();
            LCD_GOTOXY(0,0);  LCD_PUTS_P( "Press a key         " );
            key=hkb_getc();
            LCD_GOTOXY(0,1);  LCD_PUTS_P( "You pressed: " );
            LCD_GOTOXY(15,1); LCD_PUTC(key);
            LCD_GOTOXY(0,2);  LCD_PUTS_P( "Hit ESCAPE to exit\nor ENTER to repeat" );
            while(1)
                {
                   key=hkb_getc();  if(key==ENTER) break; else if(key==ESCAPE) goto TEST2;
                }  
            
          }          
      
TEST2:
        while(1)
           {
              key=0;
              LCD_CLRSCR();  LCD_GOTOXY(0,0); LCD_PUTS_P("Enter a number: ");
              test=hkb_geti(&number_buf,3,0,1);
              LCD_GOTOXY(0,2); LCD_PUTS_P("You typed: "); 
              /* if the user did not pressed ESCAPE */
              if(test) number=number_buf; LCD_PUTI(number,0);
              LCD_PUTS_P("\nPress any key!");
              key=hkb_getc();
              key=0;
              while(1)
                  {
                    LCD_CLRSCR();  LCD_GOTOXY(0,0); LCD_PUTS_P("Enter the password!");
                    hkb_geti(&number_buf,0,0,1);
                    if(number_buf==1234)
                     {
                       LCD_GOTOXY(0,2); LCD_PUTS_P("CORRECT!\nPress any key!");
                       key=hkb_getc(); break;
                     }
                    else
                       {
                         LCD_GOTOXY(0,2); LCD_PUTS_P("WRONG TRY AGAIN!\nPress any key!");
                         key=hkb_getc();
                       }
                  }
              key=0;
              LCD_CLRSCR(); 
              LCD_PUTS_P("Hit ENTER to repeat\nor hit ESCAPE to \nproceed to the \nnext TEST"); 
              while(1)
                  {
                    key=hkb_getc();  if(key==ENTER) break; else if(key==ESCAPE) goto TEST3;
                  }
           }
      
TEST3:   
      LCD_CLRSCR(); LCD_GOTOXY(0,0); LCD_PUTS_P( "Type something: " );
      hkb_gets(key_buffer,8,0,1);
      LCD_CLRSCR();
      LCD_GOTOXY(0,0); LCD_PUTS_P("You typed: "); LCD_PUTS(key_buffer);
 /* write buffer to eeprom structure and display it to lcd */     
     x=0; key=0;
     while( (c=key_buffer[x]) ) 
          {
            eeprom_wb((unsigned int)&battery_lib[0].battery_name[x],c);
            x++;
          }
      eeprom_wb((unsigned int)&battery_lib[0].battery_name[x],'\0');  
     
      LCD_GOTOXY(0,1); LCD_PUTS_P("Eeprom:    "); LCD_PUTS_E((char*)&battery_lib[0].battery_name);
      LCD_GOTOXY(0,3); LCD_PUTS_P("Press any key!");
      key=hkb_getc();
      LCD_CLRSCR();
      LCD_GOTOXY(0,0);
              LCD_PUTS_P("Hit ENTER to repeat\nthe TESTS one more\ntime !"); 
              while(1)
                  {
                    key=hkb_getc();  if(key==ENTER) break; 
                  }
      /* wait until all keys are released */
      while(hkb_scan());
    }

#endif /* #if HKEYB_SCAN_FUNCTION_ONLY == 0 */

}
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

