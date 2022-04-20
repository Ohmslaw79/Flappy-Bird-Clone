################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32.s 

OBJS += \
./startup/startup_stm32.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -I"G:/.shortcut-targets-by-id/1d2uwm78lQodgDKl-ajvBsV6jD6W-ns2K/ECE 362 Group 023 Safe Crackers/Final_Project_Code/StdPeriph_Driver/inc" -I"G:/.shortcut-targets-by-id/1d2uwm78lQodgDKl-ajvBsV6jD6W-ns2K/ECE 362 Group 023 Safe Crackers/Final_Project_Code/inc" -I"G:/.shortcut-targets-by-id/1d2uwm78lQodgDKl-ajvBsV6jD6W-ns2K/ECE 362 Group 023 Safe Crackers/Final_Project_Code/CMSIS/device" -I"G:/.shortcut-targets-by-id/1d2uwm78lQodgDKl-ajvBsV6jD6W-ns2K/ECE 362 Group 023 Safe Crackers/Final_Project_Code/CMSIS/core" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


