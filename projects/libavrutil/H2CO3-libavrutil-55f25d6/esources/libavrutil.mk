CC = avr-gcc
LD = $(CC)
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
RM = rm

CFLAGS = -c -std=gnu99 -Os -Wall -ffunction-sections -fdata-sections -mmcu=$(CPU_TYPE) -DF_CPU=$(CPU_FREQ) -I$(LIBAVRUTIL)/bld/include
LDFLAGS = -Os -mmcu=$(CPU_TYPE) -ffunction-sections -fdata-sections -Wl,--gc-sections
OBJCOPYFLAGS = -O ihex -R .eeprom
AVRDUDEFLAGS = -C $(LIBAVRUTIL)/resources/avrdude.conf -p $(CPU_TYPE) -c $(PROGRAMMER) -b 57600 -P $(PORT) -U flash:w:$(TARGET_HEX):i
RMFLAGS = -rf

OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))
LIBS = $(LIBAVRUTIL)/bld/libavrutil.a /usr/lib/avr/lib/libm.a
TARGET_HEX = $(PROJECT).hex
TARGET_ELF = $(PROJECT).elf

$(TARGET_HEX): $(TARGET_ELF)
	$(OBJCOPY) $(OBJCOPYFLAGS) $^ $@

$(TARGET_ELF): $(OBJECTS) $(LIBS)
	$(LD) $(LDFLAGS) -o $@ $^

all: $(TARGET_HEX)

install: $(TARGET_HEX)
	$(AVRDUDE) $(AVRDUDEFLAGS)

clean:
	$(RM) $(RMFLAGS) $(TARGET_ELF) $(TARGET_HEX) $(OBJECTS) *~

%.o: %.c
	$(CC) $(CFLAGS) $^

