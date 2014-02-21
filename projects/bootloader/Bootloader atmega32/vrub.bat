@rem auto create by AVRUBD at 21. 2. 2014 9:52:27
avr-gcc.exe  -mmcu=atmega32 -Wall -gdwarf-2  -Os -fsigned-char -MD -MP  -c  bootldr.c
avr-gcc.exe -mmcu=atmega32  -Wl,-section-start=.text=0x7000 bootldr.o     -o Bootldr.elf
avr-objcopy -O ihex -R .eeprom  Bootldr.elf Bootldr.hex
@pause
