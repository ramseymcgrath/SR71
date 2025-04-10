################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include xip/subdir.mk
-include utilities/subdir.mk
-include usb/phy/subdir.mk
-include usb/device/source/ehci/subdir.mk
-include usb/device/source/subdir.mk
-include usb/device/class/hid/subdir.mk
-include usb/device/class/subdir.mk
-include usb/dcd/subdir.mk
-include startup/subdir.mk
-include source/subdir.mk
-include drivers/subdir.mk
-include device/subdir.mk
-include component/uart/subdir.mk
-include component/serial_manager/subdir.mk
-include component/osa/subdir.mk
-include component/lists/subdir.mk
-include board/subdir.mk
-include azure-rtos/usbx/usbx_device_controllers/subdir.mk
-include azure-rtos/usbx/common/usbx_host_controllers/src/subdir.mk
-include azure-rtos/threadx/ports/cortex_m7/gnu/src/subdir.mk
-include azure-rtos/threadx/ports/common/subdir.mk
-include azure-rtos/netxduo/addons/websocket/subdir.mk
-include azure-rtos/netxduo/addons/web/subdir.mk
-include azure-rtos/netxduo/addons/tftp/subdir.mk
-include azure-rtos/netxduo/addons/telnet/subdir.mk
-include azure-rtos/netxduo/addons/sntp/subdir.mk
-include azure-rtos/netxduo/addons/snmp/subdir.mk
-include azure-rtos/netxduo/addons/smtp/subdir.mk
-include azure-rtos/netxduo/addons/rtsp/subdir.mk
-include azure-rtos/netxduo/addons/rtp/subdir.mk
-include azure-rtos/netxduo/addons/ptp/subdir.mk
-include azure-rtos/netxduo/addons/pppoe/subdir.mk
-include azure-rtos/netxduo/addons/ppp/subdir.mk
-include azure-rtos/netxduo/addons/pop3/subdir.mk
-include azure-rtos/netxduo/addons/nat/subdir.mk
-include azure-rtos/netxduo/addons/mqtt/subdir.mk
-include azure-rtos/netxduo/addons/mdns/subdir.mk
-include azure-rtos/netxduo/addons/http/subdir.mk
-include azure-rtos/netxduo/addons/ftp/subdir.mk
-include azure-rtos/netxduo/addons/dns/subdir.mk
-include azure-rtos/netxduo/addons/dhcp/subdir.mk
-include azure-rtos/netxduo/addons/cloud/subdir.mk
-include azure-rtos/netxduo/addons/auto_ip/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := evkmimxrt1060_usbx_host_hid_mouse
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: evkmimxrt1060_usbx_host_hid_mouse.axf

# Tool invocations
evkmimxrt1060_usbx_host_hid_mouse.axf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/evkmimxrt1060_usbx_host_hid_mouse/azure-rtos/binary/usbx/cortex_m7/mcux" -L"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/evkmimxrt1060_usbx_host_hid_mouse/azure-rtos/binary/netxduo/cortex_m7/mcux" -L"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/evkmimxrt1060_usbx_host_hid_mouse/azure-rtos/binary/filex/cortex_m7/mcux" -L"/Users/ramseymcgrath/code/embedded/mcuxpresso_workspace/evkmimxrt1060_usbx_host_hid_mouse/azure-rtos/binary/threadx/cortex_m7/mcux" -Xlinker -no-warn-rwx-segments -Xlinker -Map="evkmimxrt1060_usbx_host_hid_mouse.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -T evkmimxrt1060_usbx_host_hid_mouse_Debug.ld -o "evkmimxrt1060_usbx_host_hid_mouse.axf" $(OBJS) $(USER_OBJS) $(LIBS) -lusbx -lnetxduo -lfilex -lthreadx
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) evkmimxrt1060_usbx_host_hid_mouse.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "evkmimxrt1060_usbx_host_hid_mouse.axf"; # arm-none-eabi-objcopy -v -O binary "evkmimxrt1060_usbx_host_hid_mouse.axf" "evkmimxrt1060_usbx_host_hid_mouse.bin" ; # checksum -p MIMXRT1062xxxxA -d "evkmimxrt1060_usbx_host_hid_mouse.bin";
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
