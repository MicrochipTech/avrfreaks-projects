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
                 AVR 通用 Bootloader 测试
  File:          testcfg.h
                 test.c的用户参数配置文件，请根据你系统的实际参数进行设置。
  Version:       2.0

  Compiler:      WinAVR 20071221 + AVR Studio 4.14.589

  Author:        Shaoziyang
                 Shaoziyang@gmail.com
                 http://avrubd.googlepages.com
                 http://groups.google.com/group/avrub?hl=en

  Date:          2008.4

  See readme.htm to get more information.

*/

#ifndef _TESTCFG_H_
#define _TESTCFG_H_        1

//定义串口接收缓冲区长度
#define BUFFERSIZE         128

//系统时钟(Hz)
#ifndef F_CPU
#define F_CPU              8000000UL
#endif

//波特率(bps)
#define BAUDRATE           19200

//同步时间间隔(ms)
#define timeclk            200

//串口号：0/1/2...
#define COMPORTNo          0

//看门狗使能
#define WDG_En             1

//使用485模式
#define RS485              0

//485控制端口和引脚
#define RS485PORT          C
#define RS485TXEn          PC1

//使用LED指示状态
#define LED_En             1
#define LEDPORT            D
#define LEDPORTNo          PD4

//某些型号的单片机在初始化后需要延时
#define InitDelay          0

#endif

//End of file: testcfg.h
