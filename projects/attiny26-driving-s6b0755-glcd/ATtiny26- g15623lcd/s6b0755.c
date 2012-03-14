/***************************************************************************
****************************** s6b0755.c ************************************
******************** commands for s6b0755 glcd *****************************
**** LeRoy Olson ************************** Feb 26 2012 *******************
**************************************************************************/

#include "s6b0755.h"
#include "fontdata6x8.h"

void lcd_send_cmd(unsigned short mdata);
void lcd_init(void);
void lcd_set_invert(unsigned short val);
void lcd_send_data(unsigned short mdata);

#define MAX_COLS                96
#define MAX_ROWS                65
#define MAX_PAGES               MAX_ROWS/8

#define SCAN_DIR                0b00000001   //0 for normal 1 to flip upside down
#define BLACK_ON_WHITE          0b00000000   //0000 000x 0 white text on black 1 black text on white

/******************************************************************
*************** DISPLAY COMMANDS START HERE **********************/

//***************** display carage return *******************
void display_cr(void)
{
 if(SCAN_DIR==0B00000000)
 {
   lcd_send_cmd(SET_COL_ADDR_HI | 0X00);
    lcd_send_cmd(SET_COL_ADDR_LO | 0X00);
 }
 else
 {
   lcd_send_cmd(SET_COL_ADDR_HI | 0X02);
    lcd_send_cmd(SET_COL_ADDR_LO | 0X00);
 }
}

//***** display Home ***************************
void display_home(void)
{
  display_cr();
   lcd_send_cmd(SET_PAGE_ADDR | 0X00);
}

//************* display set line *************
void display_set_line(unsigned short line)
{
  lcd_send_cmd(SET_PAGE_ADDR | (line & 0X0F));
}

//************ display set column **********
void display_set_col(unsigned short col)
{
 if(SCAN_DIR== 0X00)
 {
    lcd_send_cmd(SET_COL_ADDR_HI | (col>>4 & 0B00000111));
     lcd_send_cmd(SET_COL_ADDR_LO | (col & 0B00001111));
 }
 else
 {
    col = col+32;
     lcd_send_cmd(SET_COL_ADDR_HI | (col>>4 & 0B00000111));
      lcd_send_cmd(SET_COL_ADDR_LO | (col & 0B00001111));
 }
}

//************display initialize *****************
void display_init(void)
{
  lcd_init();
  display_home();
  if(SCAN_DIR==0)
  {
      lcd_send_cmd(SHL_SELECT | 0B00000000);
       lcd_send_cmd(ADC_SELECT | 0B00000000);
  }
  else
  {
       lcd_send_cmd(SHL_SELECT | 0B00001000);
        lcd_send_cmd(ADC_SELECT | 0B00000001);
  }
  if(BLACK_ON_WHITE)
  {
    lcd_set_invert(BLACK_ON_WHITE);
  }
}

//*************** display text *****************
void display_text(code char *addr)
{
 code const char *charptr;
 unsigned short line =0;
 unsigned short temp =0;

 display_cr();

 while (*addr != 0&& line<MAX_COLS)
 {
  charptr=((((unsigned int)*addr)-32)*6)+ charmap; // can use glcd font creator
  for(temp=0;temp<6;temp++)
  {
          lcd_send_data(*charptr++);
          line++;
  }
  *addr++;
 }
 while (line<MAX_COLS)
 {
    lcd_send_data(0X00);
    line++;
 }
}

//************ user put ** User defined stream destination ******
void display_user_putc(int cc)
{
 code const char *charptr;
 unsigned short line=0;
 unsigned short temp=0;

 if (cc != 0 && line<MAX_COLS)
 {
  charptr=((((unsigned int)cc)-32)*6)+ charmap;
  for (temp=0;temp<6;temp++)
  {
    lcd_send_data(*charptr++);
    line++;
  }
 }
}

//*********** display clear *******************
void display_clear(void)
{
  unsigned short i,j;
  display_home();
  for (i=0;i<MAX_PAGES;i++)
  {
   display_set_line(i);
   display_cr();
   for (j=0;j<MAX_COLS;j++)
   {
    lcd_send_data(0X00);
   }
  }
  display_home();
}