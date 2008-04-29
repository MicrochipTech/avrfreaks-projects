REM Initial programming of the uC using the programmer cable mentioned in chapter 8 of the thesis
REM assuming the parallel port LPT1 is used

REM set the fuse-bits of the uC
avrdude -p atmega128 -c xil -P lpt1 -U lfuse:w:0xff:m
avrdude -p atmega128 -c xil -P lpt1 -U hfuse:w:0xdc:m
avrdude -p atmega128 -c xil -P lpt1 -U efuse:w:0xff:m

REM download Bootloader
cd bootloader
make clean
make
avrdude -p atmega128 -c xil -P lpt1 -U flash:w:bootloader.hex
cd..

REM protect the bootloader from overwriting
avrdude -p atmega128 -c xil -P lpt1 -U lock:w:0xef:m


