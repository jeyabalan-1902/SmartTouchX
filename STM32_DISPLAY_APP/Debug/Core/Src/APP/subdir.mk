################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/APP/RTC_app.c \
../Core/Src/APP/display_spi_app.c \
../Core/Src/APP/esp32_uart_app.c \
../Core/Src/APP/gsm_app.c \
../Core/Src/APP/rfm69_app.c \
../Core/Src/APP/touch_spi_app.c \
../Core/Src/APP/user_app.c 

OBJS += \
./Core/Src/APP/RTC_app.o \
./Core/Src/APP/display_spi_app.o \
./Core/Src/APP/esp32_uart_app.o \
./Core/Src/APP/gsm_app.o \
./Core/Src/APP/rfm69_app.o \
./Core/Src/APP/touch_spi_app.o \
./Core/Src/APP/user_app.o 

C_DEPS += \
./Core/Src/APP/RTC_app.d \
./Core/Src/APP/display_spi_app.d \
./Core/Src/APP/esp32_uart_app.d \
./Core/Src/APP/gsm_app.d \
./Core/Src/APP/rfm69_app.d \
./Core/Src/APP/touch_spi_app.d \
./Core/Src/APP/user_app.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/APP/%.o Core/Src/APP/%.su Core/Src/APP/%.cyclo: ../Core/Src/APP/%.c Core/Src/APP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/MQTT" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/DISPLAY" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/RFM" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/RFM" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/APP" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/SD_CARD" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/APP" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/SD_CARD" -I../Core/Inc -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/cJSON" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/MQTT" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/cJSON" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/include" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-APP

clean-Core-2f-Src-2f-APP:
	-$(RM) ./Core/Src/APP/RTC_app.cyclo ./Core/Src/APP/RTC_app.d ./Core/Src/APP/RTC_app.o ./Core/Src/APP/RTC_app.su ./Core/Src/APP/display_spi_app.cyclo ./Core/Src/APP/display_spi_app.d ./Core/Src/APP/display_spi_app.o ./Core/Src/APP/display_spi_app.su ./Core/Src/APP/esp32_uart_app.cyclo ./Core/Src/APP/esp32_uart_app.d ./Core/Src/APP/esp32_uart_app.o ./Core/Src/APP/esp32_uart_app.su ./Core/Src/APP/gsm_app.cyclo ./Core/Src/APP/gsm_app.d ./Core/Src/APP/gsm_app.o ./Core/Src/APP/gsm_app.su ./Core/Src/APP/rfm69_app.cyclo ./Core/Src/APP/rfm69_app.d ./Core/Src/APP/rfm69_app.o ./Core/Src/APP/rfm69_app.su ./Core/Src/APP/touch_spi_app.cyclo ./Core/Src/APP/touch_spi_app.d ./Core/Src/APP/touch_spi_app.o ./Core/Src/APP/touch_spi_app.su ./Core/Src/APP/user_app.cyclo ./Core/Src/APP/user_app.d ./Core/Src/APP/user_app.o ./Core/Src/APP/user_app.su

.PHONY: clean-Core-2f-Src-2f-APP

