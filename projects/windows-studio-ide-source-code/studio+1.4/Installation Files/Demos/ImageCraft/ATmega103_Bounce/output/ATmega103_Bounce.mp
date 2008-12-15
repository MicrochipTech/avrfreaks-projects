NOTE: AVR Code addresses are word addresses
All other addresses (including ones in FLASH) are byte addresses

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                           idata   0060   0120 =    288. bytes (rel,con)

       Addr  Global Symbol
      -----  --------------------------------
       0060  __idata_start
       0180  __idata_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            text   0180   0342 =    834. bytes (rel,con)

       Addr  Global Symbol
      -----  --------------------------------
       00C0  __text_start
       00C0  __start
       00E0  _exit
       00E1  _UART0_Init
       00E8  _INT0_Init
       00EA  _MakeLedRed
       00EE  _MakeLedGreen
       00F0  _main
       01A3  isd_ISD
       01A6  _SetBreakHere
       0260  _INT0_ISR
       0261  __text_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            data   0060   0120 =    288. bytes (rel,con)

       Addr  Global Symbol
      -----  --------------------------------
       0060  __data_start
       0060  _memory
       0100  _message
       0180  __data_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                          vector   0000   0004 =      4. bytes (abs,ovr)

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                      isd_vector   0000   0008 =      8. bytes (abs,ovr)

Files Linked      [ module(s) ]

C:\ICC\lib\crtatmega.o  [ crtatmeg ]
Bounce.o          [ Bounce.c ]
<library>         [ ISD16_AT, INT0_ISR ]

User Global Definitions

ram_end = 0xfff
hwstk_size = 0x20

User Base Address Definitions

data = 0x60
func_lit = 0x60
eeprom:1.4096

