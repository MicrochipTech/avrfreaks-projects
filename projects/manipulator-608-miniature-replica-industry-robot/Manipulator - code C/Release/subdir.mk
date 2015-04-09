################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Adc.c \
../Functions.c \
../Globals_Defs.c \
../Init.c \
../Menu.c \
../PcComands.c \
../dma_driver.c \
../hd44780.c \
../interrupts.c \
../main.c \
../usart.c \
../wdt_driver.c 

OBJS += \
./Adc.o \
./Functions.o \
./Globals_Defs.o \
./Init.o \
./Menu.o \
./PcComands.o \
./dma_driver.o \
./hd44780.o \
./interrupts.o \
./main.o \
./usart.o \
./wdt_driver.o 

C_DEPS += \
./Adc.d \
./Functions.d \
./Globals_Defs.d \
./Init.d \
./Menu.d \
./PcComands.d \
./dma_driver.d \
./hd44780.d \
./interrupts.d \
./main.d \
./usart.d \
./wdt_driver.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atxmega128a1 -DF_CPU=32000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


