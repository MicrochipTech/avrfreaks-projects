disk_reader: disk_reader.c
	gcc -std=gnu99 -g -Wall -Os -o disk_reader disk_reader.c -lm -DF_CPU=16000000 -I../firmware

all: disk_reader

clean: 
	rm -f disk_reader
