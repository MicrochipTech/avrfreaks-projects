CC = iccavr
CFLAGS =  -IC:\ICC\include\ -e  -l -g -Wa-W 
ASFLAGS = $(CFLAGS)  -Wa-g
LFLAGS =  -LC:\ICC\lib\ -m -g -dram_end:0x25f -bdata:0x60.0x25f -bfunc_lit:0x1a.0x800 -dhwstk_size:32 -beeprom:1.512 -fihx_coff
FILES = Bat-n-Ball.o 

AT90S8515_BatNBall:	$(FILES)
	$(CC) -o AT90S8515_BatNBall $(LFLAGS) @AT90S8515_BatNBall.lk  -lISD16AT90S8515
Bat-n-Ball.o: C:/ICC/include/stdio.h C:/ICC/include/_const.h C:/ICC/include/ina90.h C:/ICC/include/macros.h C:/ICC/include/io8515.h
Bat-n-Ball.o:	D:\Projects\Freelance\Flash_Designs\Studio+\1.4\Installation Files\Demos\ImageCraft\AT90S8515_Bat-n-Ball\Bat-n-Ball.c
	$(CC) -c $(CFLAGS) D:\Projects\Freelance\Flash_Designs\Studio+\1.4\Installation Files\Demos\ImageCraft\AT90S8515_Bat-n-Ball\Bat-n-Ball.c
