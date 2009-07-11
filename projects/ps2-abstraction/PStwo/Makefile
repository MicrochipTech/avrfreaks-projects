TARGET=atmega8
ISP=usbasp

COMPILE = avr-gcc -mmcu=$(TARGET)# -DDEBUG_LEVEL=2
ARCHIVE = avr-ar rcs
OBJCOPY = avr-objcopy -O ihex

example: example.o pstwo.o
	$(COMPILE) -o main.elf example.o pstwo.o
	$(OBJCOPY) main.elf main.hex

clean:
	rm -f *.o *.elf *.hex

example.o: example.c
	$(COMPILE) -O2 -c -o example.o example.c

pstwo.o: pstwo.c
	$(COMPILE) -O2 -c pstwo.c -o pstwo.o

flash:
	avrdude -c ${ISP} -p ${TARGET} -U flash:w:main.hex

