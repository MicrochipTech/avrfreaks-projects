CC = iccavr
CFLAGS =  -IC:\ICC\include\ -e -DATMEGA  -l -g -Mavr_mega 
ASFLAGS = $(CFLAGS)  -Wa-g
LFLAGS =  -LC:\ICC\lib\ -m -g -ucrtatmega.o -dram_end:0xfff -bdata:0x60.0xfff -bfunc_lit:0x60.0x800 -dhwstk_size:32 -beeprom:1.4096 -fihx_coff
FILES = Bat-n-Ball.o 

ATmega103_BatNBall:	$(FILES)
	$(CC) -o ATmega103_BatNBall $(LFLAGS) @ATmega103_BatNBall.lk  -lcatmega -lISD16ATMEGA103
Bat-n-Ball.o: C:/ICC/include/stdio.h C:/ICC/include/_const.h C:/ICC/include/ina90.h C:/ICC/include/macros.h C:/ICC/include/iom103.h
Bat-n-Ball.o:	C:\Studio+\Demos\ImageCraft\ATmega103_Bat-n-Ball\Bat-n-Ball.c
	$(CC) -c $(CFLAGS) C:\Studio+\Demos\ImageCraft\ATmega103_Bat-n-Ball\Bat-n-Ball.c
