################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../AVR1315/eeprom_driver.c 

OBJS += \
./AVR1315/eeprom_driver.o 

C_DEPS += \
./AVR1315/eeprom_driver.d 


# Each subdirectory must supply rules for building sources it contributes
AVR1315/%.o: ../AVR1315/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atxmega128a1 -DF_CPU=32000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


