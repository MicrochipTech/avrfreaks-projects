cd "C:\Studio+\Demos\AVR_Studio4\AT90S8515_Bounce\"
C:
del at90s8515_bounce.map
del at90s8515_bounce.lst
"C:\Program Files\Atmel\AVR Tools\AvrAssembler\avrasm32.exe" -fI "C:\Studio+\Demos\AVR_Studio4\AT90S8515_Bounce\Bounce.asm" -o "at90s8515_bounce.hex" -I "C:\Studio+\Demos\AVR_Studio4\AT90S8515_Bounce" -I "C:\Program Files\Atmel\AVR Tools\AvrAssembler\AppNotes" -l "at90s8515_bounce.lst"
