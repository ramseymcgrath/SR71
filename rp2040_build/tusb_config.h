#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

// Defined by compiler flags for flexibility
#ifndef CFG_TUSB_MCU
#define CFG_TUSB_MCU          OPT_MCU_RP2040
#endif
// tusb_config.h
#ifndef CFG_TUSB_RHPORT0_MODE
#define CFG_TUSB_RHPORT0_MODE   OPT_MODE_DEVICE
#endif

#ifndef CFG_TUSB_RHPORT1_MODE
#define CFG_TUSB_RHPORT1_MODE   OPT_MODE_HOST
#endif

#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG        0
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS           OPT_OS_PICO
#endif

// Enable device stack (we're acting as a USB device to the host computer)
#define CFG_TUD_ENABLED       1

// Enable host stack with MAX3421E (we're acting as a USB host to the connected device)
#define CFG_TUH_ENABLED       1
#define CFG_TUH_MAX3421       1

// Configuration
#define MAX3421E_CS_PIN       PIN_CS
#define MAX3421E_INT_PIN       9
#define MAX3421E_POLL_MS      10
// TinyUSB Board Configuration
#define BOARD_TUH_RHPORT       1  // MAX3421 is on port 1
#define BOARD_TUH_MAX3421_INT  MAX3421E_INT_PIN

//--------------------------------------------------------------------
// DEVICE CONFIGURATION
//--------------------------------------------------------------------

#define CFG_TUD_CDC           0  // Enable CDC (serial port)
#define CFG_TUD_MSC           0  // Enable Mass Storage (if proxying storage devices)
#define CFG_TUD_HID           1  // Enable HID (if proxying keyboards/mice)
#define CFG_TUD_MIDI          0  // Disable MIDI (unless needed)
#define CFG_TUD_VENDOR        0  // Disable Vendor (unless needed)

// CDC FIFO size
#define CFG_TUD_CDC_RX_BUFSIZE 256
#define CFG_TUD_CDC_TX_BUFSIZE 256

// MSC FIFO size
#define CFG_TUD_MSC_BUFSIZE   512

// HID buffer size
#define CFG_TUD_HID_BUFSIZE   64

//--------------------------------------------------------------------
// HOST CONFIGURATION
//--------------------------------------------------------------------

// MAX3421E Host Controller configuration
#define CFG_TUH_DEVICE_MAX    1  // Only 1 device supported via MAX3421E
#define CFG_TUH_ENUMERATION_BUFSIZE 256

// Host driver configuration
#define CFG_TUH_HUB           0  // No hub support needed
#define CFG_TUH_HID           4  // Support multiple HID devices
#define CFG_TUH_MSC           0  // Support 1 Mass Storage device
#define CFG_TUH_CDC           0  // Support 1 CDC device

// HID buffer size
#define CFG_TUH_HID_BUFSIZE   64

#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */