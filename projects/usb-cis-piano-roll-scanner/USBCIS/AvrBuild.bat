@ECHO OFF
"C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "C:\AVRFTDI\labels.tmp" -fI -W+ie -o "C:\AVRFTDI\avrftdi.hex" -d "C:\AVRFTDI\avrftdi.obj" -e "C:\AVRFTDI\AVRFTDI.eep" -m "C:\AVRFTDI\avrftdi.map" "C:\AVRFTDI\overclock2.asm"
