/*
 Original project: ATmega32_tft_ov7670_REV4.1
 written by Guanfu_wang (or unknown author)

 modified for XMEGA evaluation board by: 
 Ozhan KD (www.KnowledgePlus.ir)
*/
#include "avr_compiler_.h"
#include "lcd.h"
#include "ov7670.h"
#include "int.h"
#include "delay.h"

void main(void) 
{	 
 EXCLK_DDR|=1<<EXCLK_BIT ;
 HC245_OE_DDR|=1<<HC245_OE_BIT;	
 DISEN_EXCLK;
 DISEN_245; 
 
 OSC_CTRL|=OSC_RC32MEN_bm;
 while(!(OSC_STATUS & OSC_RC32MRDY_bm));
 CCP = CCP_IOREG_gc;
 CLK_CTRL=CLK_SCLKSEL_RC32M_gc;
 /*
 OSC_PLLCTRL=OSC_PLLSRC_RC2M_gc|8;//  16 MHz
 OSC_CTRL|=OSC_PLLEN_bm;
 while(!(OSC_STATUS &OSC_PLLRDY_bm )); 
 CCP=CCP_IOREG_gc;                   
 CLK_CTRL=CLK_SCLKSEL_PLL_gc;
 */
 PORTCFG_CLKEVOUT=PORTCFG_CLKOUT_PC7_gc;
 PORTC_DIRSET=PIN7_bm; 
 DATA_OUPUT();
 LCD_BL_H();
 LCD_Init(); 
 LCD_write_english_string(0,0,"OV7670 Init ...",WHITE,BLACK); 
 while(1!=OV7670_init());
 delay_ms(200);
 LCD_write_english_string(0,15,"OV7670 Init 0K",WHITE,BLACK);  
 delay_ms(2000);
 LCD_write_english_string(0,47,"www.KnowledgePlus.ir",WHITE,BLACK);  
 delay_ms(5000);
 LCD_Clear(0);
 DATA_INPUT(); 
 Init_INT0();	
 while(1);
}

