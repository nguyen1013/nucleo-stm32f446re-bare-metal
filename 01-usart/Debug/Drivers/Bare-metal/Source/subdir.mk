################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Bare-metal/Source/adc.c \
../Drivers/Bare-metal/Source/timer.c \
../Drivers/Bare-metal/Source/uart2.c 

OBJS += \
./Drivers/Bare-metal/Source/adc.o \
./Drivers/Bare-metal/Source/timer.o \
./Drivers/Bare-metal/Source/uart2.o 

C_DEPS += \
./Drivers/Bare-metal/Source/adc.d \
./Drivers/Bare-metal/Source/timer.d \
./Drivers/Bare-metal/Source/uart2.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Bare-metal/Source/%.o Drivers/Bare-metal/Source/%.su Drivers/Bare-metal/Source/%.cyclo: ../Drivers/Bare-metal/Source/%.c Drivers/Bare-metal/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F446RETx -DSTM32F446xx -c -I../Inc -I"../Drivers/CMSIS/Include" -I"../Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"../Drivers/Bare-metal/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Bare-2d-metal-2f-Source

clean-Drivers-2f-Bare-2d-metal-2f-Source:
	-$(RM) ./Drivers/Bare-metal/Source/adc.cyclo ./Drivers/Bare-metal/Source/adc.d ./Drivers/Bare-metal/Source/adc.o ./Drivers/Bare-metal/Source/adc.su ./Drivers/Bare-metal/Source/timer.cyclo ./Drivers/Bare-metal/Source/timer.d ./Drivers/Bare-metal/Source/timer.o ./Drivers/Bare-metal/Source/timer.su ./Drivers/Bare-metal/Source/uart2.cyclo ./Drivers/Bare-metal/Source/uart2.d ./Drivers/Bare-metal/Source/uart2.o ./Drivers/Bare-metal/Source/uart2.su

.PHONY: clean-Drivers-2f-Bare-2d-metal-2f-Source

