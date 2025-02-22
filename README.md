# SR71 USB Mouse Proxy

This codebase is designed for embedded systems to proxy HID packets butween a device and pc and if needed alter or add packets. Outside input is accepted in UDP format using an ethernet connection (similar to KMBox NET). All processing is done on the device, no drivers, api key or internet connection is ever required on either computer. This project is targeted for 2 different embedded hardware types, Arduino-like and MCX-based chips (beta). 

## Device requirements (ino build)

This tool is primarily tested on the teensy 4.1 chip, but it should work on any embedded device with the following features:

- 1 usb port capable of running in device mode (most chips can do it)
- 1 usb port capable of running in host mode (either integrated or host shield)
- On-device usb phy or support for a device like a wd5500 (Teensy, esp32, etc)
- OLED status display (any 4 pin SPI is fine)
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

## Quick-start

### Arduino-like devices (aka teensy)

This quickstart is geared toward the Teensy 4.1. Read the datasheet for other devices to adapt it.

#### Getting ready

You'll need the following tools and software for this build

- Teensyduino (or equivalet)
- Git
- Micro-usb cord
- USB cord matching your otg connection
- Build hardware (device, usb host, 5 pin otg cable, ethernet phy, ethernet cable)

#### Hardware

The following components need to be connected on the following pins (you can change the definitions with `#define`'s). 
> [!TIP]
> I recommend building on a bread board first and validating everything works before soldering
- External serial to TX/TX 8 (if used)
- USB host to the built-in teensy port (not sure about the arduino host, i think its spi 1)
- Ethernet phy to built in teensy port (not sure about arduino, check your datasheet)
- OLED to spi 1

#### Flashing

Start off by connecting your device to your flashing PC (ideally not your main pc but its fine either way probably). 

- Get your IDE ready and start a new project. Clone this repo and you can import all of the files from the `arduino` dir into your project. (skip doxyfile)
- Use a usb device descriptor tool (like `scripts/getUsbDescriptors.py` ) to pull your existing mouse descriptors. The product name, manufacturer name and serial should be added to `ardunio/teensy4_usb_descriptor.c` following the existing format. 
- Update the PID/VID values in your arduino libarary using either the included `usb_desc.h` (imported by default) or using your default avr lib file. Make sure to click verify in the Arduino IDE to recompile the hex before you load it. 
> [!WARNING]  
> Most loaders will no longer recognize your device automatically after this step. For example a teensy will need the flash button to be pressed before youre able to flash it again
- Connect your usb otg cable and ethernet phy as needed, and add the display to your spi port
- Flash your chip with `arduino/arduino_build.ino`, making sure to put the device usb port into keyboard/mouse/joystick mode and not in a mode that includes "serial". If desired a side serial device can be added and used on a seperate PC.
- Connect your device in the normal use mode (otg to mouse, teensy to main pc, ethernet to route or what ever.)
> [!TIP]
> You should plug your device into your second PC first and validate that the descriptors look as expected
- Tada!

# Releases

Release packages are available but generating your own from your own descriptors is recommended.

# Documents

You can find the C documentation [here](/docs/html/annotated.html)
