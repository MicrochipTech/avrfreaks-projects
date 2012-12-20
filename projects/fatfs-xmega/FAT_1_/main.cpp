// Written by Ozhan KD (www.knowledgeplus.ir)

#include "avr_compiler.h"
#include "XMEGA_ff_v2.h"
#include "XMEGA_cpu_v2.h"
#include "XMEGA_mmc_v2.h"
#include "XMEGA_spi_v1.h"
#include "XMEGA_timer_v2.h"
#include "XMEGA_lcd_v2.h"

FRESULT res;
unsigned int nbytes;
FATFS fat;
FIL file;
char path[]="0:/test.txt";
char buffer[]="KnowledgePlus.ir";
extern BYTE CardType;

DWORD get_fattime (void)
{
 return	  ((DWORD)(2012 - 1980) << 25)
			| ((DWORD)1 << 21)
			| ((DWORD)1 << 16)
			| ((DWORD)1 << 11)
			| ((DWORD)1 << 5)
			| ((DWORD)2 >> 1);
}
void main()
{
 bool valid_card_f=true;
 PMIC_CTRL=PMIC_LOLVLEN_bm;
 sei();
 LCDInit();
 LCDGotoXY(0,0);

 // initialize MMC interface
 if(disk_initialize(0))
 {
  LCDStringRam("NoCARD");
  valid_card_f=false;
 }
 else
 {
  switch(CardType)
  {
   case(CT_MMC):
    LCDStringRam("mmc-V3");
    break;

   case (CT_SD1):
    LCDStringRam("SDC-V1");
    break;

   case (CT_SD2):
    LCDStringRam("SDC-V2");
    break;

  case (CT_SDC):
   LCDStringRam("SDC-  ");
   break;

  case (CT_BLOCK):
   LCDStringRam("BLOCK ");
   valid_card_f=false;
   break;

  case (CT_SDHC):
   LCDStringRam("SDHC  ");
   break;

  default:
   LCDStringRam("BLOCK ");
   valid_card_f=false;  }
 }

 if(valid_card_f)
 {
  LCDGotoXY(0,1);
  if ((res=f_mount(0,&fat))==FR_OK)
  {
   LCDStringRam("Mount OK");
   if ((res=f_open(&file,path,FA_CREATE_ALWAYS|FA_WRITE|FA_READ))==FR_OK)
   {
    LCDGotoXY(0,2);
    LCDStringRam("File Opened");
    if ((res=f_write(&file,buffer,sizeof(buffer)-1,&nbytes))==FR_OK)
    {
     LCDGotoXY(12,2);
     LCDStringRam("Write OK");
     res=f_lseek(&file,0); // Added in this version
     if ((res=f_read(&file,buffer,sizeof(buffer)-1,&nbytes))==FR_OK)
     {
      LCDGotoXY(0,3);
      LCDStringRam(buffer);
     }
    }
    else LCDStringRam("Write Error");
   }
  f_close(&file);
  f_mount(0, NULL);
  }
  else LCDStringRam("Mount Error");
 }
 while(1);
}

