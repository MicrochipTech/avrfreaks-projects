cd "C:\Program Files\Atmel\AVR Tools\AvrStudio4\Tiny26HexDispDriver\"
C:
del "C:\Program Files\Atmel\AVR Tools\AvrStudio4\Tiny26HexDispDriver\tiny26hexdispdriver.map"
del "C:\Program Files\Atmel\AVR Tools\AvrStudio4\Tiny26HexDispDriver\tiny26hexdispdriver.lst"
"C:\Program Files\Atmel\AVR Tools\AvrAssembler\avrasm32.exe" -fI "C:\Program Files\Atmel\AVR Tools\AvrStudio4\Tiny26HexDispDriver\Tiny26HexDispDriver.asm" -o "C:\Program Files\Atmel\AVR Tools\AvrStudio4\Tiny26HexDispDriver\tiny26hexdispdriver.hex" -d "C:\Program Files\Atmel\AVR Tools\AvrStudio4\Tiny26HexDispDriver\tiny26hexdispdriver.obj" -e "C:\Program Files\Atmel\AVR Tools\AvrStudio4\Tiny26HexDispDriver\tiny26hexdispdriver.eep" -I "C:\Program Files\Atmel\AVR Tools\AvrStudio4\Tiny26HexDispDriver" -I "C:\Program Files\Atmel\AVR Tools\AvrAssembler\AppNotes" -w  -m "C:\Program Files\Atmel\AVR Tools\AvrStudio4\Tiny26HexDispDriver\tiny26hexdispdriver.map"
