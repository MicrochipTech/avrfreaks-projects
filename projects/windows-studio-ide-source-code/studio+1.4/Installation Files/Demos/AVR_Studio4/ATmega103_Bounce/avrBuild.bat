cd "C:\Studio+\Demos\AVR_Studio4\ATmega103_Bounce\"
C:
del atmega103_bounce.map
del atmega103_bounce.lst
"C:\Program Files\Atmel\AVR Tools\AvrAssembler\avrasm32.exe" -fI "C:\Studio+\Demos\AVR_Studio4\ATmega103_Bounce\Bounce.asm" -o "atmega103_bounce.hex" -I "C:\Studio+\Demos\AVR_Studio4\ATmega103_Bounce" -I "C:\Program Files\Atmel\AVR Tools\AvrAssembler\AppNotes" -l "atmega103_bounce.lst"
