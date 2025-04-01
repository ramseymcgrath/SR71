#include "tusb.h"
#include "pico/stdlib.h"
#include "adafruit_max3421e.h"

//--------------------------------------------------------------------
// Device Descriptor
//--------------------------------------------------------------------
uint8_t const desc_device[] = {
    // Size, Type, USB Version, Class, Subclass, Protocol, EP0 Size
    18, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40,
    // Vendor ID, Product ID, Device Version
    0xC0, 0x16, 0xDC, 0x27, 0x00, 0x01,
    // Strings, Configurations
    0x01, 0x02, 0x03, 0x01
};

//--------------------------------------------------------------------
// Configuration Descriptor
//--------------------------------------------------------------------
uint8_t const desc_configuration[] = {
    // Config header
    9, 0x02, 32, 0, 1, 1, 0, 0x80, 50,
    
    // Interface Association (for CDC)
    8, 0x0B, 0, 2, 0x02, 0x02, 0x00, 0,
    
    // CDC Control Interface
    9, 0x04, 0, 0, 1, 0x02, 0x02, 0x01, 0,
    
    // CDC Header
    5, 0x24, 0x00, 0x10, 0x01,
    
    // CDC Call Management
    5, 0x24, 0x01, 0x00, 0x01,
    
    // CDC ACM
    4, 0x24, 0x02, 0x02,
    
    // CDC Union
    5, 0x24, 0x06, 0x00, 0x01,
    
    // Endpoint Notification
    7, 0x05, 0x81, 0x03, 0x08, 0x00, 0xFF,
    
    // CDC Data Interface
    9, 0x04, 1, 0, 2, 0x0A, 0x00, 0x00, 0,
    
    // Endpoint In
    7, 0x05, 0x82, 0x02, 0x40, 0x00, 0x00,
    
    // Endpoint Out
    7, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00
};

//--------------------------------------------------------------------
// String Descriptors
//--------------------------------------------------------------------
static char const* string_desc_arr[] = {
    (const char[]) { 0x09, 0x04 }, // 0: Supported language is English (0x0409)
    "Raspberry Pi",                // 1: Manufacturer
    "Pico MAX3421E Host",          // 2: Product
    "123456",                      // 3: Serial
    "MAX3421E CDC",                // 4: CDC Interface
    "MAX3421E Data"                // 5: CDC Data Interface
};

//--------------------------------------------------------------------
// TinyUSB Callbacks
//--------------------------------------------------------------------
uint8_t const* tud_descriptor_device_cb(void) {
    return desc_device;
}

uint8_t const* tud_descriptor_configuration_cb(uint8_t index) {
    (void)index; // for multiple configurations
    return desc_configuration;
}

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;
    static uint16_t desc_str[32];
    uint8_t chr_count;

    if (index == 0) {
        memcpy(&desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else {
        if (!(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0]))) return NULL;

        const char* str = string_desc_arr[index];
        chr_count = strlen(str);
        if (chr_count > 31) chr_count = 31;

        // Convert ASCII string into UTF-16
        for (uint8_t i = 0; i < chr_count; i++) {
            desc_str[1 + i] = str[i];
        }
    }

    // First byte is length (including header), second byte is string type
    desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);
    return desc_str;
}

//--------------------------------------------------------------------
// HID Report Descriptor (Example for simple keyboard)
//--------------------------------------------------------------------
uint8_t const hid_report_descriptor[] = {
    0x05, 0x01,  // Usage Page (Generic Desktop)
    0x09, 0x06,  // Usage (Keyboard)
    0xA1, 0x01,  // Collection (Application)
    0x05, 0x07,  // Usage Page (Key Codes)
    0x19, 0xE0,  // Usage Minimum (224)
    0x29, 0xE7,  // Usage Maximum (231)
    0x15, 0x00,  // Logical Minimum (0)
    0x25, 0x01,  // Logical Maximum (1)
    0x75, 0x01,  // Report Size (1)
    0x95, 0x08,  // Report Count (8)
    0x81, 0x02,  // Input (Data, Variable, Absolute)
    0x95, 0x01,  // Report Count (1)
    0x75, 0x08,  // Report Size (8)
    0x81, 0x01,  // Input (Constant)
    0x95, 0x05,  // Report Count (5)
    0x75, 0x01,  // Report Size (1)
    0x05, 0x08,  // Usage Page (LEDs)
    0x19, 0x01,  // Usage Minimum (1)
    0x29, 0x05,  // Usage Maximum (5)
    0x91, 0x02,  // Output (Data, Variable, Absolute)
    0x95, 0x01,  // Report Count (1)
    0x75, 0x03,  // Report Size (3)
    0x91, 0x01,  // Output (Constant)
    0x95, 0x06,  // Report Count (6)
    0x75, 0x08,  // Report Size (8)
    0x15, 0x00,  // Logical Minimum (0)
    0x25, 0x65,  // Logical Maximum (101)
    0x05, 0x07,  // Usage Page (Key Codes)
    0x19, 0x00,  // Usage Minimum (0)
    0x29, 0x65,  // Usage Maximum (101)
    0x81, 0x00,  // Input (Data, Array)
    0xC0         // End Collection
};

//--------------------------------------------------------------------
// HID Callbacks
//--------------------------------------------------------------------
// Invoked when received GET HID REPORT DESCRIPTOR request
uint8_t const* tud_hid_descriptor_report_cb(uint8_t instance) {
    return hid_report_descriptor;
}

// Invoked when received GET_REPORT control request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, 
                              hid_report_type_t report_type, uint8_t* buffer, 
                              uint16_t reqlen) {
    // TODO: Implement if needed
    return 0;
}

// Invoked when received SET_REPORT control request
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, 
                          hid_report_type_t report_type, uint8_t const* buffer, 
                          uint16_t bufsize) {
    // TODO: Implement if needed
}


//--------------------------------------------------------------------
// MSC Callbacks
//--------------------------------------------------------------------

// Test Unit Ready callback
bool tud_msc_test_unit_ready_cb(uint8_t lun) {
    return true; // Ready
}

//--------------------------------------------------------------------
// HID Host Callbacks
//--------------------------------------------------------------------

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, 
                               uint8_t const* report, uint16_t len) {
    // TODO: Handle received HID reports
}

int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], 
    void* buffer, uint16_t bufsize) {
// TODO: Implement SCSI commands
return 0;
}
