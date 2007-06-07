
/****************************************************************************
 Title  :   C  file for the HD44780U LCD library (lcd_io.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      2/2/2003 6:35:54 μμ
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.
 
*****************************************************************************/
#ifndef _IO_REG_MACRO_MODE_
#define _IO_REG_MACRO_MODE_  1     /* In case you have the new assignment mode io headers */
#endif

#ifndef  _SFR_ASM_COMPAT
#define  _SFR_ASM_COMPAT     1     /* This is for GCC 3.2 */
#endif

#ifndef AVRGCC_VERSION
#define AVRGCC_VERSION                330        /* AVRGCC version for including the correct files  */
#endif

#if AVRGCC_VERSION == 330

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#else

#include <io.h>
#include <eeprom.h>
#include <pgmspace.h>

#endif /* #if AVRGCC_VERSION == 330 */

#include "lcd_io.h"
#if LCD_IO_MODE == 3
#include "i2c.h"
#endif




#if !defined(outp)
#define outp(value,port)  outb(port,value)
#endif

#if !defined(inp)
#define inp(port)  inb(port)
#endif

/*******************************************************************************************************/
#if defined(__AVR_ATmega103__)

#undef DDRC 
#undef DDRF 
#undef PINC 
#undef PORTF

#if SREG > 0X3F

#define DDRC   0X20
#define DDRF   0X20
#define PINC   0X20
#define PORTF  0X20

#elif SREG <= 0X3F 

#define DDRC   0
#define DDRF   0
#define PINC   0
#define PORTF  0

#endif  /* #if SREG > 0X3F */ 

#endif  /* #if defined(__AVR_ATmega103__) */
/*******************************************************************************************************/


/* constants/macros */
#ifndef CONCAT1
#define CONCAT1(a, b) CONCAT2(a, b)
#endif

#ifndef CONCAT2
#define CONCAT2(a, b) a ## b
#endif


/* Some clever (i hope!) thinking triks */
#if LCD_IO_MODE == 7 && NUMBER_OF_LCD_UNITS == 1

#warning "MODE 7 USING 1 LCD UNIT. WILL REVERT TO MODE 6"
#undef   LCD_IO_MODE
#define  LCD_IO_MODE            6
#define  LCD_READ_REQUIRED      0
#define  LCD_E_SIGNAL_PORT      LCD_0_E_SIGNAL_PORT
#define  LCD_E_PIN              LCD_0_E_PIN 
#define  LCD_CHARS_PER_LINE     LCD_0_CHARS_PER_LINE
#define  LCD_LINES              LCD_0_LINES 
#undef   LCD_MULTIPLEX_ENABLE
#define  LCD_MULTIPLEX_ENABLE   0
#undef   LCD_READ_REQUIRED
#define  LCD_READ_REQUIRED      0
#undef   LCD_1_CHARS_PER_LINE     
#undef   LCD_1_LINES              
#undef   LCD_1_E_SIGNAL_PORT      
#undef   LCD_1_E_PIN              

           
#elif   LCD_IO_MODE == 7 && NUMBER_OF_LCD_UNITS > 1

#if NUMBER_OF_LCD_UNITS > 3
#error "UP TO 3 LCD UNITS ARE ALLOWED!"
#endif


#endif /* #if LCD_IO_MODE == 7 && NUMBER_OF_LCD_UNITS == 1 */

#if LCD_SAVE_MORE_CODE_SPACE == 1
#warning "LCD IN ABSOLUTELY MINIMUM CONFIGURATION"
#endif

#if LCD_READ_REQUIRED == 1 && LCD_SAVE_MORE_CODE_SPACE == 1
#undef LCD_BACKUP_REQUIRED
#define LCD_BACKUP_REQUIRED   0
#warning "NO BACKUP FUNCTIONS WHILE MINIMUM CODE SELECTED"
#endif 

#if LCD_MULTIPLEX_ENABLE == 1 && LCD_READ_REQUIRED == 1
#undef  LCD_MULTIPLEX_ENABLE 
#define LCD_MULTIPLEX_ENABLE   0
#warning "NO MULTIPLEXING WHILE LCD READING WANTED"
#endif

/* Calculation of the LCD_E_PULSE duration in cpu cycles. sbi() and cbi() are taken in to account. */

#define LCD_E_PULSE_CYCLES_ACCURATE  ( (LCD_E_PULSE_WIDTH_US*1000L)/(1000000000/(F_CPU*10)) )
#define LCD_E_PULSE_CYCLES_REAL      (LCD_E_PULSE_CYCLES_ACCURATE/10)

#if   LCD_E_PULSE_CYCLES_ACCURATE - (LCD_E_PULSE_CYCLES_REAL*10) >=5
#define LCD_E_PULSE_CYCLES           LCD_E_PULSE_CYCLES_REAL
#else 
#define LCD_E_PULSE_CYCLES           (LCD_E_PULSE_CYCLES_REAL-1)
#endif

#if   LCD_E_PULSE_CYCLES <= 0
#define LCD_E_DELAY()             { }

#elif LCD_E_PULSE_CYCLES == 1
#define LCD_E_DELAY()             __asm__ volatile("nop")

#elif LCD_E_PULSE_CYCLES == 2 
#define LCD_E_DELAY()             __asm__ volatile("nop \n\t nop \n\t" ::)

#elif LCD_E_PULSE_CYCLES >= 3


#define DELAY_C_ACCURATE(x)      ({  __asm__ volatile (  "ldi r25,%0 \n\t"           \
                                                         "L_%=:      \n\t"           \
                                                         "dec r25    \n\t"           \
                                                         "brne L_%=  \n\t"           \
                                                         : /* NO OUTPUT */           \
                                                         : "g" ((unsigned char)(x))  \
		                                         : "r25"	             \
           	                                       );                            \
                                  })                          


#define LCD_E_DELAY_LOOPS          (LCD_E_PULSE_CYCLES/3)
  

#if   LCD_E_PULSE_CYCLES-(LCD_E_DELAY_LOOPS*3) == 0

#define  LCD_E_DELAY()            DELAY_C_ACCURATE(LCD_E_DELAY_LOOPS) 

#elif LCD_E_PULSE_CYCLES-(LCD_E_DELAY_LOOPS*3) == 1
 
#define  LCD_E_DELAY()            { DELAY_C_ACCURATE(LCD_E_DELAY_LOOPS); __asm__ volatile("nop"); } 

#elif LCD_E_PULSE_CYCLES-(LCD_E_DELAY_LOOPS*3) == 2
 
#define  LCD_E_DELAY()            { DELAY_C_ACCURATE(LCD_E_DELAY_LOOPS);                         \
                                    __asm__ volatile("nop \n\t nop \n\t" ::);                    \
                                  } 
#elif LCD_E_PULSE_CYCLES-(LCD_E_DELAY_LOOPS*3) == 3
 
#define  LCD_E_DELAY()            { DELAY_C_ACCURATE(LCD_E_DELAY_LOOPS);                         \
                                    __asm__ volatile("nop \n\t nop \n\t nop \n\t" ::);           \
                                  } 
#elif LCD_E_PULSE_CYCLES-(LCD_E_DELAY_LOOPS*3) == 4
 
#define  LCD_E_DELAY()            { DELAY_C_ACCURATE(LCD_E_DELAY_LOOPS);                         \
                                    __asm__ volatile("nop \n\t nop \n\t nop \n\t nop \n\t" ::);  \
                                  } 


#endif 

#endif /* #if   LCD_E_PULSE_CYCLES == 1 */                                 

#define LCD_POWER_ON_DELAY ( (50*(F_CPU/6000))-3 )    /* 50 milliseconds */
#define LCD_INIT_DELAY     ( (10*(F_CPU/6000))-3 )    /* 10 milliseconds */


/* CONVERSION OF LCD_DELAY_TIME_US TO TRUE MICROSECONDS */


#define LCD_SMALL_DELAY    ( ((LCD_DELAY_TIME_US*(F_CPU/60000))/100)-3 )
#define LCD_LONG_DELAY     ( LCD_SMALL_DELAY*40 )





/*######################################################################################################*/
#if LCD_IO_MODE == 2
/*######################################################################################################*/

#define LCD_DATA_OUT_REG      CONCAT1(PORT,LCD_DATA_SIGNAL_PORT)
#define LCD_DATA_DDR_REG      CONCAT1(DDR,LCD_DATA_SIGNAL_PORT)

#define LCD_CLOCK_OUT_REG     CONCAT1(PORT,LCD_CLOCK_SIGNAL_PORT) 
#define LCD_CLOCK_DDR_REG     CONCAT1(DDR,LCD_CLOCK_SIGNAL_PORT) 

#define LCD_READ_REQUIRED     0

#define Toggle_clock()        ({ sbi(LCD_CLOCK_OUT_REG, LCD_CLOCK_PIN); LCD_E_DELAY(); \
                                 cbi(LCD_CLOCK_OUT_REG, LCD_CLOCK_PIN); LCD_E_DELAY(); \
                              }) 

#define DATA(x)               ({ if(x) sbi(LCD_DATA_OUT_REG, LCD_DATA_PIN); \
                                 else cbi(LCD_DATA_OUT_REG, LCD_DATA_PIN);  \
                              })
        
#define Toggle_data()         ({ sbi(LCD_DATA_OUT_REG, LCD_DATA_PIN); LCD_E_DELAY();   \
                                 cbi(LCD_DATA_OUT_REG, LCD_DATA_PIN); LCD_E_DELAY();   \
                              })



#if defined(__AVR_ATmega103__)
#if SREG > 0X3F

#if LCD_DATA_OUT_REG == 0x20 || LCD_CLOCK_OUT_REG == 0x20
#error "SOME OF THE LCD PINS ARE INPUTS ONLY"
#endif

#elif SREG <= 0X3F 
#if LCD_DATA_OUT_REG == 0 || LCD_CLOCK_OUT_REG == 0
#error "SOME OF THE LCD PINS ARE INPUTS ONLY"
#endif

#endif  /* #if SREG > 0X3F */ 

#endif  /* #if defined(__AVR_ATmega103__) */

#define LCD_DONT_READ_BUSY_FLAG  0 
#define LCD_READ_REQUIRED        0
#define NUMBER_OF_LCD_UNITS      1

/*######################################################################################################*/
#elif LCD_IO_MODE == 3
/*######################################################################################################*/



/*######################################################################################################*/
#elif LCD_IO_MODE == 6
/*######################################################################################################*/

#define lcd_toggle_e()  ({ cbi(LCD_E_OUT_REG, LCD_E_PIN); LCD_E_DELAY();  \
                           sbi(LCD_E_OUT_REG, LCD_E_PIN); LCD_E_DELAY();  \
                        })

#if LCD_READ_REQUIRED == 1  
#undef  LCD_MULTIPLEX_REQUIRED
#define LCD_MULTIPLEX_REQUIRED   0
#endif

#if LCD_LINES == 1 || LCD_LINES == 2 || LCD_LINES == 4
#else
#error THE LCD LINES MUST BE 1 OR 2 OR 4 !
#endif



#define LCD_D4_OUT_REG       CONCAT1(PORT, LCD_DATA4_PORT)
#define LCD_D5_OUT_REG       CONCAT1(PORT, LCD_DATA5_PORT)
#define LCD_D6_OUT_REG       CONCAT1(PORT, LCD_DATA6_PORT)
#define LCD_D7_OUT_REG       CONCAT1(PORT, LCD_DATA7_PORT)
#define LCD_RS_OUT_REG       CONCAT1(PORT, LCD_RS_SIGNAL_PORT)
#define LCD_RW_OUT_REG       CONCAT1(PORT, LCD_RW_SIGNAL_PORT)
#define LCD_E_OUT_REG        CONCAT1(PORT, LCD_E_SIGNAL_PORT)

#define LCD_D4_DDR_REG       CONCAT1(DDR, LCD_DATA4_PORT)
#define LCD_D5_DDR_REG       CONCAT1(DDR, LCD_DATA5_PORT)
#define LCD_D6_DDR_REG       CONCAT1(DDR, LCD_DATA6_PORT)
#define LCD_D7_DDR_REG       CONCAT1(DDR, LCD_DATA7_PORT)
#define LCD_RS_DDR_REG       CONCAT1(DDR, LCD_RS_SIGNAL_PORT)
#define LCD_RW_DDR_REG       CONCAT1(DDR, LCD_RW_SIGNAL_PORT)
#define LCD_E_DDR_REG        CONCAT1(DDR, LCD_E_SIGNAL_PORT)



#if   LCD_READ_REQUIRED == 1

#define LCD_D4_PIN_REG        CONCAT1(PIN, LCD_DATA4_PORT)
#define LCD_D5_PIN_REG        CONCAT1(PIN, LCD_DATA5_PORT)
#define LCD_D6_PIN_REG        CONCAT1(PIN, LCD_DATA6_PORT)
#define LCD_D7_PIN_REG        CONCAT1(PIN, LCD_DATA7_PORT)

#elif LCD_READ_REQUIRED == 0

#undef LCD_RW_OUT_REG
#undef LCD_RW_DDR_REG

#endif

#if defined(__AVR_ATmega103__)
#if SREG > 0X3F

#if LCD_D4_OUT_REG == 0x20 || LCD_D5_OUT_REG == 0x20 || LCD_D6_OUT_REG == 0x20 ||  \
    LCD_D7_OUT_REG == 0x20 || LCD_RS_OUT_REG == 0x20 || LCD_E_OUT_REG == 0x20
#error "SOME OF THE LCD PINS ARE INPUTS ONLY"
#endif

#if (LCD_D4_DDR_REG == 0x20 || LCD_D5_DDR_REG == 0x20 || LCD_D6_DDR_REG == 0x20 ||  \
     LCD_D7_DDR_REG == 0x20) && LCD_READ_REQUIRED == 1 
#error "THE LCD DATA PINS ARE NOT BIDIRECTIONAL"
#endif

#elif SREG <= 0X3F 

#if LCD_D4_OUT_REG == 0 || LCD_D5_OUT_REG == 0 || LCD_D6_OUT_REG == 0 || LCD_D7_OUT_REG == 0 || \
    LCD_RS_OUT_REG == 0 || LCD_E_OUT_REG == 0
#error "SOME OF THE LCD PINS ARE INPUTS ONLY"
#endif

#if (LCD_D4_DDR_REG == 0 || LCD_D5_DDR_REG == 0 || LCD_D6_DDR_REG == 0 || LCD_D7_DDR_REG == 0)  \
     &&  LCD_READ_REQUIRED == 1 
#error "THE LCD DATA PINS ARE NOT BIDIRECTIONAL"
#endif

#endif  /* #if SREG > 0X3F */ 

#endif  /* #if defined(__AVR_ATmega103__) */

#define LCD_DONT_READ_BUSY_FLAG  0
#define NUMBER_OF_LCD_UNITS      1

/*######################################################################################################*/
#elif LCD_IO_MODE == 7
/*######################################################################################################*/

#define lcd_toggle_e()   ({ cbivar(lcd_E_port, lcd_E_pin); LCD_E_DELAY(); \
                            sbivar(lcd_E_port, lcd_E_pin); LCD_E_DELAY(); \
                         })

#if SREG > 0X3F
#define IO_TO_MEM_OFFSET  0
#else
#define IO_TO_MEM_OFFSET  0X20
#endif

#define LCD_READ_REQUIRED 0  

#if LCD_0_LINES == 1 || LCD_0_LINES == 2 || LCD_0_LINES == 4
#else
#error THE LCD LINES MUST BE 1 OR 2 OR 4 !
#endif

#if LCD_1_LINES == 1 || LCD_1_LINES == 2 || LCD_1_LINES == 4
#else
#error THE LCD LINES MUST BE 1 OR 2 OR 4 !
#endif

#if NUMBER_OF_LCD_UNITS >= 3
#if LCD_2_LINES == 1 || LCD_2_LINES == 2 || LCD_2_LINES == 4
#else
#error THE LCD LINES MUST BE 1 OR 2 OR 4 !
#endif
#endif


#define LCD_D4_OUT_REG        CONCAT1(PORT, LCD_DATA4_PORT)
#define LCD_D5_OUT_REG        CONCAT1(PORT, LCD_DATA5_PORT)
#define LCD_D6_OUT_REG        CONCAT1(PORT, LCD_DATA6_PORT)
#define LCD_D7_OUT_REG        CONCAT1(PORT, LCD_DATA7_PORT)
#define LCD_RS_OUT_REG        CONCAT1(PORT, LCD_RS_SIGNAL_PORT)
#define LCD_0_E_OUT_REG       CONCAT1(PORT, LCD_0_E_SIGNAL_PORT)
#define LCD_1_E_OUT_REG       CONCAT1(PORT, LCD_1_E_SIGNAL_PORT)

#if NUMBER_OF_LCD_UNITS >=3
#define LCD_2_E_OUT_REG       CONCAT1(PORT, LCD_2_E_SIGNAL_PORT)
#endif

#define LCD_D4_DDR_REG        CONCAT1(DDR, LCD_DATA4_PORT)
#define LCD_D5_DDR_REG        CONCAT1(DDR, LCD_DATA5_PORT)
#define LCD_D6_DDR_REG        CONCAT1(DDR, LCD_DATA6_PORT)
#define LCD_D7_DDR_REG        CONCAT1(DDR, LCD_DATA7_PORT)
#define LCD_RS_DDR_REG        CONCAT1(DDR, LCD_RS_SIGNAL_PORT)
#define LCD_0_E_DDR_REG       CONCAT1(DDR, LCD_0_E_SIGNAL_PORT)
#define LCD_1_E_DDR_REG       CONCAT1(DDR, LCD_1_E_SIGNAL_PORT)

#if NUMBER_OF_LCD_UNITS >=3
#define LCD_2_E_DDR_REG       CONCAT1(DDR, LCD_2_E_SIGNAL_PORT)
#endif

#if defined(__AVR_ATmega103__)
#if SREG > 0X3F

#if LCD_D4_OUT_REG == 0x20 || LCD_D5_OUT_REG == 0x20 || LCD_D6_OUT_REG == 0x20 ||  \
    LCD_D7_OUT_REG == 0x20 || LCD_RS_OUT_REG == 0x20 || LCD_0_E_OUT_REG == 0x20 || \
    LCD_1_E_OUT_REG == 0x20
#error "SOME OF THE LCD PINS ARE INPUTS ONLY"
#endif

#if NUMBER_OF_LCD_UNITS >=3 &&  LCD_2_E_OUT_REG == 0x20
#error "THE LCD LCD_2_E_PIN IS INPUT ONLY"
#endif


#elif SREG <= 0X3F 

#if LCD_D4_OUT_REG == 0 || LCD_D5_OUT_REG == 0 || LCD_D6_OUT_REG == 0 || LCD_D7_OUT_REG == 0 || \
    LCD_RS_OUT_REG == 0 || LCD_0_E_OUT_REG == 0 || LCD_1_E_OUT_REG == 0
#error "SOME OF THE LCD PINS ARE INPUTS ONLY"
#endif

#if NUMBER_OF_LCD_UNITS >=3 &&  LCD_2_E_OUT_REG == 0
#error "THE LCD LCD_2_E_PIN IS INPUT ONLY"
#endif

#endif  /* #if SREG > 0X3F */ 

#endif  /* #if defined(__AVR_ATmega103__) */

#define LCD_DONT_READ_BUSY_FLAG  0
#define LCD_READ_REQUIRED        0

/*######################################################################################################*/
#endif /* elif LCD_IO_MODE == 7 */ 
/*######################################################################################################*/

/* lcd_write() mode argument values */
#define INIT_MODE                2
#define DATA_MODE                1
#define CMD_MODE                 0

#if LCD_READ_REQUIRED == 1  
/* lcd_read() argument values */
#define BUSY_FLAG                0
#define ADDRESS_COUNTER          0
#define DATA                     1

#endif



/*######################################################################################################*/
/*                                        TYPE DEFINITIONS                                              */
/*######################################################################################################*/


typedef unsigned char  u08;
typedef unsigned short u16;


/*######################################################################################################*/
/*                                    LOCAL FUNCTION PROTOTYPES                                         */
/*######################################################################################################*/

static void delay(unsigned long int number_of_loops);
static void lcd_write(unsigned char data, unsigned char mode);

#if   LCD_IO_MODE == 7
static void sbivar(unsigned char port, unsigned char bit);
static void cbivar(unsigned char port, unsigned char bit);
#elif LCD_IO_MODE == 2
static void toggle_clock(void);
static void toggle_data(void);
static void data(unsigned char logic_state);
#endif

#if  LCD_READ_REQUIRED == 1
#if  LCD_DONT_READ_BUSY_FLAG == 0
static void          wait_until_busy_flag_is_clear(void);
#endif
static unsigned char lcd_read(unsigned char rs);
#endif
 

/*######################################################################################################*/
/*                                      GLOBAL VARIABLES                                                */
/*######################################################################################################*/

static unsigned char x,y;
static unsigned char lcd_line_start[4]={ LCD_START_LINE1,LCD_START_LINE2,
                                         LCD_START_LINE3,LCD_START_LINE4
                                       };
#if LCD_SAVE_MORE_CODE_SPACE == 0
static unsigned char putc_lock=0;
static unsigned char lcd_init_lock=0;
#endif

/*********************************************************************************************************/
#if LCD_IO_MODE == 7
/*********************************************************************************************************/

#if LCD_SAVE_MORE_CODE_SPACE == 0 || LCD_CLRLINE_NEEDED == 1 
static unsigned char lcd_chars_per_line=LCD_0_CHARS_PER_LINE;
#endif
static unsigned char lcd_E_port=LCD_0_E_OUT_REG, lcd_E_pin=LCD_0_E_PIN;
static unsigned char current_lcd_unit=0, lcd_lines=LCD_0_LINES;
static struct  {
                  unsigned char x;
                  unsigned char y;
               } lcd_coordinates[NUMBER_OF_LCD_UNITS];

/*********************************************************************************************************/
#elif LCD_IO_MODE == 3
/*********************************************************************************************************/

#if LCD_SAVE_MORE_CODE_SPACE == 0 || LCD_CLRLINE_NEEDED == 1 
static unsigned char lcd_chars_per_line=LCD_0_CHARS_PER_LINE;
#endif

static unsigned char lcd_i2c_address=LCD_0_I2C_ADDRESS;
static unsigned char lcd_lines=LCD_0_LINES;
#if NUMBER_OF_LCD_UNITS >= 2
static unsigned char current_lcd_unit=0;
#endif

#if NUMBER_OF_LCD_UNITS >= 2 || LCD_SAVE_MORE_CODE_SPACE == 0
static struct  {
                  unsigned char x;
                  unsigned char y;
               } lcd_coordinates[NUMBER_OF_LCD_UNITS];
#endif

#if   LCD_READ_REQUIRED == 1 && LCD_BACKUP_REQUIRED == 1
#if   LCD_BACKUP_LOCATION == 0 

static unsigned char lcd_backup[LCD_0_LINES][LCD_0_CHARS_PER_LINE]__attribute__((section(".eeprom")));

#elif LCD_BACKUP_LOCATION == 1

static unsigned char lcd_backup[LCD_0_LINES][LCD_0_CHARS_PER_LINE];

#endif /* #if   LCD_BACKUP_LOCATION == 0 -> #elif LCD_BACKUP_LOCATION == 1 */
#endif /* #if   LCD_READ_REQUIRED == 1 && LCD_BACKUP_REQUIRED == 1 */

/*********************************************************************************************************/
#elif LCD_IO_MODE == 6
/*********************************************************************************************************/

#if   LCD_READ_REQUIRED == 1 && LCD_BACKUP_REQUIRED == 1
#if   LCD_BACKUP_LOCATION == 0 

static unsigned char lcd_backup[LCD_LINES][LCD_CHARS_PER_LINE]__attribute__((section(".eeprom")));

#elif LCD_BACKUP_LOCATION == 1

static unsigned char lcd_backup[LCD_LINES][LCD_CHARS_PER_LINE];

#endif   /* #if   LCD_BACKUP_LOCATION == 0 -> #elif LCD_BACKUP_LOCATION == 1 */
#endif   /* #if   LCD_READ_REQUIRED == 1 && LCD_BACKUP_REQUIRED == 1 */

/*********************************************************************************************************/
#endif   /* #if LCD_IO_MODE == 7 -> #elif LCD_IO_MODE == 3 -> #elif LCD_IO_MODE == 6 */
/*********************************************************************************************************/

/*######################################################################################################*/
/*                                  LOCAL FUNCTION DEFINITIONS                                          */
/*######################################################################################################*/
/* USE THE ASM DELAY FUNCTION IF YOU EXPERIENCE ANY PROBLEMS. I HAVEN'T FOUND ONE YET. */
/*
void delay(unsigned long number_of_loops) 
{
  __asm__ volatile ( "cp  %A0,__zero_reg__ \n\t"  \
                     "cpc %B0,__zero_reg__ \n\t"  \
                     "cpc %C0,__zero_reg__ \n\t"  \
                     "cpc %D0,__zero_reg__ \n\t"  \
                     "breq L_EXIT_%=       \n\t"  \
                     "L_%=:                \n\t"  \
                     "subi %A0,lo8(-(-1))  \n\t"  \
                     "sbci %B0,hi8(-(-1))  \n\t"  \
                     "sbci %C0,hlo8(-(-1)) \n\t"  \
                     "sbci %D0,hhi8(-(-1)) \n\t"  \
                     "brne L_%=            \n\t"  \
                     "L_EXIT_%=:           \n\t"  \
                     :                            \
                     : "w" (number_of_loops)      \
                   );                             \
                                              
return;
}
*/

static void delay(unsigned long number_of_loops)
{
   while ( number_of_loops ) { number_of_loops--; }  
} 

/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if LCD_IO_MODE == 7   /* lcd_write() function for 6 bit i/o one lcd unit */
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/


static void sbivar(unsigned char port, unsigned char bit)
{
    *((unsigned char*)(port+IO_TO_MEM_OFFSET)) |= (1<<bit);

return;
}
/*######################################################################################################*/

static void cbivar(unsigned char port, unsigned char bit)
{
    *((unsigned char*)(port+IO_TO_MEM_OFFSET)) &= (~(1<<bit));

return;
}
/*######################################################################################################*/

static void lcd_write(unsigned char data, unsigned char mode) 
{
#if LCD_SAVE_MORE_CODE_SPACE == 0
if(lcd_init_lock == 0) { lcd_init(); }
#endif
/*****************************  LCD BYTE WRITE ENGINE START  ********************************************/
       cbi(LCD_D4_OUT_REG, LCD_D4_PIN);
       cbi(LCD_D5_OUT_REG, LCD_D5_PIN);
       cbi(LCD_D6_OUT_REG, LCD_D6_PIN);
       cbi(LCD_D7_OUT_REG, LCD_D7_PIN);
       cbi(LCD_RS_OUT_REG, LCD_RS_PIN);

       if(mode==DATA_MODE) sbi(LCD_RS_OUT_REG, LCD_RS_PIN); 
       /* output high nibble first */ 
       if(data&0x10) sbi(LCD_D4_OUT_REG, LCD_D4_PIN);   
       if(data&0x20) sbi(LCD_D5_OUT_REG, LCD_D5_PIN);   
       if(data&0x40) sbi(LCD_D6_OUT_REG, LCD_D6_PIN);   
       if(data&0x80) sbi(LCD_D7_OUT_REG, LCD_D7_PIN);   
       /* Strobe E pin (Time is defined in lcd_io.h) */
       lcd_toggle_e();

       /* output low nibble */
       cbi(LCD_D4_OUT_REG, LCD_D4_PIN);
       cbi(LCD_D5_OUT_REG, LCD_D5_PIN);
       cbi(LCD_D6_OUT_REG, LCD_D6_PIN);
       cbi(LCD_D7_OUT_REG, LCD_D7_PIN);

       /* if INIT_MODE skip this section else execute it */
       if(mode!=INIT_MODE)
        {
          if(data&0x01) sbi(LCD_D4_OUT_REG, LCD_D4_PIN);   
          if(data&0x02) sbi(LCD_D5_OUT_REG, LCD_D5_PIN);   
          if(data&0x04) sbi(LCD_D6_OUT_REG, LCD_D6_PIN);   
          if(data&0x08) sbi(LCD_D7_OUT_REG, LCD_D7_PIN);   
          /* Strobe E pin (Time is defined in lcd_io.h) */
          lcd_toggle_e();
        }

 delay(LCD_SMALL_DELAY);
/*****************************  LCD BYTE WRITE ENGINE END  **********************************************/

return;
}
/*------------------------------------------------------------------------------------------------------*/
void  select_lcd(unsigned char lcd_unit)
{
#if LCD_SAVE_MORE_CODE_SPACE == 0
if(lcd_init_lock == 0) { lcd_init(); }
#endif
/* Save the current lcd unit x,y coordinates */
   lcd_coordinates[current_lcd_unit].x=x;
   lcd_coordinates[current_lcd_unit].y=y;

   if(lcd_unit==LCD_0)
    {
       lcd_E_port= LCD_0_E_OUT_REG;
       lcd_E_pin = LCD_0_E_PIN;
#if LCD_SAVE_MORE_CODE_SPACE == 0 || LCD_CLRLINE_NEEDED == 1 
       lcd_chars_per_line = LCD_0_CHARS_PER_LINE;
#endif
       lcd_lines = LCD_0_LINES;
       current_lcd_unit=lcd_unit;     /* Make the requested lcd unit current (active) */
       x=lcd_coordinates[lcd_unit].x;  /* Load the saved x,y coordinates of the specified lcd unit */
       y=lcd_coordinates[lcd_unit].y;
    }
   else if(lcd_unit==LCD_1)
         {
            lcd_E_port= LCD_1_E_OUT_REG;
            lcd_E_pin = LCD_1_E_PIN;
#if LCD_SAVE_MORE_CODE_SPACE == 0 || LCD_CLRLINE_NEEDED == 1 
            lcd_chars_per_line = LCD_1_CHARS_PER_LINE;
#endif
            lcd_lines = LCD_1_LINES;
            current_lcd_unit=lcd_unit;
            x=lcd_coordinates[lcd_unit].x;
            y=lcd_coordinates[lcd_unit].y;
         }
#if NUMBER_OF_LCD_UNITS >=3
   else if(lcd_unit==LCD_2)
         {
            lcd_E_port= LCD_2_E_OUT_REG;
            lcd_E_pin = LCD_2_E_PIN;
#if LCD_SAVE_MORE_CODE_SPACE == 0 || LCD_CLRLINE_NEEDED == 1 
            lcd_chars_per_line = LCD_2_CHARS_PER_LINE;
#endif
            lcd_lines = LCD_2_LINES;
            current_lcd_unit=lcd_unit;
            x=lcd_coordinates[lcd_unit].x;
            y=lcd_coordinates[lcd_unit].y;
         }
#endif
return;
}

/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif LCD_IO_MODE == 6   /* lcd_write() function for 6 bit i/o  with multi lcd units */
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#if LCD_READ_REQUIRED == 1 
static void wait_until_busy_flag_is_clear(void)
/* loops while lcd is busy, reads address counter */
{
    
while( (lcd_read(BUSY_FLAG) & (1<<LCD_BUSY)) ); 

return;  
}


static unsigned char lcd_read(unsigned char rs) 
{
register unsigned char data=0;

/****************************  LCD BYTE READ ENGINE START  **********************************************/
/* RS=1: read data, RS=0: read busy flag, RW=1  read mode */  
    cbi(LCD_RS_OUT_REG, LCD_RS_PIN);
    if(rs) { sbi(LCD_RS_OUT_REG, LCD_RS_PIN); }      

    /* configure data pins as input */
    cbi(LCD_D7_DDR_REG, LCD_D7_PIN );
    cbi(LCD_D6_DDR_REG, LCD_D6_PIN );
    cbi(LCD_D5_DDR_REG, LCD_D5_PIN );
    cbi(LCD_D4_DDR_REG, LCD_D4_PIN );
    /* set R/W pin for reading from LCD */
    sbi(LCD_RW_OUT_REG, LCD_RW_PIN);
    LCD_E_DELAY(); 

    if(bit_is_set(LCD_D7_PIN_REG, LCD_D7_PIN)) { data|=(1<<7); }
    if(bit_is_set(LCD_D6_PIN_REG, LCD_D6_PIN)) { data|=(1<<6); }
    if(bit_is_set(LCD_D5_PIN_REG, LCD_D5_PIN)) { data|=(1<<5); }
    if(bit_is_set(LCD_D4_PIN_REG, LCD_D4_PIN)) { data|=(1<<4); }

    lcd_toggle_e();

    if(bit_is_set(LCD_D7_PIN_REG, LCD_D7_PIN)) { data|=(1<<3); }
    if(bit_is_set(LCD_D6_PIN_REG, LCD_D6_PIN)) { data|=(1<<2); }
    if(bit_is_set(LCD_D5_PIN_REG, LCD_D5_PIN)) { data|=(1<<1); }
    if(bit_is_set(LCD_D4_PIN_REG, LCD_D4_PIN)) { data|=(1<<0); }
    
    lcd_toggle_e();

    if(rs) { wait_until_busy_flag_is_clear(); }  
/*****************************  LCD BYTE READ ENGINE END  ***********************************************/

return(data);
}

#endif  /* #if LCD_READ_REQUIRED == 1  */
/*######################################################################################################*/

static void lcd_write(unsigned char data, unsigned char mode) 
{
#if LCD_SAVE_MORE_CODE_SPACE == 0
if(lcd_init_lock == 0) { lcd_init(); }
#endif

 /* save and set DDR & PORT registers in case another program altered those values */
#if LCD_MULTIPLEX_ENABLE == 1 && LCD_READ_REQUIRED == 0

unsigned char ddr_states=0;
unsigned char port_states=0;

if(inp(LCD_D4_DDR_REG)&(1<<LCD_D4_PIN)) { ddr_states|=(1<<4); }
if(inp(LCD_D4_OUT_REG)&(1<<LCD_D4_PIN)) { port_states|=(1<<4); }
sbi(LCD_D4_DDR_REG, LCD_D4_PIN );

if(inp(LCD_D5_DDR_REG)&(1<<LCD_D5_PIN)) { ddr_states|=(1<<5); }
if(inp(LCD_D5_OUT_REG)&(1<<LCD_D5_PIN)) { port_states|=(1<<5); }
sbi(LCD_D5_DDR_REG, LCD_D5_PIN );


if(inp(LCD_D6_DDR_REG)&(1<<LCD_D6_PIN)) { ddr_states|=(1<<6); }
if(inp(LCD_D6_OUT_REG)&(1<<LCD_D6_PIN)) { port_states|=(1<<6); }
sbi(LCD_D6_DDR_REG, LCD_D6_PIN );


if(inp(LCD_D7_DDR_REG)&(1<<LCD_D7_PIN)) { ddr_states|=(1<<7); }
if(inp(LCD_D7_OUT_REG)&(1<<LCD_D7_PIN)) { port_states|=(1<<7); }
sbi(LCD_D7_DDR_REG, LCD_D7_PIN );


if(inp(LCD_RS_DDR_REG)&(1<<LCD_RS_PIN)) { ddr_states|=(1<<0); }
if(inp(LCD_RS_OUT_REG)&(1<<LCD_RS_PIN)) { port_states|=(1<<0); }
sbi(LCD_RS_DDR_REG, LCD_RS_PIN );

#endif  /* LCD_MULTIPLEX_ENABLE == 1 */

#if LCD_READ_REQUIRED == 1 && LCD_MULTIPLEX_ENABLE == 0
       cbi(LCD_RW_OUT_REG, LCD_RW_PIN); 
       sbi(LCD_D4_DDR_REG, LCD_D4_PIN );
       sbi(LCD_D5_DDR_REG, LCD_D5_PIN );
       sbi(LCD_D6_DDR_REG, LCD_D6_PIN );
       sbi(LCD_D7_DDR_REG, LCD_D7_PIN );
#endif
/***************************  LCD BYTE WRITE ENGINE START  **********************************************/
       /* output high nibble first */ 
       /* Make all data pins & rs pin go low */
       cbi(LCD_D4_OUT_REG, LCD_D4_PIN);
       cbi(LCD_D5_OUT_REG, LCD_D5_PIN);
       cbi(LCD_D6_OUT_REG, LCD_D6_PIN);
       cbi(LCD_D7_OUT_REG, LCD_D7_PIN);
       cbi(LCD_RS_OUT_REG, LCD_RS_PIN);

       if(mode==DATA_MODE) sbi(LCD_RS_OUT_REG, LCD_RS_PIN); 

       if(data&0x10) sbi(LCD_D4_OUT_REG, LCD_D4_PIN);  
       if(data&0x20) sbi(LCD_D5_OUT_REG, LCD_D5_PIN);  
       if(data&0x40) sbi(LCD_D6_OUT_REG, LCD_D6_PIN);  
       if(data&0x80) sbi(LCD_D7_OUT_REG, LCD_D7_PIN);  
       /* Strobe E pin (Time is defined in lcd_io.h) */
       lcd_toggle_e();

       /* output low nibble */
       /* Make all data pins go low. RS is already low. */
       cbi(LCD_D4_OUT_REG, LCD_D4_PIN);
       cbi(LCD_D5_OUT_REG, LCD_D5_PIN);
       cbi(LCD_D6_OUT_REG, LCD_D6_PIN);
       cbi(LCD_D7_OUT_REG, LCD_D7_PIN);

       /* if INIT_MODE skip this section else execute it */
       if(mode!=INIT_MODE)
        {
          if(data&0x01) sbi(LCD_D4_OUT_REG, LCD_D4_PIN);  
          if(data&0x02) sbi(LCD_D5_OUT_REG, LCD_D5_PIN);  
          if(data&0x04) sbi(LCD_D6_OUT_REG, LCD_D6_PIN);  
          if(data&0x08) sbi(LCD_D7_OUT_REG, LCD_D7_PIN);  
          /* Strobe E pin (Time is defined in lcd_io.h) */
          lcd_toggle_e();
#if LCD_READ_REQUIRED == 1
          wait_until_busy_flag_is_clear(); /* Check Busy Flag. */
        }
       else{ delay(LCD_SMALL_DELAY); }   /* when INIT_MODE add a delay since busy flag cannot be checked */
#elif LCD_READ_REQUIRED == 0
        }

       delay(LCD_SMALL_DELAY); /* When reading is not needed (6 pins are used), add a delay always. */
#endif
/***************************  LCD BYTE WRITE ENGINE END  ************************************************/

/* Restore the DDR & PORT registers, if multiplexing wanted */     
#if LCD_MULTIPLEX_ENABLE == 1 && LCD_READ_REQUIRED == 0 

if(ddr_states&(1<<4)) { sbi(LCD_D4_DDR_REG, LCD_D4_PIN ); } 
if(ddr_states&(1<<5)) { sbi(LCD_D5_DDR_REG, LCD_D5_PIN ); }
if(ddr_states&(1<<6)) { sbi(LCD_D6_DDR_REG, LCD_D6_PIN ); }
if(ddr_states&(1<<7)) { sbi(LCD_D7_DDR_REG, LCD_D7_PIN ); }
if(ddr_states&(1<<0)) { sbi(LCD_RS_DDR_REG, LCD_RS_PIN ); }

if(port_states&(1<<4)) { sbi(LCD_D4_OUT_REG, LCD_D4_PIN ); } 
if(port_states&(1<<5)) { sbi(LCD_D5_OUT_REG, LCD_D5_PIN ); }
if(port_states&(1<<6)) { sbi(LCD_D6_OUT_REG, LCD_D6_PIN ); }
if(port_states&(1<<7)) { sbi(LCD_D7_OUT_REG, LCD_D7_PIN ); }
if(port_states&(1<<0)) { sbi(LCD_RS_OUT_REG, LCD_RS_PIN ); }

#endif  /* LCD_MULTIPLEX_ENABLE == 1 */
}

/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif LCD_IO_MODE == 2  /* lcd_write() function for 2 bit i/o */
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

static void toggle_clock(void)
{
sbi(LCD_CLOCK_OUT_REG, LCD_CLOCK_PIN);
LCD_E_DELAY(); 
cbi(LCD_CLOCK_OUT_REG, LCD_CLOCK_PIN);
LCD_E_DELAY(); 

return;
}
/*######################################################################################################*/

static void toggle_data(void)
{
sbi(LCD_DATA_OUT_REG, LCD_DATA_PIN);
LCD_E_DELAY();
cbi(LCD_DATA_OUT_REG, LCD_DATA_PIN);
LCD_E_DELAY();  

return;
}
/*######################################################################################################*/

static void data(unsigned char logic_state)
{

if(logic_state)
 { 
   sbi(LCD_DATA_OUT_REG, LCD_DATA_PIN);
 }
else{ 
      cbi(LCD_DATA_OUT_REG, LCD_DATA_PIN);
    } 

toggle_clock();

return;
}
/*######################################################################################################*/

static void lcd_write(unsigned char lcd_data,unsigned char mode) 
{
register unsigned char x=0, rs=0;
#if LCD_SAVE_MORE_CODE_SPACE == 0
if(lcd_init_lock == 0) { lcd_init(); }
#endif
/***************************  LCD BYTE WRITE ENGINE START  **********************************************/
       /* INITIALIZATION */
       /* Set clock and data pins as outputs, at low state and set rs value */ 
       sbi(LCD_CLOCK_DDR_REG, LCD_CLOCK_PIN); 
       sbi(LCD_DATA_DDR_REG, LCD_DATA_PIN);

       cbi(LCD_CLOCK_OUT_REG, LCD_CLOCK_PIN);
       cbi(LCD_DATA_OUT_REG, LCD_DATA_PIN); 
       if(mode==DATA_MODE) rs=1;  
       
       /* SEND HIGH NIBBLE AND THE APPROPRIATE CONTROL SIGNALS (DATA & CLOCK ARE LOW) */
       x=6;
       while(x--) { toggle_clock(); }   /* clear shift register */           
       data(1);
       data(rs);
       x=0x80; 
       while(x>=0x10)
           {
              data(lcd_data&x);
              x=(x>>1); 
           }
       /* Strobe E pin. The pulse duration is defined in io.h as "LCD_E_PULSE_WIDTH_US". */
       toggle_data();   


       /* IF MODE IS NOT "INIT_MODE" SEND LOW NIBBLE AND THE APPROPRIATE CONTROL SIGNALS */
       if(mode!=INIT_MODE)
        {       
           /* Clock and Data are already low */
           x=6;
           while(x--) { toggle_clock(); }   /* clear shift register */       
           data(1);
           data(rs); 
           x=0x08;
           while(x>=0x01)
               { 
                   data(lcd_data&x);
                   x=(x>>1);
               }
           /* Strobe E pin. The pulse duration is defined in io.h as "LCD_E_PULSE_WIDTH_US". */
           toggle_data();
        }        
       
       delay(LCD_SMALL_DELAY);     /* wait for command to execute  */     
/***************************  LCD BYTE WRITE ENGINE END  ************************************************/

return;      
}
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif LCD_IO_MODE == 3  /* lcd_write() function for 2 bit i/o */
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#if NUMBER_OF_LCD_UNITS >= 2

/*------------------------------------------------------------------------------------------------------*/
/*    The below code block defined by those lines, is always executed if  NUMBER_OF_LCD_UNITS >= 2      */            
/*------------------------------------------------------------------------------------------------------*/

void  select_lcd(unsigned char lcd_unit)
{
#if LCD_SAVE_MORE_CODE_SPACE == 0
if(lcd_init_lock == 0) { lcd_init(); }
#endif
/* Save the current lcd unit x,y coordinates */
   lcd_coordinates[current_lcd_unit].x=x;
   lcd_coordinates[current_lcd_unit].y=y;

   if(lcd_unit==LCD_0)
    {
       lcd_i2c_address=LCD_0_I2C_ADDRESS;
#if LCD_SAVE_MORE_CODE_SPACE == 0 || LCD_CLRLINE_NEEDED == 1 
       lcd_chars_per_line = LCD_0_CHARS_PER_LINE;
#endif
       lcd_lines = LCD_0_LINES;
       current_lcd_unit=lcd_unit;     /* Make the requested lcd unit current (active) */
       x=lcd_coordinates[lcd_unit].x;  /* Load the saved x,y coordinates of the specified lcd unit */
       y=lcd_coordinates[lcd_unit].y;
    }
   else if(lcd_unit==LCD_1)
         {
            lcd_i2c_address=LCD_1_I2C_ADDRESS;
#if LCD_SAVE_MORE_CODE_SPACE == 0 || LCD_CLRLINE_NEEDED == 1 
            lcd_chars_per_line = LCD_1_CHARS_PER_LINE;
#endif
            lcd_lines = LCD_1_LINES;
            current_lcd_unit=lcd_unit;
            x=lcd_coordinates[lcd_unit].x;
            y=lcd_coordinates[lcd_unit].y;
         }
/*------------------------------------------------------------------------------------------------------*/
/*    The above code block defined by those lines, is always executed if  NUMBER_OF_LCD_UNITS >= 2      */ 
/*------------------------------------------------------------------------------------------------------*/

#if NUMBER_OF_LCD_UNITS >= 3
  else if(lcd_unit==LCD_2)
         {
            lcd_i2c_address=LCD_2_I2C_ADDRESS;
#if LCD_SAVE_MORE_CODE_SPACE == 0 || LCD_CLRLINE_NEEDED == 1 
            lcd_chars_per_line = LCD_2_CHARS_PER_LINE;
#endif
            lcd_lines = LCD_2_LINES;
            current_lcd_unit=lcd_unit;
            x=lcd_coordinates[lcd_unit].x;
            y=lcd_coordinates[lcd_unit].y;
         }
#endif   /* #if NUMBER_OF_LCD_UNITS >= 3 */

#if NUMBER_OF_LCD_UNITS >= 4
   else if(lcd_unit==LCD_3)
         {
            lcd_i2c_address=LCD_3_I2C_ADDRESS;
#if LCD_SAVE_MORE_CODE_SPACE == 0 || LCD_CLRLINE_NEEDED == 1 
            lcd_chars_per_line = LCD_3_CHARS_PER_LINE;
#endif
            lcd_lines = LCD_3_LINES;
            current_lcd_unit=lcd_unit;
            x=lcd_coordinates[lcd_unit].x;
            y=lcd_coordinates[lcd_unit].y;
         }
#endif /* #if NUMBER_OF_LCD_UNITS >= 4 */
#if NUMBER_OF_LCD_UNITS >= 5
  else if(lcd_unit==LCD_4)
         {
            lcd_i2c_address=LCD_4_I2C_ADDRESS;
#if LCD_SAVE_MORE_CODE_SPACE == 0 || LCD_CLRLINE_NEEDED == 1 
            lcd_chars_per_line = LCD_4_CHARS_PER_LINE;
#endif
            lcd_lines = LCD_4_LINES;
            current_lcd_unit=lcd_unit;
            x=lcd_coordinates[lcd_unit].x;
            y=lcd_coordinates[lcd_unit].y;
         }
#endif   /* #if NUMBER_OF_LCD_UNITS >= 5 */

#if NUMBER_OF_LCD_UNITS >= 6
   else if(lcd_unit==LCD_5)
         {               
            lcd_i2c_address=LCD_5_I2C_ADDRESS;
#if LCD_SAVE_MORE_CODE_SPACE == 0 || LCD_CLRLINE_NEEDED == 1 
            lcd_chars_per_line = LCD_5_CHARS_PER_LINE;
#endif
            lcd_lines = LCD_5_LINES;
            current_lcd_unit=lcd_unit;
            x=lcd_coordinates[lcd_unit].x;
            y=lcd_coordinates[lcd_unit].y;
         }
#endif /* #if NUMBER_OF_LCD_UNITS >= 6 */
#if NUMBER_OF_LCD_UNITS >= 7
  else if(lcd_unit==LCD_6)
         {
            lcd_i2c_address=LCD_6_I2C_ADDRESS;
#if LCD_SAVE_MORE_CODE_SPACE == 0 || LCD_CLRLINE_NEEDED == 1 
            lcd_chars_per_line = LCD_6_CHARS_PER_LINE;
#endif
            lcd_lines = LCD_6_LINES;
            current_lcd_unit=lcd_unit;
            x=lcd_coordinates[lcd_unit].x;
            y=lcd_coordinates[lcd_unit].y;
         }
#endif   /* #if NUMBER_OF_LCD_UNITS >= 7 */
#if NUMBER_OF_LCD_UNITS >= 8
  else if(lcd_unit==LCD_7)
         {
            lcd_i2c_address=LCD_7_I2C_ADDRESS;
#if LCD_SAVE_MORE_CODE_SPACE == 0 || LCD_CLRLINE_NEEDED == 1 
            lcd_chars_per_line = LCD_7_CHARS_PER_LINE;
#endif
            lcd_lines = LCD_7_LINES;
            current_lcd_unit=lcd_unit;
            x=lcd_coordinates[lcd_unit].x;
            y=lcd_coordinates[lcd_unit].y;
         }
#endif   /* #if NUMBER_OF_LCD_UNITS >= 8 */




return;
}
#endif   /* #if NUMBER_OF_LCD_UNITS >= 2 */
/*######################################################################################################*/

#if LCD_READ_REQUIRED == 1 

#if LCD_DONT_READ_BUSY_FLAG == 0
 
static void wait_until_busy_flag_is_clear(void)
/* loops while lcd is busy, reads address counter */
{
unsigned char counter1=0;

    while( (lcd_read(BUSY_FLAG) & (1<<LCD_BUSY)) )
        {
           counter1++;
           if(counter1>3) { break; }
        }
     

return;  
}
/*######################################################################################################*/

static unsigned char lcd_read(unsigned char rs) 
{

unsigned char i2c_tx_data=0, i2c_rx_data=0;
volatile unsigned char data=0;

    /* RS=1: read data, RS=0: read busy flag, RW=1  read mode */  

    if(rs) wait_until_busy_flag_is_clear();

    i2c_tx_data = (1<<LCD_E_PIN);
    if(rs){ i2c_tx_data |= (1<<LCD_RS_PIN); }
    i2c_tx_data |= (1<<LCD_D7_PIN);
    i2c_tx_data |= (1<<LCD_D6_PIN);
    i2c_tx_data |= (1<<LCD_D5_PIN);
    i2c_tx_data |= (1<<LCD_D4_PIN);

/* Alternative way of setting the required bits for reading the lcd. */
/*
    i2c_tx_data = ( 0xFF & (~(1<<LCD_RW_PIN)) );
    if(rs==0){ i2c_tx_data &= (~(1<<LCD_RS_PIN)); }
*/

    i2c_start_tx(lcd_i2c_address);     
    i2c_put_byte(i2c_tx_data);

if(rs==0)
 {
    i2c_put_byte( i2c_tx_data & (~(1<<LCD_E_PIN)) );
    i2c_put_byte( i2c_tx_data | (1<<LCD_E_PIN) );
 }

    i2c_tx_data |= (1<<LCD_RW_PIN);
    i2c_put_byte(i2c_tx_data);

    i2c_start_rx(lcd_i2c_address);
    i2c_rx_data=i2c_get_byte(I2C_QUIT);


    if(i2c_rx_data&(1<<LCD_D7_PIN)) { data|=(1<<7); }
    if(i2c_rx_data&(1<<LCD_D6_PIN)) { data|=(1<<6); }
    if(i2c_rx_data&(1<<LCD_D5_PIN)) { data|=(1<<5); }
    if(i2c_rx_data&(1<<LCD_D4_PIN)) { data|=(1<<4); }
 
    i2c_start_tx(lcd_i2c_address);         
    i2c_put_byte( i2c_tx_data & (~(1<<LCD_E_PIN)) );
    i2c_put_byte( i2c_tx_data | (1<<LCD_E_PIN) );

    i2c_start_rx(lcd_i2c_address);
    i2c_rx_data=i2c_get_byte(I2C_QUIT);

    if(i2c_rx_data&(1<<LCD_D7_PIN)) { data|=(1<<3); }
    if(i2c_rx_data&(1<<LCD_D6_PIN)) { data|=(1<<2); }
    if(i2c_rx_data&(1<<LCD_D5_PIN)) { data|=(1<<1); }
    if(i2c_rx_data&(1<<LCD_D4_PIN)) { data|=(1<<0); }
 
if(rs)
 {
    i2c_start_tx(lcd_i2c_address);         
    i2c_put_byte( i2c_tx_data & (~(1<<LCD_E_PIN)) );
    i2c_put_byte( i2c_tx_data | (1<<LCD_E_PIN) );

 }

    i2c_stop();


return(data);
}
/*######################################################################################################*/

#elif LCD_DONT_READ_BUSY_FLAG == 1 

static unsigned char lcd_read(unsigned char rs) 
{

unsigned char i2c_tx_data=0, i2c_rx_data=0;
volatile unsigned char data=0;

    /* RS=1: read data, RS=0: read busy flag, RW=1  read mode */  
    i2c_tx_data = 0xFF;
    if(rs==0){ i2c_tx_data &= (~(1<<LCD_RS_PIN)); }

    i2c_start_tx(lcd_i2c_address);         
    i2c_put_byte(i2c_tx_data);
    i2c_start_rx(lcd_i2c_address);
    i2c_rx_data=i2c_get_byte(I2C_QUIT);

    if(i2c_rx_data&(1<<LCD_D7_PIN)) { data|=(1<<7); }
    if(i2c_rx_data&(1<<LCD_D6_PIN)) { data|=(1<<6); }
    if(i2c_rx_data&(1<<LCD_D5_PIN)) { data|=(1<<5); }
    if(i2c_rx_data&(1<<LCD_D4_PIN)) { data|=(1<<4); }
 
    i2c_start_tx(lcd_i2c_address);         
    i2c_put_byte( i2c_tx_data & (~(1<<LCD_E_PIN)) );
    i2c_put_byte( i2c_tx_data | (1<<LCD_E_PIN) );
    i2c_start_rx(lcd_i2c_address);
    i2c_rx_data=i2c_get_byte(I2C_QUIT);

    if(i2c_rx_data&(1<<LCD_D7_PIN)) { data|=(1<<3); }
    if(i2c_rx_data&(1<<LCD_D6_PIN)) { data|=(1<<2); }
    if(i2c_rx_data&(1<<LCD_D5_PIN)) { data|=(1<<1); }
    if(i2c_rx_data&(1<<LCD_D4_PIN)) { data|=(1<<0); }
 
    i2c_start_tx(lcd_i2c_address);         
    i2c_put_byte( i2c_tx_data & (~(1<<LCD_E_PIN)) );
    i2c_put_byte( i2c_tx_data | (1<<LCD_E_PIN) );
    i2c_stop();


return(data);
}

#endif  /* #if LCD_DONT_READ_BUSY_FLAG == 0 -> #elif LCD_DONT_READ_BUSY_FLAG == 1  */

#endif  /* #if LCD_READ_REQUIRED == 1  */
/*######################################################################################################*/


static void lcd_write(unsigned char lcd_data,unsigned char mode) 
{
unsigned char i2c_tx_data=0;
#if LCD_SAVE_MORE_CODE_SPACE == 0
if(lcd_init_lock == 0) { lcd_init(); }
#endif

#if LCD_READ_REQUIRED == 1 && LCD_DONT_READ_BUSY_FLAG == 0   
if(mode!=INIT_MODE) { wait_until_busy_flag_is_clear(); }
#endif
       /* INITIALIZATION */
       /* Set clock and data pins as outputs, at low state and set rs value */ 
       i2c_tx_data = (1<<LCD_E_PIN);
       if(mode==DATA_MODE) { i2c_tx_data |= (1<<LCD_RS_PIN); } 
 
       if(lcd_data&0x10) i2c_tx_data |= (1<<LCD_D4_PIN);  
       if(lcd_data&0x20) i2c_tx_data |= (1<<LCD_D5_PIN);   
       if(lcd_data&0x40) i2c_tx_data |= (1<<LCD_D6_PIN);   
       if(lcd_data&0x80) i2c_tx_data |= (1<<LCD_D7_PIN);   

       i2c_start_tx(lcd_i2c_address);         
       i2c_put_byte(i2c_tx_data);
       i2c_put_byte( i2c_tx_data & (~(1<<LCD_E_PIN)) );
       i2c_put_byte( i2c_tx_data | (1<<LCD_E_PIN) );
 
    
       /* IF MODE IS NOT "INIT_MODE" SEND LOW NIBBLE AND THE APPROPRIATE CONTROL SIGNALS */
       if(mode!=INIT_MODE)
        {       
           /* Reset the data bits */
           i2c_tx_data &= (~((1<<LCD_D4_PIN)|(1<<LCD_D5_PIN)|(1<<LCD_D6_PIN)|(1<<LCD_D7_PIN)));

           if(lcd_data&0x01) i2c_tx_data |= (1<<LCD_D4_PIN);  
           if(lcd_data&0x02) i2c_tx_data |= (1<<LCD_D5_PIN);   
           if(lcd_data&0x04) i2c_tx_data |= (1<<LCD_D6_PIN);   
           if(lcd_data&0x08) i2c_tx_data |= (1<<LCD_D7_PIN);   

           i2c_put_byte(i2c_tx_data);                    
           i2c_put_byte( i2c_tx_data & (~(1<<LCD_E_PIN)) );
           i2c_put_byte( i2c_tx_data | (1<<LCD_E_PIN) );
#if LCD_READ_REQUIRED == 1 && LCD_DONT_READ_BUSY_FLAG == 0
        }
       else{ delay(LCD_SMALL_DELAY); }   /* when INIT_MODE add a delay since busy flag cannot be checked */
#elif LCD_READ_REQUIRED == 0 || (LCD_READ_REQUIRED == 1 && LCD_DONT_READ_BUSY_FLAG == 1)
        }

       delay(LCD_SMALL_DELAY); /* When reading is not needed (6 pins are used), add a delay always. */
#endif

i2c_stop();


return;      
}


/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif /* #if LCD_IO_MODE == 7 -> #elif LCD_IO_MODE == 6,2,3 */
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

/*######################################################################################################*/
/*                                 PUBLIC FUNCTION DEFINITIONS                                          */
/*######################################################################################################*/
void lcd_command(unsigned char cmd)
/* send command <cmd> to LCD */
{
    lcd_write(cmd,CMD_MODE);
    /* this command needs more waiting time to execute */
    if(cmd==(1<<LCD_CLR) || cmd==(1<<LCD_HOME))
     { 
#if LCD_READ_REQUIRED == 0 || LCD_IO_MODE == 2 || LCD_IO_MODE == 7 || LCD_DONT_READ_BUSY_FLAG == 1
        delay(LCD_LONG_DELAY);
#endif 
        x=0;
        y=0;
     }       

return;    
}
/*######################################################################################################*/

void lcd_gotoxy(unsigned char lcd_x, unsigned char lcd_y)
/* goto position (x,y) */
{
#if LCD_SAVE_MORE_CODE_SPACE == 0

#if LCD_IO_MODE == 7 || LCD_IO_MODE == 3

/* If out of range values for the lcd coordinates are given, lock lcd_putc() and return */
  putc_lock=0;
  if(lcd_x >= lcd_chars_per_line || lcd_y >= lcd_lines) { putc_lock=1; return; }

#elif LCD_IO_MODE == 6 || LCD_IO_MODE == 2 

/* If out of range values for the lcd coordinates are given, lock lcd_putc() and return */
  putc_lock=0;
  if(lcd_x >= LCD_CHARS_PER_LINE || lcd_y >= LCD_LINES) { putc_lock=1; return; }

#endif   /* #if LCD_IO_MODE == 7 -> #elif LCD_IO_MODE == 6 || LCD_IO_MODE == 2 */

#endif   /* #if LCD_SAVE_MORE_CODE_SPACE == 0 */

  lcd_command((1<<LCD_DDRAM)+lcd_line_start[lcd_y]+lcd_x);
  x=lcd_x;
  y=lcd_y;

return;
}
/*######################################################################################################*/

void lcd_putc(unsigned char c)
/* print character at current cursor position */
{
#if LCD_SAVE_MORE_CODE_SPACE == 0
/*******************************************************************************************************/
#if LCD_IO_MODE == 7 || LCD_IO_MODE == 3
/*******************************************************************************************************/

 if(!putc_lock)
  {
    if(lcd_lines !=1)
     {  if(c=='\n') { if(y<lcd_lines-1) lcd_gotoxy(0,(y+1)); }
        else if(x<lcd_chars_per_line) { lcd_write(c, DATA_MODE); x++; }
#if LCD_AUTO_LINE_FEED == 1
        else if(y<lcd_lines-1) { lcd_gotoxy(0,(y+1)); lcd_write(c, DATA_MODE); x++; }
        else { lcd_gotoxy(0,0); lcd_write(c, DATA_MODE); x++; }
#endif
     }
    else{
          if(c=='\n') { return; }
          if(x<lcd_chars_per_line) { lcd_write(c, DATA_MODE); x++; }
        }
  }

/*******************************************************************************************************/
#elif LCD_IO_MODE == 6 || LCD_IO_MODE == 2 
/*******************************************************************************************************/

 if(!putc_lock)
  {
#if LCD_LINES!=1
       if(c=='\n') { if(y<LCD_LINES-1) lcd_gotoxy(0,(y+1)); } 
       else if(x<LCD_CHARS_PER_LINE) { lcd_write(c, DATA_MODE); x++; }
#if LCD_AUTO_LINE_FEED == 1
       else if(y<LCD_LINES-1) { lcd_gotoxy(0,(y+1)); lcd_write(c, DATA_MODE); x++; }
       else { lcd_gotoxy(0,0); lcd_write(c, DATA_MODE); x++; }
#endif

#else
       if(c=='\n') { return; }
       if(x<LCD_CHARS_PER_LINE) { lcd_write(c, DATA_MODE); x++; }
#endif
  }

/*******************************************************************************************************/
#endif  /*   #if LCD_IO_MODE == 7 -> #elif LCD_IO_MODE == 6 || LCD_IO_MODE == 2 */    
/*******************************************************************************************************/
#elif LCD_SAVE_MORE_CODE_SPACE == 1

lcd_write(c, DATA_MODE);
x++;

#endif   /* #if LCD_SAVE_MORE_CODE_SPACE == 0 */

return;
}
/*######################################################################################################*/
#if LCD_PUT_I_NEEDED == 1
void lcd_put_i(int value, unsigned char dot_position, unsigned char number_of_chars)
/* print signed integer on lcd with or without comma (no auto linefeed) */
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
             *(lcd_data+position)=LCD_DECIMAL_POINT;
             dot_position=0;
          }
      } 
     position--;
     *(lcd_data+position)=(value%radix)+'0';
     value/=radix;

  }while((value || dot_position)&& dot_position <= 5); 

/* Move the array contents to the start of the array filling the rest with spaces (Left align data) */
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
/*######################################################################################################*/

#if LCD_PUTC_CGRAM_NEEDED == 1
void lcd_putc_cgram(const unsigned char *user_char, unsigned char char_position)
{
register unsigned char x1=0;

if(char_position<=7)
 {    
   lcd_command((1<<LCD_CGRAM)+(char_position*8));
   for(x1=0; x1<=7; x1++)
     {
        lcd_write(PRG_RDB(user_char++), DATA_MODE); 
     }
 }
else{
       lcd_command((1<<LCD_CGRAM));
       for(x1=0; x1<char_position; x1++)
         { 
            if(x1>=64) { break; } 
            lcd_write(PRG_RDB(user_char++), DATA_MODE); 
         }
    }     

lcd_gotoxy(x,y);

return;
}
#endif   /* #if LCD_PUTC_CGRAM_NEEDED == 1 */
/*######################################################################################################*/

#if LCD_CLRLINE_NEEDED == 1
void lcd_clrline(unsigned char line)
/* clear a specific lcd line and set cursor at the start of the line */
{
#if LCD_IO_MODE == 7 || LCD_IO_MODE == 3
   if(lcd_lines==1)
    {
       lcd_gotoxy(0,0);
       while(x<lcd_chars_per_line) { lcd_putc(' '); }
       lcd_gotoxy(0,0);
    }    
   else if(line<lcd_lines)
         {
            lcd_gotoxy(0,line);
            while(x<lcd_chars_per_line) { lcd_putc(' '); }
            lcd_gotoxy(0,line);
         }   

#elif LCD_IO_MODE == 6 || LCD_IO_MODE == 2 

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

#endif  /*  #if LCD_IO_MODE == 7 -> #elif LCD_IO_MODE == 6 || LCD_IO_MODE == 2 */
}
#endif   /* #if LCD_CLRLINE_NEEDED == 1 */
/*######################################################################################################*/
#if LCD_GETXY_NEEDED == 1
unsigned int lcd_getxy(void)
{
/*
    The return value of the "lcd_getxy()" function is an integer,
    with the high byte containing the current line number (y) and the low byte 
    containing the char position in that line (x).
    If the lower byte has a value of 20 that means that you filled that line.
    This position result can only happen when no lcd reading is available.
    When lcd reading is available the maximum x == 19. 
*/

/* It may seems strange that i use a union but this approach saves a lot of code space */
union double_byte_union {
                           unsigned int  word; 
                           unsigned char byte[2];
                         } lcd_address;

#if LCD_READ_REQUIRED == 0 || LCD_IO_MODE == 2 || LCD_IO_MODE == 7 || LCD_DONT_READ_BUSY_FLAG == 1 

lcd_address.byte[1]=y;
lcd_address.byte[0]=x;

#elif LCD_READ_REQUIRED == 1 && LCD_DONT_READ_BUSY_FLAG == 0 && ( LCD_IO_MODE == 6 || LCD_IO_MODE == 3)  

unsigned char address_counter=0;

/* Get the lcd's AC */
address_counter=lcd_read(BUSY_FLAG);
/* Turn off the busy flag's bit so that the address only remains. */
address_counter&=(~(1<<LCD_BUSY)); 

if(address_counter<(LCD_START_LINE1+0x14)) { lcd_address.byte[1]=0; } 
else if(address_counter<(LCD_START_LINE3+0x14)) { lcd_address.byte[1]=2; } 
else if(address_counter<(LCD_START_LINE2+0x14)) { lcd_address.byte[1]=1; }
else lcd_address.byte[1]=3;

lcd_address.byte[0]=(address_counter-lcd_line_start[lcd_address.byte[1]]);

#endif   /* #if LCD_READ_REQUIRED == 0 -> #elif LCD_READ_REQUIRED == 1 */

return(lcd_address.word);
}
#endif   /* #if LCD_GETXY_NEEDED == 1 */
/*######################################################################################################*/
#if LCD_PUTS_NEEDED == 1
void lcd_puts(const unsigned char *s)
/* print string on lcd (no auto linefeed) */
{
    register unsigned char c;

    while( (c = *s++) ) { lcd_putc(c);  }

return;
}
#endif
/*######################################################################################################*/
#if LCD_PUTS_P_NEEDED == 1
void lcd_puts_p(const unsigned char *progmem_s)
/* print string from program memory on lcd (no auto linefeed) */
{
    register unsigned char c;

    while( (c = PRG_RDB(progmem_s++)) ) { lcd_putc(c);  }

return;
}
#endif   /* #if LCD_PUTS_P_NEEDED == 1 */
/*######################################################################################################*/
#if LCD_PUTS_E_NEEDED == 1
void lcd_puts_e(unsigned char *eeprom_s)
/* print string from eeprom on lcd (no auto linefeed) */
{
 register unsigned char c;
   
    while( (c=eeprom_rb((unsigned int)eeprom_s++))&& c!=0xFF ) { lcd_putc(c);  }

return;               
}
#endif   /* #if LCD_PUTS_E_NEEDED == 1 */
/*######################################################################################################*/

#if LCD_READ_REQUIRED == 1

unsigned char lcd_getc(void)
{


return(lcd_read(DATA));
}
/*######################################################################################################*/

#if LCD_BACKUP_REQUIRED == 1  

/********************************************************************************************************/
#if LCD_IO_MODE == 6
/********************************************************************************************************/

void lcd_backup_scr(void)
{
unsigned char x1=0, x2=0, lcd_x=x, lcd_y=y;

   

for(x1=0; x1<LCD_LINES; x1++)
  {
     lcd_gotoxy(0,x1); 
     for(x2=0; x2<LCD_CHARS_PER_LINE; x2++)
       {

#if   LCD_BACKUP_LOCATION == 0
          eeprom_wb((lcd_backup[x1]+x2),lcd_read(DATA));
#elif LCD_BACKUP_LOCATION == 1
          *(lcd_backup[x1]+x2)=lcd_read(DATA);
#endif
       }
  }

x=lcd_x;
y=lcd_y;
lcd_gotoxy(x,y);

return;
}

void lcd_restore_scr(void)
{
unsigned char x1=0, x2=0, lcd_x=x, lcd_y=y;

lcd_clrscr();   
lcd_gotoxy(0,0);

for(x1=0; x1<LCD_LINES; x1++)
  {

     lcd_gotoxy(0,x1); 
     for(x2=0; x2<LCD_CHARS_PER_LINE; x2++)
       {
#if   LCD_BACKUP_LOCATION == 0
          lcd_putc( eeprom_rb((lcd_backup[x1]+x2)) );
#elif LCD_BACKUP_LOCATION == 1
          lcd_putc(*(lcd_backup[x1]+x2));
#endif
       }
  }

x=lcd_x;
y=lcd_y;
lcd_gotoxy(x,y);

return;
}

/********************************************************************************************************/
#elif LCD_IO_MODE == 3
/********************************************************************************************************/

void lcd_backup_scr(void)
{
unsigned char x1=0, x2=0, lcd_x=x, lcd_y=y;

   

for(x1=0; x1<lcd_lines; x1++)
  {
     lcd_gotoxy(0,x1); 
     for(x2=0; x2<lcd_chars_per_line; x2++)
       {

#if   LCD_BACKUP_LOCATION == 0
          eeprom_wb((lcd_backup[x1]+x2),lcd_read(DATA));
#elif LCD_BACKUP_LOCATION == 1
          *(lcd_backup[x1]+x2)=lcd_read(DATA);
#endif
       }
  }

x=lcd_x;
y=lcd_y;
lcd_gotoxy(x,y);

return;
}

void lcd_restore_scr(void)
{
unsigned char x1=0, x2=0, lcd_x=x, lcd_y=y;

lcd_clrscr();   
lcd_gotoxy(0,0);

for(x1=0; x1<lcd_lines; x1++)
  {

     lcd_gotoxy(0,x1); 
     for(x2=0; x2<lcd_chars_per_line; x2++)
       {
#if   LCD_BACKUP_LOCATION == 0
          lcd_putc( eeprom_rb((lcd_backup[x1]+x2)) );
#elif LCD_BACKUP_LOCATION == 1
          lcd_putc(*(lcd_backup[x1]+x2));
#endif
       }
  }

x=lcd_x;
y=lcd_y;
lcd_gotoxy(x,y);

return;
}

/********************************************************************************************************/
#endif   /* #if LCD_IO_MODE == 6 -> #elif LCD_IO_MODE == 3 */
/********************************************************************************************************/

#endif  /* #if LCD_BACKUP_REQUIRED == 1 */

#endif  /* #if LCD_READ_REQUIRED == 1 */

/*######################################################################################################*/
/*                                LCD INITIALIZATION FUNCTION                                           */
/*######################################################################################################*/

/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if LCD_IO_MODE == 6 || LCD_IO_MODE == 2 
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

void lcd_init(void)
{
/* initialize display and select type of cursor */
/* dispAttr: LCD_DISP_OFF, LCD_DISP_ON, LCD_DISP_ON_CURSOR, LCD_DISP_CURSOR_BLINK */

#if LCD_SAVE_MORE_CODE_SPACE == 0
lcd_init_lock=1;
#endif

#if LCD_LINES==1
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_1LINE 
#else
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_2LINES 
#endif 

#if LCD_IO_MODE == 6  

sbi(LCD_E_DDR_REG,  LCD_E_PIN);
sbi(LCD_E_OUT_REG,  LCD_E_PIN); 
sbi(LCD_RS_DDR_REG, LCD_RS_PIN);
#if LCD_READ_REQUIRED == 1 
sbi(LCD_RW_DDR_REG, LCD_RW_PIN);
cbi(LCD_RW_OUT_REG, LCD_RW_PIN);
#endif

#if LCD_MULTIPLEX_ENABLE == 0 && LCD_READ_REQUIRED == 0
sbi(LCD_D4_DDR_REG, LCD_D4_PIN);
sbi(LCD_D5_DDR_REG, LCD_D5_PIN);
sbi(LCD_D6_DDR_REG, LCD_D6_PIN);
sbi(LCD_D7_DDR_REG, LCD_D7_PIN);
#endif

#endif  /* #if LCD_IO_MODE == 6 */



/*------ Initialize lcd to 4 bit i/o mode -------*/
/* initial write to lcd is 8bit using delay since busy flag can't be checked here anyway */

    delay(LCD_POWER_ON_DELAY);                          /* Wait 20 milliseconds  */
     
    lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);
    delay(LCD_INIT_DELAY);                              /* Wait 5 milliseconds */
    
    lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);
    lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);
    lcd_write(LCD_FUNCTION_4BIT_1LINE, INIT_MODE);      /* set IO mode to 4bit */
 
    /* from now on the lcd accepts only 4 bit I/O, so we can use lcd_command() */    
    lcd_command(LCD_FUNCTION_DEFAULT);      /* function set: display lines  */
    lcd_command(LCD_DISP_OFF);              /* display off                  */
    lcd_command(LCD_CLEAR_SCREEN);          /* display clear                */
    lcd_command(LCD_MODE_DEFAULT);          /* set entry mode               */
    lcd_command(LCD_DISP_ON);               /* LCD DISPLAY ON (DEFAULT)     */

return;
}

/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif LCD_IO_MODE == 7 || LCD_IO_MODE == 3
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

/* initialize display and select type of cursor */
void lcd_init(void)
{
#if NUMBER_OF_LCD_UNITS >= 2 || LCD_SAVE_MORE_CODE_SPACE == 0
unsigned char lcd_unit=0;
#endif

#if LCD_SAVE_MORE_CODE_SPACE == 0
lcd_init_lock=1;
#endif

#if LCD_IO_MODE == 7

sbi(LCD_D4_DDR_REG,  LCD_D4_PIN);
sbi(LCD_D5_DDR_REG,  LCD_D5_PIN);
sbi(LCD_D6_DDR_REG,  LCD_D6_PIN);
sbi(LCD_D7_DDR_REG,  LCD_D7_PIN);
sbi(LCD_RS_DDR_REG,  LCD_RS_PIN);
sbi(LCD_0_E_DDR_REG, LCD_0_E_PIN);                 
sbi(LCD_0_E_OUT_REG, LCD_0_E_PIN); 
sbi(LCD_1_E_DDR_REG, LCD_1_E_PIN);                 
sbi(LCD_1_E_OUT_REG, LCD_1_E_PIN); 

#if NUMBER_OF_LCD_UNITS >= 3
sbi(LCD_2_E_DDR_REG, LCD_2_E_PIN  );                 
sbi(LCD_2_E_OUT_REG, LCD_2_E_PIN  );
#endif 

#endif   /* #if LCD_IO_MODE == 7 */

/* Reset all members of the lcd_coordinates structure to 0. */
#if LCD_SAVE_MORE_CODE_SPACE == 0
for(lcd_unit=0; lcd_unit<sizeof(lcd_coordinates); lcd_unit++)
  {
     *(((unsigned char*)lcd_coordinates)+lcd_unit)=0;
  }
#endif

/*------ Initialize lcd to 4 bit i/o mode -------*/
/* initial write to lcd is 8bit using delay since busy flag can't be checked here anyway */

#if NUMBER_OF_LCD_UNITS >= 2
for(lcd_unit=0; lcd_unit<NUMBER_OF_LCD_UNITS; lcd_unit++)
  {
      select_lcd(lcd_unit);
#endif
      delay(LCD_POWER_ON_DELAY);                          /* Wait 20 milliseconds  */
      lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);
      delay(LCD_INIT_DELAY);                              /* Wait 5 milliseconds */

      lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);
      lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);
      lcd_write(LCD_FUNCTION_4BIT_1LINE, INIT_MODE);      /* set IO mode to 4bit */
             
      /* from now on the lcd accepts only 4 bit I/O, so we can use lcd_command() */    
      /* function set: set how many lines the lcd unit has.  */
      if(lcd_lines==1) { lcd_command(LCD_FUNCTION_4BIT_1LINE);  }  
      else { lcd_command(LCD_FUNCTION_4BIT_2LINES); }
    
      lcd_command(LCD_DISP_OFF);                               /* display off                  */
      lcd_command(LCD_CLEAR_SCREEN);                           /* display clear                */ 
      lcd_command(LCD_MODE_DEFAULT);                           /* set entry mode               */
      lcd_command(LCD_DISP_ON);                                /* Display on                   */

#if NUMBER_OF_LCD_UNITS >= 2
  } 
select_lcd(LCD_0);
#endif

return;  
}

/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/


/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

