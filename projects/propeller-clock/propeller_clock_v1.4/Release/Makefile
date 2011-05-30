################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init
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
AVR_APP_PATH :=C:/Program Files/Atmel/AVR Studio 5.0/extensions/Application/Avr ToolChain/bin/
QUOTE := "
ADDITIONAL_DEPENDENCIES:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../images


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../analog_clk.c \
../blink.c \
../ext_int.c \
../globals.c \
../i2c_com.c \
../images/image_lams.c \
../images/image_tux51.c \
../images/image_tux65.c \
../images/image_win7.c \
../image_process.c \
../rs232.c \
../propeller.c \
../spi_com.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
analog_clk.o \
blink.o \
ext_int.o \
globals.o \
i2c_com.o \
images/image_lams.o \
images/image_tux51.o \
images/image_tux65.o \
images/image_win7.o \
image_process.o \
rs232.o \
propeller.o \
spi_com.o


OBJS_AS_ARGS +=  \
analog_clk.o \
blink.o \
ext_int.o \
globals.o \
i2c_com.o \
images/image_lams.o \
images/image_tux51.o \
images/image_tux65.o \
images/image_win7.o \
image_process.o \
rs232.o \
propeller.o \
spi_com.o


C_DEPS +=  \
analog_clk.d \
blink.d \
ext_int.d \
globals.d \
i2c_com.d \
images/image_lams.d \
images/image_tux51.d \
images/image_tux65.d \
images/image_win7.d \
image_process.d \
rs232.d \
propeller.d \
spi_com.d


C_DEPS_AS_ARGS +=  \
analog_clk.d \
blink.d \
ext_int.d \
globals.d \
i2c_com.d \
images/image_lams.d \
images/image_tux51.d \
images/image_tux65.d \
images/image_win7.d \
image_process.d \
rs232.d \
propeller.d \
spi_com.d


OUTPUT_FILE_PATH +=propeller.elf

OUTPUT_FILE_PATH_AS_ARGS +=propeller.elf

HEX_FLASH_FILE_PATH +=propeller.hex

HEX_FLASH_FILE_PATH_AS_ARGS +=propeller.hex

HEX_EEPROM_FILE_PATH +=propeller.eep

HEX_EEPROM_FILE_PATH_AS_ARGS +=propeller.eep

LSS_FILE_PATH +=propeller.lss

LSS_FILE_PATH_AS_ARGS +=propeller.lss

MAP_FILE_PATH =propeller.map

MAP_FILE_PATH_AS_ARGS =propeller.map

ADDITIONAL_DEPENDENCIES:= $(HEX_FLASH_FILE_PATH) size

# AVR/GNU C Compiler



























./%.o: .././%.c ../propeller.avrgccproj
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE) -funsigned-char -funsigned-bitfields -Os -fpack-struct -fshort-enums -Wall -c  -mmcu=atmega64   -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<

images/%.o: ../images/%.c ../propeller.avrgccproj
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE) -funsigned-char -funsigned-bitfields -Os -fpack-struct -fshort-enums -Wall -c  -mmcu=atmega64   -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<



# AVR/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

# AVR/GNU C Linker
$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS)
	@echo Building target: $@
	@echo Invoking: AVR/GNU C Linker
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE)  -mmcu=atmega64   -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS)
	@echo Finished building target: $@



$(HEX_FLASH_FILE_PATH): $(OUTPUT_FILE_PATH)
	$(QUOTE)$(AVR_APP_PATH)avr-objcopy.exe$(QUOTE) -O ihex -R .eeprom -R .fuse -R .lock -R .signature  $(QUOTE)$<$(QUOTE) $(QUOTE)$@$(QUOTE)

$(HEX_EEPROM_FILE_PATH): $(OUTPUT_FILE_PATH)
	-$(QUOTE)$(AVR_APP_PATH)avr-objcopy.exe$(QUOTE) -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex $(QUOTE)$<$(QUOTE) $(QUOTE)$@$(QUOTE) || exit 0

$(LSS_FILE_PATH): $(OUTPUT_FILE_PATH)
	$(QUOTE)$(AVR_APP_PATH)avr-objdump.exe$(QUOTE) -h -S $(QUOTE)$<$(QUOTE) > $(QUOTE)$@$(QUOTE)

size: $(OUTPUT_FILE_PATH)
	@$(QUOTE)$(AVR_APP_PATH)avr-size.exe$(QUOTE) -C --mcu=atmega64 $(OUTPUT_FILE_PATH_AS_ARGS)

# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS)$(C_DEPS_AS_ARGS) $(QUOTE)$(EXECUTABLES)$(QUOTE) $(OUTPUT_FILE_PATH_AS_ARGS) $(HEX_FLASH_FILE_PATH_AS_ARGS) $(HEX_EEPROM_FILE_PATH_AS_ARGS) $(LSS_FILE_PATH_AS_ARGS) $(MAP_FILE_PATH_AS_ARGS)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets