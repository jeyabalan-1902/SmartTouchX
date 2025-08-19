################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/SD_CARD/fatfs_sd.c \
../Core/Src/SD_CARD/sd_benchmark.c \
../Core/Src/SD_CARD/sd_diskio_spi.c \
../Core/Src/SD_CARD/sd_functions.c \
../Core/Src/SD_CARD/sd_spi.c 

OBJS += \
./Core/Src/SD_CARD/fatfs_sd.o \
./Core/Src/SD_CARD/sd_benchmark.o \
./Core/Src/SD_CARD/sd_diskio_spi.o \
./Core/Src/SD_CARD/sd_functions.o \
./Core/Src/SD_CARD/sd_spi.o 

C_DEPS += \
./Core/Src/SD_CARD/fatfs_sd.d \
./Core/Src/SD_CARD/sd_benchmark.d \
./Core/Src/SD_CARD/sd_diskio_spi.d \
./Core/Src/SD_CARD/sd_functions.d \
./Core/Src/SD_CARD/sd_spi.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/SD_CARD/%.o Core/Src/SD_CARD/%.su Core/Src/SD_CARD/%.cyclo: ../Core/Src/SD_CARD/%.c Core/Src/SD_CARD/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/MQTT" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/DISPLAY" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/RFM" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/RFM" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/APP" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/SD_CARD" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/APP" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/SD_CARD" -I../Core/Inc -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/cJSON" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/MQTT" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/cJSON" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/include" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-SD_CARD

clean-Core-2f-Src-2f-SD_CARD:
	-$(RM) ./Core/Src/SD_CARD/fatfs_sd.cyclo ./Core/Src/SD_CARD/fatfs_sd.d ./Core/Src/SD_CARD/fatfs_sd.o ./Core/Src/SD_CARD/fatfs_sd.su ./Core/Src/SD_CARD/sd_benchmark.cyclo ./Core/Src/SD_CARD/sd_benchmark.d ./Core/Src/SD_CARD/sd_benchmark.o ./Core/Src/SD_CARD/sd_benchmark.su ./Core/Src/SD_CARD/sd_diskio_spi.cyclo ./Core/Src/SD_CARD/sd_diskio_spi.d ./Core/Src/SD_CARD/sd_diskio_spi.o ./Core/Src/SD_CARD/sd_diskio_spi.su ./Core/Src/SD_CARD/sd_functions.cyclo ./Core/Src/SD_CARD/sd_functions.d ./Core/Src/SD_CARD/sd_functions.o ./Core/Src/SD_CARD/sd_functions.su ./Core/Src/SD_CARD/sd_spi.cyclo ./Core/Src/SD_CARD/sd_spi.d ./Core/Src/SD_CARD/sd_spi.o ./Core/Src/SD_CARD/sd_spi.su

.PHONY: clean-Core-2f-Src-2f-SD_CARD

