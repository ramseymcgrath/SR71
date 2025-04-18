/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

// This header is NOT meant to be included when compiling
// user sketches in Arduino.  The low-level functions
// provided by usb_dev.c are meant to be called only by
// code which provides higher-level interfaces to the user.

#include <stdint.h>
#include <stddef.h>

#define ENDPOINT_TRANSMIT_UNUSED 0x00020000
#define ENDPOINT_TRANSMIT_ISOCHRONOUS 0x00C40000
#define ENDPOINT_TRANSMIT_BULK 0x00C80000
#define ENDPOINT_TRANSMIT_INTERRUPT 0x00CC0000
#define ENDPOINT_RECEIVE_UNUSED 0x00000002
#define ENDPOINT_RECEIVE_ISOCHRONOUS 0x000000C4
#define ENDPOINT_RECEIVE_BULK 0x000000C8
#define ENDPOINT_RECEIVE_INTERRUPT 0x000000CC

/*
Each group of #define lines below corresponds to one of the
settings in the Tools > USB Type menu.  This file defines what
type of USB device is actually created for each of those menu
options.

Each "interface" is a set of functionality your PC or Mac will
use and treat as if it is a unique device.  Within each interface,
the "endpoints" are the actual communication channels.  Most
interfaces use 1, 2 or 3 endpoints.  By editing only this file,
you can customize the USB Types to be any collection of interfaces.

To modify a USB Type, delete the XYZ_INTERFACE lines for any
interfaces you wish to remove, and copy them from another USB Type
for any you want to add.

Give each interface a unique number, and edit NUM_INTERFACE to
reflect the total number of interfaces.

Next, assign unique endpoint numbers to all the endpoints across
all the interfaces your device has.  You can reuse an endpoint
number for transmit and receive, but the same endpoint number must
not be used twice to transmit, or twice to receive.

Most endpoints also require their maximum size, and some also
need an interval specification (the number of milliseconds the
PC will check for data from that endpoint).  For existing
interfaces, usually these other settings should not be changed.

Edit NUM_ENDPOINTS to be at least the largest endpoint number used.

Edit the ENDPOINT*_CONFIG lines so each endpoint is configured
the proper way (transmit, receive, or both).

If you are using existing interfaces (making your own device with
a different set of interfaces) the code in all other files should
automatically adapt to the new endpoints you specify here.

If you need to create a new type of interface, you'll need to write
the code which sends and receives packets, and presents an API to
the user.  Usually, a pair of files are added for the actual code,
and code is also added in usb_dev.c for any control transfers,
interrupt-level code, or other very low-level stuff not possible
from the packet send/receive functons.  Code also is added in
usb_inst.c to create an instance of your C++ object.  This message
gives a quick summary of things you will need to know:
https://forum.pjrc.com/threads/49045?p=164512&viewfull=1#post164512

You may edit the Vendor and Product ID numbers, and strings.  If
the numbers are changed, Teensyduino may not be able to automatically
find and reboot your board when you click the Upload button in
the Arduino IDE.  You will need to press the Program button on
Teensy to initiate programming.

Some operating systems, especially Windows, may cache USB device
info.  Changes to the device name may not update on the same
computer unless the vendor or product ID numbers change, or the
"bcdDevice" revision code is increased.

If these instructions are missing steps or could be improved, please
let me know?  http://forum.pjrc.com/forums/4-Suggestions-amp-Bug-Reports
*/

#if defined(USB_SERIAL)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x0483
#define MANUFACTURER_NAME {'R', 'a', 'z', 'e', 'r'}
#define MANUFACTURER_NAME_LEN 5
#define PRODUCT_NAME {'R', 'a', 'z', 'e', 'r', ' ', 'B', 'a', 's', 'i', 'l', 'i', 's', 'k', ' ', 'V', '3'}
#define PRODUCT_NAME_LEN 17
#define EP0_SIZE 64
#define NUM_ENDPOINTS 4
#define NUM_USB_BUFFERS 12
#define NUM_INTERFACE 3
#define CDC_IAD_DESCRIPTOR 1 // Serial
#define CDC_STATUS_INTERFACE 0
#define CDC_DATA_INTERFACE 1
#define CDC_ACM_ENDPOINT 2
#define CDC_RX_ENDPOINT 3
#define CDC_TX_ENDPOINT 4
#define CDC_ACM_SIZE 16
#define CDC_RX_SIZE_480 512
#define CDC_TX_SIZE_480 512
#define CDC_RX_SIZE_12 64
#define CDC_TX_SIZE_12 64
#define EXPERIMENTAL_INTERFACE 2
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_UNUSED
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_BULK

#elif defined(USB_DUAL_SERIAL)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 5
#define NUM_INTERFACE 4
#define CDC_IAD_DESCRIPTOR 1 // Serial
#define CDC_STATUS_INTERFACE 0
#define CDC_DATA_INTERFACE 1
#define CDC_ACM_ENDPOINT 2
#define CDC_RX_ENDPOINT 3
#define CDC_TX_ENDPOINT 3
#define CDC_ACM_SIZE 16
#define CDC_RX_SIZE_480 512
#define CDC_TX_SIZE_480 512
#define CDC_RX_SIZE_12 64
#define CDC_TX_SIZE_12 64
#define CDC2_STATUS_INTERFACE 2 // SerialUSB1
#define CDC2_DATA_INTERFACE 3
#define CDC2_ACM_ENDPOINT 4
#define CDC2_RX_ENDPOINT 5
#define CDC2_TX_ENDPOINT 5
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT5_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK

#elif defined(USB_TRIPLE_SERIAL)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 7
#define NUM_INTERFACE 6
#define CDC_IAD_DESCRIPTOR 1 // Serial
#define CDC_STATUS_INTERFACE 0
#define CDC_DATA_INTERFACE 1
#define CDC_ACM_ENDPOINT 2
#define CDC_RX_ENDPOINT 3
#define CDC_TX_ENDPOINT 3
#define CDC_ACM_SIZE 16
#define CDC_RX_SIZE_480 512
#define CDC_TX_SIZE_480 512
#define CDC_RX_SIZE_12 64
#define CDC_TX_SIZE_12 64
#define CDC2_STATUS_INTERFACE 2 // SerialUSB1
#define CDC2_DATA_INTERFACE 3
#define CDC2_ACM_ENDPOINT 4
#define CDC2_RX_ENDPOINT 5
#define CDC2_TX_ENDPOINT 5
#define CDC3_STATUS_INTERFACE 4 // SerialUSB2
#define CDC3_DATA_INTERFACE 5
#define CDC3_ACM_ENDPOINT 6
#define CDC3_RX_ENDPOINT 7
#define CDC3_TX_ENDPOINT 7
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT5_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT6_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT7_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK

#elif defined(USB_KEYBOARDONLY)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 4
#define NUM_USB_BUFFERS 14
#define NUM_INTERFACE 3
#define SEREMU_INTERFACE 1 // Serial emulation
#define SEREMU_TX_ENDPOINT 2
#define SEREMU_TX_SIZE 64
#define SEREMU_TX_INTERVAL 1 // TODO: is this ok for 480 Mbit speed
#define SEREMU_RX_ENDPOINT 2
#define SEREMU_RX_SIZE 32
#define SEREMU_RX_INTERVAL 2 // TODO: is this ok for 480 Mbit speed
#define KEYBOARD_INTERFACE 0 // Keyboard
#define KEYBOARD_ENDPOINT 3
#define KEYBOARD_SIZE 8
#define KEYBOARD_INTERVAL 1  // TODO: is this ok for 480 Mbit speed
#define KEYMEDIA_INTERFACE 2 // Keyboard Media Keys
#define KEYMEDIA_ENDPOINT 4
#define KEYMEDIA_SIZE 8
#define KEYMEDIA_INTERVAL 4 // TODO: is this ok for 480 Mbit speed
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_INTERRUPT + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT

#elif defined(USB_HID)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 6
#define NUM_USB_BUFFERS 24
#define NUM_INTERFACE 5
#define SEREMU_INTERFACE 2 // Serial emulation
#define SEREMU_TX_ENDPOINT 2
#define SEREMU_TX_SIZE 64
#define SEREMU_TX_INTERVAL 1
#define SEREMU_RX_ENDPOINT 2
#define SEREMU_RX_SIZE 32
#define SEREMU_RX_INTERVAL 2
#define KEYBOARD_INTERFACE 0 // Keyboard
#define KEYBOARD_ENDPOINT 3
#define KEYBOARD_SIZE 8
#define KEYBOARD_INTERVAL 1
#define KEYMEDIA_INTERFACE 4 // Keyboard Media Keys
#define KEYMEDIA_ENDPOINT 4
#define KEYMEDIA_SIZE 8
#define KEYMEDIA_INTERVAL 4
#define MOUSE_INTERFACE 1 // Mouse
#define MOUSE_ENDPOINT 5
#define MOUSE_SIZE 8
#define MOUSE_INTERVAL 1
#define JOYSTICK_INTERFACE 3 // Joystick
#define JOYSTICK_ENDPOINT 6
#define JOYSTICK_SIZE 12 //  12 = normal, 64 = extreme joystick
#define JOYSTICK_INTERVAL 2
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_INTERRUPT + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT5_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT6_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT

#elif defined(USB_SERIAL_HID)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define DEVICE_CLASS 0xEF
#define DEVICE_SUBCLASS 0x02
#define DEVICE_PROTOCOL 0x01
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 7
#define NUM_INTERFACE 6
#define CDC_IAD_DESCRIPTOR 1
#define CDC_STATUS_INTERFACE 0
#define CDC_DATA_INTERFACE 1 // Serial
#define CDC_ACM_ENDPOINT 2
#define CDC_RX_ENDPOINT 3
#define CDC_TX_ENDPOINT 3
#define CDC_ACM_SIZE 16
#define CDC_RX_SIZE_480 512
#define CDC_TX_SIZE_480 512
#define CDC_RX_SIZE_12 64
#define CDC_TX_SIZE_12 64
#define KEYBOARD_INTERFACE 2 // Keyboard
#define KEYBOARD_ENDPOINT 4
#define KEYBOARD_SIZE 8
#define KEYBOARD_INTERVAL 1
#define KEYMEDIA_INTERFACE 5 // Keyboard Media Keys
#define KEYMEDIA_ENDPOINT 5
#define KEYMEDIA_SIZE 8
#define KEYMEDIA_INTERVAL 4
#define MOUSE_INTERFACE 3 // Mouse
#define MOUSE_ENDPOINT 6
#define MOUSE_SIZE 8
#define MOUSE_INTERVAL 2
#define JOYSTICK_INTERFACE 4 // Joystick
#define JOYSTICK_ENDPOINT 7
#define JOYSTICK_SIZE 12 //  12 = normal, 64 = extreme joystick
#define JOYSTICK_INTERVAL 1
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT5_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT6_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT7_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT

#elif defined(USB_TOUCHSCREEN)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 5
#define NUM_INTERFACE 4
#define SEREMU_INTERFACE 1 // Serial emulation
#define SEREMU_TX_ENDPOINT 2
#define SEREMU_TX_SIZE 64
#define SEREMU_TX_INTERVAL 1
#define SEREMU_RX_ENDPOINT 2
#define SEREMU_RX_SIZE 32
#define SEREMU_RX_INTERVAL 2
#define KEYBOARD_INTERFACE 0 // Keyboard
#define KEYBOARD_ENDPOINT 3
#define KEYBOARD_SIZE 8
#define KEYBOARD_INTERVAL 1
#define KEYMEDIA_INTERFACE 2 // Keyboard Media Keys
#define KEYMEDIA_ENDPOINT 4
#define KEYMEDIA_SIZE 8
#define KEYMEDIA_INTERVAL 4
#define MULTITOUCH_INTERFACE 3 // Touchscreen
#define MULTITOUCH_ENDPOINT 5
#define MULTITOUCH_SIZE 9
#define MULTITOUCH_FINGERS 10
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_INTERRUPT + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT5_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT

#elif defined(USB_HID_TOUCHSCREEN)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 6
#define NUM_INTERFACE 5
#define SEREMU_INTERFACE 2 // Serial emulation
#define SEREMU_TX_ENDPOINT 2
#define SEREMU_TX_SIZE 64
#define SEREMU_TX_INTERVAL 1
#define SEREMU_RX_ENDPOINT 2
#define SEREMU_RX_SIZE 32
#define SEREMU_RX_INTERVAL 2
#define KEYBOARD_INTERFACE 0 // Keyboard
#define KEYBOARD_ENDPOINT 3
#define KEYBOARD_SIZE 8
#define KEYBOARD_INTERVAL 1
#define KEYMEDIA_INTERFACE 3 // Keyboard Media Keys
#define KEYMEDIA_ENDPOINT 4
#define KEYMEDIA_SIZE 8
#define KEYMEDIA_INTERVAL 4
#define MOUSE_INTERFACE 1 // Mouse
#define MOUSE_ENDPOINT 6
#define MOUSE_SIZE 8
#define MOUSE_INTERVAL 2
#define MULTITOUCH_INTERFACE 4 // Touchscreen
#define MULTITOUCH_ENDPOINT 5
#define MULTITOUCH_SIZE 9
#define MULTITOUCH_FINGERS 10
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_INTERRUPT + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT5_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT6_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT

#elif defined(USB_MIDI)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 4
#define NUM_INTERFACE 2
#define SEREMU_INTERFACE 1 // Serial emulation
#define SEREMU_TX_ENDPOINT 2
#define SEREMU_TX_SIZE 64
#define SEREMU_TX_INTERVAL 1
#define SEREMU_RX_ENDPOINT 2
#define SEREMU_RX_SIZE 32
#define SEREMU_RX_INTERVAL 2
#define MIDI_INTERFACE 0 // MIDI
#define MIDI_NUM_CABLES 1
#define MIDI_TX_ENDPOINT 3
#define MIDI_TX_SIZE_12 64
#define MIDI_TX_SIZE_480 512
#define MIDI_RX_ENDPOINT 3
#define MIDI_RX_SIZE_12 64
#define MIDI_RX_SIZE_480 512
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_INTERRUPT + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK

#elif defined(USB_MIDI4)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 3
#define NUM_INTERFACE 2
#define SEREMU_INTERFACE 1 // Serial emulation
#define SEREMU_TX_ENDPOINT 2
#define SEREMU_TX_SIZE 64
#define SEREMU_TX_INTERVAL 1
#define SEREMU_RX_ENDPOINT 2
#define SEREMU_RX_SIZE 32
#define SEREMU_RX_INTERVAL 2
#define MIDI_INTERFACE 0 // MIDI
#define MIDI_NUM_CABLES 4
#define MIDI_TX_ENDPOINT 3
#define MIDI_TX_SIZE_12 64
#define MIDI_TX_SIZE_480 512
#define MIDI_RX_ENDPOINT 3
#define MIDI_RX_SIZE_12 64
#define MIDI_RX_SIZE_480 512
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_INTERRUPT + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK

#elif defined(USB_MIDI16)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 3
#define NUM_INTERFACE 2
#define SEREMU_INTERFACE 1 // Serial emulation
#define SEREMU_TX_ENDPOINT 2
#define SEREMU_TX_SIZE 64
#define SEREMU_TX_INTERVAL 1
#define SEREMU_RX_ENDPOINT 2
#define SEREMU_RX_SIZE 32
#define SEREMU_RX_INTERVAL 2
#define MIDI_INTERFACE 0 // MIDI
#define MIDI_NUM_CABLES 16
#define MIDI_TX_ENDPOINT 3
#define MIDI_TX_SIZE_12 64
#define MIDI_TX_SIZE_480 512
#define MIDI_RX_ENDPOINT 3
#define MIDI_RX_SIZE_12 64
#define MIDI_RX_SIZE_480 512
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_INTERRUPT + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK

#elif defined(USB_MIDI_SERIAL)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 4
#define NUM_INTERFACE 3
#define CDC_IAD_DESCRIPTOR 1
#define CDC_STATUS_INTERFACE 0
#define CDC_DATA_INTERFACE 1 // Serial
#define CDC_ACM_ENDPOINT 2
#define CDC_RX_ENDPOINT 3
#define CDC_TX_ENDPOINT 3
#define CDC_ACM_SIZE 16
#define CDC_RX_SIZE_480 512
#define CDC_TX_SIZE_480 512
#define CDC_RX_SIZE_12 64
#define CDC_TX_SIZE_12 64
#define MIDI_INTERFACE 2 // MIDI
#define MIDI_NUM_CABLES 1
#define MIDI_TX_ENDPOINT 4
#define MIDI_TX_SIZE_12 64
#define MIDI_TX_SIZE_480 512
#define MIDI_RX_ENDPOINT 4
#define MIDI_RX_SIZE_12 64
#define MIDI_RX_SIZE_480 512
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK

#elif defined(USB_MIDI4_SERIAL)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 4
#define NUM_INTERFACE 3
#define CDC_IAD_DESCRIPTOR 1
#define CDC_STATUS_INTERFACE 0
#define CDC_DATA_INTERFACE 1 // Serial
#define CDC_ACM_ENDPOINT 2
#define CDC_RX_ENDPOINT 3
#define CDC_TX_ENDPOINT 3
#define CDC_ACM_SIZE 16
#define CDC_RX_SIZE_480 512
#define CDC_TX_SIZE_480 512
#define CDC_RX_SIZE_12 64
#define CDC_TX_SIZE_12 64
#define MIDI_INTERFACE 2 // MIDI
#define MIDI_NUM_CABLES 4
#define MIDI_TX_ENDPOINT 4
#define MIDI_TX_SIZE_12 64
#define MIDI_TX_SIZE_480 512
#define MIDI_RX_ENDPOINT 4
#define MIDI_RX_SIZE_12 64
#define MIDI_RX_SIZE_480 512
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK

#elif defined(USB_MIDI16_SERIAL)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 4
#define NUM_INTERFACE 3
#define CDC_IAD_DESCRIPTOR 1
#define CDC_STATUS_INTERFACE 0
#define CDC_DATA_INTERFACE 1 // Serial
#define CDC_ACM_ENDPOINT 2
#define CDC_RX_ENDPOINT 3
#define CDC_TX_ENDPOINT 3
#define CDC_ACM_SIZE 16
#define CDC_RX_SIZE_480 512
#define CDC_TX_SIZE_480 512
#define CDC_RX_SIZE_12 64
#define CDC_TX_SIZE_12 64
#define MIDI_INTERFACE 2 // MIDI
#define MIDI_NUM_CABLES 16
#define MIDI_TX_ENDPOINT 4
#define MIDI_TX_SIZE_12 64
#define MIDI_TX_SIZE_480 512
#define MIDI_RX_ENDPOINT 4
#define MIDI_RX_SIZE_12 64
#define MIDI_RX_SIZE_480 512
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK

#elif defined(USB_RAWHID)
#define RAWHID_USAGE_PAGE 0xFFAB // recommended: 0xFF00 to 0xFFFF
#define RAWHID_USAGE 0x0200      // recommended: 0x0100 to 0xFFFF
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 4
#define NUM_INTERFACE 2
#define RAWHID_INTERFACE 0 // RawHID
#define RAWHID_TX_ENDPOINT 3
#define RAWHID_TX_SIZE 64
#define RAWHID_TX_INTERVAL 1 // TODO: is this ok for 480 Mbit speed
#define RAWHID_RX_ENDPOINT 4
#define RAWHID_RX_SIZE 64
#define RAWHID_RX_INTERVAL 1 // TODO: is this ok for 480 Mbit speed
#define SEREMU_INTERFACE 1   // Serial emulation
#define SEREMU_TX_ENDPOINT 2
#define SEREMU_TX_SIZE 64
#define SEREMU_TX_INTERVAL 1 // TODO: is this ok for 480 Mbit speed
#define SEREMU_RX_ENDPOINT 2
#define SEREMU_RX_SIZE 32
#define SEREMU_RX_INTERVAL 2 // TODO: is this ok for 480 Mbit speed
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_INTERRUPT + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_INTERRUPT + ENDPOINT_TRANSMIT_UNUSED

#elif defined(USB_FLIGHTSIM)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 3
#define NUM_INTERFACE 2
#define FLIGHTSIM_INTERFACE 0 // Flight Sim Control
#define FLIGHTSIM_TX_ENDPOINT 3
#define FLIGHTSIM_TX_SIZE 64
#define FLIGHTSIM_TX_INTERVAL 1
#define FLIGHTSIM_RX_ENDPOINT 3
#define FLIGHTSIM_RX_SIZE 64
#define FLIGHTSIM_RX_INTERVAL 1
#define SEREMU_INTERFACE 1 // Serial emulation
#define SEREMU_TX_ENDPOINT 2
#define SEREMU_TX_SIZE 64
#define SEREMU_TX_INTERVAL 1
#define SEREMU_RX_ENDPOINT 2
#define SEREMU_RX_SIZE 32
#define SEREMU_RX_INTERVAL 2
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_INTERRUPT + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK

#elif defined(USB_FLIGHTSIM_JOYSTICK)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 4
#define NUM_INTERFACE 3
#define FLIGHTSIM_INTERFACE 0 // Flight Sim Control
#define FLIGHTSIM_TX_ENDPOINT 3
#define FLIGHTSIM_TX_SIZE 64
#define FLIGHTSIM_TX_INTERVAL 1
#define FLIGHTSIM_RX_ENDPOINT 3
#define FLIGHTSIM_RX_SIZE 64
#define FLIGHTSIM_RX_INTERVAL 1
#define SEREMU_INTERFACE 1 // Serial emulation
#define SEREMU_TX_ENDPOINT 2
#define SEREMU_TX_SIZE 64
#define SEREMU_TX_INTERVAL 1
#define SEREMU_RX_ENDPOINT 2
#define SEREMU_RX_SIZE 32
#define SEREMU_RX_INTERVAL 2
#define JOYSTICK_INTERFACE 2 // Joystick
#define JOYSTICK_ENDPOINT 4
#define JOYSTICK_SIZE 12 //  12 = normal, 64 = extreme joystick
#define JOYSTICK_INTERVAL 1
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_INTERRUPT + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT

#elif defined(USB_MTPDISK)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 4
#define NUM_INTERFACE 2
#define MTP_INTERFACE 1 // MTP Disk
#define MTP_TX_ENDPOINT 3
#define MTP_TX_SIZE_12 64
#define MTP_TX_SIZE_480 512
#define MTP_RX_ENDPOINT 3
#define MTP_RX_SIZE_12 64
#define MTP_RX_SIZE_480 512
#define MTP_EVENT_ENDPOINT 4
#define MTP_EVENT_SIZE 32
#define MTP_EVENT_INTERVAL_12 10 // 10 = 10 ms
#define MTP_EVENT_INTERVAL_480 7 // 7 = 8 ms
#define SEREMU_INTERFACE 0       // Serial emulation
#define SEREMU_TX_ENDPOINT 2
#define SEREMU_TX_SIZE 64
#define SEREMU_TX_INTERVAL 1
#define SEREMU_RX_ENDPOINT 2
#define SEREMU_RX_SIZE 32
#define SEREMU_RX_INTERVAL 2
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_INTERRUPT + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT

#elif defined(USB_MTPDISK_SERIAL)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 5
#define NUM_INTERFACE 3
#define CDC_IAD_DESCRIPTOR 1
#define CDC_STATUS_INTERFACE 0
#define CDC_DATA_INTERFACE 1 // Serial
#define CDC_ACM_ENDPOINT 2
#define CDC_RX_ENDPOINT 3
#define CDC_TX_ENDPOINT 3
#define CDC_ACM_SIZE 16
#define CDC_RX_SIZE_480 512
#define CDC_TX_SIZE_480 512
#define CDC_RX_SIZE_12 64
#define CDC_TX_SIZE_12 64
#define MTP_INTERFACE 2 // MTP Disk
#define MTP_TX_ENDPOINT 4
#define MTP_TX_SIZE_12 64
#define MTP_TX_SIZE_480 512
#define MTP_RX_ENDPOINT 4
#define MTP_RX_SIZE_12 64
#define MTP_RX_SIZE_480 512
#define MTP_EVENT_ENDPOINT 5
#define MTP_EVENT_SIZE 32
#define MTP_EVENT_INTERVAL_12 10 // 10 = 10 ms
#define MTP_EVENT_INTERVAL_480 7 // 7 = 8 ms
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT5_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT

#elif defined(USB_AUDIO)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 4
#define NUM_INTERFACE 4
#define SEREMU_INTERFACE 0 // Serial emulation
#define SEREMU_TX_ENDPOINT 2
#define SEREMU_TX_SIZE 64
#define SEREMU_TX_INTERVAL 1
#define SEREMU_RX_ENDPOINT 2
#define SEREMU_RX_SIZE 32
#define SEREMU_RX_INTERVAL 2
#define AUDIO_INTERFACE 1 // Audio (uses 3 consecutive interfaces)
#define AUDIO_TX_ENDPOINT 3
#define AUDIO_TX_SIZE 180
#define AUDIO_RX_ENDPOINT 3
#define AUDIO_RX_SIZE 180
#define AUDIO_SYNC_ENDPOINT 4
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_INTERRUPT + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_ISOCHRONOUS + ENDPOINT_TRANSMIT_ISOCHRONOUS
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_ISOCHRONOUS

#elif defined(USB_MIDI_AUDIO_SERIAL)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 6
#define NUM_INTERFACE 6
#define CDC_IAD_DESCRIPTOR 1
#define CDC_STATUS_INTERFACE 0
#define CDC_DATA_INTERFACE 1 // Serial
#define CDC_ACM_ENDPOINT 2
#define CDC_RX_ENDPOINT 3
#define CDC_TX_ENDPOINT 3
#define CDC_ACM_SIZE 16
#define CDC_RX_SIZE_480 512
#define CDC_TX_SIZE_480 512
#define CDC_RX_SIZE_12 64
#define CDC_TX_SIZE_12 64
#define MIDI_INTERFACE 2 // MIDI
#define MIDI_NUM_CABLES 1
#define MIDI_TX_ENDPOINT 4
#define MIDI_TX_SIZE_12 64
#define MIDI_TX_SIZE_480 512
#define MIDI_RX_ENDPOINT 4
#define MIDI_RX_SIZE_12 64
#define MIDI_RX_SIZE_480 512
#define AUDIO_INTERFACE 3 // Audio (uses 3 consecutive interfaces)
#define AUDIO_TX_ENDPOINT 5
#define AUDIO_TX_SIZE 180
#define AUDIO_RX_ENDPOINT 5
#define AUDIO_RX_SIZE 180
#define AUDIO_SYNC_ENDPOINT 6
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT5_CONFIG ENDPOINT_RECEIVE_ISOCHRONOUS + ENDPOINT_TRANSMIT_ISOCHRONOUS
#define ENDPOINT6_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_ISOCHRONOUS

#elif defined(USB_MIDI16_AUDIO_SERIAL)
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 8
#define NUM_INTERFACE 6
#define CDC_IAD_DESCRIPTOR 1
#define CDC_STATUS_INTERFACE 0
#define CDC_DATA_INTERFACE 1 // Serial
#define CDC_ACM_ENDPOINT 2
#define CDC_RX_ENDPOINT 3
#define CDC_TX_ENDPOINT 3
#define CDC_ACM_SIZE 16
#define CDC_RX_SIZE_480 512
#define CDC_TX_SIZE_480 512
#define CDC_RX_SIZE_12 64
#define CDC_TX_SIZE_12 64
#define MIDI_INTERFACE 2 // MIDI
#define MIDI_NUM_CABLES 16
#define MIDI_TX_ENDPOINT 4
#define MIDI_TX_SIZE_12 64
#define MIDI_TX_SIZE_480 512
#define MIDI_RX_ENDPOINT 4
#define MIDI_RX_SIZE_12 64
#define MIDI_RX_SIZE_480 512
#define AUDIO_INTERFACE 3 // Audio (uses 3 consecutive interfaces)
#define AUDIO_TX_ENDPOINT 5
#define AUDIO_TX_SIZE 180
#define AUDIO_RX_ENDPOINT 5
#define AUDIO_RX_SIZE 180
#define AUDIO_SYNC_ENDPOINT 6
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_INTERRUPT
#define ENDPOINT3_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_BULK + ENDPOINT_TRANSMIT_BULK
#define ENDPOINT5_CONFIG ENDPOINT_RECEIVE_ISOCHRONOUS + ENDPOINT_TRANSMIT_ISOCHRONOUS
#define ENDPOINT6_CONFIG ENDPOINT_RECEIVE_UNUSED + ENDPOINT_TRANSMIT_ISOCHRONOUS

#elif defined(USB_EVERYTHING)
#define RAWHID_USAGE_PAGE 0xFFAB // recommended: 0xFF00 to 0xFFFF
#define RAWHID_USAGE 0x0200      // recommended: 0x0100 to 0xFFFF
#define DEVICE_CLASS 0xEF
#define DEVICE_SUBCLASS 0x02
#define DEVICE_PROTOCOL 0x01
#define VENDOR_ID 0x8A25
#define PRODUCT_ID 0x1120
#define MANUFACTURER_NAME {'G', 'l', 'o', 'r', 'i', 'o', 'u', 's'}
#define MANUFACTURER_NAME_LEN 8
#define PRODUCT_NAME {'M', 'o', 'd', 'e', 'l', ' ', 'O', ' ', 'W', 'i', 'r', 'e', 'l', 'e', 's', 's'}
#define PRODUCT_NAME_LEN 16
#define EP0_SIZE 64
#define NUM_ENDPOINTS 15
#define NUM_INTERFACE 13
#define CDC_IAD_DESCRIPTOR 1
#define CDC_STATUS_INTERFACE 0
#define CDC_DATA_INTERFACE 1 // Serial
#define CDC_ACM_ENDPOINT 1
#define CDC_RX_ENDPOINT 2
#define CDC_TX_ENDPOINT 2
#define CDC_ACM_SIZE 16
#define CDC_RX_SIZE 64
#define CDC_TX_SIZE 64
#define MIDI_INTERFACE 2 // MIDI
#define MIDI_NUM_CABLES 16
#define MIDI_TX_ENDPOINT 3
#define MIDI_TX_SIZE 64
#define MIDI_RX_ENDPOINT 3
#define MIDI_RX_SIZE 64
#define KEYBOARD_INTERFACE 3 // Keyboard
#define KEYBOARD_ENDPOINT 4
#define KEYBOARD_SIZE 8
#define KEYBOARD_INTERVAL 1
#define MOUSE_INTERFACE 4 // Mouse
#define MOUSE_ENDPOINT 5
#define MOUSE_SIZE 8
#define MOUSE_INTERVAL 2
#define RAWHID_INTERFACE 5 // RawHID
#define RAWHID_TX_ENDPOINT 6
#define RAWHID_TX_SIZE 64
#define RAWHID_TX_INTERVAL 1
#define RAWHID_RX_ENDPOINT 6
#define RAWHID_RX_SIZE 64
#define RAWHID_RX_INTERVAL 1
#define FLIGHTSIM_INTERFACE 6 // Flight Sim Control
#define FLIGHTSIM_TX_ENDPOINT 9
#define FLIGHTSIM_TX_SIZE 64
#define FLIGHTSIM_TX_INTERVAL 1
#define FLIGHTSIM_RX_ENDPOINT 9
#define FLIGHTSIM_RX_SIZE 64
#define FLIGHTSIM_RX_INTERVAL 1
#define JOYSTICK_INTERFACE 7 // Joystick
#define JOYSTICK_ENDPOINT 10
#define JOYSTICK_SIZE 12 //  12 = normal, 64 = extreme joystick
#define JOYSTICK_INTERVAL 1
/*
  #define MTP_INTERFACE		8	// MTP Disk
  #define MTP_TX_ENDPOINT	11
  #define MTP_TX_SIZE		64
  #define MTP_RX_ENDPOINT	3
  #define MTP_RX_SIZE		64
  #define MTP_EVENT_ENDPOINT	11
  #define MTP_EVENT_SIZE	16
  #define MTP_EVENT_INTERVAL	10
*/
#define KEYMEDIA_INTERFACE 8 // Keyboard Media Keys
#define KEYMEDIA_ENDPOINT 12
#define KEYMEDIA_SIZE 8
#define KEYMEDIA_INTERVAL 4
#define AUDIO_INTERFACE 9 // Audio (uses 3 consecutive interfaces)
#define AUDIO_TX_ENDPOINT 13
#define AUDIO_TX_SIZE 180
#define AUDIO_RX_ENDPOINT 13
#define AUDIO_RX_SIZE 180
#define AUDIO_SYNC_ENDPOINT 14
#define MULTITOUCH_INTERFACE 12 // Touchscreen
#define MULTITOUCH_ENDPOINT 15
#define MULTITOUCH_SIZE 9
#define MULTITOUCH_FINGERS 10
#define ENDPOINT1_CONFIG ENDPOINT_TRANSMIT_ONLY
#define ENDPOINT2_CONFIG ENDPOINT_TRANSMIT_AND_RECEIVE
#define ENDPOINT3_CONFIG ENDPOINT_TRANSMIT_AND_RECEIVE
#define ENDPOINT4_CONFIG ENDPOINT_TRANSMIT_ONLY
#define ENDPOINT5_CONFIG ENDPOINT_TRANSMIT_ONLY
#define ENDPOINT6_CONFIG ENDPOINT_TRANSMIT_AND_RECEIVE
#define ENDPOINT7_CONFIG ENDPOINT_TRANSMIT_AND_RECEIVE
#define ENDPOINT8_CONFIG ENDPOINT_TRANSMIT_ONLY
#define ENDPOINT9_CONFIG ENDPOINT_TRANSMIT_AND_RECEIVE
#define ENDPOINT10_CONFIG ENDPOINT_TRANSMIT_ONLY
#define ENDPOINT11_CONFIG ENDPOINT_TRANSMIT_AND_RECEIVE
#define ENDPOINT12_CONFIG ENDPOINT_TRANSMIT_ONLY
#define ENDPOINT13_CONFIG (ENDPOINT_RECEIVE_ISOCHRONOUS | ENDPOINT_TRANSMIT_ISOCHRONOUS)
#define ENDPOINT14_CONFIG ENDPOINT_TRANSMIT_ISOCHRONOUS
#define ENDPOINT15_CONFIG ENDPOINT_TRANSMIT_ONLY

#endif

#ifdef USB_DESC_LIST_DEFINE
#if defined(NUM_ENDPOINTS) && NUM_ENDPOINTS > 0
// NUM_ENDPOINTS = number of non-zero endpoints (0 to 7)
extern const uint32_t usb_endpoint_config_table[NUM_ENDPOINTS];

typedef struct
{
  uint16_t wValue;
  uint16_t wIndex;
  const uint8_t *addr;
  uint16_t length;
} usb_descriptor_list_t;

extern const usb_descriptor_list_t usb_descriptor_list[];
#endif // NUM_ENDPOINTS
#endif // USB_DESC_LIST_DEFINE
