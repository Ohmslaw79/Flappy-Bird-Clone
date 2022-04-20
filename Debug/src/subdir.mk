################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/background.c \
../src/ball.c \
../src/hw_8_main.c \
../src/lcd.c \
../src/main.c \
../src/midi.c \
../src/support.c \
../src/syscalls.c \
../src/system_stm32f0xx.c \
../src/tetris_theme.c 

OBJS += \
./src/background.o \
./src/ball.o \
./src/hw_8_main.o \
./src/lcd.o \
./src/main.o \
./src/midi.o \
./src/support.o \
./src/syscalls.o \
./src/system_stm32f0xx.o \
./src/tetris_theme.o 

C_DEPS += \
./src/background.d \
./src/ball.d \
./src/hw_8_main.d \
./src/lcd.d \
./src/main.d \
./src/midi.d \
./src/support.d \
./src/syscalls.d \
./src/system_stm32f0xx.d \
./src/tetris_theme.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F091RCTx -DDEBUG -DSTM32F091 -DUSE_STDPERIPH_DRIVER -I"G:/.shortcut-targets-by-id/1d2uwm78lQodgDKl-ajvBsV6jD6W-ns2K/ECE 362 Group 023 Safe Crackers/Final_Project_Code/StdPeriph_Driver/inc" -I"G:/.shortcut-targets-by-id/1d2uwm78lQodgDKl-ajvBsV6jD6W-ns2K/ECE 362 Group 023 Safe Crackers/Final_Project_Code/inc" -I"G:/.shortcut-targets-by-id/1d2uwm78lQodgDKl-ajvBsV6jD6W-ns2K/ECE 362 Group 023 Safe Crackers/Final_Project_Code/CMSIS/device" -I"G:/.shortcut-targets-by-id/1d2uwm78lQodgDKl-ajvBsV6jD6W-ns2K/ECE 362 Group 023 Safe Crackers/Final_Project_Code/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


