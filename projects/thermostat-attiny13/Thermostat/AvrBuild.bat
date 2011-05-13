@ECHO OFF
"C:\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "H:\Projekty\Thermostat\labels.tmp" -fI -W+ie -o "H:\Projekty\Thermostat\Thermostat.hex" -d "H:\Projekty\Thermostat\Thermostat.obj" -e "H:\Projekty\Thermostat\Thermostat.eep" -m "H:\Projekty\Thermostat\Thermostat.map" "H:\Projekty\Thermostat\thermostatv4.asm"
