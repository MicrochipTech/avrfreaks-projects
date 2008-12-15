NOTE: AVR Code addresses are word addresses
All other addresses (including ones in FLASH) are byte addresses

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                           idata   001A   0120 =    288. bytes (rel,con)

       Addr  Global Symbol
      -----  --------------------------------
       001A  __idata_start
       013A  __idata_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            text   013A   033E =    830. bytes (rel,con)

       Addr  Global Symbol
      -----  --------------------------------
       009D  __text_start
       009D  __start
       00BC  _exit
       00BD  _UART0_Init
       00C4  _INT0_Init
       00C7  _MakeLedRed
       00CB  _MakeLedGreen
       00CD  _main
       017F  isd_ISD
       0182  _SetBreakHere
       023B  _INT0_ISR
       023C  __text_end

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
                          vector   0000   0002 =      2. bytes (abs,ovr)

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                      isd_vector   0000   0004 =      4. bytes (abs,ovr)

Files Linked      [ module(s) ]

C:\ICC\lib\crtavr.o  [ crtavr.s ]
Bounce.o          [ Bounce.c ]
<library>         [ ISD16_AT, INT0_ISR ]

User Global Definitions

ram_end = 0x25f
hwstk_size = 0x20

User Base Address Definitions

data = 0x60
func_lit = 0x1a
eeprom:1.512

