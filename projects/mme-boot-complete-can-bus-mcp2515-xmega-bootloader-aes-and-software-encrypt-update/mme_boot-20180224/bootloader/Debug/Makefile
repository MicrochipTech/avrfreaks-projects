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
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../aes.c \
../mcp2515.c \
../mme_bootloader.c \
../spiX.c


PREPROCESSING_SRCS +=  \
../sp_driver.S


ASM_SRCS += 


OBJS +=  \
aes.o \
mcp2515.o \
mme_bootloader.o \
spiX.o \
sp_driver.o

OBJS_AS_ARGS +=  \
aes.o \
mcp2515.o \
mme_bootloader.o \
spiX.o \
sp_driver.o

C_DEPS +=  \
aes.d \
mcp2515.d \
mme_bootloader.d \
spiX.d \
sp_driver.d

C_DEPS_AS_ARGS +=  \
aes.d \
mcp2515.d \
mme_bootloader.d \
spiX.d \
sp_driver.d

OUTPUT_FILE_PATH +=mme_boot.elf

OUTPUT_FILE_PATH_AS_ARGS +=mme_boot.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler









./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 5.4.0
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\XMEGAC_DFP\1.1.50\include"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax -g2 -Wall -mmcu=atxmega192c3 -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\XMEGAC_DFP\1.1.50\gcc\dev\atxmega192c3" -c -std=gnu99 -DF_CPU=48000000L -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler
./sp_driver.o: .././sp_driver.S
	@echo Building file: $<
	@echo Invoking: AVR/GNU Assembler : 5.4.0
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -Wa,-gdwarf2 -x assembler-with-cpp -c -mmcu=atxmega192c3 -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\XMEGAC_DFP\1.1.50\gcc\dev\atxmega192c3" -I "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\XMEGAC_DFP\1.1.50\include"  -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -mrelax -Wa,-g   -o "$@" "$<" 
	@echo Finished building: $<
	


./%.o: .././%.S
	@echo Building file: $<
	@echo Invoking: AVR/GNU Assembler : 5.4.0
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -Wa,-gdwarf2 -x assembler-with-cpp -c -mmcu=atxmega192c3 -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\XMEGAC_DFP\1.1.50\gcc\dev\atxmega192c3" -I "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\XMEGAC_DFP\1.1.50\include"  -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -mrelax -Wa,-g   -o "$@" "$<" 
	@echo Finished building: $<
	




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 5.4.0
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="mme_boot.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mrelax -mmcu=atxmega192c3 -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\XMEGAC_DFP\1.1.50\gcc\dev\atxmega192c3" -Wl,--section-start=.text=0x030000  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "mme_boot.elf" "mme_boot.hex"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "mme_boot.elf" "mme_boot.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "mme_boot.elf" > "mme_boot.lss"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "mme_boot.elf" "mme_boot.srec"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "mme_boot.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "mme_boot.elf" "mme_boot.a" "mme_boot.hex" "mme_boot.lss" "mme_boot.eep" "mme_boot.map" "mme_boot.srec" "mme_boot.usersignatures"
	