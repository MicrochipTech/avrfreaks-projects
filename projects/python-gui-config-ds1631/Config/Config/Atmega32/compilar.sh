#!bin/bash
clear
echo "Borrando archivos temporales"
rm demo*
rm DS1631.hex
sleep 1
echo "Compilando"
avr-gcc -gstabs -Os -Wl,-u,vfprintf -lprintf_flt -lm -mmcu=atmega32 -c $1 -o demo.o
avr-gcc -gstabs -Wl,-u,vfprintf -lprintf_flt -lm -mmcu=atmega32 -o demo.elf demo.o
avr-gcc -gstabs -Wl,-u,vfprintf -lprintf_flt -lm -mmcu=atmega32 -Wl,-Map,demo.map -o demo.elf demo.o
avr-objcopy -j .text -j .data -O ihex demo.elf main.hex
avr-size main.hex

#uisp  -dprog=dapa --erase -dlpt=/dev/parport0
#uisp  -dprog=dapa --upload if=main.hex -dlpt=/dev/parport0
