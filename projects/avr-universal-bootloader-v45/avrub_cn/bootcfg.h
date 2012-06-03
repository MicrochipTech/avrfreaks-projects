/*

                           e Y8b    Y8b YV4.08P888 88e
                          d8b Y8b    Y8b Y888P 888 888D
                         d888b Y8b    Y8b Y8P  888 88"
                        d8WuHan888b    Y8b Y   888 b,
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


  Project:       AVR 通用 BootLoader
  File:          bootcfg.h
                 用户配置参数，请根据你系统的实际情况进行设置
  Version:       4.5

  Compiler:      WinAVR20071221 + AVR Studio 4.14 b589

  Author:        Shaoziyang
                 Shaoziyang@gmail.com
                 http://avrubd.googlepages.com
                 http://groups.google.com/group/avrub?hl=en
                 
  Date:          2008.8

  See readme.htm to get more information.

*/

#ifndef _BOOTCFG_H_
#define _BOOTCFG_H_        1

//定义串口接收缓冲区长度
#define BUFFERSIZE         128

//系统时钟(Hz)
#ifndef F_CPU
#define F_CPU              8000000UL
#endif

//波特率
#define BAUDRATE           19200

//Boot区起始地址: 按字节计算
//如果定义为0或不定义BootStart，可以节约空间
#define BootStart          2 * 0x1C00UL

//下载过程中进行数据校验
//ChipCheck和BootStart必须联合使用，校验才有效
#define ChipCheck          1

//安全升级模式下，只有升级成功后才会退出安全模式，适合远程升级
#define SafeMode           1

//在安全升级模式下，将在EEPROM中设置一个标志位
#define FlagAddr           E2END

//Bootloader触发模式  0:串口  1:电平
#define LEVELMODE          0

#define LEVELPORT          D
#define LEVELPIN           PD7
//触发电平 1:高  0:低
#define PINLEVEL           0

//等待密码的超时时间 = TimeOutCnt * timeclk
//超时次数
#define TimeOutCnt         20

//同步时间间隔(ms)
#define timeclk            500

//等待文件的超时时间 = TimeOutCntC * timeclk
//发送'C'的最大次数
#define TimeOutCntC        50

//同步密码长度
#define CONNECTCNT         5

//同步密码
#if LEVELMODE == 0
unsigned char ConnectKey[] = {0x41, 0x56, 0x52, 0x55, 0x42};
#endif

//串口号: 0/1/2..
#define COMPORTNo          0

//看门狗使能
#define WDG_En             1

//使用RS485模式
#define RS485              0
//RS485/RS422控制端口和引脚
#define RS485PORT          E
#define RS485TXEn          PE2

//使用LED指示状态
#define LED_En             1
//LED控制端口和引脚
#define LEDPORT            A
#define LEDPORTNo          PA4

//某些旧型号的单片机在初始化后需要延时
#define InitDelay          0

//校验方法  0:CRC16  1:累加和
#define CRCMODE            0

//提示模式: 显示更多交互提示信息，不用可以节省空间
#define VERBOSE            1

//提示信息
#if VERBOSE
#if LEVELMODE
const char msg6[] = "升级模式.";
const char msg7[] = "正常启动.";
#else
//等待密码
const char msg1[] = "等待接收密码.";
#endif
//超时
const char msg2[] = "超时.";
//等待接收数据
const char msg3[] = "等待接收数据.";
//升级成功
const char msg4[] = "升级成功.";
//升级失败
const char msg5[] = "升级失败.";
#endif

//允许解密
#define Decrypt            0

//加密算法
//PC1_128, 128位 PC1 加密算法
#define PC1_128            1
//PC1_256, 256位 PC1 加密算法
#define PC1_256            2
//AES_128, 128位 AES 加密算法
#define AES_128            3
//AES_256, 256位 AED 加密算法
#define AES_256            4

#define Algorithm          PC1_128

#if Decrypt

#if (Algorithm == PC1_128) || (Algorithm == AES_128)
//定义解密密钥: 128位
unsigned char DecryptKey[16] ={
  0xD0, 0x94, 0x3F, 0x8C, 0x29, 0x76, 0x15, 0xD8,
  0x20, 0x40, 0xE3, 0x27, 0x45, 0xD8, 0x48, 0xAD
};
#elif (Algorithm == PC1_256) || (Algorithm == AES_256)
//定义解密密钥: 256位
unsigned char DecryptKey[32] ={
  0xD0, 0x94, 0x3F, 0x8C, 0x29, 0x76, 0x15, 0xD8,
  0x20, 0x40, 0xE3, 0x27, 0x45, 0xD8, 0x48, 0xAD,
  0xEA, 0x8B, 0x2A, 0x73, 0x16, 0xE9, 0xB0, 0x49,
  0x45, 0xB3, 0x39, 0x28, 0x0A, 0xC3, 0x28, 0x3C
};

#endif

#endif

#endif

//End of file: bootcfg.h
