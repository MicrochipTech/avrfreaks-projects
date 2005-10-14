
#include <inttypes.h>
#include <io.h>   
#include <progmem.h> 
#include "lcd.h"
 

#define LCD_E_DELAY()   {asm volatile("nop");asm volatile("nop");\
						 asm volatile("nop");asm volatile("nop");}  // delay 500ns with 8Mhz
#define LCD_E_HIGH()    sbi(LCD_PORT, LCD_E_BIT)
#define LCD_E_LOW()     cbi(LCD_PORT, LCD_E_BIT)


// delay constants
// define the flags as...
// CFLAGS += -Wp,-DLCDPORTISA	-DCPU8MHZ		#port for LCD code

#if defined (CPU8MHZ)
#define F_CPU 8000000   
#elif defined (CPU16MHZ)
#define F_CPU 16000000
#else
you must define the CPU speed in the make file
eg CFLAGS += -DCPU8MHZ		
#endif
					
#define K_DELAY_100us	F_CPU/61349
// delay t * 100us, ie t is number of 100us units.
// got this off the net!

static void Delay(unsigned int t) 
{
	unsigned int i;
	if (t==0) return;
	while (t--) for(i=0;i<K_DELAY_100us; i++);
}

//------------------------------------------------------------------------

static void ToggleE(void)
{
    LCD_E_HIGH();
    LCD_E_DELAY();
    LCD_E_LOW();
}

//------------------------------------------------------------------------
static void LCDWrite(uint8_t data,uint8_t rs) 
{
   
   // since the LCD is operating in 4 bit mode, this function writes 
   // the high nibble first then the low nibble.
    
	outp(LCD_PORT_FOR_OUTPUT, LCD_PORT_DDR );  // data pins as output 

	rs <<=LCD_RS_BIT; 
    outp( (data>>4)  |rs, LCD_PORT );
    ToggleE();
    outp( (data&0x0F)|rs, LCD_PORT );
    ToggleE();
    
    outp(0x0F, LCD_PORT);    // all data pins high (inactive) 

}
//------------------------------------------------------------------------
static uint8_t LCDRead(uint8_t rs) 
{
    if (rs) 
      sbi(LCD_PORT, LCD_RS_BIT);    // RS=1: read data      
    else    
      cbi(LCD_PORT, LCD_RS_BIT);    // RS=0: read busy flag
     
    sbi(LCD_PORT, LCD_RW_BIT);      // RW=1  read mode      

    outp(LCD_PORT_FOR_INPUT, LCD_PORT_DDR);      // configure data pins as input
    
    LCD_E_HIGH();
    LCD_E_DELAY();
    
    uint8_t dataH = inp(LCD_PORT_PIN);     // read high nibble first
     
    LCD_E_LOW();
    LCD_E_DELAY();                  // Enable 500ns low         
    LCD_E_HIGH();
    LCD_E_DELAY();
    
    uint8_t dataL = inp(LCD_PORT_PIN);     // read low nibble        
    
    LCD_E_LOW();
    
    return ( (dataH<<4) | (dataL&0x0F) );
}

//------------------------------------------------------------------------
// loops while lcd is busy, reads address counter 
static unsigned char LCDWaitbusy(void)
{
    unsigned char c;
    
    while ( (c=LCDRead(RS0)) & LCD_BUSY) 
    {
    }

    return (c);  // return address counter  
}
//------------------------------------------------------------------------
// goto start of next line 
void LCDNewline(uint8_t pos)
{
    uint8_t addressCounter;
	
    if ( pos < (LCD_START_LINE2) )
        addressCounter = LCD_START_LINE2;
    else
        addressCounter = LCD_START_LINE1;
        
    LCDCommand(LCD_DDRAM+addressCounter);
}
//------------------------------------------------------------------------

void LCDCommand(uint8_t cmd)
{
    LCDWaitbusy();
    LCDWrite(cmd,RS0);
}
//------------------------------------------------------------------------

void LCDGotoXY(uint8_t x, uint8_t y)
{
    if ( y==0 ) 
        LCDCommand(LCD_DDRAM+LCD_START_LINE1+x);
    else
        LCDCommand(LCD_DDRAM+LCD_START_LINE2+x);
}
//------------------------------------------------------------------------
// clear lcd and set cursor to home position
void LCDClrscr(void)
{
    LCDCommand(LCD_CLR);
}
//------------------------------------------------------------------------

void LCDHome(void)
{
    LCDCommand(LCD_HOME);
}
//------------------------------------------------------------------------
// print character at current cursor position 

void LCDPutc(char c)
{

    unsigned char pos;
    
    pos = LCDWaitbusy();   // read busy-flag and address counter
    if (c=='\n')
        LCDNewline(pos);
    else
        LCDWrite(c, RS1);
        
}
//------------------------------------------------------------------------
// print string on lcd (no auto linefeed)

void LCDPuts(const char *s)
{
    char c;

    while ( (c = *s++) ) 
    {
        LCDPutc(c);
    }
}

//------------------------------------------------------------------------

void LCDPutsN(const char *s, uint8_t n)
{
    char c;

    while ( n-- && (c = *s++) ) 
    {
        LCDPutc(c);
    }
}
//------------------------------------------------------------------------

void LCDPuts_P(const char *progmem_s)
{
    char c;

    while ( (c = pgm_read_byte(progmem_s++)) ) 
    {
        LCDPutc(c);
    }
}
//------------------------------------------------------------------------

void LCDInit(uint8_t dispAttr)
{
    outp( LCD_PORT_FOR_OUTPUT, LCD_PORT_DDR ); // all port bits as output 
    
    Delay(400);                          // wait 40ms  (go to be >30ms or more after power)
  
    LCDCommand(LCD_FUNCTION_INIT);      // function set: 4 bit mode 
    LCDCommand(LCD_FUNCTION_INIT);      // need to do it twice
    LCDCommand(LCD_FUNCTION_INIT_2LINES); // 2 lines, display off
    LCDCommand(LCD_DISP_OFF);           // display off                  
    LCDClrscr();                        // display clear
    LCDCommand(LCD_ENTRY_INC);          // set entry mode               
    LCDCommand(dispAttr);               // display/cursor control       
} 
