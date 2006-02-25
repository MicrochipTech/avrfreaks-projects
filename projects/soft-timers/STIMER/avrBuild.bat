@ECHO OFF
"E:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "H:\STIMER\labels.tmp" -fI -W+ie -o "H:\STIMER\stimer.hex" -d "H:\STIMER\stimer.obj" -e "H:\STIMER\STIMER.eep" -m "H:\STIMER\stimer.map" "H:\STIMER\MAIN.asm"
