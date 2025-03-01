################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lwip/src/netif/bridgeif.c \
../lwip/src/netif/bridgeif_fdb.c \
../lwip/src/netif/ethernet.c \
../lwip/src/netif/lowpan6.c \
../lwip/src/netif/lowpan6_ble.c \
../lwip/src/netif/lowpan6_common.c \
../lwip/src/netif/slipif.c \
../lwip/src/netif/zepif.c 

C_DEPS += \
./lwip/src/netif/bridgeif.d \
./lwip/src/netif/bridgeif_fdb.d \
./lwip/src/netif/ethernet.d \
./lwip/src/netif/lowpan6.d \
./lwip/src/netif/lowpan6_ble.d \
./lwip/src/netif/lowpan6_common.d \
./lwip/src/netif/slipif.d \
./lwip/src/netif/zepif.d 

OBJS += \
./lwip/src/netif/bridgeif.o \
./lwip/src/netif/bridgeif_fdb.o \
./lwip/src/netif/ethernet.o \
./lwip/src/netif/lowpan6.o \
./lwip/src/netif/lowpan6_ble.o \
./lwip/src/netif/lowpan6_common.o \
./lwip/src/netif/slipif.o \
./lwip/src/netif/zepif.o 


# Each subdirectory must supply rules for building sources it contributes
lwip/src/netif/%.o: ../lwip/src/netif/%.c lwip/src/netif/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MCXN947VDF -DCPU_MCXN947VDF_cm33 -DCPU_MCXN947VDF_cm33_core0 -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 -DLWIP_SUPPORT_CUSTOM_PBUF=1 -DCHECKSUM_GEN_UDP=1 -DCHECKSUM_GEN_TCP=1 -DCHECKSUM_GEN_ICMP=1 -DCHECKSUM_GEN_ICMP6=1 -DCHECKSUM_CHECK_IP=1 -DCHECKSUM_CHECK_UDP=1 -DCHECKSUM_CHECK_TCP=1 -DCHECKSUM_CHECK_ICMP=1 -DCHECKSUM_CHECK_ICMP6=1 -DTIMER_PORT_TYPE_CTIMER=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -DUSE_RTOS=0 -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/drivers" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/device" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities/debug_console" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/uart" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities/debug_console/config" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/serial_manager" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/lists" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/device/periph" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/CMSIS" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/CMSIS/m-profile" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities/str" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/port" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/gpio" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/phy" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/src" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/src/include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/src/include/lwip" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/port/sys_arch/dynamic" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/osa/config" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/osa" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/host/class" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/host" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/button" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/timer" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/timer_manager" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/led" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/phy" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/common_task" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/i2c" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/silicon_id" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/drivers/mem_interface" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/drivers/flash" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/drivers/nboot" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/drivers/runbootloader" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/board" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/template" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/freertos/freertos-kernel/include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/freertos/freertos-kernel/portable/GCC/ARM_CM33/non_secure" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/source" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/source/generated" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-lwip-2f-src-2f-netif

clean-lwip-2f-src-2f-netif:
	-$(RM) ./lwip/src/netif/bridgeif.d ./lwip/src/netif/bridgeif.o ./lwip/src/netif/bridgeif_fdb.d ./lwip/src/netif/bridgeif_fdb.o ./lwip/src/netif/ethernet.d ./lwip/src/netif/ethernet.o ./lwip/src/netif/lowpan6.d ./lwip/src/netif/lowpan6.o ./lwip/src/netif/lowpan6_ble.d ./lwip/src/netif/lowpan6_ble.o ./lwip/src/netif/lowpan6_common.d ./lwip/src/netif/lowpan6_common.o ./lwip/src/netif/slipif.d ./lwip/src/netif/slipif.o ./lwip/src/netif/zepif.d ./lwip/src/netif/zepif.o

.PHONY: clean-lwip-2f-src-2f-netif

