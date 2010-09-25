compiler:= avr-gcc
cflags:= -O2 -gstabs -Wall -Wstrict-prototypes -std=gnu99
includes:= -I   /usr/lib/avr/include
libdir:= -L    /usr/lib/avr/lib
link:=  -lm -Wl,-u,vfprintf -lprintf_min
target:= flasher
mcu:=atmega32

MCU=$(mcu)
AVRDUDE=avrdude
AVRDUDE_PROGRAMMER = avrisp2
AVRDUDE_PORT = usb

AVRDUDE_WRITE_FLASH =  -U flash:w:$(target).hex
AVRDUDE_BASIC = -p $(mcu) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) -i 100 -B 100
AVRDUDE_FLAGS = $(AVRDUDE_BASIC) $(AVRDUDE_NO_VERIFY) $(AVRDUDE_VERBOSE) $(AVRDUDE_ERASE_COUNTER)


source_files:=$(wildcard *.c)
object_files:=$(patsubst %.c,%.o,$(source_files))

hex: $(target)
	avr-objcopy -R .eeprom -O ihex $(target).elf $(target).hex

$(target): $(object_files)
	$(compiler) -mmcu=$(mcu) $(cflags) $^ $(libdir) $(link) -o $(target).elf

%.o: %.c
	$(compiler) -c -mmcu=$(mcu) $(cflags) $(includes)  -c $<
	
clean: 
	rm -f $(target).elf $(target).hex
	rm -f $(object_files)

# Program the device.  
program: $(target).hex 
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)


