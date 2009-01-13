make clean
make all
avr-objdump -h -S main.elf > main.lss
