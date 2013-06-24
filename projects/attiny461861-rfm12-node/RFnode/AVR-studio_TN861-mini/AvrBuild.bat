@ECHO OFF
"C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "X:\AVRproj\RFM12B\TN861-mini\labels.tmp" -fI -W+ie -C V2 -o "X:\AVRproj\RFM12B\TN861-mini\tinyRF.hex" -d "X:\AVRproj\RFM12B\TN861-mini\tinyRF.obj" -e "X:\AVRproj\RFM12B\TN861-mini\tinyRF.eep" -m "X:\AVRproj\RFM12B\TN861-mini\tinyRF.map" "X:\AVRproj\RFM12B\TN861-mini\main.asm"
