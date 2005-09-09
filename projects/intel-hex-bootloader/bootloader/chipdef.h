
#ifndef CHIPDEF_H
#define CHIPDEF_H

#include <avr/io.h>

// TODO: make use of RAMEND in the avr-libc io-files and
// avoid a lot of by-device definitions here

#if defined(__AVR_ATmega169__)
  #define sig_byte3 0x1E
  #define sig_byte2 0x94
  #define sig_byte1 0x05
  
  #define devtype 0x79       // Mega 169 device code
  
//  #define PAGESIZE 128  // 2*64 Words = Size in Bytes
  
  #ifdef _B128
    #define APP_PAGES ((2*8192 / SPM_PAGESIZE)- (2*128 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
  #ifdef _B256
    #define APP_PAGES ((2*8192 / SPM_PAGESIZE)- (2*256 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
  #ifdef _B512
    #define APP_PAGES ((2*8192 / SPM_PAGESIZE)- (2*512 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
  #ifdef _B1024
    #define APP_PAGES ((2*8192 / SPM_PAGESIZE)- (2*1024 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
  #ifdef _B2048
    #error "_B2048 not suppoted on this device"
  #endif   

#elif defined(__AVR_ATmega16__)

  #define sig_byte3 0x1E
  #define sig_byte2 0x94
  #define sig_byte1 0x03
  
  #define devtype 0x75       // Mega16 device code
  
//  #define PAGESIZE 128       // Size in Bytes
  
  #ifdef _B128
    #define APP_PAGES ((2*8192 / SPM_PAGESIZE)- (2*128 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
  #ifdef _B256
    #define APP_PAGES ((2*8192 / SPM_PAGESIZE)- (2*256 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
  #ifdef _B512
    #define APP_PAGES ((2*8192 / SPM_PAGESIZE)- (2*512 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
   #ifdef _B1024
    #define APP_PAGES ((2*8192 / SPM_PAGESIZE)- (2*1024 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif  
  #ifdef _B2048
    #error "_B2048 not suppoted on this device"
  #endif   

#elif defined(__AVR_ATmega8__)

  #define sig_byte3 0x1E
  #define sig_byte2 0x93
  #define sig_byte1 0x07
  
  #define devtype 0x77       // Mega8 boot device code
  
//  #define PAGESIZE 64        // Size in Bytes
  
  #ifdef _B128
    #define APP_PAGES ((2*4096 / SPM_PAGESIZE)- (2*128 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
  #ifdef _B256
    #define APP_PAGES ((2*4096 / SPM_PAGESIZE)- (2*256 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
  #ifdef _B512
    #define APP_PAGES ((2*4096 / SPM_PAGESIZE)- (2*512 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
   #ifdef _B1024
    #define APP_PAGES ((2*4096 / SPM_PAGESIZE)- (2*1024 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif  
  #ifdef _B2048
    #error "_B2048 not suppoted on this device"
  #endif   
#elif defined(__AVR_ATmega162__)

  #define sig_byte3 0x1E
  #define sig_byte2 0x94
  #define sig_byte1 0x04
  
// #define devtype 0x??       // Mega162 boot device code
  
//  #define PAGESIZE 128        // Size in Bytes
  
  #ifdef _B128
    #define APP_PAGES ((2*8192 / SPM_PAGESIZE)- (2*128 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
  #ifdef _B256
    #define APP_PAGES ((2*8192 / SPM_PAGESIZE)- (2*256 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
  #ifdef _B512
    #define APP_PAGES ((2*8192 / SPM_PAGESIZE)- (2*512 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
   #ifdef _B1024
    #define APP_PAGES ((2*8192 / SPM_PAGESIZE)- (2*1024 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif  
  #ifdef _B2048
    #error "_B2048 not suppoted on this device"
  #endif   

#elif defined(__AVR_ATmega32__)

  #define sig_byte3 0x1E
  #define sig_byte2 0x95
  #define sig_byte1 0x02
  
  #define devtype 0x73       // Mega32 device code
  
//  #define PAGESIZE 128       // Size in Bytes
  
  #ifdef _B128
    #define APP_PAGES ((2*16384UL / SPM_PAGESIZE)- (2*128 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
  #ifdef _B256
    #define APP_PAGES ((2*16384UL / SPM_PAGESIZE)- (2*256 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
  #ifdef _B512
    #define APP_PAGES ((2*16384UL / SPM_PAGESIZE)- (2*512 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
   #ifdef _B1024
    #define APP_PAGES ((2*16384UL / SPM_PAGESIZE)- (2*1024 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif  
  #ifdef _B2048
    #define APP_PAGES ((2*16384UL / SPM_PAGESIZE)- (2*2048 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif  

#elif defined(__AVR_ATmega128__)

  #define sig_byte3 0x1E
  #define sig_byte2 0x97
  #define sig_byte1 0x02
  
  #define devtype 0x44      // Mega128 device code
  
//  #define PAGESIZE 128       // Size in Bytes
  
  #ifdef _B512
    #define APP_PAGES ((2*65536UL / SPM_PAGESIZE)- (2*512 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
  #ifdef _B1024
    #define APP_PAGES ((2*65536UL / SPM_PAGESIZE)- (2*1024 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif  
  #ifdef _B2048
    #define APP_PAGES ((2*65536UL / SPM_PAGESIZE)- (2*2048 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif
  #ifdef _B4096
    #define APP_PAGES ((2*65536UL / SPM_PAGESIZE)- (2*4096 / SPM_PAGESIZE )) 
    #define APP_END APP_PAGES * SPM_PAGESIZE 
  #endif  

#else
	#error "no definition for MCU available in chipdef.h"
#endif

#endif
