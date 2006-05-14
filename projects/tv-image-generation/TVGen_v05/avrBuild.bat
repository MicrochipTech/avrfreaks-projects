cd "D:\Projects\Mine\AVR\TVGen\"
D:
del "D:\Projects\Mine\AVR\TVGen\tvgen.map"
del "D:\Projects\Mine\AVR\TVGen\tvgen.lst"
"C:\Program Files\Atmel\AVR Tools\AvrAssembler\avrasm32.exe" -fI "D:\Projects\Mine\AVR\TVGen\TVGen.asm" -o "D:\Projects\Mine\AVR\TVGen\tvgen.hex" -d "D:\Projects\Mine\AVR\TVGen\tvgen.obj" -e "D:\Projects\Mine\AVR\TVGen\tvgen.eep" -I "D:\Projects\Mine\AVR\TVGen" -I "C:\Program Files\Atmel\AVR Tools\AvrAssembler\AppNotes" -w  -m "D:\Projects\Mine\AVR\TVGen\tvgen.map" -l "D:\Projects\Mine\AVR\TVGen\tvgen.lst"
