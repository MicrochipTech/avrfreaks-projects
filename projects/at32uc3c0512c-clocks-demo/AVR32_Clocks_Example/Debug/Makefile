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
AVR_APP_PATH :=C:/Program Files/Atmel/AVR Studio 5.0/AVR ToolChain/bin/
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../src/ \
../src/config/


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../src/main.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
src/main.o


OBJS_AS_ARGS +=  \
src/main.o


C_DEPS +=  \
src/main.d


C_DEPS_AS_ARGS +=  \
src/main.d


OUTPUT_FILE_PATH +=AVR32_Clocks_Example.elf

OUTPUT_FILE_PATH_AS_ARGS +=AVR32_Clocks_Example.elf

HEX_FLASH_FILE_PATH +=AVR32_Clocks_Example.hex

HEX_FLASH_FILE_PATH_AS_ARGS +=AVR32_Clocks_Example.hex

HEX_EEPROM_FILE_PATH +=AVR32_Clocks_Example.eep

HEX_EEPROM_FILE_PATH_AS_ARGS +=AVR32_Clocks_Example.eep

LSS_FILE_PATH +=AVR32_Clocks_Example.lss

LSS_FILE_PATH_AS_ARGS +=AVR32_Clocks_Example.lss

MAP_FILE_PATH =AVR32_Clocks_Example.map

MAP_FILE_PATH_AS_ARGS =AVR32_Clocks_Example.map

LIB_AS_ARGS +=libAVR32_Clocks_Example.a

ADDITIONAL_DEPENDENCIES:= $(HEX_FLASH_FILE_PATH) size

OUTPUT_FILE_DEP:= ./makedep.mk

# AVR32/GNU C Compiler



src/%.o: ../src/%.c
	@echo Building file: $<
	@echo Invoking: AVR32/GNU C Compiler
	$(QUOTE)$(AVR_APP_PATH)avr32-gcc.exe$(QUOTE) -DBOARD=UC3C_EK  -I"../src" -I"../src/asf/avr32/boards" -I"../src/asf/avr32/boards/uc3c_ek" -I"../src/asf/avr32/drivers/gpio" -I"../src/asf/avr32/drivers/intc" -I"../src/asf/avr32/utils" -I"../src/asf/avr32/utils/preprocessor" -I"../src/asf/common/applications/user_application/at32uc3c0512c_uc3c_ek" -I"../src/asf/common/boards" -I"../src/asf/common/utils" -I"../src/config" -I"../src/asf/avr32/drivers/cpu/cycle_counter" -I"../src/asf/avr32/services/delay"  -O0 -fdata-sections -ffunction-sections -g3 -Wall -c -std=gnu99 -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration -Wpointer-arith -mrelax  -mpart=uc3c0512c   -MD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

# AVR32/GNU C/C++ Linker
$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP)
	@echo	Building target: $@
	@echo	Invoking: AVR32/GNU C/C++ Linker
	$(QUOTE)$(AVR_APP_PATH)avr32-gcc.exe$(QUOTE) -nostartfiles -Wl,--gc-sections -T../src/asf/avr32/utils/linker_scripts/at32uc3c/0512c/gcc/link_uc3c0512c.lds -Wl,--relax -Wl,-e,_trampoline  -mpart=uc3c0512c  -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS)
	@echo	Finished building target: $@



$(HEX_FLASH_FILE_PATH): $(OUTPUT_FILE_PATH)
	$(QUOTE)$(AVR_APP_PATH)avr32-objcopy.exe$(QUOTE) -O ihex -R .eeprom -R .fuse -R .lock -R .signature  $(QUOTE)$<$(QUOTE) $(QUOTE)$@$(QUOTE)

$(HEX_EEPROM_FILE_PATH): $(OUTPUT_FILE_PATH)
	-$(QUOTE)$(AVR_APP_PATH)avr32-objcopy.exe$(QUOTE) -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex $(QUOTE)$<$(QUOTE) $(QUOTE)$@$(QUOTE) || exit 0

$(LSS_FILE_PATH): $(OUTPUT_FILE_PATH)
	$(QUOTE)$(AVR_APP_PATH)avr32-objdump.exe$(QUOTE) -h -S $(QUOTE)$<$(QUOTE) > $(QUOTE)$@$(QUOTE)

size: $(OUTPUT_FILE_PATH)
	@echo AVR Memory Usage
	@echo ----------------
	@$(QUOTE)$(AVR_APP_PATH)avr32-size.exe$(QUOTE) $(OUTPUT_FILE_PATH_AS_ARGS)

# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS)$(C_DEPS_AS_ARGS) $(EXECUTABLES) $(LIB_AS_ARGS) $(HEX_FLASH_FILE_PATH_AS_ARGS) $(HEX_EEPROM_FILE_PATH_AS_ARGS) $(LSS_FILE_PATH_AS_ARGS) $(MAP_FILE_PATH_AS_ARGS)

