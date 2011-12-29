################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
LIB_AS_ARGS :=
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
HEX_FLASH_FILE_PATH :=
HEX_FLASH_FILE_PATH_AS_ARGS :=
HEX_EEPROM_FILE_PATH :=
HEX_EEPROM_FILE_PATH_AS_ARGS :=
LSS_FILE_PATH :=
LSS_FILE_PATH_AS_ARGS :=
MAP_FILE_PATH :=
MAP_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=C:/Program Files (x86)/Atmel/AVR Studio 5.0/AVR ToolChain/bin/
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../ping.c \
../timer1.c \
../usart_stdout.c \
../main.c \
../usart.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
ping.o \
timer1.o \
usart_stdout.o \
main.o \
usart.o


OBJS_AS_ARGS +=  \
ping.o \
timer1.o \
usart_stdout.o \
main.o \
usart.o


C_DEPS +=  \
ping.d \
timer1.d \
usart_stdout.d \
main.d \
usart.d


C_DEPS_AS_ARGS +=  \
ping.d \
timer1.d \
usart_stdout.d \
main.d \
usart.d


OUTPUT_FILE_PATH +=atMega644pa\ Ping.elf

OUTPUT_FILE_PATH_AS_ARGS +="atMega644pa Ping.elf"

HEX_FLASH_FILE_PATH +=atMega644pa\ Ping.hex

HEX_FLASH_FILE_PATH_AS_ARGS +="atMega644pa Ping.hex"

HEX_EEPROM_FILE_PATH +=atMega644pa\ Ping.eep

HEX_EEPROM_FILE_PATH_AS_ARGS +="atMega644pa Ping.eep"

LSS_FILE_PATH +=atMega644pa\ Ping.lss

LSS_FILE_PATH_AS_ARGS +="atMega644pa Ping.lss"

MAP_FILE_PATH =atMega644pa\ Ping.map

MAP_FILE_PATH_AS_ARGS ="atMega644pa Ping.map"

LIB_AS_ARGS +="libatMega644pa Ping.a"

ADDITIONAL_DEPENDENCIES:= $(HEX_FLASH_FILE_PATH) $(LSS_FILE_PATH) $(HEX_EEPROM_FILE_PATH) size

OUTPUT_FILE_DEP:= ./makedep.mk

# AVR/GNU C Compiler
./ping.o: .././ping.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DF_CPU=20000000  -O1 -fpack-struct -fshort-enums -g2 -Wall -c -std=gnu99  -mmcu=atmega644pa   -MD -MP -MF"$(@:ping.o=ping.d)" -MT"$(@:ping.o=ping.d)" -o"$@" "$<"
	@echo Finished building: $<

./timer1.o: .././timer1.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DF_CPU=20000000  -O1 -fpack-struct -fshort-enums -g2 -Wall -c -std=gnu99  -mmcu=atmega644pa   -MD -MP -MF"$(@:timer1.o=timer1.d)" -MT"$(@:timer1.o=timer1.d)" -o"$@" "$<"
	@echo Finished building: $<

./usart_stdout.o: .././usart_stdout.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DF_CPU=20000000  -O1 -fpack-struct -fshort-enums -g2 -Wall -c -std=gnu99  -mmcu=atmega644pa   -MD -MP -MF"$(@:usart_stdout.o=usart_stdout.d)" -MT"$(@:usart_stdout.o=usart_stdout.d)" -o"$@" "$<"
	@echo Finished building: $<

./main.o: .././main.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DF_CPU=20000000  -O1 -fpack-struct -fshort-enums -g2 -Wall -c -std=gnu99  -mmcu=atmega644pa   -MD -MP -MF"$(@:main.o=main.d)" -MT"$(@:main.o=main.d)" -o"$@" "$<"
	@echo Finished building: $<

./usart.o: .././usart.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DF_CPU=20000000  -O1 -fpack-struct -fshort-enums -g2 -Wall -c -std=gnu99  -mmcu=atmega644pa   -MD -MP -MF"$(@:usart.o=usart.d)" -MT"$(@:usart.o=usart.d)" -o"$@" "$<"
	@echo Finished building: $<


./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DF_CPU=20000000  -O1 -fpack-struct -fshort-enums -g2 -Wall -c -std=gnu99  -mmcu=atmega644pa   -MD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<



# AVR/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

# AVR/GNU C/C++ Linker
$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU C/C++ Linker
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE)  -mmcu=atmega644pa  -Wl,-Map=$(MAP_FILE_PATH_AS_ARGS) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS)
	@echo Finished building target: $@



$(HEX_FLASH_FILE_PATH): $(OUTPUT_FILE_PATH)
	$(QUOTE)$(AVR_APP_PATH)avr-objcopy.exe$(QUOTE) -O ihex -R .eeprom -R .fuse -R .lock -R .signature  $(QUOTE)$<$(QUOTE) $(QUOTE)$@$(QUOTE)

$(HEX_EEPROM_FILE_PATH): $(OUTPUT_FILE_PATH)
	-$(QUOTE)$(AVR_APP_PATH)avr-objcopy.exe$(QUOTE) -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex $(QUOTE)$<$(QUOTE) $(QUOTE)$@$(QUOTE) || exit 0

$(LSS_FILE_PATH): $(OUTPUT_FILE_PATH)
	$(QUOTE)$(AVR_APP_PATH)avr-objdump.exe$(QUOTE) -h -S $(QUOTE)$<$(QUOTE) > $(QUOTE)$@$(QUOTE)

size: $(OUTPUT_FILE_PATH)
	@$(QUOTE)$(AVR_APP_PATH)avr-size.exe$(QUOTE) -C --mcu=atmega644pa $(OUTPUT_FILE_PATH_AS_ARGS)

# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS)$(C_DEPS_AS_ARGS) $(EXECUTABLES) $(LIB_AS_ARGS) $(HEX_FLASH_FILE_PATH_AS_ARGS) $(HEX_EEPROM_FILE_PATH_AS_ARGS) $(LSS_FILE_PATH_AS_ARGS) $(MAP_FILE_PATH_AS_ARGS)

