#include "tusb.h"
#include "adafruit_max3421e.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include <string.h>
// Create a static instance of the MAX3421E driver.
// Using SPI0, CS=17, INT=9, RESET=12.
static Adafruit_MAX3421E max3421(spi0, 17, 9, 12); 

//--------------------------------------------------------------------
// HCD Implementation
//--------------------------------------------------------------------
extern "C" {

bool hcd_init(uint8_t rhport) {
    return max3421.begin() == MAX3421E_OK;
}

void hcd_int_enable(uint8_t rhport) {
    // Enable interrupts for the MAX3421E.
    max3421.writeRegister(MAX3421E_USBIEN, USBIRQ_URESI | USBIRD_SPEEDI);
}

void hcd_int_disable(uint8_t rhport) {
    // Disable interrupts for the MAX3421E.
    max3421.writeRegister(MAX3421E_USBIEN, 0);
}

bool hcd_port_connect_status(uint8_t rhport) {
    
    return max3421.deviceConnected();
}

tusb_speed_t hcd_port_speed_get(uint8_t rhport) {
    // Ensure your class has an accessor 'deviceSpeed()'
    return (max3421.deviceSpeed() == MAX3421E_FULL_SPEED) ? TUSB_SPEED_FULL : TUSB_SPEED_LOW;
}

void hcd_port_reset(uint8_t rhport) {
    // Reset the USB bus using the MAX3421E.
    max3421.resetBus();
}

void hcd_port_reset_end(uint8_t rhport) {
    // No action required here; reset completion is handled internally.
}

bool hcd_edpt_open(uint8_t rhport, uint8_t dev_addr, tusb_desc_endpoint_t const *ep_desc) {
    // Endpoint opening is handled internally by the MAX3421E.
    return true;
}

bool hcd_setup_send(uint8_t rhport, uint8_t dev_addr, uint8_t const *setup_pkt) {
    // Construct and send a control transfer via the MAX3421E.
    // The setup packet format is assumed to follow the USB spec.
    return max3421.controlTransfer(
        setup_pkt[0],                 // bmRequestType
        setup_pkt[1],                 // bRequest
        setup_pkt[2] | (setup_pkt[3] << 8), // wValue
        setup_pkt[4] | (setup_pkt[5] << 8), // wIndex
        NULL,                         // No data stage (for IN or OUT with no data)
        setup_pkt[6] | (setup_pkt[7] << 8)  // wLength
    ) == MAX3421E_OK;
}

bool hcd_edpt_xfer(uint8_t rhport, uint8_t dev_addr, uint8_t ep_addr, uint8_t *buffer, uint16_t total_bytes) {
    // For bulk transfers, use the MAX3421E bulkTransfer.
    return max3421.bulkTransfer(ep_addr, buffer, total_bytes) == MAX3421E_OK;
}

bool hcd_edpt_abort_xfer(uint8_t rhport, uint8_t dev_addr, uint8_t ep_addr) {
    // Abort transfer is not implemented in the basic MAX3421E driver.
    return false;
}

void hcd_device_close(uint8_t rhport, uint8_t dev_addr) {
    // Device disconnection is handled internally by the MAX3421E.
}

} // extern "C"
