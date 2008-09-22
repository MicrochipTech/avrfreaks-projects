PRG            = main
OBJ            = \
	main.o serial.o rms64.o \
	twi-eeprom.o pcf-rtc.o mmc.o logger.o mulacc.o io_stuff.o \
	stack-check.o oscope.o stripchart.o calibrate.o calib2.o \
	ks0108lib/KS0108-AVR.o ks0108lib/KS0108.o

MCU_TARGET     = atmega168
DEFS           = -DF_CPU=16000000
LIBS           =

# You should not have to change anything below here.

CC             = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS        = -std=gnu99 -Wall -Os -ffunction-sections -mcall-prologues -mmcu=$(MCU_TARGET) $(DEFS) -Iks0108lib
override LDFLAGS       = -Wl,-Map,$(PRG).map -Wl,-gc-sections 

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump
AVRDUDE        = avrdude -q -q -c usbtiny -p m168

all: $(PRG).elf lst text 

fuse:
	$(AVRDUDE) -U hfuse:w:0xd4:m -U lfuse:w:0xff:m

eeread:
	$(AVRDUDE) -U eeprom:r:eeprom.hex:i

eewrite:
	$(AVRDUDE) -U eeprom:w:eeprom.hex:i

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o ks0108lib/*.o $(PRG).elf
	rm -rf $(PRG).lst $(PRG).map $(PRG).hex $(PRG).bin $(PRG).srec

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@

upload: all
	$(AVRDUDE) -U flash:w:main.hex 
