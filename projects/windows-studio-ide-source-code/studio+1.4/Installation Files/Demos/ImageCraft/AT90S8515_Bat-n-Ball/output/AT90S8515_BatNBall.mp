NOTE: AVR Code addresses are word addresses
All other addresses (including ones in FLASH) are byte addresses

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                           idata   001A   00A0 =    160. bytes (rel,con)

       Addr  Global Symbol
      -----  --------------------------------
       001A  __idata_start
       00BA  __idata_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            text   00BA   05D2 =   1490. bytes (rel,con)

       Addr  Global Symbol
      -----  --------------------------------
       005D  __text_start
       005D  __start
       007C  _exit
       007D  _UART0_Init
       0084  _INT0_Init
       0087  _MakeLedRed
       008B  _MakeLedGreen
       008D  _DrawScores
       00B8  _main
       0269  isd_ISD
       026C  _SetBreakHere
       0325  _INT0_ISR
       0326  mpy8s
       0332  mod8u
       0334  div8u
       0335  xdiv8u
       0346  __text_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            data   0060   00A0 =    160. bytes (rel,con)

       Addr  Global Symbol
      -----  --------------------------------
       0060  __data_start
       0060  _memory
       0100  __data_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                             bss   0100   0002 =      2. bytes (rel,con)

       Addr  Global Symbol
      -----  --------------------------------
       0100  _rightScore
       0100  __bss_start
       0101  _leftScore
       0102  __bss_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                          vector   0000   0002 =      2. bytes (abs,ovr)

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                      isd_vector   0000   0004 =      4. bytes (abs,ovr)

Files Linked      [ module(s) ]

C:\ICC\lib\crtavr.o  [ crtavr.s ]
Bat-n-Ball.o      [      Bat ]
<library>         [ ISD16_AT, INT0_ISR,  mpy8s.s,  div8u.s ]

User Global Definitions

ram_end = 0x25f
hwstk_size = 0x20

User Base Address Definitions

data = 0x60
func_lit = 0x1a
eeprom:1.512

