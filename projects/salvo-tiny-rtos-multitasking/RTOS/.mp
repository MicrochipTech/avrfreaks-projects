Project:      C:\Documents and Settings\JimC\Desktop\RTOS\RTOS
Build Number: 0
Date:         Sun Feb 17 17:00:44 2013
Compiler:     ICCAVR
Version:      8.10.00

NOTE: AVR Code addresses are word addresses
All other addresses (including ones in FLASH) are byte addresses
  ROM 11% full. 1700 out of 16384 bytes used (does not include absolute areas).
  RAM 78 bytes used (does not include stack usage).

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                        func_lit   0084   0010 =     16. bytes (rel,con,rom,proclab)

       Addr  Global Symbol
      -----  --------------------------------
       0084  __func_lit_start
       0094  __func_lit_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            text   0094   0694 =   1684. bytes (rel,con,rom,code)

       Addr  Global Symbol
      -----  --------------------------------
       004A  __start
       004A  __text_start
       006C  _exit
       006D  _Task1
       0078  _Task2
       0083  _Task3
       008E  _Task4
       0099  _Task5
       00A4  _Task6
       00AF  _Task7
       00BA  _Task8
       00C5  _main
       0110  _OSIdlingHook
       0112  _port_init
       011B  _timer2_init
       0125  _timer2_ovf_isr
       0156  _panel_init
       015F  _OSDispatch
       0171  _OSCtxSw
       0180  _OSCtxSwRtn
       0183  _OSReturn
       0187  _OSDethunkTFP
       018F  _OSClrWDTHook
       0191  _OSDisableHook
       0196  _OSEnableHook
       019A  _OSTimer
       01A6  _OSSched
       0289  _OSInsPrioQ
       0304  _OSCreateTask
       0339  _OSInit
       0352  _OSDelay
       0384  pop_xgset003C
       0389  push_xgsetF0FC
       038B  push_xgset30FC
       038D  push_xgset00FC
       038F  push_xgset003C
       0394  __text_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                             bss   0060   004E =     78. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       0060  __bss_start
       0060  _OSframeP
       0062  _OSlostTicks
       0063  _OSdelayQP
       0065  _OSsigQoutP
       0067  _OSsigQinP
       0069  _OSecbArea
       0071  _OScTcbP
       0073  _OSeligQP
       0075  _OStcbArea
       00AE  __bss_end

Area                               Start  End    Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                          vector   0000   0014 =     20. bytes (abs,ovr,rom,code)

Files Linked      [ module(s) ]

C:\iccv8avr\lib\crtatmega.o	[ crtatmega.s ]
.objs\rtos.o	[ rtos.s ]
.objs\board.o	[ board.s ]
.objs\salvomem.o	[ salvomem.s ]
<library>	[ salvoporticcavr.s, salvohook_wdt.s, salvohook_interrupt.s, salvotimer.s, salvosched.s, salvoqins.s ]
[ salvoinit2.s, salvoinit.s, salvodelay.s, gpop2.s, gpush.s ]

User Global Definitions

ram_end = 0x45f
hwstk_size = 0x14

Area Definitions

-bfunc_lit:0x84.0x4000
-bdata:0x60.0x45f

