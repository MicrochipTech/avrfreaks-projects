Project:      .\PWMpot
Build Number: 74
Date:         Mon Aug 03 16:24:45 2020
Compiler:     ICCAVR
Version:      8.30.00
Device:       ATMega8

NOTE: AVR Code addresses are word addresses
All other addresses (including ones in FLASH) are byte addresses
  ROM 25% full. 2048 out of 8192 bytes used (does not include absolute areas).
  RAM 17 bytes used (does not include stack usage).

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                        func_lit   006E      2 =      2. bytes (rel,con,rom,proclab)

       Addr  Global Symbol
      -----  --------------------------------
       006E  __func_lit_start
       0070  __func_lit_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                             lit   0070    175 =    373. bytes (rel,con,rom,lit)

       Addr  Global Symbol
      -----  --------------------------------
       0070  __ctype
       0070  __lit_start
       01E5  __lit_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                           idata   01E5     11 =     17. bytes (rel,con,rom,lit)

       Addr  Global Symbol
      -----  --------------------------------
       01E5  __idata_start
       01F6  __idata_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            text   01F6    678 =   1656. bytes (rel,con,rom,code)

       Addr  Global Symbol
      -----  --------------------------------
       00FB  __text_start
       00FB  __start
       011C  _exit
       011D  _port_init
       0129  _timer1_init
       0140  _uart0_init
       014B  _adc_init
       0155  _init_devices
       0160  _ADC_read
       0166  _ADC_ref
       0179  _ADC_read_ch
       0196  _putchar$device_specific$
       01A3  _main
       01B1  <LIBRARY>
       01B1  _strlen
       01BD  push_arg4
       01BF  push_arg2
       01C2  mod16u
       01C4  div16u
       01C5  xdiv16u
       01DE  pop_xgset00FC
       01E5  pop_xgsetF0FC
       01F0  push_xgsetF0FC
       01F2  push_xgset30FC
       01F4  push_xgset00FC
       01F6  push_xgset003C
       01FB  push_xgset300C
       0200  pop_xgset300C
       0205  push_xgsetF00C
       020C  pop_xgsetF00C
       0213  xicall
       021C  __utoa_helper
       025C  _utoa
       0270  _islower
       027D  _toupper
       0293  __print
       03ED  _printf
       0404  __memrev
       0428  __strrev
       0437  __text_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            data   0060     11 =     17. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       0060  __data_start
       0071  __data_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                          vector   0000      2 =      2. bytes (abs,ovr,rom,code)

Files Linked      [ module(s) ]

C:\iccv8avr\lib\crtAVR.o	[ crtavr.s ]
.\.objs\init.o	[ init.c ]
.\.objs\main.o	[ main.c ]
<library>	[ strlen.s, asave.s, div16u.s, gpop3.s, gpop5.s, gpush.s, gpushx.s, gpushz.s, icall.s, utoa.s, islower.s ]
[ ctype.s, toupper.s, _printf.s, printf.s, memrev.s, strrev.s ]

User Global Definitions

ram_end = 0x45f
hwstk_size = 0x14

Area Definitions

-bfunc_lit:0x6e.0x1000
-bdata:0x60.0x45f

Additional Flags
  -e:0x2000
