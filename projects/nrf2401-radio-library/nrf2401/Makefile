TARGETS    = nrf2401_serial_test.srec
MCU_CLK    = 16000000
MCU_TARGET = atmega168

DEFS       = -DMCU_CLK=$(MCU_CLK)
OPTIMIZE   = -O2 -g
WARN       = -Wall -Werror
INCLUDE    = -I.
CFLAGS     = -mmcu=$(MCU_TARGET) $(OPTIMIZE) $(DEFS) $(WARN) $(INCLUDE)

CC         = avr-gcc
OBJCOPY    = avr-objcopy
ARCH       = avr


all: $(TARGETS)

nrf2401_serial_test.elf: nrf2401_serial_test.o nrf2401.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

clean:
	rm -rf *.o *.elf *.a *.lst *.map *.srec
