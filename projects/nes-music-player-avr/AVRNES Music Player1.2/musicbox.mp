NOTE: AVR Code addresses are word addresses
All other addresses (including ones in FLASH) are byte addresses

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                             lit   0054   1166 =   4454. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       0054  _music_dataRect0
       0054  __lit_start
       09AC  _music_dataRect1
       0EFC  _music_dataTriangle
       11AE  _music_dataNoise
       11BA  __lit_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                           idata   11BA   0010 =     16. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       11BA  __idata_start
       11CA  __idata_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            text   11CA   06F6 =   1782. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       08E5  __text_start
       08E5  __start
       0905  _exit
       0906  _init_devices
       0938  _main
       093B  _delay_1ms
       0945  _delay_ms
       0954  _T0_counter
       0B2D  _T1_counter
       0C00  empy16s
       0C10  pop_gset2
       0C13  push_gset5
       0C15  push_gset4
       0C17  push_gset3
       0C19  push_gset2
       0C1B  push_gset1
       0C1E  pop_gset1
       0C1F  pop
       0C32  push_lset
       0C49  pop_lset
       0C60  __text_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            data   0060   0010 =     16. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       0060  _i0
       0060  __data_start
       0062  _swap0
       0063  _i1
       0065  _swap1
       0066  _i2
       0068  _volume2
       0069  _swap2
       006A  _i3
       006C  _volume3
       006D  _swap3
       006E  _T
       0070  __data_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                             bss   0070   0014 =     20. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       0070  __bss_start
       0070  _sum_volume
       0072  _fcounter3
       0074  _Tone3
       0076  _fcounter2
       0078  _Tone2
       007A  _volume1
       007B  _fcounter1
       007D  _Tone1
       007F  _volume0
       0080  _fcounter0
       0082  _Tone0
       0084  __bss_end

Area                               Start  End    Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                          vector   0000   0050 =     80. bytes (abs,ovr,rom)

Files Linked      [ module(s) ]

C:\icc\lib\crtatmega.o	[ crtatmega.s ]
musicbox.o	[ musicbox.C ]
<library>	[ emul16s.s, gpop2.s, gpush.s, lpush.s ]

User Global Definitions

ram_end = 0x45f
hwstk_size = 0x10

User Base Address Definitions

func_lit = 0x54
data = 0x60
eeprom:1.512

