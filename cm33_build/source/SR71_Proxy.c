/*
*   _____   _____    ______    __
*  / ____| |  __ \  |____  |  /_ |
*  | (___   | |__) |      / /    | |
*  \___ \  |  _  /      / /     | |
*  ____) | | | \ \     / /      | |
* |_____/  |_|  \_\   /_/       |_|   by Ramsey McGrath
*
*
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
#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"

/* Fallback for socklen_t isn’t defined */
#ifndef socklen_t
typedef unsigned int socklen_t;
#endif

#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT   0x08    /* Nonblocking i/o for this operation only */
#endif

/*******************************************************************************
 * Extern Declarations
 ******************************************************************************/
extern void SPI_Host_Init(void);
extern void SPI_SendUsbPacket(uint8_t *data, size_t length);
extern uint8_t USB_HostGetDeviceAttachState(usb_device_handle deviceHandle);

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define USB_HOST_CONFIG_INTERFACE_MAX_EP            (4U)
#define USB_HOST_CONFIG_CONFIGURATION_MAX_INTERFACE   (5U)
#define HID_MOUSE_REPORT_SIZE                         (4U)

/* UDP settings */
#define UDP_LOCAL_PORT                                (12345U)

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
/* USB host variables */
usb_host_handle         g_hostHandle        = NULL;
usb_device_handle       g_mouseDeviceHandle = NULL;
usb_host_pipe_handle    g_mousePipeHandle   = NULL;
/* Buffer for HID mouse reports */
static uint8_t g_mouseReportBuffer[HID_MOUSE_REPORT_SIZE];

/* Global variable for UDP socket */
static int udpSocket = -1;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static usb_status_t USB_HostEvent(usb_device_handle deviceHandle,
                                  usb_host_configuration_handle configurationHandle,
                                  uint32_t eventCode);
void USB_HostClockInit(void);
void USB_HostIsrEnable(void);
void USB_HostTaskFn(void *hostHandle);

static void ProcessUdpPackets(void);

extern void BOARD_LED_Toggle(void);

/*******************************************************************************
 * NOTE:
 * The following USB things are generated and not redefined here:
 *    - USB0_FS_IRQHandler
 *    - USB_HostClockInit
 *    - USB_HostIsrEnable
 *    - USB_HostTaskFn
 *    - USB_HostApplicationInit
 ******************************************************************************/

/*******************************************************************************
 * USB Host Event Callback
 ******************************************************************************/
static usb_status_t USB_HostEvent(usb_device_handle deviceHandle,
                                  usb_host_configuration_handle configurationHandle,
                                  uint32_t eventCode)
{
    usb_status_t status   = kStatus_USB_Success;
    usb_status_t status0;
    uint8_t processedInterfaces[USB_HOST_CONFIG_CONFIGURATION_MAX_INTERFACE] = {0};

    switch (eventCode & 0x0000FFFFU)
    {
        case kUSB_HostEventAttach:
            status0 = USB_HostInterface0HidMouseEvent(deviceHandle, configurationHandle, eventCode, processedInterfaces);
            if (status0 == kStatus_USB_NotSupported)
            {
                status = kStatus_USB_NotSupported;
            }
            break;

        case kUSB_HostEventNotSupported:
            usb_echo("Device not supported.\r\n");
            break;

        case kUSB_HostEventEnumerationDone:
            status0 = USB_HostInterface0HidMouseEvent(deviceHandle, configurationHandle, eventCode, processedInterfaces);
            if (status0 != kStatus_USB_Success)
            {
                status = kStatus_USB_Error;
            }
            break;

        case kUSB_HostEventDetach:
            status0 = USB_HostInterface0HidMouseEvent(deviceHandle, configurationHandle, eventCode, processedInterfaces);
            if (status0 != kStatus_USB_Success)
            {
                status = kStatus_USB_Error;
            }
            break;

        case kUSB_HostEventEnumerationFail:
            usb_echo("Enumeration failed\r\n");
            break;

        default:
            break;
    }
    return status;
}

/*******************************************************************************
 * UDP Packet Processing
 ******************************************************************************/
#define UDP_RX_BUFFER_SIZE  512  // Example size; adjust as needed

/**
 * @brief Process an incoming UDP packet using the raw API.
 *
 * @param p Pointer to the received pbuf (contains an IP packet).
 */
void process_udp_packet(struct pbuf *p) {
    if (p == NULL) {
        return;
    }

    /* Check that the pbuf is large enough to hold an IP header and a UDP header */
    if (p->len < (PBUF_IP_HLEN + sizeof(struct udp_hdr))) {
        pbuf_free(p);
        return;
    }

    /* Point to the IP header. (Assumes IPv4.) */
    struct ip_hdr *iphdr = (struct ip_hdr *)p->payload;

    /* Verify the protocol is UDP */
    if (IPH_PROTO(iphdr) != IP_PROTO_UDP) {
        pbuf_free(p);
        return;
    }

    /* Determine the length of the IP header (in bytes) */
    u16_t ip_header_len = IPH_HL(iphdr) * 4;
    if (p->len < ip_header_len + sizeof(struct udp_hdr)) {
        pbuf_free(p);
        return;
    }

    /* Point to the UDP header, which follows the IP header */
    struct udp_hdr *udphdr = (struct udp_hdr *)((char *)p->payload + ip_header_len);

    /* Get the UDP length and compute payload length */
    u16_t udp_len = ntohs(udphdr->len);
    u16_t payload_len = (udp_len > sizeof(struct udp_hdr)) ? udp_len - sizeof(struct udp_hdr) : 0;

    /* Copy the UDP payload into a local buffer (null-terminate if needed) */
    char cmd[256];
    u16_t copy_len = (payload_len < (sizeof(cmd) - 1)) ? payload_len : (sizeof(cmd) - 1);
    memcpy(cmd, (char *)udphdr + sizeof(struct udp_hdr), copy_len);
    cmd[copy_len] = '\0';

    /*
     * Assign the source IP address to a local variable.
     * This avoids taking the address of the rvalue returned by IPH_SRC().
     */
    ip4_addr_t src_ip;
    src_ip.addr = iphdr->src.addr;

    /* Print out sender’s IP (using its address), source port, payload length, and command */
    PRINTF("Received UDP packet from %s:%d, payload length: %d, command: %s\r\n",
           ip4addr_ntoa(&src_ip),
           ntohs(udphdr->src),
           payload_len,
           cmd);

    /* Process the command as needed... */

    pbuf_free(p);
}
/*******************************************************************************
 * Main Function
 ******************************************************************************/
int main(void)
{
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    BOARD_InitDebugConsole();
#endif

    if (USB_HostApplicationInit() != kStatus_USB_Success)
    {
        PRINTF("USB host initialization failed!\r\n");
        while (1) {}
    }

    /* Initialize the SPI host interface */
    SPI_Host_Init();

    lwIP_interface_init();

    PRINTF("\r\nSR71 Host starting; forwarding mouse reports over SPI and receiving UDP packets...\r\n");

    while (1)
    {
        /* Process USB host tasks */
        USB_HostTaskFn(g_hostHandle);

        /* KMBOX Processing here */
        ProcessUdpPackets();

        /* TODO: logic to only send when the report changes. */
        if (USB_HostGetDeviceAttachState(g_mouseDeviceHandle) == 1U)
        {
            SPI_SendUsbPacket(g_mouseReportBuffer, HID_MOUSE_REPORT_SIZE);
        }

        BOARD_LED_Toggle();

        /* Adjust delay as needed for responsiveness */
        SDK_DelayAtLeastUs(500000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    }
    return 0;
}

/*******************************************************************************
 * Interrupt Handlers
 ******************************************************************************/

/* LP_FLEXCOMM4_IRQn interrupt handler template */
void LP_FLEXCOMM4_IRQHANDLER(void) {
    uint32_t intStatus;
    /* Read all interrupt flags from the SPI peripheral status registers */
    intStatus = LPSPI_GetStatusFlags(LP_FLEXCOMM4_PERIPHERAL);
    LPSPI_ClearStatusFlags(LP_FLEXCOMM4_PERIPHERAL, intStatus);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
