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


  Project:       AVR ͨ�� BootLoader
  File:          bootcfg.h
                 �û����ò������������ϵͳ��ʵ�������������
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

//���崮�ڽ��ջ���������
#define BUFFERSIZE         128

//ϵͳʱ��(Hz)
#ifndef F_CPU
#define F_CPU              8000000UL
#endif

//������
#define BAUDRATE           19200

//Boot����ʼ��ַ: ���ֽڼ���
//�������Ϊ0�򲻶���BootStart�����Խ�Լ�ռ�
#define BootStart          2 * 0x1C00UL

//���ع����н�������У��
//ChipCheck��BootStart��������ʹ�ã�У�����Ч
#define ChipCheck          1

//��ȫ����ģʽ�£�ֻ�������ɹ���Ż��˳���ȫģʽ���ʺ�Զ������
#define SafeMode           1

//�ڰ�ȫ����ģʽ�£�����EEPROM������һ����־λ
#define FlagAddr           E2END

//Bootloader����ģʽ  0:����  1:��ƽ
#define LEVELMODE          0

#define LEVELPORT          D
#define LEVELPIN           PD7
//������ƽ 1:��  0:��
#define PINLEVEL           0

//�ȴ�����ĳ�ʱʱ�� = TimeOutCnt * timeclk
//��ʱ����
#define TimeOutCnt         20

//ͬ��ʱ����(ms)
#define timeclk            500

//�ȴ��ļ��ĳ�ʱʱ�� = TimeOutCntC * timeclk
//����'C'��������
#define TimeOutCntC        50

//ͬ�����볤��
#define CONNECTCNT         5

//ͬ������
#if LEVELMODE == 0
unsigned char ConnectKey[] = {0x41, 0x56, 0x52, 0x55, 0x42};
#endif

//���ں�: 0/1/2..
#define COMPORTNo          0

//���Ź�ʹ��
#define WDG_En             1

//ʹ��RS485ģʽ
#define RS485              0
//RS485/RS422���ƶ˿ں�����
#define RS485PORT          E
#define RS485TXEn          PE2

//ʹ��LEDָʾ״̬
#define LED_En             1
//LED���ƶ˿ں�����
#define LEDPORT            A
#define LEDPORTNo          PA4

//ĳЩ���ͺŵĵ�Ƭ���ڳ�ʼ������Ҫ��ʱ
#define InitDelay          0

//У�鷽��  0:CRC16  1:�ۼӺ�
#define CRCMODE            0

//��ʾģʽ: ��ʾ���ཻ����ʾ��Ϣ�����ÿ��Խ�ʡ�ռ�
#define VERBOSE            1

//��ʾ��Ϣ
#if VERBOSE
#if LEVELMODE
const char msg6[] = "����ģʽ.";
const char msg7[] = "��������.";
#else
//�ȴ�����
const char msg1[] = "�ȴ���������.";
#endif
//��ʱ
const char msg2[] = "��ʱ.";
//�ȴ���������
const char msg3[] = "�ȴ���������.";
//�����ɹ�
const char msg4[] = "�����ɹ�.";
//����ʧ��
const char msg5[] = "����ʧ��.";
#endif

//�������
#define Decrypt            0

//�����㷨
//PC1_128, 128λ PC1 �����㷨
#define PC1_128            1
//PC1_256, 256λ PC1 �����㷨
#define PC1_256            2
//AES_128, 128λ AES �����㷨
#define AES_128            3
//AES_256, 256λ AED �����㷨
#define AES_256            4

#define Algorithm          PC1_128

#if Decrypt

#if (Algorithm == PC1_128) || (Algorithm == AES_128)
//���������Կ: 128λ
unsigned char DecryptKey[16] ={
  0xD0, 0x94, 0x3F, 0x8C, 0x29, 0x76, 0x15, 0xD8,
  0x20, 0x40, 0xE3, 0x27, 0x45, 0xD8, 0x48, 0xAD
};
#elif (Algorithm == PC1_256) || (Algorithm == AES_256)
//���������Կ: 256λ
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
