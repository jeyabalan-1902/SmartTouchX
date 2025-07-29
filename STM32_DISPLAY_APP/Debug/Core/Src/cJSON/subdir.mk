################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/cJSON/cJSON.c \
../Core/Src/cJSON/cJSON_Utils.c 

OBJS += \
./Core/Src/cJSON/cJSON.o \
./Core/Src/cJSON/cJSON_Utils.o 

C_DEPS += \
./Core/Src/cJSON/cJSON.d \
./Core/Src/cJSON/cJSON_Utils.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/cJSON/%.o Core/Src/cJSON/%.su Core/Src/cJSON/%.cyclo: ../Core/Src/cJSON/%.c Core/Src/cJSON/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/MQTT" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/ST7735" -I../Core/Inc -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/cJSON" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/MQTT" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/ST7735" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/cJSON" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/include" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-cJSON

clean-Core-2f-Src-2f-cJSON:
	-$(RM) ./Core/Src/cJSON/cJSON.cyclo ./Core/Src/cJSON/cJSON.d ./Core/Src/cJSON/cJSON.o ./Core/Src/cJSON/cJSON.su ./Core/Src/cJSON/cJSON_Utils.cyclo ./Core/Src/cJSON/cJSON_Utils.d ./Core/Src/cJSON/cJSON_Utils.o ./Core/Src/cJSON/cJSON_Utils.su

.PHONY: clean-Core-2f-Src-2f-cJSON

