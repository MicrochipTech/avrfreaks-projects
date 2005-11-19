@ECHO OFF
"C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "c:\avrlcd\lcdjup~1\labels.tmp" -fI  -o "c:\avrlcd\lcdjup~1\lcdjupiter.hex" -d "c:\avrlcd\lcdjup~1\lcdjupiter.obj" -e "c:\avrlcd\lcdjup~1\lcdjupiter.eep" -m "c:\avrlcd\lcdjup~1\lcdjupiter.map" -W+ie   "C:\AVRLCD\lcdjupiter\AVRLCD.asm"
