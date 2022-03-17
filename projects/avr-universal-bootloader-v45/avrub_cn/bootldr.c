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


  Project:       AVR ͨ�� Bootloader
  File:          bootldr.c
                 ������
  Version:       4.5

  Compiler:      WinAVR 20071221 + AVR Studio 4.14.589

  Author:        Shaoziyang
                 Shaoziyang@gmail.com
                 http://avrubd.googlepages.com
                 http://groups.google.com/group/avrub?hl=en
                 
  Date:          2008.8

  �鿴 readme.htm ��ø����������.

*/

#include "bootcfg.h"
#include "bootldr.h"

//�û�������ʼ��
#define PROG_START         0x0000

//ɾ��IVT
#define noIVT              0

//���ջ�������С����С�� SPM_PAGESIZE
#if (BUFFERSIZE < SPM_PAGESIZE)
#define BUFSIZE SPM_PAGESIZE
#else
#define BUFSIZE BUFFERSIZE
#endif

//���ջ�����
unsigned char buf[BUFSIZE];

#if (BUFSIZE > 255)
unsigned int bufptr, pagptr;
#else
unsigned char bufptr, pagptr;
#endif

unsigned char ch, cl;

//��ǰFlash��ַָ��
#if (FLASHEND > 0xFFFFUL)
unsigned long int FlashAddr;
#else
unsigned int FlashAddr;
#endif

//���������ӳ����ļ�
#if Decrypt

//PC1 �����㷨�ӳ���
#if (Algorithm == PC1_128)||(Algorithm == PC1_256)
#include "pc1crypt.c"
#elif (Algorithm == AES_128)||(Algorithm == AES_256)
#include "aes.c"
#else
#error "Unknow encrypt algorithm!"
#endif

#endif  //Decrypt


//����һ��Flashҳ
void write_one_page(unsigned char *buf)
{
  boot_page_erase(FlashAddr);                  //����һ��Flashҳ
  boot_spm_busy_wait();
  for(pagptr = 0; pagptr < SPM_PAGESIZE; pagptr += 2) //��������Flash����ҳ
  {
    boot_page_fill(pagptr, buf[pagptr] | (buf[pagptr + 1] << 8));
  }
  boot_page_write(FlashAddr);                  //������ҳ����д��һ��Flashҳ
  boot_spm_busy_wait();                        //�ȴ�ҳ������
}

//��ת���û�����
void quit()
{
#if Decrypt
  DestroyKey();                                //������Կ
#endif

#if SafeMode
  if(eeprom_read_byte((uint8_t *)FlagAddr))
    (*((void(*)(void))(BootStart)))();         //��ת�� bootloader
#endif

  boot_rww_enable();                           //�����û���������д
  (*((void(*)(void))PROG_START))();            //��ת��������'jmp 0'��ʡ�ռ�
}

//д�����ݵ�����
void WriteCom(unsigned char dat)
{
#if RS485
  RS485Enable();
#endif

  UDRREG(COMPORTNo) = dat;
  //�ȴ����ݷ������
  while(!(UCSRAREG(COMPORTNo) & (1<<TXCBIT(COMPORTNo))));
  UCSRAREG(COMPORTNo) |= (1 << TXCBIT(COMPORTNo));

#if RS485
  RS485Disable();
#endif
}

//�ȴ���������
unsigned char WaitCom()
{
  while(!DataInCom());
  return ReadCom();
}

#if VERBOSE
//�����ַ�������ӻس�
void putstr(const char *str)
{
  while(*str)
    WriteCom(*str++);

  WriteCom(0x0D);
  WriteCom(0x0A);
}
#endif

//����CRCУ�飺1021
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
    //��׼CRCУ��
    crc = (crc ^ (((unsigned int) *buf) << 8));
    for(i = 8; i > 0; i--)
    {
      t = crc << 1;
      if(crc & 0x8000)
        t = t ^ 0x1021;
      crc = t;
    }
#elif (CRCMODE == 1)
    //�ۼӺ�У��
    crc += (unsigned int)(*buf);
#elif

#endif
    buf++;
  }
  ch = crc / 256;
  cl = crc % 256;
}

//�����Ҫɾ���ж�������(interrupt vect table), ���� noIVT Ϊ 1
//��������ѡ������� "-nostartfiles" ѡ��
//ɾ���ж�������
#if noIVT
void initstack(void) __attribute__ ((section(".init9")));
void initstack(void) 
{
  //���ö�ջ
  asm volatile ( ".set __stack, %0" :: "i" (RAMEND) ); 
  //��ת��������
  asm volatile ( "rjmp main");
}
#endif

//������
int main(void)
{
  unsigned char cnt;
  unsigned char packNO;
  unsigned char crch, crcl;

#if (InitDelay > 0)
#if (InitDelay > 255)
  unsigned int di;
#elif InitDelay
  unsigned char di;
#endif
#endif

#if (BUFFERSIZE > 255)
  unsigned int li;
#else
  unsigned char li;
#endif

  //�ر��жϣ��쳣����µı���
  cli();

#if WDG_En
  //�����Ź������ÿ��Ź���ʱʱ��
  wdt_enable(WDTO_1S);
#else
  //��ֹ���Ź�
#ifndef MCUSR
#define MCUSR    MCUCSR
#endif
  MCUSR = 0;
  wdt_disable();
#endif

  //ʱ�ӳ�ʼ��:CTCģʽ
  TimerInit();

#if RS485
  //��ʼ�� RS485 �˿�
  DDRREG(RS485PORT) |= (1 << RS485TXEn);
  RS485Disable();
#endif

#if LED_En
  //����LED�˿�Ϊ���״̬
  DDRREG(LEDPORT) |= (1 << LEDPORTNo);
#endif

  //���ڳ�ʼ��
  ComInit();

#if InitDelay > 0
  //ĳЩ�����ͺŵĵ�Ƭ����Ҫ������ʱ
  for(di = InitDelay; di > 0; di--)
    __asm__ __volatile__ ("nop": : );
#endif

#if LEVELMODE
  //���ŵ�ƽ����Bootloaderģʽ
  //���ö˿�Ϊ����״̬
  DDRREG(LEVELPORT) &= ~(1 << LEVELPIN);
#if PINLEVEL
  if(PINREG(LEVELPORT) & (1 << LEVELPIN))
#else
  if(!(PINREG(LEVELPORT) & (1 << LEVELPIN)))
#endif
  {
#if VERBOSE
    //��ʾ��������ģʽ
    putstr(msg6);
#endif
  }
  else
  {
#if VERBOSE
    //��ʾ�����û�����
    putstr(msg7);
#endif

    quit();
  }

#else

//��������Bootloaderģʽ
#if VERBOSE
  //��ʾ�ȴ�����
  putstr(msg1);
#endif

  cnt = TimeOutCnt;
  cl = 0;
  while(1)
  {
#if WDG_En
    //������Ź�
    wdt_reset();
#endif

    if(TIFRREG & (1<<OCF1A))    //T1���
    {
      TIFRREG |= (1 << OCF1A);

      if(cl == CONNECTCNT)      //�ж���������
        break;

#if LED_En
      LEDAlt();                 //LEDָʾ״̬
#endif

      cnt--;
      if(cnt == 0)              //���ӳ�ʱ
      {

#if VERBOSE
        putstr(msg2);           //��ʾ��ʱ
#endif
        quit();                 //�˳�bootloader
      }
    }

    if(DataInCom())             //���յ���������
    {
      if(ReadCom() == ConnectKey[cl])  //�Ƚ�����
        cl++;
      else
        cl = 0;
    }
  }

#endif  //LEVELMODE

#if VERBOSE
  putstr(msg3);                 //��ʾ�ȴ������ļ�
#endif

//��ȫģʽ�£���EEPROM�����ñ�־λ
#if SafeMode
  eeprom_write_byte((uint8_t *)FlagAddr, 0x1);
#endif

#if Decrypt
  DecryptInit();
#endif

  //ÿ��ʱ϶��PC������һ�������ַ���C�����ȴ������֡�soh��
  cnt = TimeOutCntC;
  while(1)
  {
    if(TIFRREG & (1 << OCF1A))  //T1���
    {
      TIFRREG |= (1 << OCF1A);
      WriteCom(XMODEM_RWC) ;    //���� "C"

#if LED_En
      LEDAlt();                 //LEDָʾ״̬
#endif

      cnt--;
      if(cnt == 0)              //��ʱ
      {
#if VERBOSE
        putstr(msg2);           //��ʾ��ʱ
#endif
        quit();                 //�˳� bootloader
      }
    }

#if WDG_En
    wdt_reset();                //������Ź�
#endif

    if(DataInCom())
    {
      if(ReadCom() == XMODEM_SOH)  //XMODEM���ʼ
        break;
    }
  }

  TCCR1B = 0;                   //�رն�ʱ��1

  //��ʼ��������
  packNO = 0;
  bufptr = 0;
  cnt = 0;
  FlashAddr = 0;
  do
  {
    packNO++;
    ch =  WaitCom();                          //��ȡ�����
    cl = ~WaitCom();
    if ((packNO == ch) && (packNO == cl))
    {
      for(li = BUFFERSIZE; li > 0; li--)      //��������һ֡����
      {
        buf[bufptr++] = WaitCom();
      }
      crch = WaitCom();                       //��ȡУ���ֽ�
      crcl = WaitCom();
      crc16(&buf[bufptr - BUFFERSIZE]);       //����У��
      if((crch == ch) && (crcl == cl))
      {
#if BootStart
        if(FlashAddr < BootStart)             //����д��Boot��
        {
#endif

#if Decrypt
          DecryptBlock(&buf[bufptr - BUFFERSIZE], BUFFERSIZE); //���ܻ�����
#endif

#if BUFFERSIZE <= SPM_PAGESIZE
          if(bufptr >= SPM_PAGESIZE)          //����������д�����ݣ������������
          {                                   //���ն��֡��д��һҳ
            write_one_page(buf);              //д�뻺�������ݵ�Flash��
            FlashAddr += SPM_PAGESIZE;        //�޸�Flashҳ��ַ
            bufptr = 0;
          }
#else
          while(bufptr > 0)                   //����һ֡��д����ҳ��
          {
            write_one_page(&buf[BUFSIZE - bufptr]);
            FlashAddr += SPM_PAGESIZE;        //�޸�Flashҳ��ַ
            bufptr -= SPM_PAGESIZE;
          }
#endif

#if BootStart
        }
        else                                  //����BootStart��Χ������д����
        {
          bufptr = 0;                         //���ý���ָ��
        }
#endif

//��ȡд���Flash���ݲ��ͻ��������������Ƚ�
#if (ChipCheck > 0) && (BootStart > 0)
#if (BUFFERSIZE < SPM_PAGESIZE)
        if((bufptr == 0) && (FlashAddr < BootStart))
#else
        if(FlashAddr < BootStart)
#endif
        {
          boot_rww_enable();                  //������û�����
          cl = 1;                             //��������־λ
          for(pagptr = 0; pagptr < BUFSIZE; pagptr++)
          {
#if (FLASHEND > 0xFFFFUL)
            if(pgm_read_byte_far(FlashAddr - BUFSIZE + pagptr) != buf[pagptr])
#else
            if(pgm_read_byte(FlashAddr - BUFSIZE + pagptr) != buf[pagptr])
#endif
            {
              cl = 0;                         //���ô����־λ
              break;
            }
          }
          if(cl)                              //У����ȷ��������������
          {
            WriteCom(XMODEM_ACK);
            cnt = 0;
          }
          else
          {
            WriteCom(XMODEM_NAK);             //У����������ط�
            cnt++;                            //���������1
            FlashAddr -= BUFSIZE;             //����FlashAddr��ַ
          }
        }
        else                                  //��У��Boot����ֱ�ӷ�����ȷ��Ӧ
        {
          WriteCom(XMODEM_ACK);
          cnt = 0;
        }
#else
        WriteCom(XMODEM_ACK);                 //��У�飬ֱ�ӷ�����ȷ��Ӧ
        cnt = 0;
#endif

#if WDG_En
        wdt_reset();                          //������Ź�
#endif

#if LED_En
        LEDAlt();                             //LED ָʾ����״̬
#endif
      }
      else //CRC
      {
        WriteCom(XMODEM_NAK);                 //Ҫ���ط�����
        cnt++;
      }
    }
    else //PackNo
    {
      WriteCom(XMODEM_NAK);                   //Ҫ���ط�����
      cnt++;
    }

    if(cnt > 3)                               //�����������̫�࣬��ֹ����
      break;
  }
  while(WaitCom() != XMODEM_EOT);
  WriteCom(XMODEM_ACK);


#if VERBOSE
  if(cnt == 0)
  {
#if SafeMode
    //��ȫģʽʱ�����־λ
    eeprom_write_byte((uint8_t *)FlagAddr, 0);
#endif

    //�����ɹ�
    putstr(msg4);                             //��ʾ�����ɹ�
  }
  else
  {
    //����ʧ��
    putstr(msg5);                             //��ʾ����ʧ��

#if WDG_En
    while(1);                                 //�ȴ����Ź���λ
#else
#if (BootStart > 0)
    (*((void(*)(void))(BootStart)))();        //��ת�� bootloader
#endif
#endif
  }

#else

  if(cnt > 0)
  {
#if WDG_En
    while(1);                                 //����ʧ�ܣ��ȴ����Ź���λ
#else    
#if (BootStart > 0)
    (*((void(*)(void))(BootStart)))();
#endif
#endif
  }
#if SafeMode
  else
  {
    //��ȫģʽʱ�����־λ
    eeprom_write_byte((uint8_t *)FlagAddr, 0);
  }
#endif

#endif

  quit();                                     //�˳�bootloader
  return 0;
}

//End of file: bootldr.c
