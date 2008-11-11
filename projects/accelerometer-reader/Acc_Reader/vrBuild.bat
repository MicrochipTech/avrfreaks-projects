@ECHO OFF
"H:\Installed\AvrAssembler2\avrasm2.exe" -S "H:\Installed\Projects\Acc_Reader\labels.tmp" -fI -W+ie -o "H:\Installed\Projects\Acc_Reader\Acc_Reader.hex" -d "H:\Installed\Projects\Acc_Reader\Acc_Reader.obj" -e "H:\Installed\Projects\Acc_Reader\Acc_Reader.eep" -m "H:\Installed\Projects\Acc_Reader\Acc_Reader.map" "H:\Installed\Projects\Acc_Reader\Acc_Reader.asm"
