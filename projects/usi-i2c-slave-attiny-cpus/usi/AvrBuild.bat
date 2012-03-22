@ECHO OFF
"C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "L:\8515\SuperAnalizer\PowerMCU\labels.tmp" -fI -W+ie -C V2 -o "L:\8515\SuperAnalizer\PowerMCU\PowerMCU.hex" -d "L:\8515\SuperAnalizer\PowerMCU\PowerMCU.obj" -e "L:\8515\SuperAnalizer\PowerMCU\PowerMCU.eep" -m "L:\8515\SuperAnalizer\PowerMCU\PowerMCU.map" "L:\8515\SuperAnalizer\PowerMCU\PowerMCU.asm"
