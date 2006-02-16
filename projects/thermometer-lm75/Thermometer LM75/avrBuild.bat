cd "C:\Documents and Settings\Sergej\Desktop\Thermometer with I2C Bus TH03_V1 (Completed)\Software\"
C:
del th03_v1.map
del th03_v1.lst
"C:\Program Files\Atmel\AVR Tools\AvrAssembler\avrasm32.exe" -fI "C:\Documents and Settings\Sergej\Desktop\Thermometer with I2C Bus TH03_V1 (Completed)\Software\TH03_V1.asm" -o "th03_v1.hex" -d "th03_v1.obj" -e "th03_v1.eep" -I "C:\Documents and Settings\Sergej\Desktop\Thermometer with I2C Bus TH03_V1 (Completed)\Software" -I "C:\Program Files\Atmel\AVR Tools\AvrAssembler\AppNotes" -w 
