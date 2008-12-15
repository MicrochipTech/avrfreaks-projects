NOTE: AVR Code addresses are word addresses
All other addresses (including ones in FLASH) are byte addresses

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                           idata   0060   00A0 =    160. bytes (rel,con)

       Addr  Global Symbol
      -----  --------------------------------
       0060  __idata_start
       0100  __idata_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            text   0100   05E2 =   1506. bytes (rel,con)

       Addr  Global Symbol
      -----  --------------------------------
       0080  __text_start
       0080  __start
       00A0  _exit
       00A1  _UART0_Init
       00A8  _INT0_Init
       00AA  _MakeLedRed
       00AE  _MakeLedGreen
       00B0  _DrawScores
       00E1  _main
       0293  mpy8s
       029F  mod8u
       02A1  div8u
       02A2  xdiv8u
       02B3  isd_ISD
       02B6  _SetBreakHere
       0370  _INT0_ISR
       0371  __text_end

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
                          vector   0000   0004 =      4. bytes (abs,ovr)

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                      isd_vector   0000   0008 =      8. bytes (abs,ovr)

Files Linked      [ module(s) ]

C:\ICC\lib\crtatmega.o  [ crtatmeg ]
Bat-n-Ball.o      [      Bat ]
<library>         [  mpy8s.s,  div8u.s, ISD16_AT, INT0_ISR ]

User Global Definitions

ram_end = 0xfff
hwstk_size = 0x20

User Base Address Definitions

data = 0x60
func_lit = 0x60
eeprom:1.4096

