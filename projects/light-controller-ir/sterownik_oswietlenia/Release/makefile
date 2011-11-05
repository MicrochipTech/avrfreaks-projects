################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
sterownik_oswietlenia.lss \

FLASH_IMAGE += \
sterownik_oswietlenia.hex \

EEPROM_IMAGE += \
sterownik_oswietlenia.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: sterownik_oswietlenia.elf secondary-outputs

# Tool invocations
sterownik_oswietlenia.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,sterownik_oswietlenia.map -mmcu=atmega8 -o"sterownik_oswietlenia.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

sterownik_oswietlenia.lss: sterownik_oswietlenia.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S sterownik_oswietlenia.elf  >"sterownik_oswietlenia.lss"
	@echo 'Finished building: $@'
	@echo ' '

sterownik_oswietlenia.hex: sterownik_oswietlenia.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex sterownik_oswietlenia.elf  "sterownik_oswietlenia.hex"
	@echo 'Finished building: $@'
	@echo ' '

sterownik_oswietlenia.eep: sterownik_oswietlenia.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex sterownik_oswietlenia.elf  "sterownik_oswietlenia.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: sterownik_oswietlenia.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega8 sterownik_oswietlenia.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) sterownik_oswietlenia.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
