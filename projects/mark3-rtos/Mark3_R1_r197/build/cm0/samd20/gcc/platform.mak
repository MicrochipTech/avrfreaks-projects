# Platform-specific options

CC=arm-none-eabi-gcc
CPP=arm-none-eabi-gcc

CFLAGS=-Os -g3 -Wall -pedantic -c -mthumb -mcpu=cortex-m0plus -DARM -D__SAMD20J18__ -fmessage-length=0 -fdata-sections -ffunction-sections -DK_ADDR=uint32_t -DK_WORD=uint32_t -fno-rtti -fno-exceptions -mlong-calls

CPPFLAGS=-Os -g3 -Wall -pedantic -c -mthumb -mcpu=cortex-m0plus -DARM -D__SAMD20J18__ -fmessage-length=0 -fdata-sections -ffunction-sections -DK_ADDR=uint32_t -DK_WORD=uint32_t -fno-rtti -fno-exceptions -mlong-calls

LINK=arm-none-eabi-gcc
LFLAGS= -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mthumb -mcpu=cortex-m0plus

AR=arm-none-eabi-ar
ARFLAGS=rcs

OBJCOPY=arm-none-eabi-objcopy
OBJCOPY_FLAGS=-O ihex -R .eeprom -R .fuse -R .lock -R .signature

CLANG=true
CLANGFLAGS=-D __ARM_SAMD20J18__ --analyze -fdiagnostics-show-category=name -Weverything  

CFLAGS+=-I/usr/lib/avr/include/
CPPFLAGS+=-I/usr/lib/avr/include/
CLANGFLAGS+=-I/usr/lib/avr/include/
