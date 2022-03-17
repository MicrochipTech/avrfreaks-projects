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
  File:          bootldr.h
                 ��Ҫ�ĺ궨��Ͳ�������
  Version:       4.5

  Compiler:      WinAVR 20071221 + AVR Studio 4.14.589

  Author:        Shaoziyang
                 Shaoziyang@gmail.com
                 http://avrubd.googlepages.com
                 http://groups.google.com/group/avrub?hl=en

  Date:          2008.8

  See readme.htm to get more information.

*/

#ifndef _BOOTLDR_H_
#define _BOOTLDR_H_        1

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

///////////////////////////////////////////////////////////////
//�������ݲ�Ҫ����޸ģ�������֪����Ҫ��ʲô������ʲô
///////////////////////////////////////////////////////////////

//ĳЩ�汾��ͷ�ļ���û�а����˶���
#ifndef SPM_PAGESIZE
#error "Not define SPM_PAGESIZE, please define below or update your WinAVR"
//#define SPM_PAGESIZE       XXX
#endif

//ĳЩ�汾��ͷ�ļ���û�а����˶���
#ifndef FLASHEND
#error "Not define FLASHEND, please define below or update your WinAVR"
//#define FLASHEND           XXX
#endif

//����������֮������Ǳ�����Լ����ϵ
#if BUFFERSIZE >= SPM_PAGESIZE
#if (BUFFERSIZE / SPM_PAGESIZE * SPM_PAGESIZE) != BUFFERSIZE
#error "Result of (BUFFERSIZE / SPM_PAGESIZE) is not a Integer!"
#error "Please check and set 'BUFFERSIZE/SPM_PAGESIZE' Macro again!"
#endif
#else
#if (SPM_PAGESIZE /BUFFERSIZE * BUFFERSIZE) != SPM_PAGESIZE
#error "Result of (BUFFERSIZE / SPM_PAGESIZE) is not a Integer!"
#error "Please check and set 'BUFFERSIZE/SPM_PAGESIZE' Macro again!"
#endif
#endif

//���㲨���ʼĴ���
#define BAUDREG            ((unsigned int)((F_CPU * 10) / (16UL * BAUDRATE) - 5) / 10)

//��鲨�������
//����Ķ����ڲ�ͬ��C������֮����ܻ��в���
#define FreqTemp           (16UL * BAUDRATE * (((F_CPU * 10) / (16 * BAUDRATE) + 5)/ 10))
#if ((FreqTemp * 50) > (51 * F_CPU) || (FreqTemp * 50) < (49 * F_CPU))
#error "BaudRate error > 2% ! Please check BaudRate and F_CPU value."
#endif

//�ڲ�ʹ�õĺ궨��
#define CONCAT(a, b)       a ## b
#define CONCAT3(a, b, c)   a ## b ## c

//�˿��Լ�λ����
#define PORTREG(No)        CONCAT(PORT, No)
#define PINREG(No)         CONCAT(PIN, No)
#define UDRREG(No)         CONCAT(UDR, No)
#define DDRREG(No)         CONCAT(DDR, No)
#define TXCBIT(No)         CONCAT(TXC, No)
#define RXCBIT(No)         CONCAT(RXC, No)
#define RXENBIT(No)        CONCAT(RXEN, No)
#define TXENBIT(No)        CONCAT(TXEN, No)
#define URSELBIT(No)       CONCAT(URSEL, No)

//���ڳ�ʼ����Ҫ�Ĵ���
#define UBRRHREG(No)       CONCAT3(UBRR, No, H)
#define UBRRLREG(No)       CONCAT3(UBRR, No, L)
#define UCSRAREG(No)       CONCAT3(UCSR, No, A)
#define UCSRBREG(No)       CONCAT3(UCSR, No, B)
#define UCSRCREG(No)       CONCAT3(UCSR, No, C)
#define UCSZBIT(No1, No2)  CONCAT3(UCSZ, No1, No2)

//����AVR��Ƭ�����ô���ʱ��Ҫʹ��URSELλ
#if defined(URSEL) || defined(URSEL0)
#define USEURSEL           (1 << URSELBIT(COMPORTNo))
#else
#define USEURSEL           0
#endif

//���崮��0��ؼĴ���
#if !defined(UDR0)
#define UBRR0H             UBRRH
#define UBRR0L             UBRRL
#define UCSR0A             UCSRA
#define UCSR0B             UCSRB
#define UCSR0C             UCSRC
#define UDR0               UDR
#define TXC0               TXC
#define RXC0               RXC
#define RXEN0              RXEN
#define TXEN0              TXEN
#define UCSZ01             UCSZ1
#define UCSZ00             UCSZ0
#define URSEL0             URSEL
#endif

//���ڳ�ʼ��
#define ComInit()                                                         \
        {                                                                 \
                           UCSRAREG(COMPORTNo) = 0;                       \
                           UCSRBREG(COMPORTNo) = (1 << RXENBIT(COMPORTNo))|(1 << TXENBIT(COMPORTNo)); \
                           UCSRCREG(COMPORTNo) = USEURSEL|(1 << UCSZBIT(COMPORTNo, 1))|(1 << UCSZBIT(COMPORTNo, 0));\
                           UBRRHREG(COMPORTNo) = BAUDREG/256;             \
                           UBRRLREG(COMPORTNo) = BAUDREG%256;             \
        }


//��תLED���״̬
#define LEDAlt()           PORTREG(LEDPORT) ^= (1 << LEDPORTNo)

//ʹ�ö�ʱ��1��1024��Ƶ��CTCģʽ4�������Ժ���Ϊ��λ��ʱ��
#define TimerInit()                                                       \
        {                                                                 \
                           OCR1A  = (unsigned int)(timeclk * (F_CPU  / (1024 * 1000.0f)));\
                           TCCR1A = 0;                                    \
                           TCCR1B = (1 << WGM12)|(1 << CS12)|(1 << CS10); \
        }

//��ʱ��1��ʱ��־�Ĵ���
#ifdef TIFR
#define TIFRREG            TIFR
#else
#define TIFRREG            TIFR1
#endif

//Xmoden�����ַ�
#define XMODEM_NUL         0x00
#define XMODEM_SOH         0x01
#define XMODEM_STX         0x02
#define XMODEM_EOT         0x04
#define XMODEM_ACK         0x06
#define XMODEM_NAK         0x15
#define XMODEM_CAN         0x18
#define XMODEM_EOF         0x1A
#define XMODEM_RWC         'C'

#if RS485
#define RS485Enable()      PORTREG(RS485PORT) |= (1 << RS485TXEn)
#define RS485Disable()     PORTREG(RS485PORT) &= ~(1 << RS485TXEn)
#endif

#define DataInCom()        (UCSRAREG(COMPORTNo) & (1 << RXCBIT(COMPORTNo)))
#define ReadCom()          UDRREG(COMPORTNo)

#endif

//End of file: bootldr.h
