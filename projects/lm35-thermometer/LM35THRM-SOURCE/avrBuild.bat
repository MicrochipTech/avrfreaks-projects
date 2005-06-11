cd "F:\My Projects\AVR\LM35THRM FINAL\"
F:
del "F:\My Projects\AVR\LM35THRM FINAL\thermometer.map"
del "F:\My Projects\AVR\LM35THRM FINAL\thermometer.lst"
"C:\Program Files\Atmel\AVR Tools\AvrAssembler\avrasm32.exe" -fI  -o "F:\My Projects\AVR\LM35THRM FINAL\thermometer.hex" -d "F:\My Projects\AVR\LM35THRM FINAL\thermometer.obj" -e "F:\My Projects\AVR\LM35THRM FINAL\thermometer.eep" -I "F:\My Projects\AVR\LM35THRM FINAL" -I "C:\Program Files\Atmel\AVR Tools\AvrAssembler\Appnotes" -w  -m "F:\My Projects\AVR\LM35THRM FINAL\thermometer.map" -l "F:\My Projects\AVR\LM35THRM FINAL\thermometer.lst" "F:\My Projects\AVR\LM35THRM FINAL\MAIN.asm"
