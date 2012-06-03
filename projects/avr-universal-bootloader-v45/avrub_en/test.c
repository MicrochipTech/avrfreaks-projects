/*

                           e Y8b    Y8b YV1.08P888 88e
                          d8b Y8b    Y8b Y888P 888 888D
                         d888b Y8b    Y8b Y8P  888 88"
                        d888WuHan8b    Y8b Y   888 b,
                       d8888888b Y8b    Y8P    888 88b,
           8888 8888       ,e,                                  888
           8888 888820078e  " Y8b Y888P ,e e, 888,8, dP"Y ,"Y88b888
           8888 8888888 88b888 Y8b Y8P d88 88b888 " C88b "8" 888888
           8888 8888888 888888  Y8b "  888   ,888    Y88D,ee 888888
           'Y88 88P'888 888888   Y8P    "YeeP"888   d,dP "88 888888
   888 88b,                    d8  888                     888
   888 88P' e88 88e  e88 88e  d88  888 e88 88e  ,"Y88b e88 888 ,e e, 888,8,
   888 8K  d888 888bd888 8Shaoziyang88d888 888b"8" 888d888 888d88 88b888 "
   888 88b,Y888 888PY888 888P 888  888Y888 888P,ee 888Y888 888888   ,888
   888 88P' "88 88"  "88 88"  888  888 "88 88" "88 888 "88 888 "YeeP"888
                                ___           ___
                  ___          /  /\         /  /\      ___
                 /__/\        /  /::\       /  /::\    /__/\
                 \  \:\      /  /:/\:\     /__/:/\:\   \  \:\
                  \__\:\    /  /::\ \:\   _\_ \:\ \:\   \__\:\
                  /  /::\  /__/:/\:\ \:\ /__/\ \:\ \:\  /  /::\
                 /  /:/\:\ \  \:\ \:\_\/ \  \:\ \:\_\/ /  /:/\:\
                /  /:/__\/  \  \:\ \:\    \  \:\_\:\  /  /:/__\/
               /__/:/        \  \:\_\/     \  \:\/:/ /__/:/
               \__\/          \  \:\        \  \::/  \__\/
                               \__\/         \__\/

  Project:       AVR Universal BootLoader Test

  File:          test.c
                 Send a character every interval(default is '>'). If receive
                 any data from uart, it will send received data instead. If
                 receive data is '~', it will go into a dead loop, waiting for
                 watchdog reset.
                 This demo program used to check whether UART communication is
                 valid. You may use Window's Hypertrm to communicate.

  Version:       2.0

  Compiler:      WinAVR 20071221 + AVR Studio 4.14.589

  Author:        Shaoziyang
                 Shaoziyang@gmail.com
                 http://avrubd.googlepages.com
                 http://groups.google.com/group/avrub?hl=en

  Date:          2008.4

*/

#include "testcfg.h"
#include "bootldr.h"

#define PROG_START         0x0000

unsigned char dat = '>';

//send data to comport
void WriteCom(unsigned char dat)
{
#if RS485
  RS485Enable();
#endif

  UDRREG(COMPORTNo) = dat;
  while(!(UCSRAREG(COMPORTNo) & (1<<TXCBIT(COMPORTNo))));
  UCSRAREG(COMPORTNo) |= (1 << TXCBIT(COMPORTNo));

#if RS485
  RS485Disable();
#endif
}

int main()
{
  asm volatile("cli": : );

#if WDG_En
  //enable watchdog
  wdt_enable(WDTO_1S);
#endif

  ComInit();
  TimerInit();

#if RS485
  DDRREG(RS485PORT) |= (1 << RS485TXEn);
  RS485Disable();
#endif

#if LED_En
  //set LED port to output
  DDRREG(LEDPORT) |= (1 << LEDPORTNo);
#endif

  while(1)
  {
#if WDG_En
    //clear watchdog
    wdt_reset();
#endif

    if(TIFRREG & (1<<OCF1A))    //T1 overflow
    {
      TIFRREG |= (1 << OCF1A);

#if LED_En
      //toggle LED
      LEDAlt();
#endif

      if(DataInCom())
      {
        dat = ReadCom();
        if(dat == '~')
#if WDG_En
          while(1);             //dead loop, wait watchdog reset
#else
          (*((void(*)(void))PROG_START))();  //jump to begin
#endif
      }
      WriteCom(dat);

    }
  }
}

//End of file: test.c
