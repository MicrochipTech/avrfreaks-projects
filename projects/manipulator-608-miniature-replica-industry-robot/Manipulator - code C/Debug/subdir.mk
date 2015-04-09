################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Adc.c \
../CPU.c \
../Menu.c \
../PID.c \
../PcComands.c \
../hd44780.c \
../interrupts.c \
../main.c \
../usart.c 

OBJS += \
./Adc.o \
./CPU.o \
./Menu.o \
./PID.o \
./PcComands.o \
./hd44780.o \
./interrupts.o \
./main.o \
./usart.o 

C_DEPS += \
./Adc.d \
./CPU.d \
./Menu.d \
./PID.d \
./PcComands.d \
./hd44780.d \
./interrupts.d \
./main.d \
./usart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atxmega128a1 -DF_CPU=32000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


