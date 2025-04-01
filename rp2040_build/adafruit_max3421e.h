#ifndef ADAFRUIT_MAX3421E_H
#define ADAFRUIT_MAX3421E_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <string.h>
#include <stdlib.h>
#define MAX3421E_EP0_FIFO     0x00
#define MAX3421E_EP1_FIFO     0x01
#define MAX3421E_EP2_FIFO     0x02
#define MAX3421E_EP3_FIFO     0x03
#define MAX3421E_EP0_BC       0x04
#define MAX3421E_EP1_BC       0x05
#define MAX3421E_EP2_BC       0x06
#define MAX3421E_EP3_BC       0x07
#define MAX3421E_EP_STAT      0x08
#define MAX3421E_EP_IRQ       0x09
#define MAX3421E_USBCTL       0x0A
#define MAX3421E_CPUCTL       0x0B
#define MAX3421E_PINCTL       0x0C
#define MAX3421E_REVISION     0x0D
#define MAX3421E_FNADDR       0x0E
#define MAX3421E_IOPINS       0x0F
#define MAX3421E_GPINIRQ      0x10
#define MAX3421E_GPINEN       0x11
#define MAX3421E_GPINPOL      0x12
#define MAX3421E_HIRQ         0x13
#define MAX3421E_HIEN         0x14
#define MAX3421E_MODE         0x15
#define MAX3421E_PERADDR      0x16
#define MAX3421E_HCTL         0x17
#define MAX3421E_HXFR         0x18
#define MAX3421E_HRSL         0x19
#define MAX3421E_HCDATA       0x1A
#define MAX3421E_HCBC         0x1B
#define MAX3421E_USBIRQ       0x1C
#define MAX3421E_USBIEN       0x1D
#define MAX3421E_USBSTAT      0x1E
#define MAX3421E_FRAME        0x1F
// SPI Defines
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define MAX3421E_INT_PIN  9
#define MAX3421E_RESET_PIN 14  // Example – use any free GPIO pin

// USBCTL bits
#define USBCTL_CHIPRES        (1 << 0)
#define USBCTL_BUSRST         (1 << 5)

// MODE bits
#define MODE_HOST             (1 << 0)
#define MODE_LOWSPEED         (1 << 1)

// HIRQ bits
#define HIRQ_DONEI            (1 << 0)
#define HIRQ_RCVDAVI          (1 << 3)

// USBIRQ bits
#define USBIRQ_URESI          (1 << 0)
#define USBIRQ_URESDNI        (1 << 1)
#define USBIRD_SPEEDI         (1 << 2)
// Error codes
typedef enum {
    MAX3421E_OK = 0,
    MAX3421E_ERR_SPI,
    MAX3421E_ERR_TIMEOUT,
    MAX3421E_ERR_NOT_CONNECTED,
    MAX3421E_ERR_ENUMERATION,
    MAX3421E_ERR_DESCRIPTOR,
    MAX3421E_ERR_STATE
} max3421e_err_t;

// USB speeds
typedef enum {
    MAX3421E_LOW_SPEED = 0,
    MAX3421E_FULL_SPEED = 1
} max3421e_speed_t;

class Adafruit_MAX3421E {
public:
    // Constructor: spi_port is a pointer to the SPI instance,
    // cs_pin is the chip-select pin number, and int_pin is the interrupt pin.
    Adafruit_MAX3421E(spi_inst_t* spi_port, uint cs_pin, uint int_pin, uint reset_pin);    
    // Public API functions
    max3421e_err_t begin();
    void task();
    bool deviceConnected();
    max3421e_err_t resetBus();
    max3421e_err_t enumerateDevice();
    max3421e_err_t readDeviceDescriptor(uint8_t* buffer, size_t bufsize, size_t* bytes_read);
    max3421e_err_t readConfigDescriptor(uint8_t* buffer, size_t bufsize, size_t* bytes_read);
    max3421e_err_t readStringDescriptor(uint8_t index, uint16_t* buffer, size_t bufsize, size_t* bytes_read);
    max3421e_speed_t deviceSpeed() const { return _device_speed; }
    
    max3421e_err_t controlTransfer(uint8_t bmRequestType, uint8_t bRequest, 
                                  uint16_t wValue, uint16_t wIndex, 
                                  uint8_t* data, uint16_t wLength);
    
    max3421e_err_t bulkTransfer(uint8_t ep_addr, uint8_t* buffer, uint16_t length);
    max3421e_err_t interruptTransfer(uint8_t ep_addr, uint8_t* buffer, uint16_t length);
    
    // Register operations
    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);
    
private:
    spi_inst_t* _spi; // SPI instance pointer
    uint _cs_pin;
    uint _int_pin;
    uint _reset_pin;  // Added
    max3421e_speed_t _device_speed;  // Added
    bool _initialized;
    bool _device_connected;

    // FIFO operations
    max3421e_err_t writeFIFO(const uint8_t* data, size_t len, size_t* bytes_written);
    max3421e_err_t readFIFO(uint8_t* data, size_t len, size_t* bytes_read);
    
    // Helper functions
    max3421e_err_t waitForInterrupt(uint8_t int_bit, uint32_t timeout_ms);
    max3421e_err_t sendSetupPacket(const uint8_t* setup_pkt);
};

#endif // ADAFRUIT_MAX3421E_H
