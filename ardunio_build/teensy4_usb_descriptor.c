#include <avr/pgmspace.h>
#include <usb_names.h> // teensy/avr/cores/teensy4/usb_names.h

#define PRODUCT_NAME {'R','a','z','e','r',' ','B','a','s','i','l','i','s','k',' ','V','3'}
#define PRODUCT_NAME_LEN 17
#define MANUFACTURER_NAME  {'R','a','z','e','r'}
#define MANUFACTURER_NAME_LEN 5

PROGMEM extern struct usb_string_descriptor_struct usb_string_manufacturer_name = {
    2 + MANUFACTURER_NAME_LEN * 2,
    3,
    MANUFACTURER_NAME
};

PROGMEM extern struct usb_string_descriptor_struct usb_string_product_name = {
    2 + PRODUCT_NAME_LEN * 2,
    3,
    PRODUCT_NAME
};
