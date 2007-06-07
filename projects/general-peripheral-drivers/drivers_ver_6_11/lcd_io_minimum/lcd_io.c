
/****************************************************************************
 Title  :   C  file for the HD44780U LCD library (lcd_io.c)
 Author:    Chris efstathiou hendrix@vivodinet.gr
 Date:      6/Jun/2007
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 
 
         
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
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#else

#include <io.h>
#include <progmem.h>
#include <eeprom.h>

#endif

#include "lcd_io.h"

#ifndef CONCAT1
#define CONCAT1(a, b) CONCAT2(a, b)
#endif

#ifndef CONCAT2
#define CONCAT2(a, b) a ## b
#endif

/* Register name forming */
#define LCD_OUT_REG          CONCAT1(PORT, LCD_PORT)

#define LCD_DDR_REG          CONCAT1(DDR, LCD_PORT)

#define INIT_MODE   2
#define DATA_MODE   1
#define CMD_MODE    0


/* Calculation of the LCD_E_PULSE duration in cpu cycles. sbi() and cbi() are taken in to account. */

#define LCD_E_PULSE_CYCLES          ( (LCD_E_PULSE_WIDTH_US*1000L)/(1000000000/F_CPU) )


#if   LCD_E_PULSE_CYCLES <= 0
#warning " LCD_E_PULSE_WIDTH_US in lcd_io.h too small!!"
#define LCD_E_DELAY()             { }

#elif LCD_E_PULSE_CYCLES == 1
#define LCD_E_DELAY()             __asm__ volatile("nop")

#elif LCD_E_PULSE_CYCLES == 2 
#define LCD_E_DELAY()             __asm__ volatile("nop \n\t nop \n\t" ::)

#elif LCD_E_PULSE_CYCLES == 3
#define LCD_E_DELAY()             __asm__ volatile("nop \n\t nop \n\t nop \n\t" ::)

#elif LCD_E_PULSE_CYCLES >= 4


#define LCD_E_DELAY_LOOPS          (LCD_E_PULSE_CYCLES/4)

#define  LCD_E_DELAY()            { delay_int(LCD_E_DELAY_LOOPS); }


#endif 

                                 

/* CONVERSION OF LCD_DELAY_TIME_US TO TRUE MICROSECONDS */

#define LCD_DELAY_LOOPS    ( ((LCD_DELAY_TIME_US*1000L)/(1000000000/F_CPU))/4 )
#if LCD_DELAY_LOOPS <=0   
#undef  LCD_DELAY_LOOPS
#define LCD_DELAY_LOOPS    1
#warning " LCD_DELAY_TIME_US in lcd_io.h too small!!"
#elif LCD_DELAY_LOOPS > 65535
#undef  LCD_DELAY_LOOPS
#define LCD_DELAY_LOOPS    65535
#warning " LCD_DELAY_TIME_US in lcd_io.h too long!!"
#endif

/* LCD_DELAY_TIME_US (about 100 microseconds) */
#define LCD_FUNCTION_DELAY_TYP()  { delay_int(LCD_DELAY_LOOPS);  }  

/* LCD_DELAY_TIME_US * 40 (about 4 ms) */
#define LCD_FUNCTION_DELAY_MAX()  { unsigned int delay_multiplier = 40;  \
                                    while(delay_multiplier) { LCD_FUNCTION_DELAY_TYP(); delay_multiplier--; } \
                                  }      

/* LCD_DELAY_TIME_US * 500 (about 50 ms) */
#define LCD_POWER_ON_DELAY()  { unsigned int delay_multiplier = 500;  \
                                while(delay_multiplier) { LCD_FUNCTION_DELAY_TYP(); delay_multiplier--; } \
                              }    
/* LCD_DELAY_TIME_US * 100 (about 10 ms) */
#define LCD_INIT_DELAY()      { unsigned int delay_multiplier = 100;  \
                                while(delay_multiplier) { LCD_FUNCTION_DELAY_TYP(); delay_multiplier--; } \
                              }    


#define lcd_toggle_e()   { LCD_OUT_REG |= (1<<LCD_E_PIN); LCD_E_DELAY(); \
                           LCD_OUT_REG &= (~(1<<LCD_E_PIN)); \
                         }


/* GLOBAL variables  */
static unsigned char x,y;

/********************************************************************************************************/
/*                                   LOCAL FUNCTION PROTOTYPES                                          */
/********************************************************************************************************/
static inline void delay_int(uint16_t __count) __attribute__((always_inline)); 



/********************************************************************************************************/
/*                                   LOCAL FUNCTION DEFINITIONS                                         */
/********************************************************************************************************/
void  delay_int(uint16_t __count)
{
	__asm__ volatile (
		"1: sbiw %0,1" "\n\t"
		"brne 1b"
		: "=w" (__count)
		: "0" (__count)
	);
} 


/********************************************************************************************************/
/*                                   PUBLIC FUNCTION DEFINITIONS                                        */
/********************************************************************************************************/

static void lcd_write(unsigned char data, unsigned char mode) 
{
                
register unsigned char data_buf = data;

LCD_DDR_REG |= (1<<LCD_RS_PIN) | (1<<LCD_E_PIN) | (0x0F);

       /* output high nibble first */ 
       data_buf = (data_buf>>4);
       LCD_OUT_REG = data_buf;
       if(mode == DATA_MODE){ LCD_OUT_REG |= (1<<LCD_RS_PIN); }
       lcd_toggle_e();

       /* Output low nibble now if we are not in the lcd initialization phase. */ 
       /* During initialization phase the lcd is in 8 bit mode but only the 4 higher data bits are connected, */ 
       /* thus we can use only the 4 higher data bits of the 8 bit lcd data input */ 
       if(mode!=INIT_MODE)
        {
           data &= 0x0F;
           LCD_OUT_REG &= 0xF0;
           LCD_OUT_REG |= data;
           lcd_toggle_e();
        }


LCD_FUNCTION_DELAY_TYP();      
} 

/*------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS */
/*------------------------------------------------------------------------------------------------------*/
void lcd_command(unsigned char cmd)
/* send command <cmd> to LCD */
{
    lcd_write(cmd,CMD_MODE);
    
}

/*------------------------------------------------------------------------------------------------------*/
void lcd_gotoxy(unsigned char lcd_x, unsigned char lcd_y)
/* goto position (x,y) */
{
    x=lcd_x;
    y=lcd_y;
#if LCD_LINES==1
    lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x); 
#endif
#if LCD_LINES==2
    if ( y==0 ) { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);  }
    else { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+x);  }
#endif
#if LCD_LINES==4
    if ( y==0 )     { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);  }
    else if ( y==1) { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+x);  }
    else if ( y==2) { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE3+x);  }
    else            { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE4+x);  }
#endif

}/* lcd_gotoxy */

/*------------------------------------------------------------------------------------------------------*/
unsigned int lcd_getxy(void)
{
  return((y*100)+x); 
}

/*------------------------------------------------------------------------------------------------------*/
/* clear lcd and set cursor to home position */
void lcd_clrscr(void)
{

   lcd_command(1<<LCD_CLR);
   /* this command needs more waiting time to execute */
   LCD_FUNCTION_DELAY_MAX();  
   x=0; y=0;
}

/*------------------------------------------------------------------------------------------------------*/
void lcd_clrline(unsigned char line)
/* clear a specific lcd line and set cursor at the start of the line */
{

#if LCD_LINES == 1
  lcd_gotoxy(0,0);
  while(x<LCD_CHARS_PER_LINE) { lcd_putc(' '); }
  lcd_gotoxy(0,0);
#elif (LCD_LINES == 2 || LCD_LINES == 4)  
  if(line<LCD_LINES)
   {
      lcd_gotoxy(0,line);
      while(x<LCD_CHARS_PER_LINE) { lcd_putc(' '); }
      lcd_gotoxy(0,line);
   }   
#endif  /*  #if LCD_LINES == 1 -> #elif (LCD_LINES == 2 || LCD_LINES == 4) */
}

/*------------------------------------------------------------------------------------------------------*/
/* set cursor to home position */
void lcd_home(void)
{

    lcd_command(1<<LCD_HOME);
    /* this command needs more waiting time to execute */
    LCD_FUNCTION_DELAY_MAX();  
    x=0; y=0;
}

/*------------------------------------------------------------------------------------------------------*/
/* print character at current cursor position */
void lcd_putc(unsigned char c)
{
    if(c=='\n') { if(y!=LCD_LINES-1) lcd_gotoxy(0,(y+1)); }
    else if(x<LCD_CHARS_PER_LINE) { lcd_write(c, DATA_MODE); x++; }  
  
}

/*------------------------------------------------------------------------------------------------------*/
/* print string on lcd (no auto linefeed) */
void lcd_puts(const unsigned char *s)
{
    register unsigned char c;

    while ( (c = *s++) ) { lcd_putc(c);  }

}

/*------------------------------------------------------------------------------------------------------*/
#if LCD_PUTS_P_NEEDED == 1

/* print string from program memory on lcd (no auto linefeed) */
void lcd_puts_p(const unsigned char *progmem_s)
{
    register unsigned char c;

    while ( (c = pgm_read_byte(progmem_s++)) ) { lcd_putc(c);  }

}
#endif
/*------------------------------------------------------------------------------------------------------*/
#if LCD_PUTS_E_NEEDED == 1

/* print string from eeprom on lcd (no auto linefeed) */
void lcd_puts_e(unsigned char *eeprom_s)
{
 register unsigned char c;
   
    while( (c=eeprom_read_byte(eeprom_s++)) && c != 0xFF ) { lcd_putc(c);  }
    /*{ if(c==0xFF) break; lcd_putc(c);  }*/               
}
#endif
/*------------------------------------------------------------------------------------------------------*/

#if LCD_PUT_I_NEEDED == 1

/* print signed integer on lcd with or without comma (no auto linefeed) */
void lcd_put_i(int value, unsigned char dot_position, unsigned char number_of_chars)
{
unsigned char lcd_data[9]={' ',' ',' ',' ',' ',' ',' ',' ',' ' }; 
unsigned char negative=0, position=sizeof(lcd_data), radix=10; 
        

/* Some adjustments if the number is negative */
if(value<0) { value=-value; *(lcd_data)='-'; negative=1;  } 

/*
   Convert 16 bit positive now made number to ascii algorithm. Position starts at array position 8
   so in the extreme case of lcd_put_i(-32767,5,8); the display should show -0,32767 (8 chars) which
   means that the position variable will count down to 2.
   Since the sign is always in array position 0 there is always an empty slot in position 1 at least.
   This empty slot is essential for left allignment.
   Also note that position 8 gets the least significant digit.
   I have tried varius code schemes but this one yields the smaller code. 
*/
do{
     if(dot_position)
      {
         if((sizeof(lcd_data)-position)>=dot_position)
          {
             position--;
             *(lcd_data+position)=',';
             dot_position=0;
          }
      } 
     position--;
     *(lcd_data+position)=(value%radix)+'0';
     value/=radix;

  }while((value || dot_position) && dot_position <= 5); 

/* Move the array contents to the start of the array filling the rest with spaces (Left align data) */
/* radix has no meaning from here on, it is just used as a counter. */
for(radix=negative; position<sizeof(lcd_data); radix++,position++)
  {
     *(lcd_data+radix)=*(lcd_data+position);
     *(lcd_data+position)=' ';
  }
          
/* Adjust the number of reserved lcd char positions */
if(number_of_chars<sizeof(lcd_data)) { radix=number_of_chars; }  

/* Display the number on the LCD screen */
for(position=0; position<radix; position++)
  {
     lcd_putc(lcd_data[position]);
  }

return;
}
#endif   /* #if LCD_PUT_I_NEEDED == 1 */

/*------------------------------------------------------------------------------------------------------*/

void lcd_init(void)
/* initialize display and select type of cursor */
/* dispAttr: LCD_DISP_OFF, LCD_DISP_ON, LCD_DISP_ON_CURSOR, LCD_DISP_CURSOR_BLINK */
{

#if LCD_LINES==1
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_1LINE 
#else
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_2LINES 
#endif 

    /*------ Initialize lcd to 4 bit i/o mode -------*/
    /* LCD power on initilization delay ~50 ms */
    LCD_POWER_ON_DELAY();
    
    /* initial write to lcd is 8bit using delay since busy flag can't be checked here anyway */
    lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);

    /* this command needs more waiting time to execute ( about 10 ms) */
    LCD_INIT_DELAY();      
    
    lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);
    
    lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);
    
    lcd_write(LCD_FUNCTION_4BIT_1LINE, INIT_MODE);  /* set IO mode to 4bit */
             
    
    /* from now on the lcd accepts only 4 bit I/O, so we can use lcd_command() */    

    lcd_command(LCD_FUNCTION_DEFAULT);      /* function set: display lines  */
    lcd_command(LCD_DISP_OFF);              /* display off                  */
    lcd_clrscr();                           /* display clear                */ 
    lcd_command(LCD_MODE_DEFAULT);          /* set entry mode               */
    lcd_command(LCD_DISP_ON);               /* display/cursor control       */

}/* lcd_init */

