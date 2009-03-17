CC = iccavr
CFLAGS =  -IC:\icc\include\ -e -DATMEGA -DATMega16  -l -g -Mavr_enhanced 
ASFLAGS = $(CFLAGS)  -Wa-g
LFLAGS =  -LC:\icc\lib\ -g -ucrtatmega.o -bfunc_lit:0x54.0x4000 -dram_end:0x45f -bdata:0x60.0x45f -dhwstk_size:16 -beeprom:1.512 -fihx_coff -S1
FILES = musicbox.o 

musicbox:	$(FILES)
	$(CC) -o musicbox $(LFLAGS) @musicbox.lk   -lstudio -lcatmega
musicbox.o: C:/icc/include/iom16v.h C:/icc/include/macros.h
musicbox.o:	F:\机器人3\led点阵传感器\AVRNES~1.0\musicbox.C
	$(CC) -c $(CFLAGS) F:\机器人3\led点阵传感器\AVRNES~1.0\musicbox.C
