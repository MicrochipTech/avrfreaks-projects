/*

                           e Y8b    Y8b YV4.08P888 88e
                          d8b Y8b    Y8b Y888P 888 888D
                         d888b Y8b    Y8b Y8P  888 88"
                        d888WuHan8b    Y8b Y   888 b,
                       d8888888b Y8b    Y8P    888 88b,
           8888 8888       ,e,                                  888
           8888 888820088e  " Y8b Y888P ,e e, 888,8, dP"Y ,"Y88b888
           8888 8888888 88b888 Y8b Y8P d88 88b888 " C88b "8" 888888
           8888 8888888 888888  Y8b "  888   ,888    Y88D,ee 888888
           'Y88 88P'888 888888   Y8P    "YeeP"888   d,dP "88 888888
   888 88b,                    d8  888                     888
   888 88P' e88 88e  e88 88e  d88  888 e88 88e  ,"Y88b e88 888 ,e e, 888,8,
   888 8K  d888 888bd888 8Shaoziyang88d888 888b"8" 888d888 888d88 88b888 "
   888 88b,Y888 888PY888 888P 888  888Y888 888P,ee 888Y888 888888   ,888
   888 88P' "88 88"  "88 88"  888  888 "88 88" "88 888 "88 888 "YeeP"888


  Project:       AVR Universal BootLoader
  File:          bootldr.c
                 main program code
  Version:       4.5

  Compiler:      WinAVR 20071221 + AVR Studio 4.14.589

  Author:        Shaoziyang
                 Shaoziyang@gmail.com
                 http://avrubd.googlepages.com
                 http://groups.google.com/group/avrub?hl=en

  Date:          2008.8

  See readme.htm to get more information.

*/

#include "bootcfg.h"
#include "bootldr.h"

//user's application start address
#define PROG_START         0x0000

//remove IVT
#define noIVT              0

//receive buffer' size will not smaller than SPM_PAGESIZE
#if (BUFFERSIZE < SPM_PAGESIZE)
#define BUFSIZE SPM_PAGESIZE
#else
#define BUFSIZE BUFFERSIZE
#endif

//define receive buffer
unsigned char buf[BUFSIZE];

#if (BUFSIZE > 255)
unsigned int bufptr, pagptr;
#else
unsigned char bufptr, pagptr;
#endif

unsigned char ch, cl;

//Flash address
#if (FLASHEND > 0xFFFFUL)
unsigned long int FlashAddr;
#else
unsigned int FlashAddr;
#endif

//include decrypt subroutine file
#if Decrypt

//PC1 decrypt algorithm subroutine
#if (Algorithm == PC1_128)||(Algorithm == PC1_256)
#include "pc1crypt.c"
#elif (Algorithm == AES_128)||(Algorithm == AES_256)
#include "aes.c"
#else
#error "Unknow encrypt algorithm!"
#endif

#endif  //Decrypt


//write one Flash page
void write_one_page(unsigned char *buf)
{
  boot_page_erase(FlashAddr);                  //erase one Flash page
  boot_spm_busy_wait();
  for(pagptr = 0; pagptr < SPM_PAGESIZE; pagptr += 2) //fill data to Flash buffer
  {
    boot_page_fill(pagptr, buf[pagptr] | (buf[pagptr + 1] << 8));
  }
  boot_page_write(FlashAddr);                  //write buffer to one Flash page
  boot_spm_busy_wait();                        //wait Flash page write finish
}

//jump to user's application
void quit()
{
#if Decrypt
    DestroyKey();                              //delete decrypt key
#endif

#if SafeMode
  if(eeprom_read_byte((uint8_t *)FlagAddr))
    (*((void(*)(void))(BootStart)))();         //jump to bootloader
#endif

  boot_rww_enable();                           //enable application section
  (*((void(*)(void))PROG_START))();            //jump
}

//send a byte to comport
void WriteCom(unsigned char dat)
{
#if RS485
  RS485Enable();
#endif

  UDRREG(COMPORTNo) = dat;
  //wait send finish
  while(!(UCSRAREG(COMPORTNo) & (1<<TXCBIT(COMPORTNo))));
  UCSRAREG(COMPORTNo) |= (1 << TXCBIT(COMPORTNo));

#if RS485
  RS485Disable();
#endif
}

//wait receive a data from comport
unsigned char WaitCom()
{
  while(!DataInCom());
  return ReadCom();
}

#if VERBOSE
//send a string to uart
void putstr(const char *str)
{
  while(*str)
    WriteCom(*str++);

  WriteCom(0x0D);
  WriteCom(0x0A);
}
#endif

//calculate CRC checksum
void crc16(unsigned char *buf)
{
#if (BUFSIZE > 255)
  unsigned int j;
#else
  unsigned char j;
#endif

#if (CRCMODE == 0)
  unsigned char i;
  unsigned int t;
#endif
  unsigned int crc;

  crc = 0;
  for(j = BUFFERSIZE; j > 0; j--)
  {
#if (CRCMODE == 0)
    //CRC1021 checksum
    crc = (crc ^ (((unsigned int) *buf) << 8));
    for(i = 8; i > 0; i--)
    {
      t = crc << 1;
      if(crc & 0x8000)
        t = t ^ 0x1021;
      crc = t;
    }
#elif (CRCMODE == 1)
    //word add up checksum
    crc += (unsigned int)(*buf);
#elif
#error "Unknow CRC MODE!"
#endif
    buf++;
  }
  ch = crc / 256;
  cl = crc % 256;
}

//If you will remove IVT(interrupt vect table), define macro noIVT to 1
//and add "-nostartfiles" in linker option
//remove interrupt vect table
#if noIVT
void initstack(void) __attribute__ ((section(".init9")));
void initstack(void) 
{
  //set stack
  asm volatile ( ".set __stack, %0" :: "i" (RAMEND) ); 
  //jump to main function
  asm volatile ( "rjmp main");
}
#endif

//Main routine
int main(void)
{
  unsigned char cnt;
  unsigned char packNO;
  unsigned char crch, crcl;

#if (InitDelay > 0)
#if (InitDelay > 255)
  unsigned int di;
#else
  unsigned char di;
#endif
#endif

#if (BUFFERSIZE > 255)
  unsigned int li;
#else
  unsigned char li;
#endif

  //disable interrupt
  cli();

#if WDG_En
  //if enable watchdog, setup timeout
  wdt_enable(WDTO_1S);
#else
  //otherwise disable watchdog
#ifndef MCUSR
#define MCUSR    MCUCSR
#endif
  MCUSR = 0;
  wdt_disable();
#endif

  //initialize timer1, CTC mode
  TimerInit();

#if RS485
  //initialize RS485 port
  DDRREG(RS485PORT) |= (1 << RS485TXEn);
  RS485Disable();
#endif

#if LED_En
  //set LED control port to output
  DDRREG(LEDPORT) |= (1 << LEDPORTNo);
#endif

  //initialize comport with special config value
  ComInit();

#if (InitDelay > 0)
  //some old kind of avr mcu may need special delay after comport initialization
  for(di = InitDelay; di > 0; di--)
    __asm__ __volatile__ ("nop": : );
#endif

#if LEVELMODE
  //according pin level to enter bootloader
  //set pin to input
  DDRREG(LEVELPORT) &= ~(1 << LEVELPIN);
#if PINLEVEL
  if(PINREG(LEVELPORT) & (1 << LEVELPIN))
#else
  if(!(PINREG(LEVELPORT) & (1 << LEVELPIN)))
#endif
  {
#if VERBOSE
    //prompt enter boot mode
    putstr(msg6);
#endif
  }
  else
  {
#if VERBOSE
    //prompt execute user application
    putstr(msg7);
#endif

    quit();
  }

#else
  //comport launch boot

#if VERBOSE
  //prompt waiting for password
  putstr(msg1);
#endif

  cnt = TimeOutCnt;
  cl = 0;
  while(1)
  {
#if WDG_En
    //clear watchdog
    wdt_reset();
#endif

    if(TIFRREG & (1<<OCF1A))    //T1 overflow
    {
      TIFRREG |= (1 << OCF1A);

      if(cl == CONNECTCNT)      //determine Connect Key
        break;

#if LED_En
      LEDAlt();                 //toggle LED
#endif

      cnt--;
      if(cnt == 0)              //connect timeout
      {

#if VERBOSE
        putstr(msg2);           //prompt timeout
#endif
        quit();                 //quit bootloader
      }
    }

    if(DataInCom())             //receive connect key
    {
      if(ReadCom() == ConnectKey[cl])  //compare ConnectKey
        cl++;
      else
        cl = 0;
    }
  }

#endif  //LEVELMODE

#if VERBOSE
  putstr(msg3);                 //prompt waiting for data
#endif

//set flag in EEPROM if SafeMode enable
#if SafeMode
  eeprom_write_byte((uint8_t *)FlagAddr, 0x1);
#endif

#if Decrypt
  DecryptInit();
#endif

  //every interval send a "C",waiting XMODEM control command <soh>
  cnt = TimeOutCntC;
  while(1)
  {
    if(TIFRREG & (1 << OCF1A))  //T1 overflow
    {
      TIFRREG |= (1 << OCF1A);
      WriteCom(XMODEM_RWC) ;    //send "C"

#if LED_En
      LEDAlt();                 //toggle LED
#endif

      cnt--;
      if(cnt == 0)              //timeout
      {
#if VERBOSE
        putstr(msg2);           //prompt timeout
#endif
        quit();                 //quit bootloader
      }
    }

#if WDG_En
    wdt_reset();                //clear watchdog
#endif

    if(DataInCom())
    {
      if(ReadCom() == XMODEM_SOH)  //XMODEM command <soh>
        break;
    }
  }

  TCCR1B = 0;                   //close timer1

  //begin to receive data
  packNO = 0;
  bufptr = 0;
  cnt = 0;
  FlashAddr = 0;
  do
  {
    packNO++;
    ch =  WaitCom();                          //get package number
    cl = ~WaitCom();
    if ((packNO == ch) && (packNO == cl))
    {
      for(li = BUFFERSIZE; li > 0; li--)      //receive a full data frame
      {
        buf[bufptr++] = WaitCom();
      }
      crch = WaitCom();                       //get checksum
      crcl = WaitCom();
      crc16(&buf[bufptr - BUFFERSIZE]);       //calculate checksum
      if((crch == ch) && (crcl == cl))
      {
#if BootStart
        if(FlashAddr < BootStart)             //avoid write to boot section
        {
#endif

#if Decrypt
          DecryptBlock(&buf[bufptr - BUFFERSIZE], BUFFERSIZE); //decrypt buffer
#endif

#if (BUFFERSIZE <= SPM_PAGESIZE)
          if(bufptr >= SPM_PAGESIZE)          //Flash page full, write flash page;otherwise receive next frame
          {                                   //receive multi frames, write one page
            write_one_page(buf);              //write data to Flash
            FlashAddr += SPM_PAGESIZE;        //modify Flash page address
            bufptr = 0;
          }
#else
          while(bufptr > 0)                   //receive one frame, write multi pages
          {
            write_one_page(&buf[BUFSIZE - bufptr]);
            FlashAddr += SPM_PAGESIZE;        //modify Flash page address
            bufptr -= SPM_PAGESIZE;
          }
#endif

#if BootStart
        }
        else                                  //ignore flash write when Flash address exceed BootStart
        {
          bufptr = 0;                         //reset receive pointer
        }
#endif

// read flash, and compare with buffer's content
#if (ChipCheck > 0) && (BootStart > 0)
#if (BUFFERSIZE < SPM_PAGESIZE)
        if((bufptr == 0) && (FlashAddr < BootStart))
#else
        if(FlashAddr < BootStart)
#endif
        {
          boot_rww_enable();                  //enable application section
          cl = 1;                             //clear error flag
          for(pagptr = 0; pagptr < BUFSIZE; pagptr++)
          {
#if (FLASHEND > 0xFFFFUL)
            if(pgm_read_byte_far(FlashAddr - BUFSIZE + pagptr) != buf[pagptr])
#else
            if(pgm_read_byte(FlashAddr - BUFSIZE + pagptr) != buf[pagptr])
#endif
            {
              cl = 0;                         //set error flag
              break;
            }
          }
          if(cl)                              //checksum equal, send ACK
          {
            WriteCom(XMODEM_ACK);
            cnt = 0;
          }
          else
          {
            WriteCom(XMODEM_NAK);             //checksum error, ask resend
            cnt++;                            //increase error counter
            FlashAddr -= BUFSIZE;             //modify Flash page address
          }
        }
        else                                  //don't need verify, send ACK directly
        {
          WriteCom(XMODEM_ACK);
          cnt = 0;
        }
#else
        WriteCom(XMODEM_ACK);                 //no verify, send ACK directly
        cnt = 0;
#endif

#if WDG_En
        wdt_reset();                          //clear watchdog
#endif

#if LED_En
        LEDAlt();                             //LED indicate update status
#endif
      }
      else //CRC
      {
        WriteCom(XMODEM_NAK);                 //require resend
        cnt++;
      }
    }
    else //PackNo
    {
      WriteCom(XMODEM_NAK);                   //require resend
      cnt++;
    }

    if(cnt > 3)                               //too many error, abort update
      break;
  }
  while(WaitCom() != XMODEM_EOT);
  WriteCom(XMODEM_ACK);


#if VERBOSE
  if(cnt == 0)
  {
#if SafeMode
    //clear flag in EEPROM
    eeprom_write_byte((uint8_t *)FlagAddr, 0);
#endif

    //update success
    putstr(msg4);                             //prompt update success
  }
  else
  {
    // update fail
    putstr(msg5);                             //prompt update fail

#if WDG_En
    while(1);                                 //dead loop, wait watchdog reset
#else
#if (BootStart > 0)
    (*((void(*)(void))(BootStart)))();        //jump to bootloader
#endif
#endif
  }

#else

  if(cnt > 0)
  {
#if WDG_En
    while(1);                                 //when update fail, use dead loop wait watchdog reset
#else
#if (BootStart > 0)
    (*((void(*)(void))(BootStart)))();
#endif
#endif
  }
#if SafeMode
  else
  {
    //clear flag in EEPROM
    eeprom_write_byte((uint8_t *)FlagAddr, 0);
  }
#endif
  
#endif

  quit();                                     //quit bootloader
  return 0;
}

//End of file: bootldr.c
