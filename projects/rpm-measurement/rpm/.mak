CFLAGS =  -e -D__ICC_VERSION=8.01.05 -D__BUILD=0 -DATMega8535  -l -A -g -MHasMul -MEnhanced -Wa-W 
ASFLAGS = $(CFLAGS) 
LFLAGS =  -g -nb:0 -e:0x2000 -Wl-W -bfunc_lit:0x2a.0x2000 -dram_end:0x25f -bdata:0x60.0x25f -dhwstk_size:20 -beeprom:0.512 -fihx_coff -S2
FILES = main.o 

default:	$(FILES)
	$(CC) -o default $(LFLAGS) @..\RPM\RPM.lk  -lcavr
