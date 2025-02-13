# SR71 USB Mouse Proxy

This codebase is designed for embedded systems to proxy HID packets butween a device and pc and if needed alter or add packets. Outside input is accepted in UDP format using an ethernet connection (similar to KMBox NET). All processing is done on the device, no drivers, api key or internet connection is ever required on either computer. 

## Device requirements

This tool is primarily tested on the teensy 4.1 chip, but it should work on any embedded device with the following features:

- 1 usb port capable of running in device mode (most chips can do it)
- 1 usb port capable of running in host mode (either integrated or host shield)
- On-device usb phy or support for a device like a wd5500 (Teensy, esp32, etc)
- SPI or tft display (on device or added)
- Optionally an external serial device (cp2102, etc) can be connected on Serial1 if supported by the device for debugging/logging without modifying the usb device

## Protocol Support

This is a drop-in replacement that can be used with software (YOLO-based, DMA-based, etc) that supports one of the following protocols over udp

- KMBoxNET
- Others (WIP)

## Features

- Custom USB descriptors are added to the device-mode output (user-provided)
- Built-in protection against all ekknod detection-vectors
- Api key authentication (an id is generated on your device for security, no need to purchase anything)
- Buffering for both HID and UDP inputs to safely handle mismatches in speed
- Smoothing for USB output to conceal rapid automated movements
- Bezier curve commands
- Auto-move commands
- Support for all operating systems

## Quick-start (for devices that support ino)

The following steps are geared mainly for the teensy build and assumes your device will be connected to an ethernet router. If the overlay pc and device are both wired to the same switch latecy is generally fine, but if the overlay PC is using wifi you should wire the device to the open ethernet port.

- Use a usb device descriptor tool (like mac-hid-dump) to get the vendor name, device name, and hid/vid. Add those to `ardunio/teensy4_usb_descriptor.c` following the existing format
- Connect your usb otg cable and ethernet phy as needed, and add the display to your spi port
- Flash your chip with `arduino/arduino_build.ino`, making sure to put the device usb port into keyboard/mouse/joystick mode
- Connect your mouse to the usb otg plug, ethernet wire to your switch, and usb cord to a PC (I recommend test on the overlay pc first)
- After you've tested a coupld mouse movements and confirmed the device name in your device manager (or any OS), swap the device plug to your gaming PC and connect your overlay pc to the device ip and id number shown on the display
- Tada!