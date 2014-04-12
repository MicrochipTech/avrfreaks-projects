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
ad9850-re.lss \

FLASH_IMAGE += \
ad9850-re.hex \

EEPROM_IMAGE += \
ad9850-re.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: ad9850-re.elf secondary-outputs

# Tool invocations
ad9850-re.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,ad9850-re.map -mmcu=atmega8 -o "ad9850-re.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

ad9850-re.lss: ad9850-re.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S ad9850-re.elf  >"ad9850-re.lss"
	@echo 'Finished building: $@'
	@echo ' '

ad9850-re.hex: ad9850-re.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex ad9850-re.elf  "ad9850-re.hex"
	@echo 'Finished building: $@'
	@echo ' '

ad9850-re.eep: ad9850-re.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex ad9850-re.elf  "ad9850-re.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: ad9850-re.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega8 ad9850-re.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) ad9850-re.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
