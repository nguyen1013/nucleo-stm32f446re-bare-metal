################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.c 

OBJS += \
./Drivers/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.o 

C_DEPS += \
./Drivers/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/Device/ST/STM32F4xx/Source/%.o Drivers/CMSIS/Device/ST/STM32F4xx/Source/%.su Drivers/CMSIS/Device/ST/STM32F4xx/Source/%.cyclo: ../Drivers/CMSIS/Device/ST/STM32F4xx/Source/%.c Drivers/CMSIS/Device/ST/STM32F4xx/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F446RETx -DSTM32F446xx -c -I../Inc -I"../Drivers/CMSIS/Include" -I"../Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"../Drivers/Bare-metal/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-CMSIS-2f-Device-2f-ST-2f-STM32F4xx-2f-Source

clean-Drivers-2f-CMSIS-2f-Device-2f-ST-2f-STM32F4xx-2f-Source:
	-$(RM) ./Drivers/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.cyclo ./Drivers/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.d ./Drivers/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.o ./Drivers/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.su

.PHONY: clean-Drivers-2f-CMSIS-2f-Device-2f-ST-2f-STM32F4xx-2f-Source

