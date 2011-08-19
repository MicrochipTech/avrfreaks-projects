#include "string.h"
#include "avr/eeprom.h"
#include "util/crc16.h"
#include "eestruct_settings.h"
#include "eestruct.h"

#if !defined(EEPROM_SIZE) || !defined(EEPROM_STRUCTURE_SIZE)
  #error EEPROM_SIZE or EEPROM_STRUCTURE_SIZE must be defined.
#endif

#if (EEPROM_STRUCTURE_SIZE==0)
  #error The EEPROM_STRUCTURE_SIZE must not be 0.
#endif

#define EEPROM_SLOTS ((EEPROM_SIZE/2)/EEPROM_STRUCTURE_SIZE) //4 extra bytes, 2 for count, 2 for crc16

#if (EEPROM_SIZE<16) || (EEPROM_STRUCTURE_SIZE<8) || (EEPROM_SLOTS<1)
  #error There is not enough room for the sizes you have specified.
#endif

#if (EEPROM_SIZE%4!=0)
  #warning ClearEEPROM only works properly when EEPROM_SIZE is 4 byte aligned
#endif

unsigned int EepromSlot=65535;
unsigned int EepromVer=65535;
unsigned char EepromLoaded=0;

unsigned int Crc16Memory(void *AData, unsigned int ASize)
{
  unsigned int crc,i1;

  crc=0xffff;
  for (i1=0;i1<ASize;i1++)
    crc=_crc16_update(crc,((unsigned char*)AData)[i1]); 
  return ~crc;
}

void ClearEEPROM()
{
  unsigned int i1;
  unsigned long ul1=0xffffffff;

  for (i1=0;i1<EEPROM_SIZE;i1+=4)
    eeprom_write_block(&ul1,(void*)i1,4);
  EepromSlot=65535;
  EepromVer=65535;
  EepromLoaded=1;
}

unsigned char LoadEEPROMStructure(void *AStructure)
{
  unsigned int i1,i2,crc,crc2,ver;
  unsigned char found=0;
  void *ptr,*ptr2=NULL;

  EepromLoaded=1;
  for (i1=0;i1<2;i1++)
    for (i2=0;i2<EEPROM_SLOTS;i2++)
      {
        ptr=(void*)(i2*EEPROM_STRUCTURE_SIZE+i1*(EEPROM_SIZE/2));
        eeprom_read_block(AStructure,ptr,EEPROM_STRUCTURE_SIZE);
        crc=Crc16Memory(AStructure,EEPROM_STRUCTURE_SIZE-2);
        memcpy(&crc2,AStructure+EEPROM_STRUCTURE_SIZE-2,2);
        if (crc==crc2)
          {
            memcpy(&ver,AStructure+EEPROM_STRUCTURE_SIZE-4,2);
            if ((!found) || (ver<16384 && EepromVer>=49152) || (ver>EepromVer && !(ver>=49152 && EepromVer<16384)))
              {
                ptr2=ptr;
                EepromSlot=i2;
                EepromVer=ver;
                found=1;
              }
          }
      }
  if (found)
    eeprom_read_block(AStructure,ptr2,EEPROM_STRUCTURE_SIZE);
  return found;
}

unsigned char SaveEEPROMStructure(void *AStructure)
{
  unsigned int crc;

  if (!EepromLoaded)
    return 0;
  EepromSlot++;
  if (EepromSlot>=EEPROM_SLOTS)
    EepromSlot=0;
  EepromVer++; //It may roll over but that is ok
  memcpy(AStructure+EEPROM_STRUCTURE_SIZE-4,&EepromVer,2);
  crc=Crc16Memory(AStructure,EEPROM_STRUCTURE_SIZE-2);
  memcpy(AStructure+EEPROM_STRUCTURE_SIZE-2,&crc,2);
  eeprom_write_block(AStructure,(void*)(EepromSlot*EEPROM_STRUCTURE_SIZE),EEPROM_STRUCTURE_SIZE);
  eeprom_write_block(AStructure,(void*)(EepromSlot*EEPROM_STRUCTURE_SIZE+(EEPROM_SIZE/2)),EEPROM_STRUCTURE_SIZE);
  return 1;
}

