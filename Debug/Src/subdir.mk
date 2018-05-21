################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/TextOut.c \
../Src/basestationNRF24.c \
../Src/bitops.c \
../Src/gpio.c \
../Src/main.c \
../Src/myNRF24.c \
../Src/myNRF24basic.c \
../Src/packing.c \
../Src/spi.c \
../Src/stm32f1xx_hal_msp.c \
../Src/stm32f1xx_it.c \
../Src/system_stm32f1xx.c \
../Src/test_packetSending.c \
../Src/test_packing.c \
../Src/usb_device.c \
../Src/usbd_cdc_if.c \
../Src/usbd_conf.c \
../Src/usbd_desc.c 

OBJS += \
./Src/TextOut.o \
./Src/basestationNRF24.o \
./Src/bitops.o \
./Src/gpio.o \
./Src/main.o \
./Src/myNRF24.o \
./Src/myNRF24basic.o \
./Src/packing.o \
./Src/spi.o \
./Src/stm32f1xx_hal_msp.o \
./Src/stm32f1xx_it.o \
./Src/system_stm32f1xx.o \
./Src/test_packetSending.o \
./Src/test_packing.o \
./Src/usb_device.o \
./Src/usbd_cdc_if.o \
./Src/usbd_conf.o \
./Src/usbd_desc.o 

C_DEPS += \
./Src/TextOut.d \
./Src/basestationNRF24.d \
./Src/bitops.d \
./Src/gpio.d \
./Src/main.d \
./Src/myNRF24.d \
./Src/myNRF24basic.d \
./Src/packing.d \
./Src/spi.d \
./Src/stm32f1xx_hal_msp.d \
./Src/stm32f1xx_it.d \
./Src/system_stm32f1xx.d \
./Src/test_packetSending.d \
./Src/test_packing.d \
./Src/usb_device.d \
./Src/usbd_cdc_if.d \
./Src/usbd_conf.d \
./Src/usbd_desc.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F103xB -I"C:/Users/User/Documents/GitHub/blue_basestation/Inc" -I"C:/Users/User/Documents/GitHub/blue_basestation/Drivers/STM32F1xx_HAL_Driver/Inc" -I"C:/Users/User/Documents/GitHub/blue_basestation/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"C:/Users/User/Documents/GitHub/blue_basestation/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/User/Documents/GitHub/blue_basestation/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/User/Documents/GitHub/blue_basestation/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"C:/Users/User/Documents/GitHub/blue_basestation/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


