################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ST7735/GFX_FUNCTIONS.c \
../Core/Src/ST7735/ST7735.c \
../Core/Src/ST7735/fonts.c \
../Core/Src/ST7735/onwords_logo.c 

OBJS += \
./Core/Src/ST7735/GFX_FUNCTIONS.o \
./Core/Src/ST7735/ST7735.o \
./Core/Src/ST7735/fonts.o \
./Core/Src/ST7735/onwords_logo.o 

C_DEPS += \
./Core/Src/ST7735/GFX_FUNCTIONS.d \
./Core/Src/ST7735/ST7735.d \
./Core/Src/ST7735/fonts.d \
./Core/Src/ST7735/onwords_logo.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/ST7735/%.o Core/Src/ST7735/%.su Core/Src/ST7735/%.cyclo: ../Core/Src/ST7735/%.c Core/Src/ST7735/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/MQTT" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/RFM" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/RFM" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/APP" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/SD_CARD" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/APP" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/SD_CARD" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/ST7735" -I../Core/Inc -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/cJSON" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/MQTT" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/ST7735" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/cJSON" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/include" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-ST7735

clean-Core-2f-Src-2f-ST7735:
	-$(RM) ./Core/Src/ST7735/GFX_FUNCTIONS.cyclo ./Core/Src/ST7735/GFX_FUNCTIONS.d ./Core/Src/ST7735/GFX_FUNCTIONS.o ./Core/Src/ST7735/GFX_FUNCTIONS.su ./Core/Src/ST7735/ST7735.cyclo ./Core/Src/ST7735/ST7735.d ./Core/Src/ST7735/ST7735.o ./Core/Src/ST7735/ST7735.su ./Core/Src/ST7735/fonts.cyclo ./Core/Src/ST7735/fonts.d ./Core/Src/ST7735/fonts.o ./Core/Src/ST7735/fonts.su ./Core/Src/ST7735/onwords_logo.cyclo ./Core/Src/ST7735/onwords_logo.d ./Core/Src/ST7735/onwords_logo.o ./Core/Src/ST7735/onwords_logo.su

.PHONY: clean-Core-2f-Src-2f-ST7735

