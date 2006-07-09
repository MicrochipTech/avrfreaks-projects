@ECHO OFF
"C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "S:\Projects\AVR_asm\AVR_DDS\labels.tmp" -fI -W+ie -o "S:\Projects\AVR_asm\AVR_DDS\AVR_DDS.hex" -d "S:\Projects\AVR_asm\AVR_DDS\AVR_DDS.obj" -e "S:\Projects\AVR_asm\AVR_DDS\AVR_DDS.eep" -m "S:\Projects\AVR_asm\AVR_DDS\AVR_DDS.map" -l "S:\Projects\AVR_asm\AVR_DDS\AVR_DDS.lst" "S:\Projects\AVR_asm\AVR_DDS\AVR_DDS.asm"
