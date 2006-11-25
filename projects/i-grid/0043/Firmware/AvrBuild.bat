@ECHO OFF
"C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "E:\0043\Firmware\labels.tmp" -fI -W+ie -o "E:\0043\Firmware\iGrid.hex" -d "E:\0043\Firmware\iGrid.obj" -e "E:\0043\Firmware\iGrid.eep" -m "E:\0043\Firmware\iGrid.map" "E:\0043\Firmware\iGrid.asm"
