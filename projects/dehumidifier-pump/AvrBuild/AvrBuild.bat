@ECHO OFF
"C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "C:\Documents and Settings\jmarquardt\My Documents\Jim\avr\dehumid_pump\labels.tmp" -fI -W+ie -I "C:\Documents and Settings\jmarquardt\My Documents\Jim\avr\Code_library" -o "C:\Documents and Settings\jmarquardt\My Documents\Jim\avr\dehumid_pump\dehumid_pump.hex" -d "C:\Documents and Settings\jmarquardt\My Documents\Jim\avr\dehumid_pump\dehumid_pump.obj" -e "C:\Documents and Settings\jmarquardt\My Documents\Jim\avr\dehumid_pump\dehumid_pump.eep" -m "C:\Documents and Settings\jmarquardt\My Documents\Jim\avr\dehumid_pump\dehumid_pump.map" "C:\Documents and Settings\jmarquardt\My Documents\Jim\avr\dehumid_pump\dehumid_pump.asm"