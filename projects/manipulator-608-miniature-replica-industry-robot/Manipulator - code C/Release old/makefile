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
Manipulator.lss \

FLASH_IMAGE += \
Manipulator.hex \

EEPROM_IMAGE += \
Manipulator.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: Manipulator.elf secondary-outputs

# Tool invocations
Manipulator.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,Manipulator.map -mmcu=atxmega128a1 -o "Manipulator.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Manipulator.lss: Manipulator.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S Manipulator.elf  >"Manipulator.lss"
	@echo 'Finished building: $@'
	@echo ' '

Manipulator.hex: Manipulator.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex Manipulator.elf  "Manipulator.hex"
	@echo 'Finished building: $@'
	@echo ' '

Manipulator.eep: Manipulator.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex Manipulator.elf  "Manipulator.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: Manipulator.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atxmega128a1 Manipulator.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) Manipulator.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
