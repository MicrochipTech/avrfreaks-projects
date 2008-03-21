rem Прошивка для Контрольной панели

rem avreal32.exe +ATmega8 -p1 -as -fCKSEL=4,CKOPT=0,BODEN=0,BODLEVEL=1 -o1MHz -w -v
avreal32.exe +ATmega8 -p1 -as -o12MHz -e -w -v -c main.hex

