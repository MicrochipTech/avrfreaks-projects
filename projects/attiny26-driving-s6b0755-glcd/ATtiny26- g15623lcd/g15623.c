/****************** g15623.c ******************************
*************** ATtiny 26 *******************************
**************** communication with glcd ******************
****** LeRoy Olson *********  Mar 1 2012 *****************/


#include "s6b0755.h"
//#include "fontdata.h"

//void switch_select(void);
void display_init(void);
void switch_select();

void lcd_reset(void);
void display_cr(void);
void lcd_write(unsigned short mdata);
void lcd_set_invert(unsigned short val);
void display_clear(void);
void Display_cr(void);
void display_text(code char *addr);
void display_set_line(unsigned short line);
void display_set_col(unsigned short col);
void display_user_putc(int cc);

int cc;
unsigned int value;

void InitMain()
{
   dt3;
   dt3;
   dt3;
         display_init(void);
         display_clear();
         lcd_set_invert(0);//0 white screen black text use 1 to reverse
  dt3;
        display_set_line(1);
        display_text("LeRoy Olson");
        display_set_line(2);
        display_text("AA aa Bb Cc Dd");
        display_set_line(3);
        display_text("================");
        display_set_line(4);
        display_text("AC VOLTS PEAK");
        display_set_line(5);
        display_text("DC MILLIAMPS");
        display_set_line(6);
        display_text("NPN SILICONE");
  dt1;
   display_set_line(7);
        display_set_col(70);
        cc=45;
        display_user_putc(cc);
        display_set_line(7);
        display_set_col(77);
        cc=56;
        display_user_putc(cc);
        display_set_line(7);
        display_set_col(84);
        cc=55;
        display_user_putc(cc);
  dt3;
}

void main()
{
 LCD_DDR_SET; // Set as output Pins 0-3,and 5
 LCD_PORT_SET;  //Set Pins 0-3 and 5 low


  InitMain();                             // Perform main initialization
}


//************ lcd reset ********************
void lcd_reset(void)
{
  LCD_RESETB=0;
  LCD_CS1B=1;
  LCD_SCK=1;
  LCD_SID=1;

  dt1;
  LCD_RESETB=1;
}
//************ lcd write *******************
void lcd_write(unsigned short mdata)
{

     short lp;
     LCD_SCK=1;
     LCD_SID=1;
     dt2;
     LCD_CS1B=0;
     dt2;
    for(lp=0; lp<8; lp++)
     {
               LCD_SID=(mdata & 0B10000000)>>7;
               LCD_SCK=0;
               dt2;
               LCD_SCK=1;
               dt2;
               mdata=mdata<<0X01;
    }
     LCD_SID=1;
     LCD_CS1B=1;
}

//****************** lcd send cmd **********************
void lcd_send_cmd(unsigned short mdata)
{
  LCD_RS=0;
  lcd_write(mdata);
  LCD_RS=1;
}

//****************** lcd send data *********************
void lcd_send_data(unsigned short mdata)
{
  LCD_RS=1;
  lcd_write(mdata);
}

//***************** lcd init **************************
void lcd_init(void)
{
     lcd_reset();
     lcd_send_cmd(SOFT_RESET);
     dt1;
     lcd_send_cmd(SET_DUTY_1);
     lcd_send_cmd(SET_DUTY_2);
     dt1;

     lcd_send_cmd(SET_BIAS);
     dt1;
     //**** Application setup done *******
     lcd_send_cmd(SET_OSC_ON);
     dt1;
     lcd_send_cmd(DC_STEP_UP);
     dt1;

     lcd_send_cmd(REG_RESISTOR | REG_RESISTOR_VAL);
     dt1;

     lcd_send_cmd(SET_EVR_1);
     lcd_send_cmd(SET_EVR_2 | VOLUME_CONTROL_VAL);
     dt1;

     lcd_send_cmd(SET_BIAS);
     dt1;
     lcd_send_cmd(POWER_CONTROL);
     dt1;

     lcd_send_cmd(CLEAR_POWER_SAVE);
     dt1;
     lcd_send_cmd(DISPLAY_INVERT | DISPLAY_INVERT_VAL);
     dt1;

     lcd_send_cmd(DISPLAY_ON);
     dt1;
}

///************** lcd set invert **********************************
void lcd_set_invert(unsigned short val)
{
  lcd_send_cmd(DISPLAY_INVERT | (DISPLAY_INVERT_VAL & val));
}