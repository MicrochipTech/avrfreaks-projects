Project:      ..\RPM\RPM
Build Number: 98
Date:         Sun Oct 10 13:43:36 2010
Compiler:     ICCAVR
Version:      8.01.05

NOTE: AVR Code addresses are word addresses
All other addresses (including ones in FLASH) are byte addresses

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                           idata   002A   0006 =      6. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       002A  __idata_start
       0030  __idata_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            text   0030   015E =    350. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       0018  __text_start
       0018  __start
       0039  _exit
       003A  _port_init
       0045  _timer2_init
       004C  _timer2_comp_isr
       005E  _int0_isr
       009F  _init_devices
       00AA  _main
       00AB  mod16u
       00AD  div16u
       00AE  xdiv16u
       00C7  __text_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            data   0060   0006 =      6. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       0060  _count_1
       0060  __data_start
       0062  _RPM
       0064  _j
       0065  _iter
       0066  __data_end

Area                               Start  End    Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                          vector   0000   0008 =      8. bytes (abs,ovr,rom)

Files Linked      [ module(s) ]

C:\iccv8avr\lib\crtAVR.o	[ crtavr.s ]
.objs\main.o	[ main.c ]
<library>	[ div16u.s ]

User Global Definitions

ram_end = 0x25f
hwstk_size = 0x14

Area Definitions

-bfunc_lit:0x2a.0x2000
-bdata:0x60.0x25f

