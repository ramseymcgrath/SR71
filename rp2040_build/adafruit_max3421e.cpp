#include "adafruit_max3421e.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include <string.h>

// Register definitions

Adafruit_MAX3421E::Adafruit_MAX3421E(spi_inst_t* spi_port, uint cs_pin, uint int_pin, uint reset_pin)
    : _spi(spi_port), _cs_pin(cs_pin), _int_pin(int_pin), _reset_pin(reset_pin),
      _initialized(false), _device_connected(false), _device_speed(MAX3421E_FULL_SPEED) {
    // Initialize reset pin only (SPI gets initialized in begin())
    gpio_init(_reset_pin);
    gpio_set_dir(_reset_pin, GPIO_OUT);
    gpio_put(_reset_pin, 1);  // Active low, start inactive
}

max3421e_err_t Adafruit_MAX3421E::begin() {
    if (_initialized) return MAX3421E_OK;

    // Initialize SPI pins
    uint baudrate = 1000 * 1000; // 1 MHz
    uint sck_pin, mosi_pin, miso_pin;
    
    if (_spi == spi0) {
        sck_pin = 18; mosi_pin = 19; miso_pin = 16; // Default SPI0 pins
    } else {
        sck_pin = 10; mosi_pin = 11; miso_pin = 8;  // Default SPI1 pins
    }

    spi_init(_spi, baudrate);
    gpio_set_function(sck_pin, GPIO_FUNC_SPI);
    gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
    gpio_set_function(miso_pin, GPIO_FUNC_SPI);

    // Initialize CS pin
    gpio_init(_cs_pin);
    gpio_set_dir(_cs_pin, GPIO_OUT);
    gpio_put(_cs_pin, 1); // Active low

    // Initialize INT pin
    gpio_init(_int_pin);
    gpio_set_dir(_int_pin, GPIO_IN);
    gpio_pull_up(_int_pin); // INT is active low

    // Hardware reset sequence
    gpio_put(_reset_pin, 0);   // Assert reset (active low)
    sleep_ms(20);              // Hold for 20ms
    gpio_put(_reset_pin, 1);   // Release reset
    sleep_ms(100);             // Wait for stabilization

    // Verify communication
    uint8_t rev = readRegister(MAX3421E_REVISION);
    if (rev != 0x12 && rev != 0x13) {
        return MAX3421E_ERR_SPI;
    }

    // Configure host mode
    writeRegister(MAX3421E_MODE, MODE_HOST);
    writeRegister(MAX3421E_HIEN, HIRQ_DONEI | HIRQ_RCVDAVI);
    writeRegister(MAX3421E_USBIEN, USBIRQ_URESI | USBIRD_SPEEDI);

    _initialized = true;
    return MAX3421E_OK;
}

void Adafruit_MAX3421E::task() {
    if (!_initialized) return;

    // Check for USB events
    uint8_t usbirq = readRegister(MAX3421E_USBIRQ);
    if (usbirq & USBIRQ_URESI) {
        _device_connected = false;
        writeRegister(MAX3421E_USBIRQ, USBIRQ_URESI); // Clear interrupt
    }
    if (usbirq & USBIRD_SPEEDI) {
        _device_speed = (readRegister(MAX3421E_MODE) & MODE_LOWSPEED) ? 
                       MAX3421E_LOW_SPEED : MAX3421E_FULL_SPEED;
        writeRegister(MAX3421E_USBIRQ, USBIRD_SPEEDI); // Clear interrupt
    }
    if (usbirq & USBIRQ_URESDNI) {
        _device_connected = true;
        writeRegister(MAX3421E_USBIRQ, USBIRQ_URESDNI); // Clear interrupt
    }
}

bool Adafruit_MAX3421E::deviceConnected() {
    return _device_connected;
}

max3421e_err_t Adafruit_MAX3421E::resetBus() {
    if (!_initialized) return MAX3421E_ERR_STATE;

    writeRegister(MAX3421E_USBCTL, USBCTL_BUSRST);
    sleep_ms(50); // Reset duration
    writeRegister(MAX3421E_USBCTL, 0);
    sleep_ms(100); // Recovery time

    return waitForInterrupt(USBIRQ_URESDNI, 500);
}

max3421e_err_t Adafruit_MAX3421E::enumerateDevice() {
    if (!_device_connected) return MAX3421E_ERR_NOT_CONNECTED;

    writeRegister(MAX3421E_PERADDR, 0); // Default address

    uint8_t dev_desc[18];
    size_t bytes_read;
    max3421e_err_t err = readDeviceDescriptor(dev_desc, sizeof(dev_desc), &bytes_read);
    if (err != MAX3421E_OK || bytes_read != 18) {
        return MAX3421E_ERR_ENUMERATION;
    }

    return MAX3421E_OK;
}

max3421e_err_t Adafruit_MAX3421E::readDeviceDescriptor(uint8_t* buffer, size_t bufsize, size_t* bytes_read) {
    uint8_t setup_pkt[8] = {
        0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x12, 0x00
    };

    max3421e_err_t err = sendSetupPacket(setup_pkt);
    if (err != MAX3421E_OK) return err;

    return readFIFO(buffer, (bufsize < 18) ? bufsize : 18,  bytes_read);
}

max3421e_err_t Adafruit_MAX3421E::readConfigDescriptor(uint8_t* buffer, size_t bufsize, size_t* bytes_read) {
    uint8_t setup_pkt[8] = {
        0x80, 0x06, 0x00, 0x02, 0x00, 0x00, 0x09, 0x00
    };

    max3421e_err_t err = sendSetupPacket(setup_pkt);
    if (err != MAX3421E_OK) return err;

    uint8_t header[9];
    err = readFIFO(header, sizeof(header), bytes_read);
    if (err != MAX3421E_OK || *bytes_read != 9) return MAX3421E_ERR_DESCRIPTOR;

    uint16_t total_length = header[2] | (header[3] << 8);
    if (total_length > bufsize) total_length = bufsize;

    if (*bytes_read >= total_length) {
        memcpy(buffer, header, total_length);
        return MAX3421E_OK;
    }

    memcpy(buffer, header, 9);
    return readFIFO(buffer + 9, total_length - 9, bytes_read);
}

max3421e_err_t Adafruit_MAX3421E::readStringDescriptor(uint8_t index, uint16_t* buffer, size_t bufsize, size_t* bytes_read) {
    uint8_t setup_pkt[8] = {
        0x80, 0x06, 0x00, 0x03, index, 0x00, 0xFF, 0x00
    };

    max3421e_err_t err = sendSetupPacket(setup_pkt);
    if (err != MAX3421E_OK) return err;

    uint8_t header[2];
    err = readFIFO(header, sizeof(header), bytes_read);
    if (err != MAX3421E_OK || *bytes_read != 2) return MAX3421E_ERR_DESCRIPTOR;

    uint8_t length = header[0];
    if (header[1] != 0x03) return MAX3421E_ERR_DESCRIPTOR;

    uint8_t* byte_buf = reinterpret_cast<uint8_t*>(buffer);
    size_t to_read = (length > bufsize * 2) ? bufsize * 2 : length;
    
    err = readFIFO(byte_buf, to_read, bytes_read);
    *bytes_read += 2; // Include header
    
    return err;
}

uint8_t Adafruit_MAX3421E::readRegister(uint8_t reg) {
    uint8_t cmd = reg & 0x7F;
    uint8_t data = 0;
    
    gpio_put(_cs_pin, 0);
    spi_write_blocking(_spi, &cmd, 1);
    spi_read_blocking(_spi, 0, &data, 1);
    gpio_put(_cs_pin, 1);
    
    return data;
}

void Adafruit_MAX3421E::writeRegister(uint8_t reg, uint8_t value) {
    uint8_t cmd = reg | 0x80;
    uint8_t buf[2] = {cmd, value};
    
    gpio_put(_cs_pin, 0);
    spi_write_blocking(_spi, buf, 2);
    gpio_put(_cs_pin, 1);
}

max3421e_err_t Adafruit_MAX3421E::writeFIFO(const uint8_t* data, size_t len, size_t* bytes_written) {
    gpio_put(_cs_pin, 0);
    uint8_t cmd = MAX3421E_EP0_FIFO | 0x80;
    spi_write_blocking(_spi, &cmd, 1);
    spi_write_blocking(_spi, data, len);
    gpio_put(_cs_pin, 1);
    return MAX3421E_OK;
}

max3421e_err_t Adafruit_MAX3421E::readFIFO(uint8_t* data, size_t len, size_t* bytes_read) {
    gpio_put(_cs_pin, 0);
    uint8_t cmd = MAX3421E_EP0_FIFO & 0x7F;
    spi_write_blocking(_spi, &cmd, 1);
    *bytes_read = spi_read_blocking(_spi, 0, data, len);
    gpio_put(_cs_pin, 1);
    return (*bytes_read == len) ? MAX3421E_OK : MAX3421E_ERR_SPI;
}

max3421e_err_t Adafruit_MAX3421E::waitForInterrupt(uint8_t int_bit, uint32_t timeout_ms) {
    uint32_t start = to_ms_since_boot(get_absolute_time());
    
    while ((to_ms_since_boot(get_absolute_time()) - start) < timeout_ms) {
        uint8_t irq = readRegister(MAX3421E_USBIRQ);
        if (irq & int_bit) {
            writeRegister(MAX3421E_USBIRQ, int_bit);
            return MAX3421E_OK;
        }
        sleep_ms(1);
    }
    return MAX3421E_ERR_TIMEOUT;
}

max3421e_err_t Adafruit_MAX3421E::sendSetupPacket(const uint8_t* setup_pkt) {
    max3421e_err_t err = writeFIFO(setup_pkt, 8, nullptr);
    if (err != MAX3421E_OK) return err;
    
    writeRegister(MAX3421E_HCTL, 0x01); // HSNAK
    writeRegister(MAX3421E_HXFR, 0x10); // SETUP token
    
    return waitForInterrupt(HIRQ_DONEI, 100);
}

max3421e_err_t Adafruit_MAX3421E::controlTransfer(uint8_t bmRequestType, uint8_t bRequest, 
    uint16_t wValue, uint16_t wIndex, uint8_t* data, uint16_t wLength) {
    uint8_t setup_pkt[8] = {
        bmRequestType, bRequest,
        (uint8_t)(wValue & 0xFF), (uint8_t)(wValue >> 8),
        (uint8_t)(wIndex & 0xFF), (uint8_t)(wIndex >> 8),
        (uint8_t)(wLength & 0xFF), (uint8_t)(wLength >> 8)
    };

    if (sendSetupPacket(setup_pkt) != MAX3421E_OK) {
        return MAX3421E_ERR_ENUMERATION;
    }

    if (wLength > 0) {
        size_t bytes_read;
        if (bmRequestType & 0x80) {
            return readFIFO(data, wLength, &bytes_read);
        } else {
            return writeFIFO(data, wLength, &bytes_read);
        }
    }

    return MAX3421E_OK;
}

max3421e_err_t Adafruit_MAX3421E::bulkTransfer(uint8_t ep_addr, uint8_t* buffer, uint16_t length) {
    size_t bytes_read;
    if (ep_addr & 0x80) {
        return readFIFO(buffer, length, &bytes_read);
    } else {
        return writeFIFO(buffer, length, nullptr);
    }
}

