@ECHO OFF
"C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "D:\projects\Assembler\avr\hd61830\labels.tmp" -fI -W+ie -o "D:\projects\Assembler\avr\hd61830\demo.hex" -d "D:\projects\Assembler\avr\hd61830\demo.obj" -e "D:\projects\Assembler\avr\hd61830\demo.eep" -m "D:\projects\Assembler\avr\hd61830\demo.map" "D:\projects\Assembler\avr\hd61830\demo.asm"
avrsp  demo.hex
