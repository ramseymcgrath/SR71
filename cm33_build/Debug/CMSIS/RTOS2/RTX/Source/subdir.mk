################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/RTOS2/RTX/Source/rtx_delay.c \
../CMSIS/RTOS2/RTX/Source/rtx_evflags.c \
../CMSIS/RTOS2/RTX/Source/rtx_evr.c \
../CMSIS/RTOS2/RTX/Source/rtx_kernel.c \
../CMSIS/RTOS2/RTX/Source/rtx_lib.c \
../CMSIS/RTOS2/RTX/Source/rtx_memory.c \
../CMSIS/RTOS2/RTX/Source/rtx_mempool.c \
../CMSIS/RTOS2/RTX/Source/rtx_msgqueue.c \
../CMSIS/RTOS2/RTX/Source/rtx_mutex.c \
../CMSIS/RTOS2/RTX/Source/rtx_semaphore.c \
../CMSIS/RTOS2/RTX/Source/rtx_system.c \
../CMSIS/RTOS2/RTX/Source/rtx_thread.c \
../CMSIS/RTOS2/RTX/Source/rtx_timer.c 

C_DEPS += \
./CMSIS/RTOS2/RTX/Source/rtx_delay.d \
./CMSIS/RTOS2/RTX/Source/rtx_evflags.d \
./CMSIS/RTOS2/RTX/Source/rtx_evr.d \
./CMSIS/RTOS2/RTX/Source/rtx_kernel.d \
./CMSIS/RTOS2/RTX/Source/rtx_lib.d \
./CMSIS/RTOS2/RTX/Source/rtx_memory.d \
./CMSIS/RTOS2/RTX/Source/rtx_mempool.d \
./CMSIS/RTOS2/RTX/Source/rtx_msgqueue.d \
./CMSIS/RTOS2/RTX/Source/rtx_mutex.d \
./CMSIS/RTOS2/RTX/Source/rtx_semaphore.d \
./CMSIS/RTOS2/RTX/Source/rtx_system.d \
./CMSIS/RTOS2/RTX/Source/rtx_thread.d \
./CMSIS/RTOS2/RTX/Source/rtx_timer.d 

OBJS += \
./CMSIS/RTOS2/RTX/Source/rtx_delay.o \
./CMSIS/RTOS2/RTX/Source/rtx_evflags.o \
./CMSIS/RTOS2/RTX/Source/rtx_evr.o \
./CMSIS/RTOS2/RTX/Source/rtx_kernel.o \
./CMSIS/RTOS2/RTX/Source/rtx_lib.o \
./CMSIS/RTOS2/RTX/Source/rtx_memory.o \
./CMSIS/RTOS2/RTX/Source/rtx_mempool.o \
./CMSIS/RTOS2/RTX/Source/rtx_msgqueue.o \
./CMSIS/RTOS2/RTX/Source/rtx_mutex.o \
./CMSIS/RTOS2/RTX/Source/rtx_semaphore.o \
./CMSIS/RTOS2/RTX/Source/rtx_system.o \
./CMSIS/RTOS2/RTX/Source/rtx_thread.o \
./CMSIS/RTOS2/RTX/Source/rtx_timer.o 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/RTOS2/RTX/Source/%.o: ../CMSIS/RTOS2/RTX/Source/%.c CMSIS/RTOS2/RTX/Source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MCXN947VDF -DCPU_MCXN947VDF_cm33 -DCPU_MCXN947VDF_cm33_core0 -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 -DLWIP_SUPPORT_CUSTOM_PBUF=1 -DCHECKSUM_GEN_UDP=1 -DCHECKSUM_GEN_TCP=1 -DCHECKSUM_GEN_ICMP=1 -DCHECKSUM_GEN_ICMP6=1 -DCHECKSUM_CHECK_IP=1 -DCHECKSUM_CHECK_UDP=1 -DCHECKSUM_CHECK_TCP=1 -DCHECKSUM_CHECK_ICMP=1 -DCHECKSUM_CHECK_ICMP6=1 -DTIMER_PORT_TYPE_CTIMER=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -DSDK_OS_FREE_RTOS -DUSE_RTOS=1 -DCMSIS_device_header='"fsl_device_registers.h"' -DRTE_COMPONENTS_H -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/drivers" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/device" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities/debug_console" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/uart" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities/debug_console/config" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/serial_manager" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/lists" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/device/periph" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/CMSIS" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/CMSIS/m-profile" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/utilities/str" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/port" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/gpio" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/phy" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/src" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/src/include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/src/include/lwip" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/port/sys_arch/dynamic" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/osa/config" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/osa" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/device/class" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/device" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/host/class" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/host" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/usb/phy" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/button" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/timer" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/timer_manager" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/component/led" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/freertos/freertos-kernel/include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/freertos/freertos-kernel/portable/GCC/ARM_CM33/secure" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/freertos/freertos-kernel/portable/GCC/ARM_CM33/non_secure" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/CMSIS/RTOS2/RTX/Include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/CMSIS/RTOS2/RTX/Config" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/CMSIS/RTOS2/Include" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/phy" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/board" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/lwip/template" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/freertos/freertos-kernel/template" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/freertos/freertos-kernel/template/ARM_CM33_3_priority_bits" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/source" -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/SR71_Proxy/source/generated" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-CMSIS-2f-RTOS2-2f-RTX-2f-Source

clean-CMSIS-2f-RTOS2-2f-RTX-2f-Source:
	-$(RM) ./CMSIS/RTOS2/RTX/Source/rtx_delay.d ./CMSIS/RTOS2/RTX/Source/rtx_delay.o ./CMSIS/RTOS2/RTX/Source/rtx_evflags.d ./CMSIS/RTOS2/RTX/Source/rtx_evflags.o ./CMSIS/RTOS2/RTX/Source/rtx_evr.d ./CMSIS/RTOS2/RTX/Source/rtx_evr.o ./CMSIS/RTOS2/RTX/Source/rtx_kernel.d ./CMSIS/RTOS2/RTX/Source/rtx_kernel.o ./CMSIS/RTOS2/RTX/Source/rtx_lib.d ./CMSIS/RTOS2/RTX/Source/rtx_lib.o ./CMSIS/RTOS2/RTX/Source/rtx_memory.d ./CMSIS/RTOS2/RTX/Source/rtx_memory.o ./CMSIS/RTOS2/RTX/Source/rtx_mempool.d ./CMSIS/RTOS2/RTX/Source/rtx_mempool.o ./CMSIS/RTOS2/RTX/Source/rtx_msgqueue.d ./CMSIS/RTOS2/RTX/Source/rtx_msgqueue.o ./CMSIS/RTOS2/RTX/Source/rtx_mutex.d ./CMSIS/RTOS2/RTX/Source/rtx_mutex.o ./CMSIS/RTOS2/RTX/Source/rtx_semaphore.d ./CMSIS/RTOS2/RTX/Source/rtx_semaphore.o ./CMSIS/RTOS2/RTX/Source/rtx_system.d ./CMSIS/RTOS2/RTX/Source/rtx_system.o ./CMSIS/RTOS2/RTX/Source/rtx_thread.d ./CMSIS/RTOS2/RTX/Source/rtx_thread.o ./CMSIS/RTOS2/RTX/Source/rtx_timer.d ./CMSIS/RTOS2/RTX/Source/rtx_timer.o

.PHONY: clean-CMSIS-2f-RTOS2-2f-RTX-2f-Source

