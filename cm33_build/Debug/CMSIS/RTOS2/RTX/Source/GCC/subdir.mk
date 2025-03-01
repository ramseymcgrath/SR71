################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../CMSIS/RTOS2/RTX/Source/GCC/irq_armv8mml.S 

OBJS += \
./CMSIS/RTOS2/RTX/Source/GCC/irq_armv8mml.o 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/RTOS2/RTX/Source/GCC/%.o: ../CMSIS/RTOS2/RTX/Source/GCC/%.S CMSIS/RTOS2/RTX/Source/GCC/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -D__REDLIB__ -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/drivers" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/device" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities/debug_console" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/uart" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities/debug_console/config" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/serial_manager" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/lists" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/device/periph" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/CMSIS" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/CMSIS/m-profile" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities/str" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/port" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/gpio" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/phy" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/src" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/src/include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/src/include/lwip" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/port/sys_arch/dynamic" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/osa/config" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/osa" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/device/class" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/device" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/host/class" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/host" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/phy" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/button" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/timer" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/timer_manager" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/led" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/freertos/freertos-kernel/include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/freertos/freertos-kernel/portable/GCC/ARM_CM33/secure" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/freertos/freertos-kernel/portable/GCC/ARM_CM33/non_secure" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/CMSIS/RTOS2/RTX/Include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/CMSIS/RTOS2/RTX/Config" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/CMSIS/RTOS2/Include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/phy" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/template" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/freertos/freertos-kernel/template" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/freertos/freertos-kernel/template/ARM_CM33_3_priority_bits" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/board" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/source" -g3 -gdwarf-4 -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-CMSIS-2f-RTOS2-2f-RTX-2f-Source-2f-GCC

clean-CMSIS-2f-RTOS2-2f-RTX-2f-Source-2f-GCC:
	-$(RM) ./CMSIS/RTOS2/RTX/Source/GCC/irq_armv8mml.o

.PHONY: clean-CMSIS-2f-RTOS2-2f-RTX-2f-Source-2f-GCC

