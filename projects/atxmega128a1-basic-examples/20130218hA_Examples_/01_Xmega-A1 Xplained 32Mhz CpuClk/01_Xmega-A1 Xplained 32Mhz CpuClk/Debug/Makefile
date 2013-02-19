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

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../Drivers


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../01_Xmega-A1\ Xplained\ 32Mhz\ CpuClk.cpp


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
01_Xmega-A1\ Xplained\ 32Mhz\ CpuClk.o


OBJS_AS_ARGS +=  \
"01_Xmega-A1 Xplained 32Mhz CpuClk.o"


C_DEPS +=  \
01_Xmega-A1\ Xplained\ 32Mhz\ CpuClk.d


C_DEPS_AS_ARGS +=  \
"01_Xmega-A1 Xplained 32Mhz CpuClk.d"


OUTPUT_FILE_PATH +=01_Xmega-A1\ Xplained\ 32Mhz\ CpuClk.elf

OUTPUT_FILE_PATH_AS_ARGS +="01_Xmega-A1 Xplained 32Mhz CpuClk.elf"

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

# AVR32/GNU C Compiler
./01_Xmega-A1\ Xplained\ 32Mhz\ CpuClk.o: .././01_Xmega-A1\ Xplained\ 32Mhz\ CpuClk.cpp
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C++ Compiler : (AVR_8_bit_GNU_Toolchain_3.4.1_830) 4.6.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Studio 6.0\extensions\Atmel\AVRGCC\3.4.1.95\AVRToolchain\bin\avr-g++.exe$(QUOTE) -funsigned-char -funsigned-bitfields -Os -fpack-struct -fshort-enums -g2 -Wall -c -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"  -mmcu=atxmega128a1   -o"$@" "$<" 
	@echo Finished building: $<


./%.o: .././%.cpp
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C++ Compiler : (AVR_8_bit_GNU_Toolchain_3.4.1_830) 4.6.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Studio 6.0\extensions\Atmel\AVRGCC\3.4.1.95\AVRToolchain\bin\avr-g++.exe$(QUOTE) -funsigned-char -funsigned-bitfields -Os -fpack-struct -fshort-enums -g2 -Wall -c -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"  -mmcu=atxmega128a1   -o"$@" "$<" 
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

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP)
	@echo Building target: $@
	@echo Invoking: AVR8/GNU Linker : (AVR_8_bit_GNU_Toolchain_3.4.1_830) 4.6.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Studio 6.0\extensions\Atmel\AVRGCC\3.4.1.95\AVRToolchain\bin\avr-g++.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="01_Xmega-A1 Xplained 32Mhz CpuClk.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group  -mmcu=atxmega128a1  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Atmel Studio 6.0\extensions\Atmel\AVRGCC\3.4.1.95\AVRToolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature  "01_Xmega-A1 Xplained 32Mhz CpuClk.elf" "01_Xmega-A1 Xplained 32Mhz CpuClk.hex"
	"C:\Program Files (x86)\Atmel\Atmel Studio 6.0\extensions\Atmel\AVRGCC\3.4.1.95\AVRToolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "01_Xmega-A1 Xplained 32Mhz CpuClk.elf" "01_Xmega-A1 Xplained 32Mhz CpuClk.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Atmel Studio 6.0\extensions\Atmel\AVRGCC\3.4.1.95\AVRToolchain\bin\avr-objdump.exe" -h -S "01_Xmega-A1 Xplained 32Mhz CpuClk.elf" > "01_Xmega-A1 Xplained 32Mhz CpuClk.lss"
	"C:\Program Files (x86)\Atmel\Atmel Studio 6.0\extensions\Atmel\AVRGCC\3.4.1.95\AVRToolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature  "01_Xmega-A1 Xplained 32Mhz CpuClk.elf" "01_Xmega-A1 Xplained 32Mhz CpuClk.srec"
	"C:\Program Files (x86)\Atmel\Atmel Studio 6.0\extensions\Atmel\AVRGCC\3.4.1.95\AVRToolchain\bin\avr-size.exe" "01_Xmega-A1 Xplained 32Mhz CpuClk.elf"
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS)$(C_DEPS_AS_ARGS) $(EXECUTABLES) 
	rm -rf "01_Xmega-A1 Xplained 32Mhz CpuClk.elf" "01_Xmega-A1 Xplained 32Mhz CpuClk.a" "01_Xmega-A1 Xplained 32Mhz CpuClk.hex" "01_Xmega-A1 Xplained 32Mhz CpuClk.lss" "01_Xmega-A1 Xplained 32Mhz CpuClk.eep" "01_Xmega-A1 Xplained 32Mhz CpuClk.map" "01_Xmega-A1 Xplained 32Mhz CpuClk.srec"
	