GCCFLAGS=-g -Os -Wall -mmcu=atmega168
AVRDUDEFLAGS=-p m168
LINKOBJECTS=bin/delay.o bin/draw.o bin/tetris.o

all: upload

build: ${LINKOBJECTS} code/main.c
	avr-gcc ${GCCFLAGS} -o bin/main.o code/main.c ${LINKOBJECTS}
	avr-objcopy -j .text -j .data -O ihex bin/main.o main.hex

bin/delay.o: code/delay.c
	avr-gcc ${GCCFLAGS} -o bin/delay.o -c code/delay.c	

bin/draw.o: code/draw.c
	avr-gcc ${GCCFLAGS} -o bin/draw.o -c code/draw.c	
	
bin/tetris.o: code/tetris.c
	avr-gcc ${GCCFLAGS} -o bin/tetris.o -c code/tetris.c	
	
disasm: build
	avr-objdump -S -d bin/main.o > main.ass
	
upload: build
	avrdude ${AVRDUDEFLAGS} -U flash:w:main.hex:a

font:
	avrdude ${AVRDUDEFLAGS} -U eeprom:w:"bin/font.raw":r

clean:
	rm -f bin/*.o
	rm -f main.hex
	rm -f main.ass
