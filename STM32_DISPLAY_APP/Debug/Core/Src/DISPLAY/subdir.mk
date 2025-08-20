################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/DISPLAY/GFX_FUNCTIONS.c \
../Core/Src/DISPLAY/ST7735.c \
../Core/Src/DISPLAY/fonts.c \
../Core/Src/DISPLAY/menu_UI.c \
../Core/Src/DISPLAY/onwords_logo.c 

OBJS += \
./Core/Src/DISPLAY/GFX_FUNCTIONS.o \
./Core/Src/DISPLAY/ST7735.o \
./Core/Src/DISPLAY/fonts.o \
./Core/Src/DISPLAY/menu_UI.o \
./Core/Src/DISPLAY/onwords_logo.o 

C_DEPS += \
./Core/Src/DISPLAY/GFX_FUNCTIONS.d \
./Core/Src/DISPLAY/ST7735.d \
./Core/Src/DISPLAY/fonts.d \
./Core/Src/DISPLAY/menu_UI.d \
./Core/Src/DISPLAY/onwords_logo.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/DISPLAY/%.o Core/Src/DISPLAY/%.su Core/Src/DISPLAY/%.cyclo: ../Core/Src/DISPLAY/%.c Core/Src/DISPLAY/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/MQTT" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/DISPLAY" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/RFM" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/RFM" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/APP" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/SD_CARD" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/APP" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/SD_CARD" -I../Core/Inc -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/cJSON" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/MQTT" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/cJSON" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/include" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-DISPLAY

clean-Core-2f-Src-2f-DISPLAY:
	-$(RM) ./Core/Src/DISPLAY/GFX_FUNCTIONS.cyclo ./Core/Src/DISPLAY/GFX_FUNCTIONS.d ./Core/Src/DISPLAY/GFX_FUNCTIONS.o ./Core/Src/DISPLAY/GFX_FUNCTIONS.su ./Core/Src/DISPLAY/ST7735.cyclo ./Core/Src/DISPLAY/ST7735.d ./Core/Src/DISPLAY/ST7735.o ./Core/Src/DISPLAY/ST7735.su ./Core/Src/DISPLAY/fonts.cyclo ./Core/Src/DISPLAY/fonts.d ./Core/Src/DISPLAY/fonts.o ./Core/Src/DISPLAY/fonts.su ./Core/Src/DISPLAY/menu_UI.cyclo ./Core/Src/DISPLAY/menu_UI.d ./Core/Src/DISPLAY/menu_UI.o ./Core/Src/DISPLAY/menu_UI.su ./Core/Src/DISPLAY/onwords_logo.cyclo ./Core/Src/DISPLAY/onwords_logo.d ./Core/Src/DISPLAY/onwords_logo.o ./Core/Src/DISPLAY/onwords_logo.su

.PHONY: clean-Core-2f-Src-2f-DISPLAY

