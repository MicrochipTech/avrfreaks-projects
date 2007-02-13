@ECHO OFF
"C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "C:\ESC Programs\ESC B1_1\labels.tmp" -fI -W+ie -o "C:\ESC Programs\ESC B1_1\ESC_B1_1.hex" -d "C:\ESC Programs\ESC B1_1\ESC_B1_1.obj" -e "C:\ESC Programs\ESC B1_1\ESC_B1_1.eep" -m "C:\ESC Programs\ESC B1_1\ESC_B1_1.map" "C:\ESC Programs\ESC B1_1\ESC_B1_1.asm"
