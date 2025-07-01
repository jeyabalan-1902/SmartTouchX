################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../RTOS/freeRTOS/portable/GCC/ARM_CM4F/port.c 

OBJS += \
./RTOS/freeRTOS/portable/GCC/ARM_CM4F/port.o 

C_DEPS += \
./RTOS/freeRTOS/portable/GCC/ARM_CM4F/port.d 


# Each subdirectory must supply rules for building sources it contributes
RTOS/freeRTOS/portable/GCC/ARM_CM4F/%.o RTOS/freeRTOS/portable/GCC/ARM_CM4F/%.su RTOS/freeRTOS/portable/GCC/ARM_CM4F/%.cyclo: ../RTOS/freeRTOS/portable/GCC/ARM_CM4F/%.c RTOS/freeRTOS/portable/GCC/ARM_CM4F/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/include" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-RTOS-2f-freeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F

clean-RTOS-2f-freeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F:
	-$(RM) ./RTOS/freeRTOS/portable/GCC/ARM_CM4F/port.cyclo ./RTOS/freeRTOS/portable/GCC/ARM_CM4F/port.d ./RTOS/freeRTOS/portable/GCC/ARM_CM4F/port.o ./RTOS/freeRTOS/portable/GCC/ARM_CM4F/port.su

.PHONY: clean-RTOS-2f-freeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F

