################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Source/timer.c 

OBJS += \
./Drivers/Source/timer.o 

C_DEPS += \
./Drivers/Source/timer.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Source/%.o Drivers/Source/%.su Drivers/Source/%.cyclo: ../Drivers/Source/%.c Drivers/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F446RETx -DSTM32F446xx -c -I../Inc -I"D:/01-jupiter/05-stm32f4/projects/template-2/Inc/Drivers/Include" -I"D:/01-jupiter/05-stm32f4/projects/template-2/CMSIS/Device/ST/STM32F4xx/Include" -I"D:/01-jupiter/05-stm32f4/projects/template-2/CMSIS/Include" -I/template-2/Drivers/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Source

clean-Drivers-2f-Source:
	-$(RM) ./Drivers/Source/timer.cyclo ./Drivers/Source/timer.d ./Drivers/Source/timer.o ./Drivers/Source/timer.su

.PHONY: clean-Drivers-2f-Source

