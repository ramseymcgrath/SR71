/*
 *   _____   _____    ______    __
 *  / ____| |  __ \  |____  |  /_ |
 *  | (___   | |__) |      / /    | |
 *  \___ \  |  _  /      / /     | |
 *  ____) | | | \ \     / /      | |
 * |_____/  |_|  \_\   /_/       |_|   by Ramsey McGrath
 *
 * This file implements the main application logic for the SR71 Host.
 * It initializes the system, sets up a UDP socket for receiving packets,
 * and processes incoming UDP packets.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_phylan8720a.h"
#include "fsl_phy.h"
#define LWIP_IPV4 1

/* USB host includes */
#include "usb_host.h"                      /* USB host APIs (e.g. USB_HostGetDeviceAttachState) */
#include "usb_host_interface_0_hid_mouse.h"
#include "usb_host_interface_1_hid_generic.h"

/* SPI host functions (provided in spi_host.c / spi_host.h) */
#include "spi_host.h"

/* Ethernet and PHY includes */
#include "fsl_enet.h"
#include "fsl_phylan8720a.h"
#include "fsl_phy.h"

/* lwIP includes */
#include "lwip/ip_addr.h"
#include "lwip/opt.h"
#include "lwip/netdb.h"
#include "lwip/init.h"
#include "lwip/sockets.h"  // For socket API
#include "lwip/netif.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/tcpip.h"    // For TCP/IP stack initialization
#include "lwip/dhcp.h"     // For DHCP (optional)

/* Fallback for socklen_t */
#ifndef socklen_t
typedef unsigned int socklen_t;
#endif

#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0x08  /* Nonblocking i/o for this operation only */
#endif

/*******************************************************************************
 * Extern Declarations
 ******************************************************************************/
extern void SPI_Host_Init(void);
extern void SPI_SendUsbPacket(uint8_t *data, size_t length);
extern uint8_t USB_HostGetDeviceAttachState(usb_device_handle deviceHandle);

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/
extern err_t ethernetif_init(struct netif *netif);
extern err_t ethernetif_output(struct netif *netif, struct pbuf *p);
extern status_t PHY_LAN8741_Write(phy_handle_t *handle, uint8_t phyReg, uint16_t data);
extern status_t PHY_LAN8741_Read(phy_handle_t *handle, uint8_t phyReg, uint16_t *pData);
extern status_t PHY_LAN8741_Init(phy_handle_t *handle, const phy_config_t *config);
/*******************************************************************************
* Code
******************************************************************************/
/*******************************************************************************
 * lwIP Callbacks
 ******************************************************************************/

status_t MDIO_Write(uint8_t phyAddr, uint8_t regAddr, uint16_t data) {
  return PHY_LAN8741_Write(LWIP_NETIF0_ENET_PERIPHERAL, phyAddr, data);
}

status_t MDIO_Read(uint8_t phyAddr, uint8_t regAddr, uint16_t *data) {
  return PHY_LAN8741_Read(LWIP_NETIF0_ENET_PERIPHERAL, phyAddr, data);
}

/*******************************************************************************
 * Main Function
 ******************************************************************************/
int main(void) {
    /* Initialize board hardware */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    BOARD_InitDebugConsole();
#endif

    /* Initialize USB host */
    if (USB_HostApplicationInit() != kStatus_USB_Success) {
        PRINTF("USB host initialization failed!\r\n");
        while (1) {}
    }

    /* Initialize the SPI host interface */
    SPI_Host_Init();

    PRINTF("\r\nSR71 Host starting; forwarding mouse reports over SPI and receiving UDP packets...\r\n");

    while (1) {
        USB_HostTaskFn(g_hostHandle);

        /* Send USB mouse report over SPI if the device is attached */
        if (USB_HostGetDeviceAttachState(g_mouseDeviceHandle) == 1U) {
            SPI_SendUsbPacket(g_mouseReportBuffer, HID_MOUSE_REPORT_SIZE);
        }

        /* Toggle LED for heartbeat */
        BOARD_LED_Toggle();

        /* Adjust delay as needed for responsiveness */
        SDK_DelayAtLeastUs(50000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    }
    return 0;
}
