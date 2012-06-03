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


  Project:       AVR 通用 Bootloader
  File:          bootldr.c
                 主程序
  Version:       4.5

  Compiler:      WinAVR 20071221 + AVR Studio 4.14.589

  Author:        Shaoziyang
                 Shaoziyang@gmail.com
                 http://avrubd.googlepages.com
                 http://groups.google.com/group/avrub?hl=en
                 
  Date:          2008.8

  查看 readme.htm 获得更多相关内容.

*/

#include "bootcfg.h"
#include "bootldr.h"

//用户程序起始地
#define PROG_START         0x0000

//删除IVT
#define noIVT              0

//接收缓冲区大小不能小于 SPM_PAGESIZE
#if (BUFFERSIZE < SPM_PAGESIZE)
#define BUFSIZE SPM_PAGESIZE
#else
#define BUFSIZE BUFFERSIZE
#endif

//接收缓冲区
unsigned char buf[BUFSIZE];

#if (BUFSIZE > 255)
unsigned int bufptr, pagptr;
#else
unsigned char bufptr, pagptr;
#endif

unsigned char ch, cl;

//当前Flash地址指针
#if (FLASHEND > 0xFFFFUL)
unsigned long int FlashAddr;
#else
unsigned int FlashAddr;
#endif

//包含解密子程序文件
#if Decrypt

//PC1 解密算法子程序
#if (Algorithm == PC1_128)||(Algorithm == PC1_256)
#include "pc1crypt.c"
#elif (Algorithm == AES_128)||(Algorithm == AES_256)
#include "aes.c"
#else
#error "Unknow encrypt algorithm!"
#endif

#endif  //Decrypt


//更新一个Flash页
void write_one_page(unsigned char *buf)
{
  boot_page_erase(FlashAddr);                  //擦除一个Flash页
  boot_spm_busy_wait();
  for(pagptr = 0; pagptr < SPM_PAGESIZE; pagptr += 2) //数据填入Flash缓冲页
  {
    boot_page_fill(pagptr, buf[pagptr] | (buf[pagptr + 1] << 8));
  }
  boot_page_write(FlashAddr);                  //将缓冲页数据写入一个Flash页
  boot_spm_busy_wait();                        //等待页编程完成
}

//跳转到用户程序
void quit()
{
#if Decrypt
  DestroyKey();                                //销毁密钥
#endif

#if SafeMode
  if(eeprom_read_byte((uint8_t *)FlagAddr))
    (*((void(*)(void))(BootStart)))();         //跳转到 bootloader
#endif

  boot_rww_enable();                           //允许用户程序区读写
  (*((void(*)(void))PROG_START))();            //跳转，这样比'jmp 0'节省空间
}

//写入数据到串口
void WriteCom(unsigned char dat)
{
#if RS485
  RS485Enable();
#endif

  UDRREG(COMPORTNo) = dat;
  //等待数据发送完成
  while(!(UCSRAREG(COMPORTNo) & (1<<TXCBIT(COMPORTNo))));
  UCSRAREG(COMPORTNo) |= (1 << TXCBIT(COMPORTNo));

#if RS485
  RS485Disable();
#endif
}

//等待串口数据
unsigned char WaitCom()
{
  while(!DataInCom());
  return ReadCom();
}

#if VERBOSE
//发送字符串并添加回车
void putstr(const char *str)
{
  while(*str)
    WriteCom(*str++);

  WriteCom(0x0D);
  WriteCom(0x0A);
}
#endif

//计算CRC校验：1021
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
    //标准CRC校验
    crc = (crc ^ (((unsigned int) *buf) << 8));
    for(i = 8; i > 0; i--)
    {
      t = crc << 1;
      if(crc & 0x8000)
        t = t ^ 0x1021;
      crc = t;
    }
#elif (CRCMODE == 1)
    //累加和校验
    crc += (unsigned int)(*buf);
#elif

#endif
    buf++;
  }
  ch = crc / 256;
  cl = crc % 256;
}

//如果你要删除中断向量表(interrupt vect table), 定义 noIVT 为 1
//并在链接选项中添加 "-nostartfiles" 选项
//删除中断向量表
#if noIVT
void initstack(void) __attribute__ ((section(".init9")));
void initstack(void) 
{
  //设置堆栈
  asm volatile ( ".set __stack, %0" :: "i" (RAMEND) ); 
  //跳转到主程序
  asm volatile ( "rjmp main");
}
#endif

//主程序
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

  //关闭中断，异常情况下的保护
  cli();

#if WDG_En
  //允许看门狗，设置看门狗超时时间
  wdt_enable(WDTO_1S);
#else
  //禁止看门狗
#ifndef MCUSR
#define MCUSR    MCUCSR
#endif
  MCUSR = 0;
  wdt_disable();
#endif

  //时钟初始化:CTC模式
  TimerInit();

#if RS485
  //初始化 RS485 端口
  DDRREG(RS485PORT) |= (1 << RS485TXEn);
  RS485Disable();
#endif

#if LED_En
  //设置LED端口为输出状态
  DDRREG(LEDPORT) |= (1 << LEDPORTNo);
#endif

  //串口初始化
  ComInit();

#if InitDelay > 0
  //某些早期型号的单片机需要额外延时
  for(di = InitDelay; di > 0; di--)
    __asm__ __volatile__ ("nop": : );
#endif

#if LEVELMODE
  //引脚电平启动Bootloader模式
  //设置端口为输入状态
  DDRREG(LEVELPORT) &= ~(1 << LEVELPIN);
#if PINLEVEL
  if(PINREG(LEVELPORT) & (1 << LEVELPIN))
#else
  if(!(PINREG(LEVELPORT) & (1 << LEVELPIN)))
#endif
  {
#if VERBOSE
    //提示进入升级模式
    putstr(msg6);
#endif
  }
  else
  {
#if VERBOSE
    //提示进入用户程序
    putstr(msg7);
#endif

    quit();
  }

#else

//串口启动Bootloader模式
#if VERBOSE
  //提示等待密码
  putstr(msg1);
#endif

  cnt = TimeOutCnt;
  cl = 0;
  while(1)
  {
#if WDG_En
    //清除看门狗
    wdt_reset();
#endif

    if(TIFRREG & (1<<OCF1A))    //T1溢出
    {
      TIFRREG |= (1 << OCF1A);

      if(cl == CONNECTCNT)      //判断连接密码
        break;

#if LED_En
      LEDAlt();                 //LED指示状态
#endif

      cnt--;
      if(cnt == 0)              //连接超时
      {

#if VERBOSE
        putstr(msg2);           //提示超时
#endif
        quit();                 //退出bootloader
      }
    }

    if(DataInCom())             //接收到连接密码
    {
      if(ReadCom() == ConnectKey[cl])  //比较密码
        cl++;
      else
        cl = 0;
    }
  }

#endif  //LEVELMODE

#if VERBOSE
  putstr(msg3);                 //提示等待接收文件
#endif

//安全模式下，在EEPROM中设置标志位
#if SafeMode
  eeprom_write_byte((uint8_t *)FlagAddr, 0x1);
#endif

#if Decrypt
  DecryptInit();
#endif

  //每个时隙向PC机发送一个控制字符“C”，等待控制字〈soh〉
  cnt = TimeOutCntC;
  while(1)
  {
    if(TIFRREG & (1 << OCF1A))  //T1溢出
    {
      TIFRREG |= (1 << OCF1A);
      WriteCom(XMODEM_RWC) ;    //发送 "C"

#if LED_En
      LEDAlt();                 //LED指示状态
#endif

      cnt--;
      if(cnt == 0)              //超时
      {
#if VERBOSE
        putstr(msg2);           //提示超时
#endif
        quit();                 //退出 bootloader
      }
    }

#if WDG_En
    wdt_reset();                //清除看门狗
#endif

    if(DataInCom())
    {
      if(ReadCom() == XMODEM_SOH)  //XMODEM命令开始
        break;
    }
  }

  TCCR1B = 0;                   //关闭定时器1

  //开始接受数据
  packNO = 0;
  bufptr = 0;
  cnt = 0;
  FlashAddr = 0;
  do
  {
    packNO++;
    ch =  WaitCom();                          //获取包序号
    cl = ~WaitCom();
    if ((packNO == ch) && (packNO == cl))
    {
      for(li = BUFFERSIZE; li > 0; li--)      //接收完整一帧数据
      {
        buf[bufptr++] = WaitCom();
      }
      crch = WaitCom();                       //获取校验字节
      crcl = WaitCom();
      crc16(&buf[bufptr - BUFFERSIZE]);       //计算校验
      if((crch == ch) && (crcl == cl))
      {
#if BootStart
        if(FlashAddr < BootStart)             //避免写入Boot区
        {
#endif

#if Decrypt
          DecryptBlock(&buf[bufptr - BUFFERSIZE], BUFFERSIZE); //解密缓冲区
#endif

#if BUFFERSIZE <= SPM_PAGESIZE
          if(bufptr >= SPM_PAGESIZE)          //缓冲区满，写入数据；否则继续接收
          {                                   //接收多个帧，写入一页
            write_one_page(buf);              //写入缓冲区内容到Flash中
            FlashAddr += SPM_PAGESIZE;        //修改Flash页地址
            bufptr = 0;
          }
#else
          while(bufptr > 0)                   //接收一帧，写入多个页面
          {
            write_one_page(&buf[BUFSIZE - bufptr]);
            FlashAddr += SPM_PAGESIZE;        //修改Flash页地址
            bufptr -= SPM_PAGESIZE;
          }
#endif

#if BootStart
        }
        else                                  //超过BootStart范围，忽略写操作
        {
          bufptr = 0;                         //重置接收指针
        }
#endif

//读取写入的Flash内容并和缓冲区的内容做比较
#if (ChipCheck > 0) && (BootStart > 0)
#if (BUFFERSIZE < SPM_PAGESIZE)
        if((bufptr == 0) && (FlashAddr < BootStart))
#else
        if(FlashAddr < BootStart)
#endif
        {
          boot_rww_enable();                  //允许读用户程序
          cl = 1;                             //清除错误标志位
          for(pagptr = 0; pagptr < BUFSIZE; pagptr++)
          {
#if (FLASHEND > 0xFFFFUL)
            if(pgm_read_byte_far(FlashAddr - BUFSIZE + pagptr) != buf[pagptr])
#else
            if(pgm_read_byte(FlashAddr - BUFSIZE + pagptr) != buf[pagptr])
#endif
            {
              cl = 0;                         //设置错误标志位
              break;
            }
          }
          if(cl)                              //校验正确，发送正常反馈
          {
            WriteCom(XMODEM_ACK);
            cnt = 0;
          }
          else
          {
            WriteCom(XMODEM_NAK);             //校验错误，请求重发
            cnt++;                            //错误计数加1
            FlashAddr -= BUFSIZE;             //修正FlashAddr地址
          }
        }
        else                                  //不校验Boot区，直接发送正确回应
        {
          WriteCom(XMODEM_ACK);
          cnt = 0;
        }
#else
        WriteCom(XMODEM_ACK);                 //不校验，直接发送正确响应
        cnt = 0;
#endif

#if WDG_En
        wdt_reset();                          //清除看门狗
#endif

#if LED_En
        LEDAlt();                             //LED 指示升级状态
#endif
      }
      else //CRC
      {
        WriteCom(XMODEM_NAK);                 //要求重发数据
        cnt++;
      }
    }
    else //PackNo
    {
      WriteCom(XMODEM_NAK);                   //要求重发数据
      cnt++;
    }

    if(cnt > 3)                               //连续出错次数太多，中止升级
      break;
  }
  while(WaitCom() != XMODEM_EOT);
  WriteCom(XMODEM_ACK);


#if VERBOSE
  if(cnt == 0)
  {
#if SafeMode
    //安全模式时清除标志位
    eeprom_write_byte((uint8_t *)FlagAddr, 0);
#endif

    //升级成功
    putstr(msg4);                             //提示升级成功
  }
  else
  {
    //升级失败
    putstr(msg5);                             //提示升级失败

#if WDG_En
    while(1);                                 //等待看门狗复位
#else
#if (BootStart > 0)
    (*((void(*)(void))(BootStart)))();        //跳转到 bootloader
#endif
#endif
  }

#else

  if(cnt > 0)
  {
#if WDG_En
    while(1);                                 //升级失败，等待看门狗复位
#else    
#if (BootStart > 0)
    (*((void(*)(void))(BootStart)))();
#endif
#endif
  }
#if SafeMode
  else
  {
    //安全模式时清除标志位
    eeprom_write_byte((uint8_t *)FlagAddr, 0);
  }
#endif

#endif

  quit();                                     //退出bootloader
  return 0;
}

//End of file: bootldr.c
