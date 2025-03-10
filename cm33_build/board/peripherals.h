/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

/***********************************************************************************************************************
 * User includes
 **********************************************************************************************************************/
#include "fsl_phylan8720a.h"

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_lpspi.h"
#include "fsl_wuu.h"
#include "fsl_lpuart.h"
#include "fsl_flexspi.h"
#include "fsl_cdog.h"
#include "fsl_gpio.h"
#include "fsl_crc.h"
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "lwip/dhcp.h"
#include "lwip/icmp.h"
#include "lwip/errno.h"
#include "ethernetif.h"
#include "fsl_enet.h"
#include "fsl_silicon_id.h"
#include "usb_host_app.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
/* Definitions for BOARD_InitPeripherals functional group */
/* NVIC interrupt vector ID (number). */
#define USB_INTERRUPT_IRQN USB0_FS_IRQn
/* NVIC interrupt handler identifier. */
#define USB_INTERRUPT_IRQHANDLER USB0_FS_IRQHandler
/* Definition of peripheral ID */
#define LP_FLEXCOMM4_PERIPHERAL ((LPSPI_Type *)LP_FLEXCOMM4)
/* Definition of clock source frequency */
#define LP_FLEXCOMM4_CLOCK_FREQ 12000000UL
/* LP_FLEXCOMM4 interrupt vector ID (number). */
#define LP_FLEXCOMM4_IRQN LP_FLEXCOMM4_IRQn
/* LP_FLEXCOMM4 interrupt handler identifier. */
#define LP_FLEXCOMM4_IRQHANDLER LP_FLEXCOMM4_IRQHandler
/* BOARD_InitPeripherals defines for WUU0 */
/* Definition of peripheral ID */
#define WUU0_PERIPHERAL WUU0
/* Definition of WUU external pin definition (event ID: 8, event ID in hex: 0x8) */
#define WUU0_EXTERN_PIN_8 8U
/* Definition of WUU Interrupt module 0 (see the MCU specific value in the reference manual). (event ID: 0, event ID in hex: 0x0) */
#define WUU0_MODULE_INTERRUPT_0 0U
/* Definition of peripheral ID */
#define LP_FLEXCOMM0_PERIPHERAL ((LPUART_Type *) LP_FLEXCOMM0)
/* Definition of the clock source frequency */
#define LP_FLEXCOMM0_CLOCK_SOURCE 12000000UL
/* FLEXIO0_CTRL: DOZEN=0, DBGE=0, FASTACC=0, FLEXEN=0 */
#define FLEXIO0_CTRL_INIT 0x0U
/* FLEXIO0_SHIFTSIEN: SSIE=0 */
#define FLEXIO0_SHIFTSIEN_INIT 0x0U
/* FLEXIO0_SHIFTEIEN: SEIE=0 */
#define FLEXIO0_SHIFTEIEN_INIT 0x0U
/* FLEXIO0_TIMIEN: TEIE=0 */
#define FLEXIO0_TIMIEN_INIT 0x0U
/* FLEXIO0_SHIFTSDEN: SSDE=0 */
#define FLEXIO0_SHIFTSDEN_INIT 0x0U
/* FLEXIO0_TIMERSDEN: TSDE=0 */
#define FLEXIO0_TIMERSDEN_INIT 0x0U
/* FLEXIO0_TRIGIEN: TRIE=0 */
#define FLEXIO0_TRIGIEN_INIT 0x0U
/* FLEXIO0_PINIEN: PSIE=0 */
#define FLEXIO0_PINIEN_INIT 0x0U
/* FLEXIO0_PINREN: PRE=0 */
#define FLEXIO0_PINREN_INIT 0x0U
/* FLEXIO0_PINFEN: PFE=0 */
#define FLEXIO0_PINFEN_INIT 0x0U
/* FLEXIO0_PINOUTD: OUTD=0 */
#define FLEXIO0_PINOUTD_INIT 0x0U
/* FLEXIO0_PINOUTE: OUTE=0 */
#define FLEXIO0_PINOUTE_INIT 0x0U
/* FLEXIO0_SHIFTCTL0: TIMSEL=0, TIMPOL=0, PINCFG=0, PINSEL=0, PINPOL=0, SMOD=0 */
#define FLEXIO0_SHIFTCTL0_INIT 0x0U
/* FLEXIO0_SHIFTCFG0: PWIDTH=0, SSIZE=0, LATST=0, INSRC=0, SSTOP=0, SSTART=0 */
#define FLEXIO0_SHIFTCFG0_INIT 0x0U
/* FLEXIO0_TIMCTL0: TRGSEL=0, TRGPOL=0, TRGSRC=0, PINCFG=0, PINSEL=0, PINPOL=0, PININS=0, ONETIM=0, TIMOD=0 */
#define FLEXIO0_TIMCTL0_INIT 0x0U
/* FLEXIO0_TIMCFG0: TIMOUT=0, TIMDEC=0, TIMRST=0, TIMDIS=0, TIMENA=0, TSTOP=0, TSTART=0 */
#define FLEXIO0_TIMCFG0_INIT 0x0U
/* Definition of peripheral ID */
#define FLEXSPI0_PERIPHERAL FLEXSPI0
/* FLEXSPI0 interrupt vector ID (number). */
#define FLEXSPI0_IRQN FLEXSPI0_IRQn
/* FLEXSPI0 interrupt handler identifier. */
#define FLEXSPI0_IRQHANDLER FLEXSPI0_IRQHandler
/* BOARD_InitPeripherals defines for CDOG0 */
/* Definition of peripheral ID */
#define CDOG0_PERIPHERAL CDOG0
/* Alias for GPIO0_ALIAS1 peripheral */
#define GPIO0_ALIAS1_GPIO GPIO0_ALIAS1
/* CRC base */
#define CRC0_PERIPHERAL CRC0
/* @TEST_ANCHOR */
/* Definition of peripheral ID */
#define LWIP_NETIF0_ENET_PERIPHERAL ENET0
/* Ethernet peripheral clock frequency */
#define LWIP_NETIF0_ENET_PERIPHERAL_CLK_FREQ 100000000UL
/* PHY address definition */
#ifndef LWIP_NETIF0_PHY_ADDRESS
#define LWIP_NETIF0_PHY_ADDRESS 0x0
#endif
/* PHY options definition */
#define LWIP_NETIF0_PHY_OPS &phylan8720a_ops
/* PHY resource definition */
#define LWIP_NETIF0_PHY_RESOURCE &lwIP_netif0_phy_resource

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
extern const lpspi_master_config_t LP_FLEXCOMM4_config;
extern const wuu_external_wakeup_pin_config_t WUU0_external_wakeup_pin_config_8;
extern const lpuart_config_t LP_FLEXCOMM0_config;
extern const flexspi_config_t FLEXSPI0_config;
extern cdog_config_t CDOG0_config;
extern const crc_config_t CRC0_config;
/* Network interface structure */
extern struct netif lwIP_netif0;
/* Ethernet interface configuration */
extern ethernetif_config_t lwIP_netif0_enet_config;
/* PHY resource */
extern phy_lan8720a_resource_t lwIP_netif0_phy_resource;

/***********************************************************************************************************************
 * Callback functions
 **********************************************************************************************************************/
/* User init callback function*/
extern void lwIP_interface_init(void);

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/

void BOARD_InitPeripherals(void);

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void);

#if defined(__cplusplus)
}
#endif

#endif /* _PERIPHERALS_H_ */
