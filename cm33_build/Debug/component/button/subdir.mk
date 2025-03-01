################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/button/fsl_component_button.c 

C_DEPS += \
./component/button/fsl_component_button.d 

OBJS += \
./component/button/fsl_component_button.o 


# Each subdirectory must supply rules for building sources it contributes
component/button/%.o: ../component/button/%.c component/button/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MCXN947VDF -DCPU_MCXN947VDF_cm33 -DCPU_MCXN947VDF_cm33_core0 -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 -DLWIP_SUPPORT_CUSTOM_PBUF=1 -DCHECKSUM_GEN_UDP=1 -DCHECKSUM_GEN_TCP=1 -DCHECKSUM_GEN_ICMP=1 -DCHECKSUM_GEN_ICMP6=1 -DCHECKSUM_CHECK_IP=1 -DCHECKSUM_CHECK_UDP=1 -DCHECKSUM_CHECK_TCP=1 -DCHECKSUM_CHECK_ICMP=1 -DCHECKSUM_CHECK_ICMP6=1 -DTIMER_PORT_TYPE_CTIMER=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -DUSE_RTOS=0 -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/drivers" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/device" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities/debug_console" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/uart" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities/debug_console/config" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/serial_manager" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/lists" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/device/periph" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/CMSIS" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/CMSIS/m-profile" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities/str" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/port" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/gpio" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/phy" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/src" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/src/include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/src/include/lwip" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/port/sys_arch/dynamic" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/osa/config" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/osa" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/host/class" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/host" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/button" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/timer" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/timer_manager" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/led" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/phy" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/common_task" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/i2c" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/silicon_id" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/drivers/mem_interface" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/drivers/flash" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/drivers/nboot" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/drivers/runbootloader" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/board" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/template" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/freertos/freertos-kernel/include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/freertos/freertos-kernel/portable/GCC/ARM_CM33/non_secure" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/source" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/source/generated" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-component-2f-button

clean-component-2f-button:
	-$(RM) ./component/button/fsl_component_button.d ./component/button/fsl_component_button.o

.PHONY: clean-component-2f-button

