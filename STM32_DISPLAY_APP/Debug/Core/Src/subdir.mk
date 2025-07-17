################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/GFX_FUNCTIONS.c \
../Core/Src/ST7735.c \
../Core/Src/cJSON.c \
../Core/Src/cJSON_Utils.c \
../Core/Src/display_ctrl.c \
../Core/Src/fonts.c \
../Core/Src/main.c \
../Core/Src/onwords_logo.c \
../Core/Src/spi_rec.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_hal_timebase_tim.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/uart_rec.c \
../Core/Src/user_app.c 

OBJS += \
./Core/Src/GFX_FUNCTIONS.o \
./Core/Src/ST7735.o \
./Core/Src/cJSON.o \
./Core/Src/cJSON_Utils.o \
./Core/Src/display_ctrl.o \
./Core/Src/fonts.o \
./Core/Src/main.o \
./Core/Src/onwords_logo.o \
./Core/Src/spi_rec.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_hal_timebase_tim.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/uart_rec.o \
./Core/Src/user_app.o 

C_DEPS += \
./Core/Src/GFX_FUNCTIONS.d \
./Core/Src/ST7735.d \
./Core/Src/cJSON.d \
./Core/Src/cJSON_Utils.d \
./Core/Src/display_ctrl.d \
./Core/Src/fonts.d \
./Core/Src/main.d \
./Core/Src/onwords_logo.d \
./Core/Src/spi_rec.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_hal_timebase_tim.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/uart_rec.d \
./Core/Src/user_app.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/include" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/GFX_FUNCTIONS.cyclo ./Core/Src/GFX_FUNCTIONS.d ./Core/Src/GFX_FUNCTIONS.o ./Core/Src/GFX_FUNCTIONS.su ./Core/Src/ST7735.cyclo ./Core/Src/ST7735.d ./Core/Src/ST7735.o ./Core/Src/ST7735.su ./Core/Src/cJSON.cyclo ./Core/Src/cJSON.d ./Core/Src/cJSON.o ./Core/Src/cJSON.su ./Core/Src/cJSON_Utils.cyclo ./Core/Src/cJSON_Utils.d ./Core/Src/cJSON_Utils.o ./Core/Src/cJSON_Utils.su ./Core/Src/display_ctrl.cyclo ./Core/Src/display_ctrl.d ./Core/Src/display_ctrl.o ./Core/Src/display_ctrl.su ./Core/Src/fonts.cyclo ./Core/Src/fonts.d ./Core/Src/fonts.o ./Core/Src/fonts.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/onwords_logo.cyclo ./Core/Src/onwords_logo.d ./Core/Src/onwords_logo.o ./Core/Src/onwords_logo.su ./Core/Src/spi_rec.cyclo ./Core/Src/spi_rec.d ./Core/Src/spi_rec.o ./Core/Src/spi_rec.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_hal_timebase_tim.cyclo ./Core/Src/stm32f4xx_hal_timebase_tim.d ./Core/Src/stm32f4xx_hal_timebase_tim.o ./Core/Src/stm32f4xx_hal_timebase_tim.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/uart_rec.cyclo ./Core/Src/uart_rec.d ./Core/Src/uart_rec.o ./Core/Src/uart_rec.su ./Core/Src/user_app.cyclo ./Core/Src/user_app.d ./Core/Src/user_app.o ./Core/Src/user_app.su

.PHONY: clean-Core-2f-Src

