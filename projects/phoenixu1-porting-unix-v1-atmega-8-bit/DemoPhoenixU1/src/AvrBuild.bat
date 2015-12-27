@ECHO OFF
"C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "E:\ProjAVR\macro\labels.tmp" -fI -W+iw -C V3 -o "E:\ProjAVR\macro\macro.hex" -d "E:\ProjAVR\macro\macro.obj" -e "E:\ProjAVR\macro\macro.eep" -m "E:\ProjAVR\macro\macro.map" "E:\ProjAVR\macro\macro.asm"
