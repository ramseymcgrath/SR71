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

#include "fsl_lpspi.h"
#include "fsl_clock.h"
#include "board.h"
#include "fsl_debug_console.h"

#ifndef LP_FLEXCOMM4_PERIPHERAL
#define LP_FLEXCOMM4_PERIPHERAL LPSPI4
#endif

/* Global handle for nonblocking SPI transfers */
static lpspi_master_handle_t s_spiHandle;

/* Flag set by the SPI callback when a transfer completes */
static volatile bool s_transferComplete = false;

/*!
 * @brief SPI callback function.
 *
 * This function is called by the LPSPI driver when a nonblocking transfer completes.
 *
 * @param base Pointer to the SPI peripheral base address.
 * @param handle Pointer to the SPI master handle.
 * @param status Status of the transfer.
 * @param userData User data (not used here).
 */
static void SPI_Callback(LPSPI_Type *base,
                         lpspi_master_handle_t *handle,
                         status_t status,
                         void *userData)
{
    if (status == kStatus_Success)
    {
        s_transferComplete = true;
    }
    else
    {
        PRINTF("SPI callback: error code %d\r\n", status);
        s_transferComplete = true;  /* still signal completion */
    }
}

/*!
 * @brief Initializes the SPI host interface.
 *
 * This function initializes the FlexComm SPI peripheral (FlexComm4) for master mode
 * and creates a handle for nonblocking transfers.
 */
void SPI_Host_Init(void)
{
    lpspi_master_config_t config;
    uint32_t srcClock_Hz = CLOCK_GetFreq(kCLOCK_LPFlexComm4); // Use the FlexComm4 clock source frequency

    /* Get default configuration */
    LPSPI_MasterGetDefaultConfig(&config);
    /* Set desired baud rate (for example, 4 MHz) */
    config.baudRate = 4000000U;

    /* Initialize the SPI master peripheral */
    LPSPI_MasterInit(LP_FLEXCOMM4_PERIPHERAL, &config, srcClock_Hz);

    /* Create the nonblocking transfer handle with our callback */
    LPSPI_MasterTransferCreateHandle(LP_FLEXCOMM4_PERIPHERAL, &s_spiHandle, SPI_Callback, NULL);
}

/*!
 * @brief Performs an SPI master transfer (nonblocking).
 *
 * This function prepares a transfer structure and kicks off a nonblocking SPI transfer.
 * It then waits (polls) until the transfer is complete.
 *
 * @param txData Pointer to the transmit buffer (can be NULL if only receiving).
 * @param rxData Pointer to the receive buffer (can be NULL if only transmitting).
 * @param dataSize Number of bytes to transfer.
 * @return status_t Returns kStatus_Success on success.
 */
status_t SPI_Host_Transfer(uint8_t *txData, uint8_t *rxData, size_t dataSize)
{
    lpspi_transfer_t transfer;
    transfer.txData      = txData;
    transfer.rxData      = rxData;
    transfer.dataSize    = dataSize;
    /* Use chip-select 0. (CTAR 0 is used by default.) */
    transfer.configFlags = kLPSPI_MasterPcs0;

    s_transferComplete = false;

    status_t status = LPSPI_MasterTransferNonBlocking(LP_FLEXCOMM4_PERIPHERAL, &s_spiHandle, &transfer);
    if (status != kStatus_Success)
    {
        PRINTF("SPI transfer nonblocking failed, error code: %d\r\n", status);
        return status;
    }

    /* Wait until transfer completes (a simple polling loop; in production code you may wish to use a timeout) */
    while (!s_transferComplete)
    {
        /* Optionally, add a small delay here to reduce CPU usage */
    }
    return kStatus_Success;
}

/*!
 * @brief Sends a USB packet over SPI.
 *
 * This function is intended to be called by your USB host code when a packet
 * must be sent over SPI to the stacked board.
 *
 * @param data Pointer to the data buffer containing the USB packet.
 * @param length Number of bytes in the USB packet.
 */
void SPI_SendUsbPacket(uint8_t *data, size_t length)
{
    status_t status = SPI_Host_Transfer(data, NULL, length);
    if (status != kStatus_Success)
    {
        PRINTF("SPI_SendUsbPacket: transfer failed with status %d\r\n", status);
    }
    else
    {
        PRINTF("SPI_SendUsbPacket: transfer succeeded\r\n");
    }
}
