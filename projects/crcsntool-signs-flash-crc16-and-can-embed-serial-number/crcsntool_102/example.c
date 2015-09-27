#include "avr/io.h"
#include "string.h"
#include "avr/pgmspace.h"
#include "util/crc16.h"

//this PROGMEM string must exist so that the crcsntool can find it.
//
//care 

//sets the flash size 032=32Kb (must be 9 chars long with xx on end, do not remove xx)
//must be outside of function for PROGMEM to work properly
const char MsgCRCL[] PROGMEM = "cRcL032xxyyyyzz";

uint32_t serialno;

int main()
  {
    //test crc (code works on less than 64K, will need to be modified for 64K or larger
    {
      uint16_t ui1,ui2,ui3;

      ui2=((pgm_read_byte(MsgCRCL+5)-'0')*10+(pgm_read_byte(MsgCRCL+6)-'0'))*1024;
      ui3=0xffff;
      for (ui1=0;ui1<ui2;ui1++)
        if (ui1<(uint16_t)MsgCRCL+7 || ui1>(uint16_t)MsgCRCL+14)
          ui3=_crc16_update(ui3,pgm_read_byte(ui1));
      ui3=~ui3;
      if (ui3!=pgm_read_word(MsgCRCL+7))
        {
          //crc failure

          //debug indicate failure here

          for(;;);
        }
    }

    //serial no
    {
      uint16_t ui1,ui3;

      ui3=0xffff;
      for (ui1=0;ui1<4;ui1++)
        ui3=_crc16_update(ui3,pgm_read_byte(MsgCRCL+9+ui1));
      ui3=~ui3;
      if (ui3!=pgm_read_word(MsgCRCL+13))
        {
          //crc failure

          //debug indicate failure here

          for(;;);
        }
      else memcpy_P(&serialno,MsgCRCL+9,4);
    }
  }
  
