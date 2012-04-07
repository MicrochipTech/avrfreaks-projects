all: init sbi.exe progs
init: sbi.c sbi.h
sbi.c: ../../sbi/sbi.c
	cp ../../sbi/sbi.c sbi.c
sbi.h: ../../sbi/sbi.h
	cp ../../sbi/sbi.h sbi.h
sbi.exe: sbi.o main.o
	gcc -o sbi.exe sbi.o main.o
sbi.o: sbi.c sbi.h funclib.h
	gcc -c -o sbi.o sbi.c
main.o: main.c sbi.h funclib.h
	gcc -c -o main.o main.c
progs: programs/counter.sbi programs/pow.sbi programs/libtest.sbi programs/jumps.sbi
sasmc.exe:
	cp ../../sasmc/sasmc.exe sasmc.exe
programs/counter.sbi: sasmc.exe programs/counter.sasm
	./sasmc.exe programs/counter.sasm programs/counter.sbi
programs/pow.sbi: sasmc.exe programs/pow.sasm
	./sasmc.exe programs/pow.sasm programs/pow.sbi
programs/libtest.sbi: sasmc.exe programs/libtest.sasm
	./sasmc.exe programs/libtest.sasm programs/libtest.sbi
programs/jumps.sbi: sasmc.exe programs/jumps.sasm
	./sasmc.exe programs/jumps.sasm programs/jumps.sbi
clean:
	rm -f sbi.c sbi.h sbi.o main.o sbi.exe sasmc.exe programs/counter.sbi programs/pow.sbi programs/libtest.sbi programs/jumps.sbi