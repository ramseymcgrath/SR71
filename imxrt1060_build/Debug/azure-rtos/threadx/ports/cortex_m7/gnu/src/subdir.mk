################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_misra.S \
../azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_context_restore.S \
../azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_context_save.S \
../azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_interrupt_control.S \
../azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_interrupt_disable.S \
../azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_interrupt_restore.S \
../azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_schedule.S \
../azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_stack_build.S \
../azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_system_return.S \
../azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_timer_interrupt.S 

OBJS += \
./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_misra.o \
./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_context_restore.o \
./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_context_save.o \
./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_interrupt_control.o \
./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_interrupt_disable.o \
./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_interrupt_restore.o \
./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_schedule.o \
./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_stack_build.o \
./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_system_return.o \
./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_timer_interrupt.o 


# Each subdirectory must supply rules for building sources it contributes
azure-rtos/threadx/ports/cortex_m7/gnu/src/%.o: ../azure-rtos/threadx/ports/cortex_m7/gnu/src/%.S azure-rtos/threadx/ports/cortex_m7/gnu/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -D__REDLIB__ -I"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/evkmimxrt1060_usbx_host_hid_mouse/source" -g3 -gdwarf-4 -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-azure-2d-rtos-2f-threadx-2f-ports-2f-cortex_m7-2f-gnu-2f-src

clean-azure-2d-rtos-2f-threadx-2f-ports-2f-cortex_m7-2f-gnu-2f-src:
	-$(RM) ./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_misra.o ./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_context_restore.o ./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_context_save.o ./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_interrupt_control.o ./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_interrupt_disable.o ./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_interrupt_restore.o ./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_schedule.o ./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_stack_build.o ./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_thread_system_return.o ./azure-rtos/threadx/ports/cortex_m7/gnu/src/tx_timer_interrupt.o

.PHONY: clean-azure-2d-rtos-2f-threadx-2f-ports-2f-cortex_m7-2f-gnu-2f-src

