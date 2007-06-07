
/*********************************************************************************************************
 Title  :   Header file for the AVR library (compatibility.h)
 Author:    Chris efstathiou hendrix@vivodinet.gr
 Date:      04/Apr/2007
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  any AVR MCU device
 Comments:  This software is FREE.

*********************************************************************************************************/

/*
   COMMENTS
*/
 

#ifndef COMPATIBILITY_H
#define COMPATIBILITY_H

/*######################################################################################################*/
/*                              START OF CONFIGURATION BLOCK                                            */
/*######################################################################################################*/

#ifndef F_CPU
#define F_CPU                  3686400        /* CPU CLOCK FREQUENCY         */
#endif


/*######################################################################################################*/
/*                             END OF CONFIGURATION BLOCK                                               */
/*######################################################################################################*/ 

/********************************************************************************************************/
/*                             PREPROCESSOR DIRECTIVES                                                  */
/********************************************************************************************************/







/********************************************************************************************************/
/*                             PUBLIC FUNCTION PROTOTYPES                                               */
/********************************************************************************************************/







/********************************************************************************************************/
/*                                 MACRO DEFINITIONS                                                    */
/********************************************************************************************************/

#ifndef _SFR_ASM_COMPAT  

#ifndef sbi 
 #define sbi(port, bit)      ( port |= (1<<bit) )  
#endif

#ifndef cbi
 #define cbi(port, bit)      ( port &= (~(1<<bit)) ) 
#endif

#ifndef inb
 #define inb(port)           (port)
#endif 

#ifndef inp
 #define inp(port)           (port)
#endif 

#ifndef outb
 #define outb(port,value)    ( port = value )  
#endif

#if !defined(outp)
 #define outp(value,port)    ( port = value ) 
#endif
 
#ifndef PRG_RDB
 #define PRG_RDB(byte)       pgm_read_byte((unsigned int*) byte)
#endif

#ifndef eeprom_rb
 #define eeprom_rb(byte)     eeprom_read_byte((unsigned char*) byte)    
#endif

#ifndef eeprom_wb
 #define eeprom_wb(address, data)     eeprom_write_byte( (unsigned char*)address, data)    
#endif

#else  /* #if  _SFR_ASM_COMPAT == 1  */

#ifndef sbi 
 #define sbi(port, bit)      ( *((unsigned char*) (port)) |= (1<<bit) )  
#endif

#ifndef cbi
 #define cbi(port, bit)      ( *((unsigned char*) (port)) &= (~(1<<bit)) ) 
#endif

#ifndef inb
 #define inb(port)           ( *((unsigned char*) (port)) )
#endif 

#ifndef inp
 #define inp(port)           ( *((unsigned char*) (port)) )
#endif 

#ifndef outb
 #define outb(port,value)    ( *((unsigned char*) (port)) = value )  
#endif

#if !defined(outp)
 #define outp(value,port)    ( *((unsigned char*) (port)) = value )
#endif
 
#ifndef PRG_RDB
 #define PRG_RDB(byte)       pgm_read_byte((unsigned int*) byte)
#endif

#ifndef eeprom_rb
 #define eeprom_rb(byte)     eeprom_read_byte((unsigned char*) byte)    
#endif

#ifndef eeprom_wb
 #define eeprom_wb(address, data)     eeprom_write_byte( (unsigned char*)address, data)    
#endif


#endif  /* #if  _SFR_ASM_COMPAT == 1  */






#endif  /* H_FILE_H    */

/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

