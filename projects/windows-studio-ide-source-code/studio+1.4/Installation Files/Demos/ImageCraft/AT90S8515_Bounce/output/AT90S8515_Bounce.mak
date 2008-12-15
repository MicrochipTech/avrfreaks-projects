CC = iccavr
CFLAGS =  -IC:\ICC\include\ -e  -l -g -Wa-W 
ASFLAGS = $(CFLAGS)  -Wa-g
LFLAGS =  -LC:\ICC\lib\ -m -g -dram_end:0x25f -bdata:0x60.0x25f -bfunc_lit:0x1a.0x800 -dhwstk_size:32 -beeprom:1.512 -fihx_coff
FILES = Bounce.o 

AT90S8515_Bounce:	$(FILES)
	$(CC) -o AT90S8515_Bounce $(LFLAGS) @AT90S8515_Bounce.lk  -lISD16AT90S8515
Bounce.o: C:/ICC/include/stdio.h C:/ICC/include/_const.h C:/ICC/include/ina90.h C:/ICC/include/macros.h C:/ICC/include/io8515.h
Bounce.o:	C:\Studio+\Demos\ImageCraft\AT90S8515_Bounce\Bounce.c
	$(CC) -c $(CFLAGS) C:\Studio+\Demos\ImageCraft\AT90S8515_Bounce\Bounce.c
