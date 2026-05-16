################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/main.c \
../Src/nucleo446start.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/tiny_printf.c 

OBJS += \
./Src/main.o \
./Src/nucleo446start.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/tiny_printf.o 

C_DEPS += \
./Src/main.d \
./Src/nucleo446start.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/tiny_printf.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F446RETx -DSTM32F446xx -c -I../Inc -I"../Drivers/CMSIS/Include" -I"../Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"../Drivers/Bare-metal/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/nucleo446start.cyclo ./Src/nucleo446start.d ./Src/nucleo446start.o ./Src/nucleo446start.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/tiny_printf.cyclo ./Src/tiny_printf.d ./Src/tiny_printf.o ./Src/tiny_printf.su

.PHONY: clean-Src

