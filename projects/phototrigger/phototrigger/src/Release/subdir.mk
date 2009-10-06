################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../dlg.c \
../eeprom.c \
../enc.c \
../hd44780.c \
../hw.c \
../lcd.c \
../main.c \
../pt.c \
../rtc.c \
../tinymenu.c 

OBJS += \
./dlg.o \
./eeprom.o \
./enc.o \
./hd44780.o \
./hw.o \
./lcd.o \
./main.o \
./pt.o \
./rtc.o \
./tinymenu.o 

C_DEPS += \
./dlg.d \
./eeprom.d \
./enc.d \
./hd44780.d \
./hw.d \
./lcd.d \
./main.d \
./pt.d \
./rtc.d \
./tinymenu.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I/usr/lib/gcc/avr/4.3.3/include -I/usr/lib/gcc/avr/4.3.3/include-fixed -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega168 -DF_CPU=1000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


