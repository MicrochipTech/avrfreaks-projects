/* ********************************************************************** *
 * ResetCtl.c
 * Boiler Reset Controller
 * ********************************************************************** */

/* CPU frequency */
#define F_CPU 8000000UL

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include <util/delay.h>
#include "lcd.h"
#include "TxUSART.h"

//*  Comment Out the following line for real time build **
 #define _SIMULATE_ 1


#ifdef _SIMULATE_
# define DELAY1 1  /* Debug/Emulate Delay1 Value */
# define DEBOUNCE 12
# define QSEC 3
# define ADC_LOOP 10
# define STARTMSG "# SIMULATION START #"
#else
# define DELAY1 10  /* Full Speed Delay1 Value */
# define DEBOUNCE 250
# define QSEC 255
# define ADC_LOOP 50
# define STARTMSG "** System Startup **"
#endif


/* ** JPad (key pad) Defines *** */
#define K_UP 1
#define K_DN 4
#define K_RT 2
#define K_LT 8
#define K_OK (K_RT + K_LT)

#define MAX_EVENT_ITEM 99

// *** Menu Defines ***
#define MAX_MENU 10
#define LCD_ROW_LEN 20

const char menu_1[] PROGMEM ={
" Exit Menu Mode(Run)"
" Set Date / Time    "
" Temp. Sensor Enable"
" Status Port Setup  "
" Data Logging Enable"
" View Events        "
" Adjust ORLT Values "
" Adjust IRAT Values "
" Adjust Config Vals "
" Save Defaults      "};

const char p_msg01[] PROGMEM ={" PRESS <+> To Exit \0"};
const char p_msg02[] PROGMEM ={"UP/Down  To Change \0"};
const char p_msg03[] PROGMEM ={"Data Logging Is ON \0"};
const char p_status_types[] PROGMEM ={
"Status\0"
"DEMAND\0"
"BURNER\0"
"DHW   \0"
"ZONE  \0"
"OFF   \0"};

//** status type codes: Demand=1, Burner=2, DHW=3, Zone=4  OFF=5 **
#define STATUS_TYPE_DEMAND 1
#define STATUS_TYPE_BURNER 2
#define STATUS_TYPE_DHW 3
#define STATUS_TYPE_ZONE 4
#define STATUS_TYPE_OFF 5

#define SZ_TEMP_TBL  12  //Size of Temperature Lookup Tables
#define INC_TEMP_TBL 10  //TempTable Increment (degrees per entry)
#define MIN_TEMP1 0      //TempTable1 Starting Temperature Value
#define MIN_TEMP2 90     //TempTable2 Starting Temperature Value


/* TempTable1 = Outside Temp Lookup 0-110 degrees F */
const uint16_t TempTable1[12] PROGMEM ={826,758,690,622,554,485,415,340,275,225,185,145};
/* TempTable2 = Boiler Temp Lookup 90-200 degrees F */
const uint16_t TempTable2[12] PROGMEM ={397,344,292,240,197,163,140,118,100,85,70,61};


// ** Function Defs **
void ioinit(void);
void ReadValuesFromEEPROM(void);
void ToggleActivityLed(void);

// ** Reset Controller Functions **
void ResetControl(void);
void powerFailTest(void);
uint16_t ReadADC(uint8_t adc, uint8_t bIntRef);
void AddEventToList(void);
uint8_t ReadStatusPort(void);
uint8_t IsEventValid(void);
uint8_t CalcTemperature(uint16_t adc, uint8_t bIntRef);
/* type: Demand=1, Burner=2, DHW=3, Zone=4  OFF=5 */
uint8_t Is_Status_On(uint8_t type, uint8_t status);
uint8_t FindFirstEventInWindow(uint8_t e_ptr, uint8_t window);
uint8_t FindNextDemandEvent(uint8_t ptr);
uint8_t CalculateDemandOnDurationPct(uint8_t e_ptr, uint8_t p_ptr);
uint16_t CalcBypassLimit(uint16_t otemp, uint16_t d_pct);
void LimitOff(void);
void LimitOn(void);
//*****************************************************************

void delay_ms(unsigned int ms);
void delay_us(unsigned int us);

void timer2_init(void); /* ** Timer2 for RTC using 32.782khz crystal ** */
void timer2_stop(void);

/* ** JoyPad Functions ** */
static uint8_t scan_jpad(uint8_t dbounce);

/* ** MENU Display Functions ** */
void press_any_msg(uint8_t row);
void show_menu(uint8_t sel_item, uint8_t top_item);
void printMenuRow(uint8_t menu_item, uint8_t disp_row, uint8_t bSelected);
void show_menuitem_and_cmds(void);
uint8_t get_menu_pick(void);

/* ** MIsc. Functions  ** */
uint8_t  set_timer(uint8_t row);
uint8_t LastDayOfMonth(uint8_t month, uint8_t year);
void  inc_timer(void);
uint8_t  adjust_int_val(uint16_t *v, uint8_t *dp, uint16_t max, uint16_t min,
                        uint8_t row, uint8_t col, uint8_t bExitOnChange);
uint8_t  adjust_byte_val(uint8_t *v, uint8_t row, uint8_t col, uint8_t mode);
void to_decimal_str(uint16_t v, char *buf);
void to_binary_str(uint8_t v, char *buf);

void printStatusPortVals(uint8_t port);
void setStatusPortVals(void);
void setStatusMask(void);
void print_FlagState(uint8_t flags, uint8_t row, uint8_t col);
uint8_t setFlagValues(uint8_t flags, char *title, uint8_t row);
void showEventList(void);


/* ** Timer/Time of Day Vars. *** */
static uint8_t rtc, signal;
static uint8_t yrs, mth, day, hrs, min, sec;

static uint8_t lcd_ok;

static char msg[42];
static uint8_t EventList[6*(MAX_EVENT_ITEM+1)]; /* 6x100 items HH,MM,SS, Temp1, Temp2, Status*/

uint8_t  EEMEM EE_PGMMSG[7];  //Reserve EEPROM space for "EEPROM*"
                              //To indicate values saved
uint8_t  EEMEM EE_yrs;
uint8_t  EEMEM EE_mth;
uint8_t  EEMEM EE_day;
uint8_t  EEMEM EE_hrs;
uint8_t  EEMEM EE_min;
uint8_t  EEMEM EE_sec;


#define SZ_ORLT  15  //Size of Outside Reset Limit Table
#define MIN_ORLT 10  //First ORLT Entry   (Outside temp)
#define INC_ORLT 5   //ORLT Increments   (degrees per entry)
// Outside Reset Limit Table (boiler Max temp / outside temp )
static uint8_t ORLT[SZ_ORLT] = {
  185,185,180,180,175,160,150,150,145,145,145,140,140,140,130};
//10  15  20  25  30  35  40  45  50  55  60  65  70  75  80 (Outside temp.)

uint8_t  EEMEM EE_ORLT[SZ_ORLT];  //Reserve EEPROM space for ORLT

#define SZ_IRAT 9    //Size of Inside Reset Adjust Table
#define MIN_IRAT 10  //First IRAT Entry   (%Demand=ON)
#define INC_IRAT 10  //IRAT Increments   (%Demand per entry)
// Inside Reset Adjustment Table (Degree adjust / %On Demand)
static int8_t IRAT[SZ_IRAT] = {-20,-20,-15,-10,-5, 0, 5, 15, 20};
                               //10  20  30  40  50 60 70 80  90 (% ON Demand)

uint8_t  EEMEM EE_IRAT[SZ_IRAT]; //Reserve EEPROM space for IRAT

//** Config Value Index Values **
#define CFG_FLAGS1 0
#define CFG_FLAGS2 1
#define CFG_VALID_STATUS_READ_CT 2
#define CFG_DEMAND_AVG_WINDOW 3
#define CFG_ADC_READ_LOOP 4
#define CFG_OSCCAL 5
#define CFG_TEMP1_OFFSET 6
#define CFG_TEMP2_OFFSET 7
#define CFG_DHW_MIN_LIMIT 8
#define CFG_TOD_OFFSET 9

static uint8_t ConfigTable[10]= {
0x01,0x20,4,6,ADC_LOOP,178,0,16,140,0};

uint8_t  EEMEM EE_ConfigTable[10]; //Reserve EEPROM space for ConfigTable
uint8_t  EEMEM EE_TempEn;
uint8_t  EEMEM EE_bDataLog;
uint8_t  EEMEM EE_StatusPortVals[4];

/* Values to Be saved to EEPROM along with LimitTable*/
static uint8_t TempEn, StatusMask, bDataLog;
static uint8_t StatusPortVals[4]={0x02,0x01,0x03,0x05};
/* global vars */
static uint8_t MenuItem, TopMenuItem, bMenuRefresh;
static uint8_t status, old_status, cmp_status, status_ct, EventPtr;
static uint16_t Temp1, Temp2, Temp3, Temp4, LimitTemp;
/* ************************************************** */
/* ************************************************** */


/* ************************************************** */
/* main()                                             */
/* ************************************************** */
int main(void)
{
  uint8_t p, cmd, config_val;
  uint16_t limit_index, limit_val;
  char str[21];

  // Init Date-Time Values
   yrs=10; mth=1; day=1;
   hrs=0; min=0; sec=0;

  //** Init I/O ports and LCD. Do a startup routine ** 
  ioinit();

  p = 0;
  MenuItem=1;
  TopMenuItem = 1;
  bMenuRefresh = 1;

  limit_index = 0;
  limit_val = 0;
  config_val = 0;


  Temp1=Temp2=Temp3=Temp4=0;
  TempEn = 0x03;   /* Default = Temp1 & Temp2 Enabled */
  setStatusMask(); /* Default = Show DHW, Demand, Boiler Status */
  bDataLog = 0;    /* Default Data Logging OFF */

  ReadValuesFromEEPROM();

  OSCCAL = ConfigTable[CFG_OSCCAL]; //** 178 Determined Experimentally **

  EventPtr = 0;
  memset(EventList, 0, sizeof(EventList));

 /* *** Display LCD Startup Message for 2 seconds *** */
  lcd_clrscr();
  lcd_puts(STARTMSG);
  delay_ms(QSEC*8);

  timer2_init(); /* ** Start RTC Timer ** */

  /* Init status compare values and counts */
  signal=0;
  old_status = status;
  cmp_status = status;
  status_ct = 0;

  /** Run The Reset Controller Program **/
  /** until user presses select (<+>)  **/
  ResetControl();
  #ifdef _SIMULATE_
   timer2_stop(); /* ** Stop RTC Timer ** */
  #endif

  /* *** Option Menu Routine  ********************** */
  delay_ms(QSEC);
  while( scan_jpad(DEBOUNCE)!=0);  /* Wait For No Key Press */
  bMenuRefresh = 1;

  while(1)
  {
   while(scan_jpad(DELAY1)!=0); /* Wait for Key Release */

   if(bMenuRefresh)
     show_menu(MenuItem, TopMenuItem);  /* Display the menu */

   MenuItem = get_menu_pick(); /* Wait for user menu choice */

   switch (MenuItem)           /* Process menu selection */
     {
      /******************************************/
      /* *** Exit Menu (RUN ResetControl() **** */
      /******************************************/
      case 1: /* Exit Menu Mode (Run ResetControl() ) */
         #ifdef _SIMULATE_
           timer2_init(); /* ** Stop RTC Timer ** */
         #endif
         ResetControl();
         #ifdef _SIMULATE_
           timer2_stop(); /* ** Stop RTC Timer ** */
         #endif

         delay_ms(QSEC);
         while( scan_jpad(DEBOUNCE)!=0);  /* Wait For No Key Press */
         bMenuRefresh = 1;
         MenuItem =1;
      break;

      /******************************************/
      /* *** SET TIME/DATE ******************** */
      /******************************************/
      case 2: /* Menu Item #1:Set Time/Date */
         lcd_clrscr();
         printMenuRow(MenuItem, 0, 0);
         timer2_stop();

         lcd_goto_rc(3,0);
         lcd_puts_p(p_msg01); /* " PRESS <+> To Exit" */

         set_timer(1);

         timer2_init();
         MenuItem =2;
         bMenuRefresh = 1;
      break;

      /******************************************/
      /* *** TEMP SENSOR ENABLE *************** */
      /******************************************/
      case 3: /* Temp Sensor Enable */
         show_menuitem_and_cmds();

         TempEn = setFlagValues(TempEn, "Temp  ", 1);

         delay_ms(QSEC);
         while( scan_jpad(DEBOUNCE)!=0);  /* Wait For No Key Press */
         bMenuRefresh = 1;
         MenuItem =3;
      break;

         /******************************************/
         /*****  STATUS PORT SETUP  ****************/
         /******************************************/
      case 4: /* Status Port Setup  */
         show_menuitem_and_cmds();

         setStatusPortVals();
         setStatusMask();

         delay_ms(QSEC);
         while( scan_jpad(DEBOUNCE)!=0);  /* Wait For No Key Press */
         bMenuRefresh = 1;
         MenuItem =4;
      break;

      /******************************************/
      /*****  Data Logging On/Off  **************/
      /******************************************/
      case 5: /* Data Logging On/Off */
         lcd_clrscr();
         printMenuRow(MenuItem, 0, 0);

         lcd_goto_rc(2,0);
         if(bDataLog)
           {
            strcpy_P(msg, p_msg03);
            strcpy((char*)&msg[17],"FF");
            lcd_puts(msg);  /* "Data Logging Is OFF" */
            bDataLog = 0;
            sprintf(msg, "%02d%02d %02d:%02d:%02d END DATA REPORTING\r\n", mth,day,hrs,min,sec);
           }
         else
          {
           bDataLog=1;
           lcd_puts_p(p_msg03);  /* "Data Logging Is ON " */
           sprintf(msg, "%02d%02d %02d:%02d:%02d BEGIN DATA REPORTING\r\n", mth,day,hrs,min,sec);
           USART_Init();
          }

         p=0;
         while(msg[p]!=0)
           USART_putchar(msg[p++]);

         delay_ms(QSEC*8);
         while( scan_jpad(DEBOUNCE)!=0);  /* Wait For No Key Press */
         bMenuRefresh = 1;
         MenuItem =5;
      break;

      /******************************************/
      /********   VIEW EVENTS      **************/
      /******************************************/
      case 6: /* View Events */
         show_menuitem_and_cmds();

         delay_ms(QSEC*8);

         lcd_clrscr();
	      showEventList();
	
         delay_ms(QSEC);
         while( scan_jpad(DEBOUNCE)!=0);  /* Wait For No Key Press */
         bMenuRefresh = 1;
         MenuItem =6;
      break;

      /******************************************/
      /*****  Adjust ORLT Values   **************/
      /******************************************/
      case 7: /* Set Outside Reset Limit Table (ORLT) Values */
         show_menuitem_and_cmds();

         limit_index = 5;
         p=0;
         cmd=K_LT;

         while(cmd != K_OK)
            {
             if((cmd==K_UP)||(cmd==K_DN)||(cmd==K_LT))
               {
                limit_val = ORLT[(uint8_t)limit_index];
                sprintf(msg, "%02d %03d ",limit_index, limit_val);
                lcd_goto_rc(1,3);
                lcd_puts(msg);
                cmd = adjust_int_val(&limit_index, &p, SZ_ORLT-1, 0, 1, 4, 1);
               }

             if(cmd==K_RT)
               {
                p=0;
                limit_val = ORLT[(uint8_t)limit_index];
                cmd = adjust_int_val(&limit_val, &p, 250, 100, 1, 8, 0);
                ORLT[(uint8_t)limit_index]=(uint8_t)limit_val;
                p=0;
               }

            }

         delay_ms(QSEC);
         while( scan_jpad(DEBOUNCE)!=0);  /* Wait For No Key Press */
         bMenuRefresh = 1;
         MenuItem =7;
      break;

      /******************************************/
      /*****  Adjust IRAT Values   **************/
      /******************************************/
      case 8: /* Set Inside Reset Adjust Table (IRAT) Values */
         show_menuitem_and_cmds();

         limit_index = 5;
         p=0;
         cmd=K_LT;

         while(cmd != K_OK)
            {
             if((cmd==K_UP)||(cmd==K_DN)||(cmd==K_LT))
               {
                limit_val = IRAT[(uint8_t)limit_index];
                sprintf(msg, "%02d %03d ",limit_index, limit_val);
                lcd_goto_rc(1,3);
                lcd_puts(msg);
                cmd = adjust_int_val(&limit_index, &p, SZ_IRAT-1, 0, 1, 4, 1);
               }

             if(cmd==K_RT)
               {
                p=0;
                limit_val = IRAT[(uint8_t)limit_index];
                cmd = adjust_int_val(&limit_val, &p, 100, 10, 1, 8, 0);
                IRAT[(uint8_t)limit_index]=(uint8_t)limit_val;
                p=0;
               }

            }

         delay_ms(QSEC);
         while( scan_jpad(DEBOUNCE)!=0);  /* Wait For No Key Press */
         bMenuRefresh = 1;
         MenuItem =8;
      break;

      /******************************************/
      /*****  ADJUST CONFIG VALS  ***************/
      /******************************************/
      case 9: /* Adjust Config Vals */
         show_menuitem_and_cmds();

         limit_index = 0;
         p=0;
         cmd=8;

         while(cmd != 10)   /* Exit Key = 10 */
            {
             if((cmd==K_UP)||(cmd==K_DN)||(cmd==K_LT))
               {
                config_val = ConfigTable[(uint8_t)limit_index];
                sprintf(msg, "%02d H:%02X D:%03d ",limit_index, config_val, config_val);
                lcd_goto_rc(1,3);
                lcd_puts(msg);
                cmd = adjust_int_val(&limit_index, &p, 9, 0, 1, 4, 1);
               }


             if(cmd==K_RT)
               {
                p=0;
                config_val = ConfigTable[(uint8_t)limit_index];
                cmd = adjust_byte_val(&config_val, 1, 9, 1);
                ConfigTable[(uint8_t)limit_index]=config_val;
                p=0;
               }

            }
         delay_ms(QSEC);

         OSCCAL = ConfigTable[CFG_OSCCAL]; //** 178 Determined Experimentally **

         while( scan_jpad(DEBOUNCE)!=0);  /* Wait For No Key Press */
         bMenuRefresh = 1;
         MenuItem =9;
      break;

      /******************************************/
      /*****  SAVE DEFAULT VALUES  **************/
      /******************************************/
      case 10: /* Save Defaults  */
         show_menuitem_and_cmds();
         strcpy(msg, "EEPROM*");
         eeprom_write_block((const void *)&msg, (void *)&EE_PGMMSG, 7);
         eeprom_write_block((const void *)&ORLT, (void *)&EE_ORLT, SZ_ORLT);
         eeprom_write_block((const void *)&ORLT, (void *)&EE_IRAT, SZ_IRAT);
         eeprom_write_block((const void *)&ConfigTable, (void *)&EE_ConfigTable, 10);
         eeprom_write_block((const void *)&StatusPortVals, (void *)&EE_StatusPortVals,4);
         eeprom_write_byte(&EE_TempEn, TempEn);
         eeprom_write_byte(&EE_bDataLog, bDataLog);
         eeprom_write_byte(&EE_yrs, yrs);
         eeprom_write_byte(&EE_mth, mth);
         eeprom_write_byte(&EE_day, day);
         eeprom_write_byte(&EE_hrs, hrs);
         eeprom_write_byte(&EE_min, min);
         eeprom_write_byte(&EE_sec, sec);

         lcd_goto_rc(1,0);
         lcd_puts(" Done...");
         delay_ms(QSEC*4);
         while( scan_jpad(DEBOUNCE)!=0);  /* Wait For No Key Press */
         bMenuRefresh = 1;
         MenuItem =10;
      break;

      default:   /* Should never go here... */
         lcd_clrscr();
         lcd_goto_rc(1,6);
         sprintf((char *)&str, "Menu Item = %d  ",MenuItem);
         lcd_puts(str);
         press_any_msg(3);
         while( scan_jpad(DEBOUNCE)==0); /* Wait for keypress */

         lcd_clrscr();
         MenuItem = 0;
      break;
     } /* END switch(MenuItem) */

  } /* END while(1)*/

} /* END main() */
/* **************************************************************** */
/* **************************************************************** */

/* **************************************************************** */
/* ioinit()   Do all the startup-time peripheral initializations.   */
/* **************************************************************** */
void ioinit(void)
{
 uint8_t x, c;

 MCUCSR |= _BV(JTD); /* Disable JTAG so PORTC can be used for I/O */
 MCUCSR |= _BV(JTD);
 DDRA = 0x00;       /* PORTA 0-3 ADC  ** PORTA 4-7 Sense Inputs */
 PORTA= 0xf0;       /* PULL UP Sense Inputs */

 DDRC = 0x0f;       /* PORTC 0-3 RELAY DRIVE (OUTPUT) */
 PORTC =0x00;       /* ALL RELAYS OFF (PINS=LOW) */

 /** Read Status bits from PORTA4-7 and PORTC0-3 **/
 status = (PINA >> 4);
 status &= StatusMask;
 status |= (PORTC << 4);

 delay_us(QSEC);    /* 25ms */

 lcd_ok = lcd_setup();

 c=5;
 if(!lcd_ok)
   c=10;
 else
   lcd_clrscr();

 /* Flash PORTC-BIT3 5 Times (10 if LCD Init fails) */
 for(x=0; x<c; x++)
    {
     ToggleActivityLed();
     delay_ms(QSEC);

     if(lcd_ok)
        lcd_putc('1' + x);

     ToggleActivityLed();
     delay_ms(QSEC);
    }
}

/* *********************************************************** */
/* ReadValuesFromEEPROM(void)                                  */
/* Read Saved Values from EEPROM (if any)                      */
/* *********************************************************** */
void ReadValuesFromEEPROM(void)
{
  strcpy(msg, "XXXXXX");
  eeprom_read_block (&msg, (void *)&EE_PGMMSG, 7);
  msg[7]=0;
  if(strcmp(msg, "EEPROM*")==0)
    {
     eeprom_read_block(&ORLT, (void *)&EE_ORLT, SZ_ORLT);
     eeprom_read_block(&ORLT, (void *)&EE_IRAT, SZ_IRAT);
     eeprom_read_block(&ConfigTable, (void *)&EE_ConfigTable, 10);
     eeprom_read_block(&StatusPortVals, (void *)&EE_StatusPortVals,4);
     TempEn = eeprom_read_byte(&EE_TempEn);
     bDataLog = eeprom_read_byte(&EE_bDataLog);
     yrs = eeprom_read_byte(&EE_yrs);
     mth = eeprom_read_byte(&EE_mth);
     day = eeprom_read_byte(&EE_day);
     hrs = eeprom_read_byte(&EE_hrs);
     min = eeprom_read_byte(&EE_min);
     sec = eeprom_read_byte(&EE_sec);

     lcd_goto_rc(1,0);
     lcd_puts("Got Data From EEPROM");
     delay_ms(QSEC*4);
    }
}


/* *********************************************************** */
/* ToggleActivityLed(void)                                     */
/* Turn the LED on PORTC BIT3 ON/OFF                           */
/* *********************************************************** */
void ToggleActivityLed(void)
{ /* ** Toggle PORTC BIT3 ** */
 if(PORTC & 0x08)
   PORTC &= 0xF7;
 else
   PORTC |=0x08;
}
/* *********************************************************** */
/* LimitOn()                                                   */
/* Energize Bypass Relay to Close Burner Circuit               */
/* *********************************************************** */
void LimitOn(void)
{ /* ** PORTC BIT1=1 ** */
  PORTC |=0x02;
}

/* *********************************************************** */
/* LimitOff()                                                  */
/* De-Energize Bypass Relay to Open Burner Circuit             */
/* *********************************************************** */
void LimitOff(void)
{ /* ** PORTC BIT1=0 ** */
  PORTC &=0xFD;
}

/* *********************************************************** */
/* ResetControl()                                              */
/* The main ResetControl routine - Runs until select is pressed*/
/* *********************************************************** */
void ResetControl(void)
{
 uint8_t i, dpct, e_ptr, p_ptr;
 uint16_t adcv1, adcv2, low_temp2, delta_t, dhw_min;
 char buf[21];
 memset(buf, 0, sizeof(buf));
 dpct=0;

 //** low_temp2 =lowest possible value of boiler temp reading **
 low_temp2 = (MIN_TEMP2-1) + ConfigTable[CFG_TEMP2_OFFSET];

 //** get DHW Minimum Limit Temp from config array
 dhw_min = ConfigTable[CFG_DHW_MIN_LIMIT];

 lcd_clrscr();
 lcd_command(LCD_DISP_ON);   /* LCD Cursor Off */

 sprintf(buf, "%02d/%02d/%02d %02d:%02d:%02d ", mth, day, yrs, hrs,min,sec);
 lcd_writestr((char*)&buf,0,1);

 lcd_goto_rc(3,0);
 lcd_puts(" PRESS <+> For Menu");
 old_status = status & 0x0f;


 while( scan_jpad(DELAY1)!=10)   /* while Select is NOT Pressed */
      {
        if(rtc==0) /* Display Time at each rtc reset */
          {
           rtc++;
           /* ** Test for Power Fail/Recovery ***/
           powerFailTest();

           /** Display Date & Time **/
           sprintf(buf, "%02d/%02d/%02d %02d:%02d:%02d ", mth, day, yrs, hrs,min,sec);
           lcd_writestr(buf,0,1);

           /** DEMO: ***/
           // PORTC = 0;
           /** END DEMO **/

           /** Display Temperatures from ADC0 & ADC1 **/
           //** Temp1 = OUTSIDE Temperature **//
           adcv1 = ReadADC(0,0);
           Temp1 = CalcTemperature(adcv1, 0);
           Temp1 += ConfigTable[CFG_TEMP1_OFFSET];

           //** Temp2 = BOILER Temperature **//
           adcv2 = ReadADC(1,1);
           Temp2 = CalcTemperature(adcv2, 1);

           /* Since the boiler temp sensor is not actually inside the boiler */
           /* we need to add a configurable offset value to approximate the  */
           /* actual boiler water temp. (determined experimentally)          */
           Temp2 += ConfigTable[CFG_TEMP2_OFFSET];

           if(ConfigTable[CFG_FLAGS1] & 0x01)   //Display temperatures
             {
              sprintf(buf, " T%3d T%3d%2d%% L%3d ", Temp1, Temp2, dpct, LimitTemp);
              buf[10]=' ';
             }
           else                                //Display RAW ADC values
              sprintf(buf, " %04d %04d %03d %03d  ", adcv1, adcv2, dpct, LimitTemp);

           lcd_writestr(buf,1,0);

           /** Read Status bits from PORTA4-7 and PORTC0-3 **/
           status = ReadStatusPort();
           status = (status >> 4);
           status &= StatusMask;
           status |= (PORTC << 4);
           status &= 0x7F; // Mask off Activity LED bit

           /* ** Display the status bits */
           sprintf(buf, "Status: 00000000");
           to_binary_str(status, (char *)&buf[8]);

           /* Insert space after bit#4 */
           for(i=16; i>11; i--)
               buf[i+1]=buf[i];
           buf[12]=' ';

           lcd_writestr(buf,2,1);

           /* *********** Detect Events ************ */
           if(IsEventValid())
             {/* *** Act on Valid Events ***** */
              /* Log The Valid Event to event list & inc. EventPtr */
              e_ptr = EventPtr;  /* Save event pointer */
              AddEventToList();

              /* ****************Process Valid Events ****************** */
              /* ************ Calculate Demand Percent Factor ********** */
              p_ptr =FindFirstEventInWindow(e_ptr, ConfigTable[CFG_DEMAND_AVG_WINDOW]);

              if(p_ptr <= MAX_EVENT_ITEM)
                   dpct = CalculateDemandOnDurationPct(e_ptr, p_ptr);


              /* **** If enabled - Log The Valid Event to serial port ***/
              if(bDataLog)
                {
                 sprintf(msg,"%02d%02d %02d:%02d:%02d, %03d, %03d, %02X, %03d, %03d %04d, %04d\r\n",mth,day,hrs,min,sec,Temp1,Temp2, status, dpct, LimitTemp, adcv1, adcv2);
                 i=0;
                 while(msg[i]!=0)
                    USART_putchar(msg[i++]);
                }
             }

             /* ******************************************************** */
             /* Boiler Bypass based on LimitTemp and Boiler Temp.        */
             /* ******************************************************** */
             LimitTemp = CalcBypassLimit(Temp1, dpct);

             //** Check for DHW Min Limit **
             if(Is_Status_On(STATUS_TYPE_DHW, status))
               {
                if(dhw_min > LimitTemp)
                    LimitTemp = dhw_min;
               }

             //** delta_t = degrees below LimitTemp at which burner will turn on again
             delta_t = (LimitTemp/10)-5;


             // LimitTemp cannot be lower than low_temp2
             if(LimitTemp <= low_temp2)
                 LimitTemp = low_temp2+1;

             if(Temp2 >= LimitTemp)
               LimitOff(); //De-Energize Bypass Relay to Open Burner Circuit
             else
               {
                if((Temp2 < (LimitTemp - delta_t))||(Temp2 <= low_temp2))
                   LimitOn();  //Energize Bypass Relay to Close Burner Circuit
               }

             /* ******************************************************** */

             ToggleActivityLed();  //** Toggle PORTC BIT3 **//

          } /* END:if(rtc==0) */

      } /*END:while( scan_jpad)*/

 lcd_command(LCD_DISP_ON_CURSOR);   /* LCD Cursor On */
}

/* ***************************************************** */
/*  ReadADC(adc, bIntRef)                                */
/* ***************************************************** */
uint16_t ReadADC(uint8_t adc, uint8_t bIntRef)
{
 uint32_t v_avg;
 uint16_t v_in;
 uint8_t x, Old_DDRA, Old_PORTA, adc_loop;

 Old_DDRA  = DDRA;     // Save PORTA DDR & Value
 Old_PORTA = PORTA;
 DDRA      = 0x00;     //* All Pins Ar INPUT *//
 PORTA     = 0xf0;     //* High 4 Bits = Pull Up ** Low 4 Bits=Tri-State *//

 v_avg=0;
 adc_loop = ConfigTable[CFG_ADC_READ_LOOP];


  ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // Set ADC prescalar to 128 - 62.5KHz sample rate @ 8MHz

  //ADCSRA |= _BV(ADPS2) | _BV(ADPS1); // Set ADC prescalar to 64


 if(bIntRef)
    ADMUX  = _BV(REFS1) | _BV(REFS0); // Set ADC reference INT 2.56v ref. (Clear Other Bits)
 else
    ADMUX  = _BV(REFS0); // Set ADC reference to AVCC(+5.0v) (Clear Other Bits)

 //ADMUX  |= _BV(ADLAR); // Left adjust ADC result to allow easy 8 bit reading
 // comment out above line for 10bit mode **

 ADMUX  |= adc;

 // ADC is set to Free-Running Mode by default.
 ADCSRA |= _BV(ADEN); // Enable ADC
 ADCSRA |= _BV(ADSC); // Start A2D Conversions

 for(x=0; x < adc_loop; x++)
    {
     /* *** Read the ADC Port *** */
     ADCSRA |= (1 << ADSC); /* Start A2D Conversions */

     while( ADCSRA & (1 << ADSC) ); /* Wait For ADC to complete */

     //delay_ms(1);
     v_in = ADCL;
     v_in |= (ADCH<<8);

     v_avg += v_in;
    }

 v_avg= v_avg/adc_loop;  /* Average The Totals */

 DDRA  = Old_DDRA;  // Restore PORTA DDR & Value
 PORTA = Old_PORTA;

 return (uint16_t)v_avg;
}

/* ***************************************************** */
/*  ReadStatusPort()                                     */
/*  Read the data on PORTA for a specified number of     */
/*  timer ticks. If any bits read '0' during this time   */
/*  return a '0' value for that bit in the result        */
/*  We need to do this because the status opto-isolators */
/*  are looking at 60 HZ AC signal so they are either    */
/*  full OFF or pulsing ON/OFF at the 60HZ AC line freq. */
/*  REMEMBER: opto LED ON= logic '0' output; OFF= "1"    */
/* ***************************************************** */
uint8_t ReadStatusPort(void)
{
 uint8_t v, t2, valid_read_count;

 // Get number of ticks to read from config array (default=4)//
 valid_read_count = ConfigTable[CFG_VALID_STATUS_READ_CT];

 v = 0xff;      // Default return = ALL BITS ='1'

 while(TCNT2 > 251);  // at the time we're readin this TCNT2 should have
                      // just been reset but if not, wait for it to overflow

 t2 = TCNT2;    // Get current value of tick counter (1/128sec. per count)
 t2 +=valid_read_count;  // set limit to TCNT2+ VALID_READ_COUNT

 while(TCNT2 < t2)       // Read PINA while clock ticks...
    v = (PINA & v);      // AND with previous reading to reflect any
                         // bits that read '0' during this period.
 return v;
}

// ************************************************************************
//  CalcTemperature(adc, bIntRef)
//  Find the first matching or lower acd value in the appropriate lookup table.
//  If not a matching value then interpolate the difference(in degrees F).
//  Return a temperature value (in degreees F)
//  NOTE: The thermistors used for this project have a negative temp
//  coefficent (resistance lowers as temp increases) so tables values go from
//  higest (adc) value to lowest (adc) value.
// ************************************************************************
uint8_t CalcTemperature(uint16_t adc, uint8_t bIntRef)
{
 uint8_t idx, ret_val, tblStartVal;
 uint16_t Tbl[SZ_TEMP_TBL];
 uint16_t diff1, diff2;

 // If using the 2.56 internal reference then use lookup table TempTable2
 if(bIntRef)
   {
    ret_val = MIN_TEMP2 + (INC_TEMP_TBL * (SZ_TEMP_TBL-1))+1;  // Default return value=higest possible reading
    tblStartVal = MIN_TEMP2;     // First temp value (each table entry=10 degrees)
    memcpy_P(Tbl, TempTable2, SZ_TEMP_TBL * sizeof(uint16_t)); // Table2 110-200 degrees
   }
 else
   {
    ret_val = MIN_TEMP1 + (INC_TEMP_TBL * (SZ_TEMP_TBL-1))+1;  // Default return value=higest possible reading
    tblStartVal = MIN_TEMP1;     // First temp value (each table entry=10 degrees)
    memcpy_P(Tbl, TempTable1, SZ_TEMP_TBL * sizeof(uint16_t)); // Table1 10-100 degrees
   }

 if(Tbl[0] < adc)           // Reading is out of range - return default value
    return tblStartVal-1;

 for(idx=1; idx<SZ_TEMP_TBL; idx++)  // Check each table entry
   {
    if(Tbl[idx] <= adc)     // if this one is lower or matching...
      {
       diff1 = (adc - Tbl[idx])*INC_TEMP_TBL;  // calc difference (if any)
       diff2 = (Tbl[idx-1] - Tbl[idx]);        // get adc value range for this step
       ret_val = tblStartVal;                  // calc temperature value
       ret_val += (INC_TEMP_TBL*idx);
       if(diff1 > 0)                           // subtract difference (if any)
          ret_val -= (uint8_t)(diff1 / diff2);
       break;
      }
   }

 return ret_val;
}

/* *********************************************************************** */
/* IsEventValid()                                                          */
/* When status changes, determine if next 5 readings (1 per sec.) match    */
/* the new value. If so then return 1 (true)  otherwise return 0 (false)   */
/* *********************************************************************** */
uint8_t IsEventValid(void)
{
 if((status & 0x0f) != old_status)  /* status value changed */
   {
    if(status_ct==0)                /* new event */
      {
       cmp_status = status & 0x0f;   /* save test status */
       status_ct = 5;                /* reset test counter */
      }
    else
      {
       if(cmp_status != (status & 0x0f)) /* status doesn't match test value */
         status_ct = 0;                  /* not a valid event -- rest test counter */
       else                              /* if status still matches test value */
         {
          if(--status_ct == 0)            /* if we got 5 matching values */
            {
             old_status = cmp_status;
             return 1;                    /* event is valid */
            }
          }
      }
   }
 return 0;
}

/* *********************************************************************** */
/* AddEventToList()                                                        */
/* Add Event data to EventList[] and increment EventPtr                    */
/* *********************************************************************** */
void AddEventToList(void)
{
 uint16_t ep;
 ep = EventPtr * 6;
 EventList[ep]   = hrs;
 EventList[++ep] = min;
 EventList[++ep] = sec;
 EventList[++ep] = (uint8_t)Temp1;
 EventList[++ep] = (uint8_t)Temp2;
 EventList[++ep] = status;

 EventPtr++;
 if(EventPtr > 99)
    EventPtr=0;
}

/* *********************************************************************** */
/* Is_Status_On(type, status)                                              */
/* type: Demand=1, Burner=2, DHW=3, Zone=4  OFF=5                          */
/* Find the specified status bit based on config settings and compare its  */
/* value with the "ON" logic setting from config settings.                 */
/* RETURN 1 if status bit matches configured "ON" state.                   */
/* *********************************************************************** */
uint8_t Is_Status_On(uint8_t type, uint8_t status)
{
 uint8_t bit, mask, val, on_val;
 mask=0x01;  // mask for bit=0;
 for(bit=0; bit<5; bit++)
    {
     on_val=StatusPortVals[bit];
     if((on_val & 0x0f)==type)
       break;
     mask = mask <<1;  //next mask bit pos.
    }

 if(bit > 3)   //error type not found or set = OFF
    return 0;

 val = status & mask;
 on_val &= 0x10;  // get logic ON= setting from StatusPortVals

 if((on_val==0)&&(val==0))  //status bit matched logic setting of 0
    return 1;

 if((on_val!=0)&&(val!=0))  //status bit matched logic setting of 1
    return 1;

 return 0;
}

/* *********************************************************************** */
/* FindFirstEventInWindow(e_ptr, window)                                   */
/* We are looking backward from the most recent event in the event list    */
/* for the first event within a configurable time windows (default=6hrs).  */
/* e_ptr = current event index, window = # of hours to look backwards      */
/* Returns: The event list index of the first entry inside the time window */
/* *********************************************************************** */
uint8_t FindFirstEventInWindow(uint8_t e_ptr, uint8_t window)
{
 uint8_t p_ptr, x;
 uint16_t ep, cur_time, prev_time, diff_time, window_time;

 p_ptr= e_ptr;  // set previous event pointer = current event pointer
 ep = e_ptr *6; // calculate current event offset
 cur_time = (EventList[ep]*60) + EventList[ep+1];

 window_time = window*60;

 for(x=0; x<MAX_EVENT_ITEM; x++) //only search (eventlist size -1) items
   {
    if(p_ptr==0)  // decrement the previous event pointer
      p_ptr=MAX_EVENT_ITEM;
    else
      p_ptr--;

    ep=p_ptr * 6;  // calculate previous event offset
    prev_time = (EventList[ep]*60) + EventList[ep+1];
    if((prev_time==0)&&(EventList[ep+4]==0))  //** empty event list item = end of data **
       break;                                 // just stop searcing and use this range **

    if(prev_time > cur_time)
      cur_time+=(24*60);  //** adjust for passing midnight **

    diff_time = cur_time - prev_time; //** calculate time difference **

    if(diff_time > window_time)  // found first event just outside the time window
      break;
   }

 if(x >= MAX_EVENT_ITEM)    //** ERROR: Searched entire list and could not   **
   return MAX_EVENT_ITEM+1; //** find event within the specified time window **


 // *** Back up 1 event ***
 if(p_ptr==MAX_EVENT_ITEM)
   p_ptr=0;
 else
   p_ptr++;

 return p_ptr;
}

/* *********************************************************************** */
/* CalculateDemandOnDurationPct(e_ptr, p_ptr)                              */
/* e_ptr = current event index, p_ptr= index of 1st event inside window    */
/* Returns: the percent of demand ON vs demand OFF times within the window */
/* *********************************************************************** */
uint8_t CalculateDemandOnDurationPct(uint8_t e_ptr, uint8_t p_ptr)
{
  uint16_t op;
  uint8_t hrs, min, status, d1, d2, ptr;
  long t1, t2, dt, d_on, d_off;

  ptr=p_ptr;

  d_on=0;
  d_off=0;

  //** Get first item data **
  op=(ptr*6);
  hrs = EventList[op];
  min = EventList[op+1];
  status = EventList[op];
  d2 = Is_Status_On(STATUS_TYPE_DEMAND, status);
  t2= (hrs*60 + min);

  while(ptr != e_ptr)
      {
       //** use previous item as first item
       d1=d2;
       t1=t2;

       //** advance event index ***
       if(ptr==MAX_EVENT_ITEM)
         ptr=0;
       else
         ptr++;

       //** Get next item data and demand value **
       op=(ptr*6);
       hrs = EventList[op];
       min = EventList[op+1];
       status = EventList[op+5];
       d2 = Is_Status_On(STATUS_TYPE_DEMAND, status);
       t2= (hrs*60 + min);

       if(t2 < t1)
         t2+=(24*60);

       dt = t2-t1;
       if(d1==1) //** First Item Demand was ON **
         d_on+=dt;
       else
         d_off+=dt; //** First Item Demand was OFF **

      } // ** process next event item **


   if(d_on==0) //** not enough data in event list ***
     return 0;

   if((d_on>0) && (d_off==0)) //** not enough data in event list ***
     return 100;

  //** Calculate demand ON/OFF time percentage for the specified event list window **
  dt= (d_on + d_off);
  d_on = (d_on * 100);


  return (uint8_t)((int)(d_on/dt));
}

/* *********************************************************************** */
/* CalcBypassLimit(otemp, dpct)                                            */
/* Calculate the boiler limit using Outside Temp (otemp) and Demand% (dpct)*/
/* using Outside Reset Limit Table(ORLT) and Inside Reset Adj.Table (IRAT) */
/* Returns: the boiler limit (in degrees F)                                */
/* *********************************************************************** */
uint16_t CalcBypassLimit(uint16_t otemp, uint16_t d_pct)
{
  int8_t limit, adj;
  uint8_t index;

  limit = otemp - MIN_ORLT;   // Subtract Minimum Outside Reset Limit Temp
  index = (uint8_t) limit/INC_ORLT;  // Convert degrees into an offset index
  if(index < SZ_ORLT)         // If in range  get limit temp
     limit = ORLT[index];
  else
     limit = ORLT[SZ_ORLT-1];  // Out of range - use MAX limit value

  adj=0;
  if((d_pct >9)&&(d_pct < 99))  // If DemandOnPercent >9% and <99%
    {
     adj = d_pct - MIN_IRAT;   // Subtract Minimum Inside Reset Adjust Temp
     index = (uint8_t) adj/INC_IRAT;  // Convert percent into an offset index
     if(index < SZ_IRAT)           // If in range get adjustment value.
        adj = IRAT[index];
     else
        adj = 0;                  // out of range = No Adjustment
    }

  limit+=(adj);    // Add inside adjustment to outside limit temp
                   // NOTE: adjustment may be a negative number.

  return (uint8_t)limit;
}

/* *********************************************************** */
/* powerFailTest()                                             */
/* Test PORTB BIT7. if=0 The Main Power has failed.            */
/* try again every 1/4 second till it becomes active again.    */
/* *********************************************************** */
void powerFailTest(void)
  {
   DDRB=0x7f;
   PORTB=0x00;  /* Disable LCD */

   if((PINB & 0x80)==0)  /* Power Fail */
     {
      PORTC=00; /** Disable Relays & Activity LED */
      while((PINB & 0x80)==0) /* do until power returns */
           delay_ms(QSEC);

      lcd_ok = lcd_setup();   /* reset/test LCD */
      lcd_puts("Power Recovered"); /* show a quick message */
      delay_ms(QSEC*4);
     }

   lcd_ok=1;
  }


/* *********************************************************** */
/* scan_jpad() Read & debounce the JoyPad switches.            */
/* Return 1=UP 2=RIGHT 4=DOWN  8=LEFT                          */
/* K_UP=1  K_DN=4 K_RT=2 K_LT=8 K_OK=10                        */
/* combine bits if multiple keys pressed.                      */
/* *********************************************************** */
uint8_t scan_jpad(uint8_t dbounce)
{
 uint8_t r, v1, v2, b, loop, match;

 if(dbounce > 16)
   b = dbounce >>2;   /* Use 1/4 debounce for # of repeated matching reads */
 else
  {
   dbounce = 16;     /* Set Minimum working values */
   b = 4;
  }

  r=0;
  DDRD = 0x08;   // ** Make PD5=INP, PD4=INP, PD3=LOW, PD2=INP **
  PORTD |= (_BV(PD2) | _BV(PD4) | _BV(PD5));
  PORTD &= ~_BV(PD3);  // ** Make PD5=1 PD4=1  PD3=0  PD2=1 **
  //PORTD = 0x34;

  match=0;
  v1 = PIND & 0x3c;  /* Mask for Bits 2,3,4,5 = 0x3C */
  for( loop=0; loop < dbounce; loop++)  /* debounce = max # of tries */
    {
     v2 = PIND & 0x3c;
     if(v2 == v1)
       {
        if(match++ == b)   /* got series of matching port reads */
           break;
       }
     else
       {
        match=0;          /* port reads don't match - try again */
        v1=v2;
       }
    }

  if(loop == dbounce)
    return(0xff);       /* read error */

  /* *** Translate to bit values *** */
  if((v2 & _BV(PD4)) == 0)
     r |= K_UP;  /*(1) Up (SW1)*/

  if((v2 & _BV(PD5)) == 0)
     r |= K_LT;  /*(8) Left (SW4)*/

  DDRD = 0x04;   // ** Make PD5=INP, PD4=INP, PD3=INP, PD2=LOW **
  PORTD |= (_BV(PD3) | _BV(PD4) | _BV(PD5));
  PORTD &= ~_BV(PD2);  // ** Make PD5=1, PD4=1, PD3=1, PD2=0 **
  //PORTD=0x38;

  match=0;
  v1 = PIND & 0x3c;
  for( loop=0; loop < dbounce; loop++)  /* debounce = max # of tries */
    {
     v2 = PIND & 0x3c;
     if(v2 == v1)
       {
        if(match++ == b)
           break;          /* got series of matching port reads */
       }
     else
       {
        match=0;         /* port reads don't match - try again */
        v1=v2;
       }
    }

  if(loop == dbounce)
    return(0xff);    /* read error */

  /* *** Translate to bit values *** */
  if((v2 & _BV(PD4)) == 0)  /* (2) Right (SW2)*/
     r |= K_RT;

  if((v2 & _BV(PD5)) == 0)  /* (4) Down (SW3)*/
     r |= K_DN;

  if((v2 & _BV(PD3)) == 0)  // ** This is for simulation only **//
     r = K_OK;              // ** since we cant' press two buttons at **//
                            // ** the same time. Simulates pressing   **//
                            // ** Left + Right buttons  (see K5 config.)** //
  return r;
}

/* ************************************************ */
/*  JOYPAD Wiring Diagram                           */
/* ************************************************ */
/*  Px2        Px5      Px3           Px4           */
/*   o(white)   o(red)   o(blk)        o(yellow)    */
/*   |          |        |             |            */
/*   |          |   +----+---+         |            */
/*   |          |   |        |         |            */
/*   |          |   |       [S1]       |            */
/*   |          |   |        |_        |            */
/*   |          |   |        +---------+            */
/*   |          |   |                  |            */
/*   |          |  [S4]               [S2]          */
/*   |          |   |                  |            */
/*   |          +---+---------+        |            */
/*   |                        |        |            */
/*   |                      [S3]       |            */
/*   |                        |        |            */
/*   +------------------------+--------+            */
/*                                                  */
/* ************************************************ */


/* *********************************************************** */
/* press_any_msg(row)                                          */
/* print the "press Any Key" message at (row)                  */
/* *********************************************************** */
 void press_any_msg(uint8_t row)
 {
  lcd_goto_rc(row,0);
  lcd_puts("  Press Any Key...  ");
  lcd_goto_rc(row,17);
 }

/* ***************************************************** */
/*  show_menu(item, top_item)                            */
/*  Show (LCD_LINES) (in this case 4) menu items on LCD  */
/*  Indicate the selected item with a ">"                */
/*  warp around to menu item 1 if necessary              */
/* ***************************************************** */
void show_menu(uint8_t sel_item, uint8_t top_item)
{
  uint8_t m, row, bSel, sel_row;

  lcd_clrscr();

  for(row=0; row<LCD_LINES; row++)  //Starting at row 0
    {
     m = row + top_item;     // calc menu item# for this row
     if(m > MAX_MENU)
       m = (m-MAX_MENU);     // adjust for menu list wrap

     bSel = 0;
     if(m == sel_item)      // If this is the selected menu item
       {
        bSel = 1;           // set the bSel flag
        sel_row = row;      // save the row#
       }
     printMenuRow(m, row, bSel);  //display the menu item on the LCD
    }

  lcd_goto_rc(sel_row, 0);  // set LCD cursor to row with select item.
}

/* *********************************************************** */
/* printMenuRow(menu_item, disp_row, bSelected)                */
/* Get the menu item text from program memory (flash) and copy */
/* it to a local buffer (msg). Then send it to the LCD         */
/* Add the '>' if bSelected = TRUE                             */
/* RETURN non-zero if last menu row.                           */
/* *********************************************************** */
void printMenuRow(uint8_t menu_item, uint8_t disp_row, uint8_t bSelected)
 {
   uint8_t row_offset;

   row_offset = (menu_item-1) * LCD_DISP_LENGTH;
   strncpy_P((char *)&msg, (char *)&menu_1[row_offset], LCD_DISP_LENGTH);
   msg[LCD_DISP_LENGTH]=0;

   if(bSelected)
     msg[0]='>';

   lcd_writestr((char*)&msg, disp_row, 0);
 }

/* *********************************************************** */
/* show_menuitem_and_cmds(void)                                */
/* display the selected menuitem on top row and the standard   */
/* menu instructions on the third & forth rows                 */
/* *********************************************************** */
void show_menuitem_and_cmds(void)
 {
  lcd_clrscr();
  printMenuRow(MenuItem, 0, 0);
  lcd_goto_rc(2,0);
  lcd_puts_p(p_msg02);  /* "UP/Down  To Change " */
  lcd_goto_rc(3,0);
  lcd_puts_p(p_msg01);  /* " PRESS <+> To Exit" */
 }

/* ***************************************************** */
/*  get_menu_pick()                                      */
/*    RETURNS selected item#                             */
/* ***************************************************** */
uint8_t get_menu_pick(void)
{
  uint8_t csr_row, key;

   /* ** Set cursor to row 0, col 0 turn on blinking cursor ** */
   if(MenuItem >= TopMenuItem)
      csr_row=MenuItem - TopMenuItem;
   else
      csr_row=(MAX_MENU-TopMenuItem) + MenuItem;

   lcd_command(LCD_DISP_ON_CURSOR_BLINK);

   key=0;
   while(key !=K_OK)   /* Key=10 = {left}+{right} = Select Command  */
     {
      bMenuRefresh=0;
      delay_ms(QSEC/2);
      key = scan_jpad(DEBOUNCE);   /* ** Read JoyPad Keys ** */
      if(key > 0)
       {
        switch (key)
          {
           case K_UP:  /* Up */
              MenuItem--;
              if(MenuItem<1)
                 MenuItem=MAX_MENU;
              if(csr_row==0)
                {
                 TopMenuItem=MenuItem;
                 bMenuRefresh=1;
                }
              else
               {
                lcd_goto_rc(csr_row,0);
                lcd_putc(' ');
                csr_row--;
                lcd_goto_rc(csr_row,0);
                lcd_putc('>');
                lcd_goto_rc(csr_row,0);
               }
	        break;
	
	        case K_DN:  /* Down */
              MenuItem++;
              if(MenuItem>MAX_MENU)
                 MenuItem=1;

              if(csr_row==(LCD_LINES-1))
                {
                 TopMenuItem++;
                 if(TopMenuItem > MAX_MENU)
                    TopMenuItem=1;

                 bMenuRefresh=1;
                }
              else
               {
                lcd_goto_rc(csr_row,0);
                lcd_putc(' ');
                csr_row++;
                lcd_goto_rc(csr_row,0);
                lcd_putc('>');
                lcd_goto_rc(csr_row,0);
               }
	        break;
	
	        case K_RT:  /* Right */
	        case K_LT:  /* Left */
	        case K_OK:  /* Left + Right (Select) */
	           while(scan_jpad(DELAY1)!=0); /* Wait for Key Release */
	           return MenuItem;
	        break;
	       }
	
	     if(bMenuRefresh)
	      {
	       show_menu(MenuItem, TopMenuItem);
	       lcd_goto_rc(csr_row,0);
	      }
	
	     while(scan_jpad(DELAY1)!=0); /* Wait for Key Release */
	    }

     } /* END while(key !=K_OK) */

 return 255;  /* error ?? */
}

/* ***************************************************** */
/*  Timer2 Initilization Routine   for RTC               */
/*  Using ext 32.768khz crystal                          */
/* ***************************************************** */
void timer2_init(void)
{
    rtc = 0;  /* Reset RTC tick counter (128 counts/sec) */

    ASSR |= _BV(AS2);  /* Set TC2 to use external crystal  */

    TIFR |= _BV(TOV2);  /* Inturrupt On TC2 Overflow */

    /* Set  Normal Count Mode */
    TCCR2 &= ~(_BV(WGM21) | _BV(WGM20));

    /* * Start timer 2 */
#ifdef _SIMULATE_
    /*  No Prescaler = 32.768khz (CS20) * */
    TCCR2 |= _BV(CS20);
#else
    //Prescaler = 128  (CS20 | CS22)
    TCCR2 |= (_BV(CS20) | _BV(CS22));
#endif
    TCNT2 = 0;

    /* Enable timer 2  interrupt on overflow. */
    /* 32768/256 = 128 interrupts/sec.  */
    TIMSK |= _BV(TOIE2);

    sei ();
};

/* ***************************************************** */
/*  Timer2 STOP Routine            for RTC               */
/* ***************************************************** */
void timer2_stop(void)
{
    TIMSK &= ~_BV(TOIE2); /* disable timer 2  interrupt. */
    rtc = 255;
    TCNT2 = 0;
 };

/* *****************************************************
 *  Timer2 Interrupt Service Routine  for RTC
 * **************************************************** */
ISR(TIMER2_OVF_vect)	
{
 rtc=0;

 /** Toggle Signal flag **/
 if(signal==0)
    signal=1;
 else
    signal=0;

 /* ** Advance Timer ** */
 inc_timer();
};

/* ***************************************************** */
/*  inc_timer()                                          */
/*  increment timer vals yrs, mth, yrs, hrs, min, sec    */
/* ***************************************************** */
void  inc_timer(void)
{
  if(++sec > 59)
    {
     sec=0;
     if(++min >59)
      {
       min=0;
       if(++hrs >23)
         {
          hrs=0;
          if(++day > LastDayOfMonth(mth, yrs))
            {
             day=1;
             if(++mth >12)
               {
                mth=1;
                if(++yrs >99)
                  yrs=0;
               }
            }
         }
      };
    };
  return;
};

/* ********************************************************* */
/*  LastDayOfMonth( month, year) return last day in month    */
/*  NOTE: 2 digit year method.                               */
/* ********************************************************* */
uint8_t LastDayOfMonth(uint8_t month, uint8_t year)
{
 const char LDOM[] ={31,28,31,30,31,30,31,31,30,31,31,31};

 if((month <1)||(month >12))
   return 0; /* ERROR */

 if((month==2)&&((year & 0x03)==0))
    return 29;
 else
    return LDOM[month-1];
}


/* ***************************************************** */
/*  set_timer(buf,row,col)  set on screen timer value    */
/*  format of buf="MM/DD/yy 00:00:00"                    */
/*  row,col=pos of 1st digit                             */
/*  values stored in mth, day, yrs, hrs, min, sec  vars  */
/* ***************************************************** */
uint8_t  set_timer(uint8_t row)
{
 uint8_t k,d,v,c,max;
 uint8_t *p;
 k=0;
 d = 0;           /* digit# */
 c = 17;          /* cursor pos */
 max=59;          /* upper limit digit0 (sec) */
 p = (uint8_t *)&sec;  /* pointer to value */

 /* ** Display The Values  **/
 sprintf((char*)&msg[0], "%02d/%02d/%02d %02d:%02d:%02d ", mth, day, yrs, hrs,min,sec);
 lcd_writestr((char*)&msg[0],row,1);
 lcd_goto_rc(row,c);

 lcd_command(LCD_DISP_ON_CURSOR);   /* Cursor ON - No Blink */
 while(k!=10)
   {
    k = scan_jpad(QSEC/2);
    if(k > 0)
      {
       switch (k)
           {
            case K_UP:   /* (1)UP  (inc. value)  */
              v = *p;
              if(v == max)
                v=0;
              else
                v++;
              *p = v;
            break;

            case K_DN:   /* (4)Down (dec. value)*/
              v = *p;
              if(v == 0)
                v = max;
              else
                v--;

              *p = v;
            break;

	         case K_RT:  /* (2)Right (prev. value)*/
	         case K_LT:  /* (8)Left (next value)*/
	            if((k==K_RT)&&(d>0))
	               d--;
	
	            if((k==K_LT)&&(d<5))
	               d++;
	
               switch (d)  //digit#
                {
                 case 0:     /* sec */
                    max=59;
                    c = 17;
                    p = (uint8_t *)&sec;
                 break;
                 case 1:    /* min */
                    max=59;
                    c = 14;
                    p = (uint8_t *)&min;
                 break;
                 case 2:  /* hrs */
                    max=23;
                    c = 11;
                    p = (uint8_t *)&hrs;
                 break;
                 case 3:  /* Year */
                    max=99;
                    c = 8;
                    p = (uint8_t *)&yrs;
                 break;
                 case 4:  /* day */
                    max=31;
                    c = 5;
                    p = (uint8_t *)&day;
                 break;

                 default:  /* month */
                    max=12;
                    c = 2;
                    p = (uint8_t *)&mth;
                 break;

	              }
	         break;
	
	         case K_OK:  /* (10)Left + Right (Select) */
	            return k;    /* DONE! */
	         break;
	        }; /* END switch(k) */
	
	    /* ** Display The New Values / Cursor pos.  **/
	    sprintf((char*)&msg[0], "%02d/%02d/%02d %02d:%02d:%02d", mth, day, yrs, hrs,min,sec);
       lcd_writestr((char*)&msg[0],row,1);
       lcd_goto_rc(row,c);

	   }; /* END if(k>0) */
	
	     while(scan_jpad(DELAY1)==k); /* Wait for Key Release */

   };  /* END while(k) */
  return k;
};


/* ********************************************************* */
/*  adjust_int_val(*v, *dp, max, min, row,col,bExitOnChange) */
/*  adjust a 16bit value(v) or digit position(dp) on LCD.    */
/*  return key code used to exit field (left or right)       */
/*  Number of digits displayed is determined by max          */
/*  If bExitOnChange is non-zero then the routine will also  */
/*  exit whenever Up/Down keys have produced a value change. */
/*  (Up/Down key code will then be returned )                */
/*  NOTE: col = position of 1st(leftmost) display digit      */
/* ********************************************************* */
uint8_t  adjust_int_val(uint16_t *v, uint8_t *dp,
                        uint16_t max, uint16_t min,
                        uint8_t row, uint8_t col, uint8_t bExitOnChange)
{
 uint8_t k, d, dmax;
 uint16_t p, m;
 short exit;

 char buf[6];
 char fmt[5] = {'%','0','1','d',0}; /* for sprintf(buf,"%01d",p) */

 p=*v;     /* local copy of value to adjust */
 d=*dp;    /* local copy of digit pointer */

 /* calculate multiplier based on digit pointer supplied */
 m=1;
 for(k=0; k<d; k++)
    m=m*10;

 exit=0;
 k=255;

 lcd_command(LCD_DISP_ON_CURSOR); /* No Blink Cursor */

 /* determine number of digits to display using max value */
 /* (this is actually the higest digit position value 0-4) */
 dmax=0;
 if(max >9)
   dmax=1;
 if(max>99)
   dmax=2;
 if(max>999)
   dmax=3;
 if(max>9999)
   dmax=4;

 fmt[2] = '1'+dmax;  /* match the format string to max digits */

 while(!exit)
   {
    if(k>0)  /* Only re-display if something changed */
     {
      sprintf((char *)&buf, fmt, p);
      lcd_writestr((char *)&buf, row, col-dmax);
      lcd_goto_rc(row, col-d);
     }
    delay_ms(QSEC/2);
    k = scan_jpad(DEBOUNCE);   /* ** Read JoyPad Keys ** */

    if(k>0)
      {
       switch (k)
           {
            case K_RT:  /* Right (2)*/
               if(d==0)
                 exit=1; /* Set V and return k */
               else
                {
                 d--;
                 m = m/10;
                 }

   	         break;

	         case K_LT:  /* Left (8)*/
               if(d==dmax)
                 exit=1; /* Set V and return k */
               else
                {
                 d++;
                 m = m*10;
                }
	         break;
	
	         case K_UP:  /* Up (1)*/	
              p+=m;
              if(p>max)
                p-=m;
              else
                {
                 if(bExitOnChange)
                    exit=1; /* Set V and return k */
                }

	         break;
	         case K_DN:  /* Down (4)*/
	           if(m<=p)
	            {
	             p-=m;
	             if(p<min)
                   p+=m;
                else
                  if(bExitOnChange)
                     exit=1; /* Set V and return k */
               }
	         break;
	
	         case K_OK:  /* Left + Right (10 Select) */
	           exit=1;
	         break;
	        } /* END switch(k) */
	
	        delay_ms(QSEC/2);
        } /* END if(k>0) */	

   }  /* END while(!exit) */

   *v=p;     /* Update the actual value */
   *dp=d;    /* pass back the last digit position */

  return k;  /* return key code that caused exit */
}

/* ********************************************************* */
/*  adjust_byte_val(*v, row,col)                             */
/*  adjust a 8bit value(v) in HEX on LCD.                    */
/*  return key code used to exit field (left or right)       */
/*  If mode = 1 display 3 digit decimal value next to hex    */
/*  NOTE: col = position of 1st(leftmost) display digit      */
/* ********************************************************* */
uint8_t  adjust_byte_val(uint8_t *v, uint8_t row, uint8_t col, uint8_t mode)
{
 uint8_t k, d, p, m;
 short exit;
 char buf[9];


 p=*v;     /* local copy of value to adjust */
 d=0;     /* local copy of digit pointer */
 m=1;
 exit=0;
 k=255;

 lcd_command(LCD_DISP_ON_CURSOR); /* No Blink Cursor */

 while(!exit)
   {
    if(k>0)  /* Only re-display if something changed */
     {
      if(mode==0)
        sprintf((char *)&buf, "%02X", p);
      else
        sprintf((char *)&buf, "%02X D:%03d", p, p);
      lcd_writestr((char *)&buf, row, col-1);
      lcd_goto_rc(row, col-d);
     }
    delay_ms(QSEC/2);
    k = scan_jpad(DEBOUNCE);   /* ** Read JoyPad Keys ** */

    if(k>0)
      {
       switch (k)
           {
            case K_RT:  /* Right (2)*/
               if(d==0)
                 exit=1; /* Set V and return k */
               else
                {
                 d=0;
                 m=1;
                }
   	         break;

	         case K_LT:  /* Left (8)*/
               if(d==1)
                 exit=1; /* Set V and return k */
               else
                {
                 d=1;
                 m=0x10;
                }
	         break;
	
	         case K_UP:  /* Up (1)*/	
              p+=m;
	         break;
	
	         case K_DN:  /* Down (4)*/
              p-=m;
	         break;
	
	         case K_OK:  /* Left + Right (10 Select) */
	           exit=1;
	         break;
	        } /* END switch(k) */
	
	        delay_ms(QSEC/2);
        } /* END if(k>0) */	

   }  /* END while(k!=10) */

   *v=p;     /* Update the actual value */

  return k;  /* return key code that caused exit */
}



/* ********************************************************* */
/*  to_decimal_str(uint16_t v, char *buf)                    */
/*  convert 16bit int (v) to 5digit decimal string in (buf)  */
/*  NOTE: buf MUST be at least 6 bytes long.                 */
/* ********************************************************* */
void to_decimal_str(uint16_t v, char *buf)
{
 uint8_t digit,c;
 uint16_t r,m;
 m = 10000;  /* digit 0=10000's,  digit 5 = 1's */
 r = v;
 for(digit=0; digit<5; digit++)
   {
    c= r/m;
    buf[digit] = c+'0';
    r = r-(c*m);
    m = m/10;
   }

 buf[digit]=0;
}

/* ********************************************************* */
/*  to_binary_str(uint8_t v, char *buf)                      */
/*  convert 8bit int (v) to 8 digit binary string in (buf)   */
/*  NOTE: buf MUST be at least 9 bytes long.                 */
/* ********************************************************* */
void to_binary_str(uint8_t v, char *buf)
{
 uint8_t bit, mask, c;

 for(bit=0; bit<8; bit++)
   {
    c = '1';
    mask = (1 << bit);
    if((v & mask)==0)
      c = '0';

    buf[7-bit] = c;
   }

 buf[8]=0;
}

/* *********************************************************** */
/* delay_ms() make repeated calls to the delay_us()            */
/* NOTE: This routine accepts an unsigned int(2bytes-16 bits)  */
/*       so the MAXIMUM DELAY available is 65535ms or 65.5sec  */
/* *********************************************************** */
 void delay_ms(unsigned int ms)
  {
   unsigned long usec, r;

   if(! ms)
     return;

   usec = (unsigned long)ms * 1000UL;

   if(usec <= UINT16_MAX )
     {
     delay_us((unsigned int)usec);
     return;
     }

   r = usec;
   while(r > UINT16_MAX)
      {
        r-= UINT16_MAX ;
        delay_us(UINT16_MAX);
      }

   if(r>5)
      delay_us((unsigned int)r);

  }

/* *********************************************************** */
/* delay_us() make the call to the Macro __delay_loop_2()      */
/* NOTE: This routine accepts an unsigned int(2bytes-16 bits)  */
/*       so the MAXIMUM DELAY available is 65535us or 65.5ms   */
/*       MINUMUM delay is  4us                                 */
/* *********************************************************** */
 void delay_us(unsigned int us)
  {
   double r, ticks;
   /* see util/delay.h  _delay_loop_2 uses 4 cycles per iteration*/
   /* so a 16bit value = 65536 * 4 or 262144us at 1MHZ */
   //const unsigned long max = 262144 / (F_CPU /1000000UL);

   if(us < 4)
     return;

   ticks = (us / 4) *  (F_CPU /1000000UL);

   if(ticks <= UINT16_MAX )
     {
     _delay_loop_2((uint16_t)ticks);
     return;
     }

   r = ticks;
   while(r > UINT16_MAX)
      {
        r-=UINT16_MAX;
        _delay_loop_2(0);
      }

   if(r>4)
      _delay_loop_2(r);

  }

/* ***************************************************** */
/*  print_FlagState(flag, buf)                           */
/*  Build display string for 4 bit Flags (TempEn/SenesEn)*/
/*  format of buf=".ON .ON OFF OFF" (bit 1=1,2=1 3=0,4=0)*/
/* ***************************************************** */
void print_FlagState(uint8_t flags, uint8_t row, uint8_t col)
{
 uint8_t  p, mask;
 mask=0x08;
 lcd_goto_rc(row,col);
 for(p=0; p<4; p++)
   {
    if(flags & mask)
      lcd_puts(" ON ");
    else
      lcd_puts("OFF ");
    mask = (mask>> 1);
   }
}

/* ***************************************************** */
/*  setFlagValues(flags, title, row) show/set 4Bit Flags */
/*  format of display="Title  ON ON OFF OFF"             */
/*  title must be a 6 bbyte string (pad with spaces)     */
/*  row = lcd row (0-4)                                  */
/*  returns new flag values                              */
/* ***************************************************** */
uint8_t setFlagValues(uint8_t flags, char *title, uint8_t row)
{
 uint8_t k, v, bit, c, mask, new_val;

 new_val = flags;
 lcd_goto_rc(row,0);
 lcd_puts(title);
 print_FlagState(new_val, row, 5);

 k=0;             /* key pressed = none */
 bit = 3;         /* bit# */
 mask = 0x08;     /* bit Mask */
 c = 5;           /* cursor pos */
 v = new_val & mask; /* Selected Flag Value */

 /* ** Display The Values  **/
 lcd_goto_rc(row,c);

 lcd_command(LCD_DISP_ON_CURSOR);   /* Cursor ON - No Blink */
 while(k!=10)
   {
    k = scan_jpad(QSEC/2);
    if(k > 0)
      {
       switch (k)
           {
            case K_UP:   /* UP  (toggle value)  */
            case K_DN:   /* Down (toggle value)*/
              v = new_val & mask;
              if(v == 0)
                new_val |= mask;
              else
                new_val &= ~(mask);

            break;

	         case K_RT:  /* Right (next. value)*/
	           if(bit>0)
	             {
	              bit--;
	              mask= (mask >> 1);
	              c+=4;
	             }
	         break;
	         case K_LT:  /* Left (next value)*/
	           if(bit<3)
	             {
	              bit++;
	              mask= (mask << 1);
	              c-=4;
	             }
	         break;
	
	         case K_OK:  /* Left + Right (Select) */
	            return new_val;    /* DONE! */
	         break;
	        }; /* END switch(k) */
	
	    /* ** Display The New Values / Cursor pos.  **/
	     print_FlagState(new_val, row, 5);
        lcd_goto_rc(row,c);

	   }; /* END if(k>0) */
	
	     while(scan_jpad(DELAY1)==k); /* Wait for Key Release */

   };  /* END while(k) */
  return new_val;
};

/* ***************************************************** */
/*  printStatusPortVals(port)                            */
/*  format of display="Status X=TTTTTT ON=B"             */
/*  Always On Second LCD Display Line (ROW 1)            */
/* ***************************************************** */
void printStatusPortVals(uint8_t port)
{
 uint8_t on, type;
 on=StatusPortVals[port]>>4;
 type= StatusPortVals[port]&0x0f;
 lcd_goto_rc(1,0);
 lcd_puts_p(&p_status_types[0]);
 lcd_goto_rc(1,6);
 lcd_putc('1'+ port);
 lcd_putc('=');
 lcd_puts_p(&p_status_types[type*7]);
 lcd_puts(" ON=");
 lcd_putc('0'+ on);
 }

/* ***************************************************** */
/*  setStatusPortVals()                                  */
/*  format of display="Status X=TTTTTT ON=B"             */
/*  Always On Second LCD Display Line (ROW 1)            */
/* ***************************************************** */
void setStatusPortVals(void)
{
 uint8_t k, c, on, type, port, v;
 port=0;
 on=StatusPortVals[port]>>4;
 type= StatusPortVals[port]&0x0f;

 lcd_command(LCD_DISP_ON_CURSOR);   /* Cursor ON - No Blink */
 printStatusPortVals(port);
 k=0;             /* key pressed = none */
 c = 6;           /* cursor pos */
 v='s';
 lcd_goto_rc(1,c);

 while(k!=10)
   {
    k = scan_jpad(QSEC/2);
    if(k > 0)
      {
       switch (k)
           {
            case K_UP:   /* UP  (toggle value)  */
            case K_DN:   /* Down (toggle value)*/
	           switch(c)
	              {
	               case 6: /* Changing port# */
	                 /* Save current values for current port# */
	                 StatusPortVals[port]=(on << 4)+ type;
                    if(k==K_DN)
                       port++;
                    else
                      port--;

                    if(port==4)
                      port=0;

                    if(port>4)
                       port=3;

                    /* get values for new port# */
                    on=StatusPortVals[port]>>4;
                    type= StatusPortVals[port]&0x0f;
                  break;
	               case 8: /* Changing Status Type */
                    if(k==K_DN)
                       type++;
                    else
                       type--;

                    if(type==6)
                      type=1;

                    if(type==0)
                      type=5;
	               break;
	
	               case 18: /* toggle Status Logic Value */
	                  if(on)
	                    on=0;
	                  else
	                    on=1;
	               break;
	              }
	           /* Save new values for current port# and display them*/
              StatusPortVals[port]=(on << 4)+ type; 	
              printStatusPortVals(port);
            break;

	         case K_RT:  /* Right (next. column/value)*/
	           v='=';
	           switch(c)
	              {
	               case 6:
	                  c=8;
	               break;
	               case 8:
	                  c=18;
	               break;
	               case 18:
	                  c=6;
	                  v='s';	
	               break;
	              }
            break;
	         case K_LT:  /* Left (next column/value)*/
	           v='=';
	           switch(c)
	              {
	               case 6:
	                  c=18;
	               break;
	               case 8:
	                  c=6;
	                  v='s';
	               break;
	               case 18:
	                  c=8;
	               break;
	              }
	         break;
	
	         case K_OK:  /* Left + Right (Select) */
	           /* Save new values for current port# before exit*/
	           StatusPortVals[port]=(on << 4)+ type;
	         break;
	        }; /* END switch(k) */
	
	     lcd_goto_rc(1,c-1);
	     lcd_putc(v);
	
	   }; /* END if(k>0) */
	
     while(scan_jpad(DELAY1)==k); /* Wait for Key Release */

   };  /* END while(k) */
  return;
};
/* ***************************************************** */
/*  setStatusMask()                                      */
/*  Read the status port settings array and build a mask */
/*  value based on the ports with type = OFF             */
/* ***************************************************** */
void setStatusMask()
{
 uint8_t bit;
 StatusMask = 0;
 for(bit=0; bit<4; bit++)
   {
    if((StatusPortVals[bit] & 0x0f) != 5)
      StatusMask |= 0x10;
    StatusMask = StatusMask >>1;
   }
}


/* ***************************************************** */
/*  showEveltList() show the 100 item event list         */
/*  format of display="nn HH:MM:SS SSSSSSSS"             */
/* ***************************************************** */
void showEventList(void)
{
 uint8_t k, p, x;
 uint16_t o;

 if(EventPtr>4)
    p=EventPtr-4; /* Show last event and previous 3 */
 else
    p=0;

 o=p*6;           /* event list offset  */
 k=0;             /* key pressed = none */

 /* ** Display The Values  **/
 //lcd_goto_rc(row,c);
 for(x=0; x<4; x++)
    {
     sprintf(msg, "%02d %02d%02d%02d %03d %03d %02X", p,EventList[o], EventList[o+1], EventList[o+2], EventList[o+3], EventList[o+4], EventList[o+5]);
     lcd_goto_rc(x,0);
     lcd_puts(msg);
     o+=6;
     p++;
     if(p>99)
        p=o=0;
    }

 lcd_command(LCD_DISP_ON_CURSOR);   /* Cursor ON - No Blink */
 while(k!=10)
   {
    k = scan_jpad(QSEC/2);
    if(k > 0)
      {
       switch (k)
           {
            case K_UP:   /* UP  (prev. 4 items)  */
               if(p > 7)
                 p-=8;
               else
                 p=92+p;
            break;

            case K_DN:   /* Down (next 4  values)*/
              /* p & o are already set correctly */
            break;

	         case K_RT:  /* Right */
	         case K_LT:  /* Left */
	         case K_OK:  /* Left + Right (Select) */
	            return;    /* DONE! */
	         break;
	        }; /* END switch(k) */
	
	    /* ** Display The New Values / Cursor pos.  **/
	    o = p*6;
       for(x=0; x<4; x++)
          {
           sprintf(msg, "%02d %02d%02d%02d %03d %03d %02X", p,EventList[o], EventList[o+1], EventList[o+2], EventList[o+3], EventList[o+4], EventList[o+5]);
           lcd_goto_rc(x,0);
           lcd_puts(msg);
           o+=6;
           p++;

          if(p>99)
             p=o=0;
          }
	

	   }; /* END if(k>0) */
	
    while(scan_jpad(DELAY1)!=0); /* Wait for Key Release */
   };  /* END while(k) */
  return;
};
















































