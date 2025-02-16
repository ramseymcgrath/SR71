# SR71 USB Mouse Proxy

This codebase is designed for embedded systems to proxy HID packets butween a device and pc and if needed alter or add packets. Outside input is accepted in UDP format using an ethernet connection (similar to KMBox NET). All processing is done on the device, no drivers, api key or internet connection is ever required on either computer. 

## Device requirements

This tool is primarily tested on the teensy 4.1 chip, but it should work on any embedded device with the following features:

- 1 usb port capable of running in device mode (most chips can do it)
- 1 usb port capable of running in host mode (either integrated or host shield)
- On-device usb phy or support for a device like a wd5500 (Teensy, esp32, etc)
- TFT status display (ILI9341 recommended but any tft should work)
> [!TIP]
> Using a seperate serial device is supported. Use it to debug or read logs without adding a com port to the main PC.

## Protocol Support

This is a drop-in replacement that can be used with software (YOLO-based, DMA-based, etc) that supports one of the following protocols over udp

- KMBoxNET
- Others (WIP)

## Features

- Custom USB descriptors are added to the device-mode output (ids are user-provided)
- Built-in protection against all ekknod detection-vectors
- Api key authentication (an id is generated on your device for security, no need to purchase anything)
- Buffering for both HID and UDP inputs to safely handle mismatches in speed
- Smoothing for USB output to conceal rapid automated movements
- Bezier curve commands
- Auto-move commands
- Windows/OSX/Linux Support
- On-screen status indicators

## Quick-start (for devices that support ino)

The following steps are geared mainly for the teensy build and assumes your device will be connected to an ethernet router. If the overlay pc and device are both wired to the same switch latecy is generally fine, but if the overlay PC is using wifi you should wire the device to the open ethernet port.

- Use a usb device descriptor tool (like `scripts/getUsbDescriptors.py` ) to pull your existing mouse descriptors. The product name, manufacturer name and serial should be added to `ardunio/teensy4_usb_descriptor.c` following the existing format. 
- Update the PID/VID values in your arduino libarary (the avr for for a teensy 4.1 `/teensy/avr/cores/teensy3/usb_desc.h`). Make sure to click verify in the Arduino IDE to recompile the hex before you load it. 
> [!WARNING]  
> Most loaders will no longer recognize your device automatically after this step. For example a teensy will need the flash button to be pressed before youre able to flash it again. 
- Connect your usb otg cable and ethernet phy as needed, and add the display to your spi port
- Flash your chip with `arduino/arduino_build.ino`, making sure to put the device usb port into keyboard/mouse/joystick mode and not in a serial mode to prevent COM ports from being added. A serial device can be added to a seperate port and plugged into a seperate PC if the feature is needed.
- Connect your mouse to the usb otg plug, ethernet wire to your switch, and usb cord to a PC
> [!TIP]
> You should plug your device into your second PC first and validate that the descriptors look as expected
- Plug the USB port on your teensy into your gaming PC and connect your overlay pc to the ip and port shown on the device (or in the serial console)
- Tada!

# Documents

You can find the C documentation [here](/docs/html/annotated.html)
