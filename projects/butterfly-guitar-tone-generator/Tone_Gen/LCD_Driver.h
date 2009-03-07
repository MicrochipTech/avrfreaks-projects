/* 
               BUTTLCD -- Butterfly LCD Driver 

               Copyright (C) Dean Camera, 2008 

            dean [at] fourwalledcubicle [dot] com 
                  www.fourwalledcubicle.com 
*/ 

#ifndef LCDDRIVER_H 
#define LCDDRIVER_H 

   // INCLUDES: 
   #include <avr/io.h> 
   #include <avr/pgmspace.h> 
   #include <avr/interrupt.h> 
   #include <stdbool.h> 
    
   // EXTERNAL VARIABLES: 
   extern volatile uint8_t ScrollFlags; 
    
   // DEFINES: 
   #define LCD_LCDREGS_START          ((uint8_t*)&LCDDR0) 
   #define LCD_SPACE_OR_INVALID_CHAR  0xFF 
    
   #define LCD_CONTRAST_LEVEL(level)  do{ LCDCCR = (0x0F & level); }while(0) 
   #define LCD_WAIT_FOR_SCROLL_DONE() do{ while (!(ScrollFlags & LCD_FLAG_SCROLL_DONE)) {} }while(0) 
    
   #define LCD_SCROLLCOUNT_DEFAULT    6 
   #define LCD_DELAYCOUNT_DEFAULT     20 
   #define LCD_TEXTBUFFER_SIZE        20 
   #define LCD_SEGBUFFER_SIZE         19 
   #define LCD_DISPLAY_SIZE           6 

   #define LCD_FLAG_SCROLL            (1 << 0) 
   #define LCD_FLAG_SCROLL_DONE       (1 << 1)    

   // PROTOTYPES: 
   void LCD_puts_f(const char *FlashData); 
   void LCD_puts(const char *Data); 
   void LCD_Init(void); 
   void LCD_ShowColons(const uint8_t ColonsOn); 
    
   #if defined(INC_FROM_DRIVER) 
     static inline void LCD_WriteChar(const uint8_t Byte, const uint8_t Digit); 
   #endif 

#endif 
