#include "hardware/spi.h"
#include <string.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/uart.h"
#include "adafruit_max3421e.h"
#include "tusb_config.h"
#include "tusb.h"

// Debug UART Configuration
#define DEBUG_UART_ID uart0
#define DEBUG_UART_TX_PIN 0
#define DEBUG_UART_RX_PIN 1
#define DEBUG_UART_BAUDRATE 115200

// Descriptor buffer sizes
#define DEVICE_DESC_BUF_SIZE   64
#define CONFIG_DESC_BUF_SIZE   256
#define STRING_DESC_BUF_SIZE   128
#define MAX_STRING_LEN         64

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

// Global state enum
typedef enum {
    STATE_WAIT_FOR_DEVICE,
    STATE_ENUMERATE,
    STATE_PROXY_READY,
    STATE_ERROR
} proxy_state_t;

// Descriptor buffers
static uint8_t device_descriptor[DEVICE_DESC_BUF_SIZE];
static uint8_t config_descriptor[CONFIG_DESC_BUF_SIZE];
static size_t device_desc_len = 0;
static size_t config_desc_len = 0;

// String descriptors (stored as UTF-8)
static char manufacturer[MAX_STRING_LEN] = "Unknown";
static char product[MAX_STRING_LEN] = "USB Proxy";
static char serial[MAX_STRING_LEN] = "0001";

// Temporary buffer for string processing (UTF-16LE data)
static uint16_t string_buffer[STRING_DESC_BUF_SIZE / 2];

//-----------------------------------------------------------------
// Debugging Utilities (UART Only)
//-----------------------------------------------------------------

void init_debug_uart() {
    uart_init(DEBUG_UART_ID, DEBUG_UART_BAUDRATE);
    gpio_set_function(DEBUG_UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(DEBUG_UART_RX_PIN, GPIO_FUNC_UART);
    
    // Enable FIFOs
    uart_set_fifo_enabled(DEBUG_UART_ID, true);
    
    uart_puts(DEBUG_UART_ID, "\n\n=== USB Proxy Debug Console ===\n");
    uart_puts(DEBUG_UART_ID, "UART initialized at 115200 baud\n");
}

void debug_print(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    // Print to UART only (not USB)
    uart_puts(DEBUG_UART_ID, buffer);
}

void print_hex_dump(const char* label, const uint8_t* data, size_t len) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "[HEX] %s (%d bytes):\n", label, len);
    uart_puts(DEBUG_UART_ID, buffer);
    
    for (size_t i = 0; i < len; i++) {
        snprintf(buffer, sizeof(buffer), "%02x ", data[i]);
        uart_puts(DEBUG_UART_ID, buffer);
        if ((i + 1) % 16 == 0 || i == len - 1) {
            uart_puts(DEBUG_UART_ID, "\n");
        }
    }
}

//-----------------------------------------------------------------
// Helper Functions
//-----------------------------------------------------------------

// Initialize the LED (if defined)
void pico_led_init(void) {
#ifdef PICO_DEFAULT_LED_PIN
    debug_print("Initializing LED on pin %d\n", PICO_DEFAULT_LED_PIN);
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif
}

// Set the LED state
void pico_set_led(bool led_on) {
#ifdef PICO_DEFAULT_LED_PIN
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#endif
}

// Convert UTF-16LE string descriptor to UTF-8
static void utf16_to_utf8(const uint16_t *utf16, size_t utf16_len, char *utf8, size_t utf8_size) {
    size_t i, j = 0;
    // Skip the header (first 16-bit word contains length and type)
    for (i = 1; i < utf16_len && j < utf8_size - 1; i++) {
        uint16_t c = utf16[i];
        if (c < 0x80) {
            utf8[j++] = (char)c;
        } else if (c < 0x800) {
            if (j + 1 >= utf8_size) break;
            utf8[j++] = 0xC0 | (c >> 6);
            utf8[j++] = 0x80 | (c & 0x3F);
        } else {
            if (j + 2 >= utf8_size) break;
            utf8[j++] = 0xE0 | (c >> 12);
            utf8[j++] = 0x80 | ((c >> 6) & 0x3F);
            utf8[j++] = 0x80 | (c & 0x3F);
        }
    }
    utf8[j] = '\0';
}

// Process string descriptor from device: read from MAX3421E, convert UTF-16LE to UTF-8.
static bool process_string_descriptor(Adafruit_MAX3421E &max3421e, uint8_t index, char *buffer, size_t buf_size) {
    debug_print("Reading string descriptor %d...\n", index);
    
    size_t bytes_read = 0;
    max3421e_err_t err = max3421e.readStringDescriptor(index, string_buffer, sizeof(string_buffer), &bytes_read);
    if (err != MAX3421E_OK) {
        debug_print("Error reading string descriptor %d: %d\n", index, err);
        return false;
    }
    if (bytes_read < 2) {
        debug_print("String descriptor %d too short: %d bytes\n", index, bytes_read);
        return false;
    }
    
    // First 16-bit word holds the total length (in bytes)
    uint8_t desc_len = string_buffer[0] & 0xFF;
    if (desc_len < 2 || ((desc_len - 2) / 2) > MAX_STRING_LEN) {
        debug_print("Invalid string descriptor length %d for index %d\n", desc_len, index);
        return false;
    }
    
    utf16_to_utf8(string_buffer, (desc_len) / 2, buffer, buf_size);
    debug_print("String descriptor %d: '%s'\n", index, buffer);
    return true;
}

//-----------------------------------------------------------------
// Main Application
//-----------------------------------------------------------------
int main() {
    // Initialize only UART for debugging (not USB stdio)
    init_debug_uart();
    
    debug_print("\nRP2040 USB Proxy Initializing...\n");
    debug_print("Build date: %s %s\n", __DATE__, __TIME__);
    
    pico_led_init();

    // Initialize MAX3421E instance with SPI port, CS, INT, and RESET pins.
    debug_print("Initializing MAX3421E...\n");
    debug_print("  SPI Port: %d\n", SPI_PORT);
    debug_print("  CS Pin: %d\n", PIN_CS);
    debug_print("  INT Pin: %d\n", MAX3421E_INT_PIN);
    debug_print("  RESET Pin: %d\n", MAX3421E_RESET_PIN);
    
    Adafruit_MAX3421E max3421e(SPI_PORT, PIN_CS, MAX3421E_INT_PIN, MAX3421E_RESET_PIN);
    if (max3421e.begin() != MAX3421E_OK) {
        debug_print("MAX3421E initialization failed!\n");
        return 1;
    }
    debug_print("MAX3421E initialized successfully\n");

    proxy_state_t state = STATE_WAIT_FOR_DEVICE;
    absolute_time_t last_state_change = get_absolute_time();

    // Initialize the reset pin for the MAX3421E (if used for hardware reset)
    debug_print("Performing MAX3421E hardware reset...\n");
    gpio_init(MAX3421E_RESET_PIN);
    gpio_set_dir(MAX3421E_RESET_PIN, GPIO_OUT);
    gpio_put(MAX3421E_RESET_PIN, 1); // Keep reset inactive (high)
    // Perform a hardware reset: drive low, hold, then drive high
    gpio_put(MAX3421E_RESET_PIN, 0);
    sleep_ms(50);
    gpio_put(MAX3421E_RESET_PIN, 1);
    sleep_ms(50);
    debug_print("Hardware reset complete\n");

    debug_print("Entering main loop...\n");

    while (true) {
        max3421e.task();

        switch (state) {
            case STATE_WAIT_FOR_DEVICE:
                if (max3421e.deviceConnected()) {
                    debug_print("Device connected detected, starting enumeration...\n");
                    state = STATE_ENUMERATE;
                    last_state_change = get_absolute_time();
                } else {
                    static absolute_time_t last_print = 0;
                    if (absolute_time_diff_us(last_print, get_absolute_time()) > 1000000) {
                        debug_print("Waiting for device...\n");
                        last_print = get_absolute_time();
                    }
                }
                break;

            case STATE_ENUMERATE: {
                // Timeout if enumeration takes too long (5 seconds)
                int64_t elapsed = absolute_time_diff_us(last_state_change, get_absolute_time());
                if (elapsed > 5000000) {
                    debug_print("Enumeration timeout after %d ms\n", elapsed / 1000);
                    state = STATE_ERROR;
                    break;
                }

                debug_print("Starting enumeration process...\n");

                // Step 1: Reset the USB bus via MAX3421E
                debug_print("Resetting USB bus...\n");
                if (max3421e.resetBus() != MAX3421E_OK) {
                    debug_print("Bus reset failed!\n");
                    state = STATE_ERROR;
                    break;
                }
                debug_print("Bus reset successful\n");

                // Step 2: Read device descriptor
                debug_print("Reading device descriptor...\n");
                if (max3421e.readDeviceDescriptor(device_descriptor, sizeof(device_descriptor), &device_desc_len) != MAX3421E_OK) {
                    debug_print("Failed to read device descriptor!\n");
                    state = STATE_ERROR;
                    break;
                }
                print_hex_dump("Device Descriptor", device_descriptor, device_desc_len);

                // Step 3: Read configuration descriptor
                debug_print("Reading configuration descriptor...\n");
                if (max3421e.readConfigDescriptor(config_descriptor, sizeof(config_descriptor), &config_desc_len) != MAX3421E_OK) {
                    debug_print("Failed to read configuration descriptor!\n");
                    state = STATE_ERROR;
                    break;
                }
                print_hex_dump("Config Descriptor", config_descriptor, config_desc_len);

                // Step 4: Read string descriptors (best effort)
                debug_print("Reading string descriptors...\n");
                if (!process_string_descriptor(max3421e, 1, manufacturer, sizeof(manufacturer))) {
                    strcpy(manufacturer, "Unknown");
                }
                if (!process_string_descriptor(max3421e, 2, product, sizeof(product))) {
                    strcpy(product, "Unknown");
                }
                if (!process_string_descriptor(max3421e, 3, serial, sizeof(serial))) {
                    strcpy(serial, "0000");
                }

                debug_print("Enumeration complete:\n");
                debug_print("  Manufacturer: %s\n", manufacturer);
                debug_print("  Product: %s\n", product);
                debug_print("  Serial: %s\n", serial);

                // Initialize TinyUSB device stack
                debug_print("Initializing TinyUSB device stack...\n");
                tusb_init();
                state = STATE_PROXY_READY;
                debug_print("Proxy ready for operation\n");
                break;
            }

            case STATE_PROXY_READY:
                // Process TinyUSB tasks for device mode
                tud_task();
                
                // If the device is disconnected, go back to waiting for a device.
                if (!max3421e.deviceConnected()) {
                    debug_print("Device disconnected, returning to wait state\n");
                    state = STATE_WAIT_FOR_DEVICE;
                    device_desc_len = 0;
                    config_desc_len = 0;
                }
                break;

            case STATE_ERROR:
                debug_print("Error state entered, attempting recovery...\n");
                // Simple error recovery: wait and retry
                sleep_ms(1000);
                state = STATE_WAIT_FOR_DEVICE;
                break;
        }

        sleep_ms(10);
    }

    return 0;
}

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len) {
    (void)desc_report;
    (void)desc_len;
    const char* protocol_str[] = { "None", "Keyboard", "Mouse" };
    uint8_t itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);
    debug_print("[%04x:%04x][%u] HID Interface%u mounted, Protocol = %s\n",
           vid, pid, dev_addr, instance, protocol_str[itf_protocol]);
    if (itf_protocol == HID_ITF_PROTOCOL_KEYBOARD || itf_protocol == HID_ITF_PROTOCOL_MOUSE) {
        if (!tuh_hid_receive_report(dev_addr, instance)) {
            debug_print("Failed to receive report\n");
        }
    }
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    debug_print("[%u] HID Interface%u is unmounted\n", dev_addr, instance);
}

void tud_mount_cb(void) {
    debug_print("Device mounted on host\n");
    pico_set_led(true);
    sleep_ms(LED_DELAY_MS);
    pico_set_led(false);
    sleep_ms(LED_DELAY_MS);
}

void tud_umount_cb(void) {
    debug_print("Device unmounted from host\n");
    pico_set_led(true);
    sleep_ms(LED_DELAY_MS);
    pico_set_led(false);
    sleep_ms(LED_DELAY_MS);
}

void tud_suspend_cb(bool remote_wakeup_en) {
    (void) remote_wakeup_en;
    debug_print("Device suspended by host\n");
    pico_set_led(true);
    sleep_ms(LED_DELAY_MS);
    pico_set_led(false);
    sleep_ms(LED_DELAY_MS);
}

void tud_resume_cb(void) {
    debug_print("Device resumed by host\n");
    pico_set_led(true);
    sleep_ms(LED_DELAY_MS);
    pico_set_led(false);
    sleep_ms(LED_DELAY_MS);
}