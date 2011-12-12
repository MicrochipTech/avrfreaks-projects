# AVR-GCC Makefile
PROJECT=main
SOURCES=main.c uart.c inicializar.c acciones.c
CC=avr-gcc
OBJCOPY=avr-objcopy
MMCU=atmega128

CFLAGS=-mmcu=$(MMCU) -DF_CPU=16000000UL -Wall

$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -j .text -j .data -O ihex $(PROJECT).elf $(PROJECT).hex

$(PROJECT).elf: $(SOURCES)
	$(CC) $(CFLAGS) -I./ -o $(PROJECT).elf $(SOURCES)

program: $(PROJECT).hex
	avrdude -p m128 -c jtag1 -P /dev/ttyUSB0 -e -U flash:w:$(PROJECT).hex
clean:
	rm -f $(PROJECT).elf
	rm -f $(PROJECT).hex
