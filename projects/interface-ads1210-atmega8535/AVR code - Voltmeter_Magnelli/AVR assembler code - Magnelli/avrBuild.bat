cd "C:\Documents and Settings\micro\Desktop\progetto magnelli\Voltmetro_Magnelli\"
C:
del voltmetro_2.map
del voltmetro_2.lst
"C:\Programmi\Atmel\AVR Tools\AvrAssembler\avrasm32.exe" -fI "C:\Documents and Settings\micro\Desktop\progetto magnelli\Voltmetro_Magnelli\Voltmetro_2.asm" -o "voltmetro_2.hex" -d "voltmetro_2.obj" -e "voltmetro_2.eep" -I "C:\Documents and Settings\micro\Desktop\progetto magnelli\Voltmetro_Magnelli" -I "C:\Programmi\Atmel\AVR Tools\AvrAssembler\AppNotes" -w  -m "voltmetro_2.map"
