#ifndef __FONT_PORT_H__
#define __FONT_PORT_H__

#if defined AVR
    #include <avr/pgmspace.h>
    
    #define FONT_STORAGE_TYPE        unsigned char
    #define FONT_ATTRIBUTE_TYPE        PROGMEM

#define Font_ReadByte(x, y) \
    pgm_read_byte( (char*)(&y[x]) )

#else
    #define FONT_STORAGE_TYPE        unsigned char
    #define FONT_ATTRIBUTE_TYPE        

    #define Font_ReadByte(x, y) (y[x])
    
#endif


#endif