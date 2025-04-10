#include "ux_api.h"
#include "ux_device_stack.h"
#include "ux_host_stack.h"
#include "ux_host_class_hid.h"
#include "ux_host_class_hid_mouse.h"
#include "ux_device_class_hid.h"
#include "ux_hcd_ehci.h"
#include "ux_dcd_nxp_dci.h"
#include "tx_api.h"

#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "usb_phy.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"

/* Fix missing USB request macros if not defined */
/* Macros if missing */
#ifndef UX_REQUEST_TARGET_INTERFACE
#define UX_REQUEST_TARGET_INTERFACE        0x01
#endif
#ifndef UX_REQUEST_TYPE_STANDARD
#define UX_REQUEST_TYPE_STANDARD           0x00
#endif
#ifndef UX_REQUEST_IN
#define UX_REQUEST_IN                      0x80
#endif
#ifndef UX_GET_DESCRIPTOR
#define UX_GET_DESCRIPTOR                  0x06
#endif
#ifndef UX_HOST_CLASS_HID_REPORT_DESCRIPTOR
#define UX_HOST_CLASS_HID_REPORT_DESCRIPTOR 0x22
#endif
#define USB_DEVICE_CONFIG_EHCI (1U)
extern UINT _ux_host_class_hub_entry(struct UX_HOST_CLASS_COMMAND_STRUCT *);
extern VOID _ux_hcd_ehci_interrupt_handler(VOID);
extern usb_device_handle g_deviceHandle;
extern UINT _ux_host_stack_device_request(UX_DEVICE *, ULONG bmRequestType, ULONG bRequest,
                                          ULONG wValue, ULONG wIndex,
                                          UCHAR *data_pointer, ULONG requested_length, ULONG *actual_length);
UINT usbx_host_control_transfer(UX_DEVICE *device,
                                ULONG bmRequestType, ULONG bRequest,
                                ULONG wValue, ULONG wIndex,
                                UCHAR *data_pointer, ULONG requested_length, ULONG *actual_length);
UINT ux_host_hid_report_descriptor_fetch(UX_HOST_CLASS_HID *hid, UCHAR *buffer, ULONG size, ULONG *actual_length);
UINT usb_host_hw_setup(void);
UINT usb_device_hw_setup(void);
UINT usbx_host_hcd_register(void);
UINT usbx_device_controller_init(void);


#define UX_APP_MEM_SIZE   (64*1024)
#define UX_APP_STACK_SIZE  (4096)

__attribute__((aligned(8))) ULONG usbx_mem[UX_APP_MEM_SIZE/sizeof(ULONG)];
ULONG app_stack[UX_APP_STACK_SIZE/sizeof(ULONG)];

TX_THREAD app_thread;

/***** Clone-related data *****/
UCHAR hid_report_desc[512];
ULONG hid_report_len;

UCHAR device_framework[1024];
ULONG device_framework_len;

UX_HOST_CLASS_HID *hid_class_inst = UX_NULL;
UX_DEVICE *dev_inst = UX_NULL;

VOID *g_deviceHandle = NULL;

/******** Host change callback *********/
UINT usbx_host_change_callback(ULONG event, UX_HOST_CLASS *host_class, VOID *instance)
{
    if ((event == UX_DEVICE_CONNECTION) && (instance))
    {
        UX_DEVICE *dev = (UX_DEVICE*)instance;
        PRINTF("Host device VID:0x%04x PID:0x%04x\n",
               dev->ux_device_descriptor.idVendor,
               dev->ux_device_descriptor.idProduct);
    }
    return UX_SUCCESS;
}

/********* Main thread **********/
void clone_thread(ULONG arg)
{
    UINT status;

    PRINTF("\nWaiting for HID mouse connection...\n");

    while(1)
    {
        UX_HOST_CLASS *hid_class;
        status = ux_host_stack_class_get(_ux_system_host_class_hid_name, &hid_class);
        if(status) { tx_thread_sleep(10); continue; }

        status = ux_host_stack_class_instance_get(hid_class, 0, (VOID**)&hid_class_inst);
        if(status) { tx_thread_sleep(10); continue; }

        if(hid_class_inst->ux_host_class_hid_state != UX_HOST_CLASS_INSTANCE_LIVE){
            tx_thread_sleep(10); continue; }

        break;
    }


    dev_inst = (UX_DEVICE*) hid_class_inst->ux_host_class_hid_device;

    /* Fetch **raw** config descriptor as sent by device during enumeration */
    static UCHAR cfg_desc_buffer[1024] __attribute__((aligned(4)));
    ULONG actual_length = 0;
    UINT wTotalLength;

    status = usbx_host_control_transfer(dev_inst,
        UX_REQUEST_IN | UX_REQUEST_TYPE_STANDARD | UX_REQUEST_TARGET_DEVICE,
        UX_GET_DESCRIPTOR, (UX_CONFIGURATION_DESCRIPTOR_ITEM << 8) | 0, 0,
        cfg_desc_buffer, 9, &actual_length);
    if(status != UX_SUCCESS || actual_length < 9 || actual_length > sizeof(cfg_desc_buffer)){
        PRINTF("Config descriptor header fetch failed 0x%x (%lu bytes)\n", status, actual_length);
        while(1);
    }
    wTotalLength = (ULONG)(cfg_desc_buffer[2] | (cfg_desc_buffer[3]<<8));
    if(wTotalLength > sizeof(cfg_desc_buffer)){
        PRINTF("Descriptor too big: %lu bytes!\n", wTotalLength);
        while(1);
    }
    status = usbx_host_control_transfer(dev_inst,
        UX_REQUEST_IN | UX_REQUEST_TYPE_STANDARD | UX_REQUEST_TARGET_DEVICE,
        UX_GET_DESCRIPTOR, (UX_CONFIGURATION_DESCRIPTOR_ITEM << 8) | 0, 0,
        cfg_desc_buffer, wTotalLength, &actual_length);
    if(status != UX_SUCCESS || actual_length < 9 || actual_length > sizeof(cfg_desc_buffer)){
        PRINTF("Config desc fetch failed 0x%x (%lu/%lu)\n", status, actual_length, wTotalLength);
        while(1);
    }

    UCHAR *cfg_desc = cfg_desc_buffer;
    ULONG cfg_len = actual_length;

    PRINTF("Config total length: %lu bytes\n", cfg_len);

    /* Fetch HID report descriptor using control transfer */
    ULONG actual_len = 0;
    status = ux_host_hid_report_descriptor_fetch(hid_class_inst, hid_report_desc, sizeof(hid_report_desc), &actual_len);
    if(status){
        PRINTF("Error HID report desc get: %x\n", status);
        while(1);
    }
    hid_report_len = actual_len;
    PRINTF("HID report length: %lu bytes\n", hid_report_len);

    /* Build device descriptor + config block */
    device_framework_len = 18 + cfg_len;
    ux_utility_memory_copy(device_framework,
                           (UCHAR*)&dev_inst->ux_device_descriptor, 18);
    ux_utility_memory_copy(device_framework+18,
                           cfg_desc, cfg_len);

    PRINTF("Starting DEVICE on second port...\n");

    usb_device_hw_setup();

    status = ux_device_stack_initialize(
        UX_NULL, 0,
        device_framework, device_framework_len,
        (UCHAR*)"\x09\x04", 2, (UCHAR*)"\x09\x04", 2, UX_NULL);
    if(status) { PRINTF("Dev stack init fail 0x%x\n",status); while(1); }

    UX_SLAVE_CLASS_HID_PARAMETER hid_param;
    ux_utility_memory_set(&hid_param, 0, sizeof(hid_param));
    hid_param.ux_device_class_hid_parameter_report_address = hid_report_desc;
    hid_param.ux_device_class_hid_parameter_report_length = hid_report_len;

    status = ux_device_stack_class_register(_ux_system_slave_class_hid_name,
                                            ux_device_class_hid_entry,
                                            1,0,&hid_param);
    if(status) { PRINTF("device hid class reg fail 0x%x\n",status); while(1); }

    usbx_device_controller_init();

    PRINTF("==== HID CLONE ready -- plug USB2 to PC ====\n");

    UX_HOST_CLASS_HID_MOUSE *mouse = (UX_HOST_CLASS_HID_MOUSE *)
        hid_class_inst->ux_host_class_hid_client->ux_host_class_hid_client_local_instance;

    ULONG btn; SLONG x,y;

    while(1)
    {
    	UINT ret1 = _ux_host_class_hid_mouse_buttons_get(mouse, &btn);
    	UINT ret2 = _ux_host_class_hid_mouse_position_get(mouse, &x, &y);
    	if((ret1 == UX_SUCCESS) && (ret2 == UX_SUCCESS))
    	    PRINTF("Buttons: %02lx Pos: %d,%d\n", btn,x,y);
    	else
    	    PRINTF("Mouse disconnected or error\n");

    }
}

/********* RTOS create **********/
void tx_application_define(void *first_unused_memory)
{
    UINT status;

    ux_system_initialize(usbx_mem, sizeof(usbx_mem), UX_NULL, 0);

    status = ux_host_stack_initialize(usbx_host_change_callback);
    if(status) while(1);

    ux_host_stack_class_register(_ux_system_host_class_hub_name, _ux_host_class_hub_entry);
    ux_host_stack_class_register(_ux_system_host_class_hid_name, ux_host_class_hid_entry);
    ux_host_class_hid_client_register(_ux_system_host_class_hid_client_mouse_name,
                                      ux_host_class_hid_mouse_entry);
    usb_host_hw_setup();
    usbx_host_hcd_register();

    tx_thread_create(&app_thread, "clone_thread", clone_thread, 0,
                     app_stack, UX_APP_STACK_SIZE,
                     20,20,1,TX_AUTO_START);
}

int main(void)
{
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    NVIC_SetPriority(USB_OTG1_IRQn,5);
    NVIC_SetPriority(USB_OTG2_IRQn,5);
    NVIC_EnableIRQ(USB_OTG1_IRQn);
    NVIC_EnableIRQ(USB_OTG2_IRQn);

    PRINTF("i.MX RT1060 KMBOX\n");
    tx_kernel_enter();
}

/************* BSP-specific USB functions ***********/
UINT usb_host_hw_setup(void)
{
    CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M,480000000);
    CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M,480000000);

    usb_phy_config_struct_t phyConfig = {0};
    phyConfig.D_CAL=0x0C; phyConfig.TXCAL45DP=6; phyConfig.TXCAL45DM=6;
    USB_EhciPhyInit(0, 480000000, &phyConfig);
    return 0;
}

UINT usbx_host_hcd_register(void)
{
    return ux_host_stack_hcd_register(_ux_system_host_hcd_ehci_name,
                                      _ux_hcd_ehci_initialize,
                                      (ULONG)USB1_BASE, USB_OTG1_IRQn);
}

UINT usb_device_hw_setup(void)
{
    CLOCK_EnableUsbhs1PhyPllClock(kCLOCK_Usbphy480M,480000000);
    CLOCK_EnableUsbhs1Clock(kCLOCK_Usb480M,480000000);
    CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
    CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M, 480000000U);
    usb_phy_config_struct_t phyConfig = {
        BOARD_USB_PHY_D_CAL,
        BOARD_USB_PHY_TXCAL45DP,
        BOARD_USB_PHY_TXCAL45DM,
    };
    phyConfig.D_CAL=0x0C; phyConfig.TXCAL45DP=6; phyConfig.TXCAL45DM=6;
    USB_EhciPhyInit(USB_DEVICE_CONFIG_EHCI, BOARD_XTAL0_CLK_HZ, &phyConfig);
    USBPHY2->CTRL_CLR = USBPHY_CTRL_ENUTMILEVEL2_MASK | USBPHY_CTRL_ENUTMILEVEL3_MASK;
    return 0;
}

UINT usbx_device_controller_init(void)
{
	return _ux_dcd_nxp_dci_initialize(USB_DEVICE_CONFIG_EHCI, &g_deviceHandle);
}

/************* ISR handlers **************/

void USB_OTG1_IRQHandler(void)
{
	_ux_hcd_ehci_interrupt_handler();
}

void USB_OTG2_IRQHandler(void)
{
    USB_DeviceEhciIsrFunction(g_deviceHandle);
    __DSB();
}

UINT usbx_host_control_transfer(UX_DEVICE *device,
                                ULONG bmRequestType, ULONG bRequest,
                                ULONG wValue, ULONG wIndex,
                                UCHAR *data_pointer, ULONG requested_length, ULONG *actual_length)
{
    UX_TRANSFER *transfer;
    UINT status;
    ULONG request;

    // Use the control endpoint's transfer request
    transfer = &device->ux_device_control_endpoint.ux_endpoint_transfer_request;

    transfer->ux_transfer_request_type = (UINT) bmRequestType;
    transfer->ux_transfer_request_function = (UINT) bRequest;
    transfer->ux_transfer_request_value = (UINT) wValue;
    transfer->ux_transfer_request_index = (UINT) wIndex;
    transfer->ux_transfer_request_data_pointer = data_pointer;
    transfer->ux_transfer_request_requested_length = requested_length;

    // Actually perform the transfer
    status = ux_host_stack_transfer_request(transfer);

    if (status == UX_SUCCESS)
    {
        if (actual_length)
            *actual_length = transfer->ux_transfer_request_actual_length;
    }
    else
    {
        if (actual_length)
            *actual_length = 0;
    }

    return status;
}

/************* HID report fetch via control transfer **************/
UINT ux_host_hid_report_descriptor_fetch(UX_HOST_CLASS_HID *hid, UCHAR *buffer, ULONG size, ULONG *actual_length)
{
    UX_INTERFACE *interface = hid->ux_host_class_hid_interface;
    UX_DEVICE *device = interface->ux_interface_configuration->ux_configuration_device;

    return usbx_host_control_transfer(device,
        UX_REQUEST_IN | UX_REQUEST_TYPE_STANDARD | UX_REQUEST_TARGET_INTERFACE,
        UX_GET_DESCRIPTOR,
        (UX_HOST_CLASS_HID_REPORT_DESCRIPTOR << 8) | 0,
        interface->ux_interface_descriptor.bInterfaceNumber,
        buffer, size, actual_length);
}

