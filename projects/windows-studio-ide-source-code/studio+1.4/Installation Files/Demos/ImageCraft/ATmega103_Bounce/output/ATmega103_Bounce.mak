CC = iccavr
CFLAGS =  -IC:\ICC\include\ -e -DATMEGA  -l -g -Mavr_mega 
ASFLAGS = $(CFLAGS)  -Wa-g
LFLAGS =  -LC:\ICC\lib\ -m -g -ucrtatmega.o -dram_end:0xfff -bdata:0x60.0xfff -bfunc_lit:0x60.0x800 -dhwstk_size:32 -beeprom:1.4096 -fihx_coff
FILES = Bounce.o 

ATmega103_Bounce:	$(FILES)
	$(CC) -o ATmega103_Bounce $(LFLAGS) @ATmega103_Bounce.lk  -lcatmega -lISD16ATMEGA103
Bounce.o: C:/ICC/include/stdio.h C:/ICC/include/_const.h C:/ICC/include/ina90.h C:/ICC/include/macros.h C:/ICC/include/iom103.h
Bounce.o:	C:\Studio+\Demos\ImageCraft\ATmega103_Bounce\Bounce.c
	$(CC) -c $(CFLAGS) C:\Studio+\Demos\ImageCraft\ATmega103_Bounce\Bounce.c
