################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/MQTT/MQTTConnectClient.c \
../Core/Src/MQTT/MQTTDeserializePublish.c \
../Core/Src/MQTT/MQTTFormat.c \
../Core/Src/MQTT/MQTTPacket.c \
../Core/Src/MQTT/MQTTSerializePublish.c \
../Core/Src/MQTT/MQTTSim800.c \
../Core/Src/MQTT/MQTTSubscribeClient.c \
../Core/Src/MQTT/MQTTUnsubscribeClient.c 

OBJS += \
./Core/Src/MQTT/MQTTConnectClient.o \
./Core/Src/MQTT/MQTTDeserializePublish.o \
./Core/Src/MQTT/MQTTFormat.o \
./Core/Src/MQTT/MQTTPacket.o \
./Core/Src/MQTT/MQTTSerializePublish.o \
./Core/Src/MQTT/MQTTSim800.o \
./Core/Src/MQTT/MQTTSubscribeClient.o \
./Core/Src/MQTT/MQTTUnsubscribeClient.o 

C_DEPS += \
./Core/Src/MQTT/MQTTConnectClient.d \
./Core/Src/MQTT/MQTTDeserializePublish.d \
./Core/Src/MQTT/MQTTFormat.d \
./Core/Src/MQTT/MQTTPacket.d \
./Core/Src/MQTT/MQTTSerializePublish.d \
./Core/Src/MQTT/MQTTSim800.d \
./Core/Src/MQTT/MQTTSubscribeClient.d \
./Core/Src/MQTT/MQTTUnsubscribeClient.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/MQTT/%.o Core/Src/MQTT/%.su Core/Src/MQTT/%.cyclo: ../Core/Src/MQTT/%.c Core/Src/MQTT/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/MQTT" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/RFM" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/RFM" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/APP" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/SD_CARD" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/APP" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/SD_CARD" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/ST7735" -I../Core/Inc -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/cJSON" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/MQTT" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Inc/ST7735" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/Core/Src/cJSON" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/include" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS/freeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/kjeya/Documents/MCU_INTEGRATION/STM32_DISPLAY_APP/RTOS" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-MQTT

clean-Core-2f-Src-2f-MQTT:
	-$(RM) ./Core/Src/MQTT/MQTTConnectClient.cyclo ./Core/Src/MQTT/MQTTConnectClient.d ./Core/Src/MQTT/MQTTConnectClient.o ./Core/Src/MQTT/MQTTConnectClient.su ./Core/Src/MQTT/MQTTDeserializePublish.cyclo ./Core/Src/MQTT/MQTTDeserializePublish.d ./Core/Src/MQTT/MQTTDeserializePublish.o ./Core/Src/MQTT/MQTTDeserializePublish.su ./Core/Src/MQTT/MQTTFormat.cyclo ./Core/Src/MQTT/MQTTFormat.d ./Core/Src/MQTT/MQTTFormat.o ./Core/Src/MQTT/MQTTFormat.su ./Core/Src/MQTT/MQTTPacket.cyclo ./Core/Src/MQTT/MQTTPacket.d ./Core/Src/MQTT/MQTTPacket.o ./Core/Src/MQTT/MQTTPacket.su ./Core/Src/MQTT/MQTTSerializePublish.cyclo ./Core/Src/MQTT/MQTTSerializePublish.d ./Core/Src/MQTT/MQTTSerializePublish.o ./Core/Src/MQTT/MQTTSerializePublish.su ./Core/Src/MQTT/MQTTSim800.cyclo ./Core/Src/MQTT/MQTTSim800.d ./Core/Src/MQTT/MQTTSim800.o ./Core/Src/MQTT/MQTTSim800.su ./Core/Src/MQTT/MQTTSubscribeClient.cyclo ./Core/Src/MQTT/MQTTSubscribeClient.d ./Core/Src/MQTT/MQTTSubscribeClient.o ./Core/Src/MQTT/MQTTSubscribeClient.su ./Core/Src/MQTT/MQTTUnsubscribeClient.cyclo ./Core/Src/MQTT/MQTTUnsubscribeClient.d ./Core/Src/MQTT/MQTTUnsubscribeClient.o ./Core/Src/MQTT/MQTTUnsubscribeClient.su

.PHONY: clean-Core-2f-Src-2f-MQTT

