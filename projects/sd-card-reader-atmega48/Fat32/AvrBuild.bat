@ECHO OFF
"H:\Installed\AvrAssembler2\avrasm2.exe" -S "H:\Installed\Projects\Fat32\labels.tmp" -fI -W+ie -o "H:\Installed\Projects\Fat32\Fat32.hex" -d "H:\Installed\Projects\Fat32\Fat32.obj" -e "H:\Installed\Projects\Fat32\Fat32.eep" -m "H:\Installed\Projects\Fat32\Fat32.map" "H:\Installed\Projects\Fat32\Fat32.asm"
