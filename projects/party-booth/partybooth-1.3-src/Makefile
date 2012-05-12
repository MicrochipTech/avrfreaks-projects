PROGRAM		= partybooth
VERSION		= 1.3
DEVICE		= attiny13
CLOCK		= 1200000UL
PROGRAMMER	= usbtiny

LDFLAGS	= -Wl,-Map=$(PROGRAM).map -Wl,--cref 

all:	$(PROGRAM).hex

zipsrc:
	zip -r -b /tmp partybooth-$(VERSION)-src.zip * -x '*.zip' -x '*.o'\
	    -x '*.hex' -x '*.elf' -x '*.lst' -x '*.map' -x out.txt \
	    -x '*/.svn/*' -x 'nbproject/private/*'
	
upload: all
	sudo avrdude -c $(PROGRAMMER) -p $(DEVICE) -U flash:w:partybooth.hex
	
clean:
	rm -f $(PROGRAM).hex $(PROGRAM).elf *.o $(PROGRAM).lst $(PROGRAM).map

%.hex: %.elf
	avr-objcopy -j .text -j .data -O ihex $< $@

$(PROGRAM).elf: main.o
	avr-gcc -g -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -o $@ $< $(LDFLAGS)
	avr-size -C --mcu=$(DEVICE) $(PROGRAM).elf

%.o: %.cpp
	avr-g++ -g -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -c $< -o $@

disassemble:	$(PROGRAM).elf
	avr-objdump -h -S $(PROGRAM).elf > $(PROGRAM).lst

.PHONY :	all upload clean disassemble zipsrc
