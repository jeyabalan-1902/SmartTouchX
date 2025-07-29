################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../RTOS/freeRTOS/croutine.c \
../RTOS/freeRTOS/event_groups.c \
../RTOS/freeRTOS/list.c \
../RTOS/freeRTOS/queue.c \
../RTOS/freeRTOS/stream_buffer.c \
../RTOS/freeRTOS/tasks.c \
../RTOS/freeRTOS/timers.c 

OBJS += \
./RTOS/freeRTOS/croutine.o \
./RTOS/freeRTOS/event_groups.o \
./RTOS/freeRTOS/list.o \
./RTOS/freeRTOS/queue.o \
./RTOS/freeRTOS/stream_buffer.o \
./RTOS/freeRTOS/tasks.o \
./RTOS/freeRTOS/timers.o 

C_DEPS += \
./RTOS/freeRTOS/croutine.d \
./RTOS/freeRTOS/event_groups.d \
./RTOS/freeRTOS/list.d \
./RTOS/freeRTOS/queue.d \
./RTOS/freeRTOS/stream_buffer.d \
./RTOS/freeRTOS/tasks.d \
./RTOS/freeRTOS/timers.d 


# Each subdirectory must supply rules for building sources it contributes
RTOS/freeRTOS/%.o RTOS/freeRTOS/%.su RTOS/freeRTOS/%.cyclo: ../RTOS/freeRTOS/%.c RTOS/freeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/MQTT" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/ST7735" -I../Core/Inc -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/cJSON" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/MQTT" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/ST7735" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/cJSON" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/include" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-RTOS-2f-freeRTOS

clean-RTOS-2f-freeRTOS:
	-$(RM) ./RTOS/freeRTOS/croutine.cyclo ./RTOS/freeRTOS/croutine.d ./RTOS/freeRTOS/croutine.o ./RTOS/freeRTOS/croutine.su ./RTOS/freeRTOS/event_groups.cyclo ./RTOS/freeRTOS/event_groups.d ./RTOS/freeRTOS/event_groups.o ./RTOS/freeRTOS/event_groups.su ./RTOS/freeRTOS/list.cyclo ./RTOS/freeRTOS/list.d ./RTOS/freeRTOS/list.o ./RTOS/freeRTOS/list.su ./RTOS/freeRTOS/queue.cyclo ./RTOS/freeRTOS/queue.d ./RTOS/freeRTOS/queue.o ./RTOS/freeRTOS/queue.su ./RTOS/freeRTOS/stream_buffer.cyclo ./RTOS/freeRTOS/stream_buffer.d ./RTOS/freeRTOS/stream_buffer.o ./RTOS/freeRTOS/stream_buffer.su ./RTOS/freeRTOS/tasks.cyclo ./RTOS/freeRTOS/tasks.d ./RTOS/freeRTOS/tasks.o ./RTOS/freeRTOS/tasks.su ./RTOS/freeRTOS/timers.cyclo ./RTOS/freeRTOS/timers.d ./RTOS/freeRTOS/timers.o ./RTOS/freeRTOS/timers.su

.PHONY: clean-RTOS-2f-freeRTOS

