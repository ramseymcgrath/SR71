/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016,2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "usb_host_config.h"
#if ((defined USB_HOST_CONFIG_CDC) && (USB_HOST_CONFIG_CDC))
#include "usb_host.h"
#include "usb_host_cdc.h"
#include "usb_host_devices.h"

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)

static void USB_HostCdcClearInHaltCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;

    cdcInstance->controlTransfer = NULL;
    if (cdcInstance->inCallbackFn != NULL)
    {
        /* callback to application, the callback function is initialized in USB_HostCdcDataRecv */
        cdcInstance->inCallbackFn(cdcInstance->inCallbackParam, cdcInstance->stallDataBuffer,
                                  cdcInstance->stallDataLength, kStatus_USB_TransferStall);
    }
    (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

static void USB_HostCdcClearOutHaltCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;

    cdcInstance->controlTransfer = NULL;
    if (cdcInstance->outCallbackFn != NULL)
    {
        /* callback to application，the callback function is initialized in USB_HostCdcDataSend */
        cdcInstance->outCallbackFn(cdcInstance->outCallbackParam, cdcInstance->stallDataBuffer,
                                   cdcInstance->stallDataLength, kStatus_USB_TransferStall);
    }
    (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}
static void USB_HostCdcClearInterruptHaltCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;

    cdcInstance->controlTransfer = NULL;
    if (cdcInstance->interruptCallbackFn != NULL)
    {
        /* callback to application */
        cdcInstance->interruptCallbackFn(cdcInstance->interruptCallbackParam, cdcInstance->stallDataBuffer,
                                         cdcInstance->stallDataLength, kStatus_USB_TransferStall);
    }
    (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

static usb_status_t USB_HostCdcClearHalt(usb_host_cdc_instance_struct_t *cdcInstance,
                                         usb_host_transfer_t *stallTransfer,
                                         host_inner_transfer_callback_t callbackFn,
                                         uint8_t endpoint)
{
    usb_status_t status;
    usb_host_transfer_t *transfer;

    /* malloc one transfer */
    status = USB_HostMallocTransfer(cdcInstance->hostHandle, &transfer);
    if (status != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("allocate transfer error\r\n");
#endif
        return status;
    }
    cdcInstance->stallDataBuffer = stallTransfer->transferBuffer;
    cdcInstance->stallDataLength = stallTransfer->transferSofar;
    /* save the application callback function */
    cdcInstance->controlCallbackFn    = NULL;
    cdcInstance->controlCallbackParam = NULL;
    /* initialize transfer */
    transfer->callbackFn                 = callbackFn;
    transfer->callbackParam              = cdcInstance;
    transfer->transferBuffer             = NULL;
    transfer->transferLength             = 0;
    transfer->setupPacket->bRequest      = USB_REQUEST_STANDARD_CLEAR_FEATURE;
    transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_ENDPOINT;
    transfer->setupPacket->wValue  = USB_SHORT_TO_LITTLE_ENDIAN(USB_REQUEST_STANDARD_FEATURE_SELECTOR_ENDPOINT_HALT);
    transfer->setupPacket->wIndex  = USB_SHORT_TO_LITTLE_ENDIAN(endpoint);
    transfer->setupPacket->wLength = 0;
    status                         = USB_HostSendSetup(cdcInstance->hostHandle, cdcInstance->controlPipe, transfer);

    if (status != kStatus_USB_Success)
    {
        (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
    }
    cdcInstance->controlTransfer = transfer;

    return status;
}
#endif

/*!
 * @brief cdc data in pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostCdcDataInPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;
#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    if (status == kStatus_USB_TransferStall)
    {
        if (USB_HostCdcClearHalt(cdcInstance, transfer, USB_HostCdcClearInHaltCallback,
                                 (USB_REQUEST_TYPE_DIR_IN |
                                  ((usb_host_pipe_t *)cdcInstance->inPipe)->endpointAddress)) == kStatus_USB_Success)
        {
            (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
            return;
        }
    }
#endif

    if (cdcInstance->inCallbackFn != NULL)
    {
        /* callback to application, the callback function is initialized in USB_HostCdcDataRecv */
        cdcInstance->inCallbackFn(cdcInstance->inCallbackParam, transfer->transferBuffer, transfer->transferSofar,
                                  status);
    }
    (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

/*!
 * @brief cdc data out pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostCdcDataOutPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;
#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    if (status == kStatus_USB_TransferStall)
    {
        if (USB_HostCdcClearHalt(cdcInstance, transfer, USB_HostCdcClearOutHaltCallback,
                                 (USB_REQUEST_TYPE_DIR_OUT |
                                  ((usb_host_pipe_t *)cdcInstance->outPipe)->endpointAddress)) == kStatus_USB_Success)
        {
            (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
            return;
        }
    }
#endif
    if (cdcInstance->outCallbackFn != NULL)
    {
        /* callback to application，the callback function is initialized in USB_HostCdcDataSend */
        cdcInstance->outCallbackFn(cdcInstance->outCallbackParam, transfer->transferBuffer, transfer->transferSofar,
                                   status);
    }

    (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

/*!
 * @brief cdc data out pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostCdcInterruptPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    if (status == kStatus_USB_TransferStall)
    {
        if (USB_HostCdcClearHalt(
                cdcInstance, transfer, USB_HostCdcClearInterruptHaltCallback,
                (USB_REQUEST_TYPE_DIR_OUT | ((usb_host_pipe_t *)cdcInstance->interruptPipe)->endpointAddress)) ==
            kStatus_USB_Success)
        {
            (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
            return;
        }
    }
#endif

    if (cdcInstance->interruptCallbackFn != NULL)
    {
        cdcInstance->interruptCallbackFn(cdcInstance->interruptCallbackParam, transfer->transferBuffer,
                                         transfer->transferSofar, status);
    }
    (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

/*!
 * @brief cdc data out pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostCdcControlPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;

    if (cdcInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostCdcControl,
        USB_HostCdcSetControlInterface
        or USB_HostCdcSetDataInterface, but is the same function */
        cdcInstance->controlCallbackFn(cdcInstance->controlCallbackParam, transfer->transferBuffer,
                                       transfer->transferSofar, status);
    }
    (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

/*!
 * @brief cdc open data interface.
 *
 * @param cdcInstance     cdc instance pointer.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostCdcOpenDataInterface(usb_host_cdc_instance_struct_t *cdcInstance)
{
    usb_status_t status;
    uint8_t ep_index = 0;
    usb_host_pipe_init_t pipeInit;
    usb_descriptor_endpoint_t *ep_desc = NULL;
    usb_host_interface_t *interfaceHandle;

    if (cdcInstance->inPipe != NULL)
    {
        status = USB_HostClosePipe(cdcInstance->hostHandle, cdcInstance->inPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        cdcInstance->inPipe = NULL;
    }

    if (cdcInstance->outPipe != NULL)
    {
        status = USB_HostClosePipe(cdcInstance->hostHandle, cdcInstance->outPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        cdcInstance->outPipe = NULL;
    }
    status = USB_HostOpenDeviceInterface(cdcInstance->deviceHandle, cdcInstance->dataInterfaceHandle);
    if (status != kStatus_USB_Success)
    {
        return status;
    }
    /* open interface pipes */
    interfaceHandle = (usb_host_interface_t *)cdcInstance->dataInterfaceHandle;

    for (ep_index = 0; ep_index < interfaceHandle->epCount; ++ep_index)
    {
        ep_desc = interfaceHandle->epList[ep_index].epDesc;
        if (((ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
             USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
            ((ep_desc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_BULK))
        {
            pipeInit.devInstance     = cdcInstance->deviceHandle;
            pipeInit.pipeType        = USB_ENDPOINT_BULK;
            pipeInit.direction       = USB_IN;
            pipeInit.endpointAddress = (ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.interval        = ep_desc->bInterval;
            pipeInit.maxPacketSize   = (uint16_t)((USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                                 USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK));
            pipeInit.numberPerUframe = (uint8_t)((USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                                  USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK));
            pipeInit.nakCount        = USB_HOST_CONFIG_MAX_NAK;

            cdcInstance->bulkInPacketSize = pipeInit.maxPacketSize;
            status                        = USB_HostOpenPipe(cdcInstance->hostHandle, &cdcInstance->inPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("usb_host_audio_set_interface fail to open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
        }
        else if (((ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                  USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT) &&
                 ((ep_desc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_BULK))
        {
            pipeInit.devInstance     = cdcInstance->deviceHandle;
            pipeInit.pipeType        = USB_ENDPOINT_BULK;
            pipeInit.direction       = USB_OUT;
            pipeInit.endpointAddress = (ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.interval        = ep_desc->bInterval;
            pipeInit.maxPacketSize   = (uint16_t)((USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                                 USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK));
            pipeInit.numberPerUframe = (uint8_t)((USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                                  USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK));
            pipeInit.nakCount        = USB_HOST_CONFIG_MAX_NAK;

            cdcInstance->bulkOutPacketSize = pipeInit.maxPacketSize;
            status = USB_HostOpenPipe(cdcInstance->hostHandle, &cdcInstance->outPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("usb_host_cdc_set_dat_interface fail to open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
        }
        else
        {
        }
    }
    return kStatus_USB_Success;
}

/*!
 * @brief cdc set data interface callback, open pipes.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostCdcSetDataInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;

    cdcInstance->controlTransfer = NULL;
    if (status == kStatus_USB_Success)
    {
        status = USB_HostCdcOpenDataInterface(cdcInstance);
    }

    if (cdcInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostCdcControl,
        USB_HostCdcSetControlInterface
        or USB_HostCdcSetDataInterface, but is the same function */
        cdcInstance->controlCallbackFn(cdcInstance->controlCallbackParam, NULL, 0, status);
    }
    (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

/*!
 * @brief cdc open control interface.
 *
 * @param cdcInstance     cdc instance pointer.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostCdcOpenControlInterface(usb_host_cdc_instance_struct_t *cdcInstance)
{
    usb_status_t status;
    uint8_t ep_index = 0;
    usb_host_pipe_init_t pipeInit;
    usb_descriptor_endpoint_t *ep_desc = NULL;
    usb_host_interface_t *interfaceHandle;

    if (cdcInstance->interruptPipe != NULL)
    {
        status = USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->interruptPipe, NULL);
        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe\r\n");
#endif
        }
        status = USB_HostClosePipe(cdcInstance->hostHandle, cdcInstance->interruptPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        cdcInstance->interruptPipe = NULL;
    }

    status = USB_HostOpenDeviceInterface(cdcInstance->deviceHandle, cdcInstance->controlInterfaceHandle);
    if (status != kStatus_USB_Success)
    {
        return status;
    }
    /* open interface pipes */
    interfaceHandle = (usb_host_interface_t *)cdcInstance->controlInterfaceHandle;

    for (ep_index = 0; ep_index < interfaceHandle->epCount; ++ep_index)
    {
        ep_desc = interfaceHandle->epList[ep_index].epDesc;
        if (((ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
             USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
            ((ep_desc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_INTERRUPT))
        {
            pipeInit.devInstance     = cdcInstance->deviceHandle;
            pipeInit.pipeType        = USB_ENDPOINT_INTERRUPT;
            pipeInit.direction       = USB_IN;
            pipeInit.endpointAddress = (ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.interval        = ep_desc->bInterval;
            pipeInit.maxPacketSize   = (uint16_t)((USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                                 USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK));
            pipeInit.numberPerUframe = (uint8_t)((USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                                  USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK));
            pipeInit.nakCount        = USB_HOST_CONFIG_MAX_NAK;

            cdcInstance->packetSize = pipeInit.maxPacketSize;

            status = USB_HostOpenPipe(cdcInstance->hostHandle, &cdcInstance->interruptPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("USB_HostCdcSetControlInterface fail to open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
        }
    }
    return kStatus_USB_Success;
}

/*!
 * @brief cdc set control interface callback, open pipes.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostCdcSetContorlInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;

    cdcInstance->controlTransfer = NULL;
    if (status == kStatus_USB_Success)
    {
        status = USB_HostCdcOpenControlInterface(cdcInstance);
    }

    if (cdcInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostCdcControl,
        USB_HostCdcSetControlInterface
        or USB_HostCdcSetDataInterface, but is the same function */
        cdcInstance->controlCallbackFn(cdcInstance->controlCallbackParam, NULL, 0, status);
    }
    (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

/*!
 * @brief initialize the cdc instance.
 *
 * This function allocate the resource for cdc instance.
 *
 * @param deviceHandle       the device handle.
 * @param classHandle return class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
usb_status_t USB_HostCdcInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle)
{
    usb_host_cdc_instance_struct_t *control_ptr =
        (usb_host_cdc_instance_struct_t *)OSA_MemoryAllocate(sizeof(usb_host_cdc_instance_struct_t));
    uint32_t info_value = 0U;
    void *temp;

    if (control_ptr == NULL)
    {
        return kStatus_USB_AllocFail;
    }

    control_ptr->deviceHandle           = deviceHandle;
    control_ptr->controlInterfaceHandle = NULL;
    control_ptr->dataInterfaceHandle    = NULL;
    (void)USB_HostHelperGetPeripheralInformation(deviceHandle, (uint32_t)kUSB_HostGetHostHandle, &info_value);
    temp                    = (uint32_t *)info_value;
    control_ptr->hostHandle = (usb_host_handle)temp;
    (void)USB_HostHelperGetPeripheralInformation(deviceHandle, (uint32_t)kUSB_HostGetDeviceControlPipe, &info_value);
    temp                     = (uint32_t *)info_value;
    control_ptr->controlPipe = (usb_host_pipe_handle)temp;

    *classHandle = control_ptr;
    return kStatus_USB_Success;
}

/*!
 * @brief set control interface.
 *
 * This function bind the control interface with the cdc instance.
 *
 * @param classHandle      the class handle.
 * @param interfaceHandle  the control interface handle.
 * @param alternateSetting the alternate setting value.
 * @param callbackFn       this callback is called after this function completes.
 * @param callbackParam    the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 * @retval kStatus_USB_Busy           callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  callback return status, the transfer is stall by device.
 * @retval kStatus_USB_Error          callback return status, open pipe fail, please reference to USB_HostOpenPipe.
 */
usb_status_t USB_HostCdcSetControlInterface(usb_host_class_handle classHandle,
                                            usb_host_interface_handle interfaceHandle,
                                            uint8_t alternateSetting,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam)
{
    usb_status_t status;
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_host_transfer_t *transfer;

    status = kStatus_USB_Success;
    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidParameter;
    }

    cdcInstance->controlInterfaceHandle = interfaceHandle;

    /* cancel transfers */
    if (cdcInstance->interruptPipe != NULL)
    {
        status = USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->interruptPipe, NULL);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe\r\n");
#endif
        }
    }

    if (alternateSetting == 0U)
    {
        if (callbackFn != NULL)
        {
            status = USB_HostCdcOpenControlInterface(cdcInstance);
            callbackFn(callbackParam, NULL, 0U, status);
        }
    }
    else
    {
        if (USB_HostMallocTransfer(cdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error to get transfer\r\n");
#endif
            return kStatus_USB_Error;
        }
        cdcInstance->controlCallbackFn    = callbackFn;
        cdcInstance->controlCallbackParam = callbackParam;
        /* initialize transfer */
        transfer->callbackFn                 = USB_HostCdcSetContorlInterfaceCallback;
        transfer->callbackParam              = cdcInstance;
        transfer->setupPacket->bRequest      = USB_REQUEST_STANDARD_SET_INTERFACE;
        transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
        transfer->setupPacket->wIndex        = USB_SHORT_TO_LITTLE_ENDIAN(
            ((usb_host_interface_t *)cdcInstance->controlInterfaceHandle)->interfaceDesc->bInterfaceNumber);
        transfer->setupPacket->wValue  = USB_SHORT_TO_LITTLE_ENDIAN(alternateSetting);
        transfer->setupPacket->wLength = 0;
        transfer->transferBuffer       = NULL;
        transfer->transferLength       = 0;
        status                         = USB_HostSendSetup(cdcInstance->hostHandle, cdcInstance->controlPipe, transfer);

        if (status == kStatus_USB_Success)
        {
            cdcInstance->controlTransfer = transfer;
        }
        else
        {
            (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
        }
    }

    return status;
}

/*!
 * @brief set data interface.
 *
 * This function bind the control interface with the cdc instance.
 *
 * @param classHandle      the class handle.
 * @param interfaceHandle  the data interface handle.
 * @param alternateSetting the alternate setting value.
 * @param callbackFn       this callback is called after this function completes.
 * @param callbackParam    the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 * @retval kStatus_USB_Busy           callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  callback return status, the transfer is stall by device.
 * @retval kStatus_USB_Error          callback return status, open pipe fail, please reference to USB_HostOpenPipe.
 */
usb_status_t USB_HostCdcSetDataInterface(usb_host_class_handle classHandle,
                                         usb_host_interface_handle interfaceHandle,
                                         uint8_t alternateSetting,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam)
{
    usb_status_t status;
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_host_transfer_t *transfer;

    status = kStatus_USB_Success;
    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidParameter;
    }

    cdcInstance->dataInterfaceHandle = interfaceHandle;

    /* cancel transfers */
    if (cdcInstance->inPipe != NULL)
    {
        status = USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->inPipe, NULL);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe\r\n");
#endif
        }
    }

    if (cdcInstance->outPipe != NULL)
    {
        status = USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->outPipe, NULL);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe\r\n");
#endif
        }
    }

#if defined(USB_HOST_CONFIG_CDC_ECM) && USB_HOST_CONFIG_CDC_ECM
    usb_host_interface_t *ifPointer  = (usb_host_interface_t *)interfaceHandle;
    usb_descriptor_union_t *ifBuffer = (usb_descriptor_union_t *)(ifPointer->interfaceExtension);
    uint32_t extIndex                = 0;

    for (; extIndex < ifPointer->interfaceExtensionLength;
         extIndex += ifBuffer->common.bLength,
         ifBuffer = (usb_descriptor_union_t *)((uint8_t *)ifBuffer + ifBuffer->common.bLength))
    {
        if (ifBuffer->common.bDescriptorType == USB_DESCRIPTOR_TYPE_INTERFACE &&
            ifBuffer->interface.bAlternateSetting == alternateSetting)
        {
            ifPointer->epCount = ifBuffer->interface.bNumEndpoints;
            break;
        }
    }

    for (uint32_t epIndex = 0; epIndex < ifPointer->epCount && extIndex < ifPointer->interfaceExtensionLength;
         extIndex += ifBuffer->common.bLength,
                  ifBuffer = (usb_descriptor_union_t *)((uint8_t *)ifBuffer + ifBuffer->common.bLength))
    {
        if (ifBuffer->common.bDescriptorType == USB_DESCRIPTOR_TYPE_ENDPOINT)
        {
            ifPointer->epList[epIndex].epDesc = (usb_descriptor_endpoint_t *)ifBuffer;
            epIndex++;
        }
    }
#endif

    if (alternateSetting == 0U)
    {
        if (callbackFn != NULL)
        {
            status = USB_HostCdcOpenDataInterface(cdcInstance);
            callbackFn(callbackParam, NULL, 0, status);
        }
    }
    else
    {
        if (USB_HostMallocTransfer(cdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error to get transfer\r\n");
#endif
            return kStatus_USB_Error;
        }
        cdcInstance->controlCallbackFn    = callbackFn;
        cdcInstance->controlCallbackParam = callbackParam;
        /* initialize transfer */
        transfer->callbackFn                 = USB_HostCdcSetDataInterfaceCallback;
        transfer->callbackParam              = cdcInstance;
        transfer->setupPacket->bRequest      = USB_REQUEST_STANDARD_SET_INTERFACE;
        transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
        transfer->setupPacket->wIndex        = USB_SHORT_TO_LITTLE_ENDIAN(
            ((usb_host_interface_t *)cdcInstance->dataInterfaceHandle)->interfaceDesc->bInterfaceNumber);
        transfer->setupPacket->wValue  = USB_SHORT_TO_LITTLE_ENDIAN(alternateSetting);
        transfer->setupPacket->wLength = 0;
        transfer->transferBuffer       = NULL;
        transfer->transferLength       = 0;
        status                         = USB_HostSendSetup(cdcInstance->hostHandle, cdcInstance->controlPipe, transfer);

        if (status == kStatus_USB_Success)
        {
            cdcInstance->controlTransfer = transfer;
        }
        else
        {
            (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
        }
    }

    return status;
}

/*!
 * @brief de-initialize the cdc instance.
 *
 * This function release the resource for cdc instance.
 *
 * @param deviceHandle   the device handle.
 * @param classHandle    the class handle.
 *
 * @retval kStatus_USB_Success        The device is de-initialized successfully.
 */
usb_status_t USB_HostCdcDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle)
{
    usb_status_t status                         = kStatus_USB_Error;
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;

    if (deviceHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (classHandle != NULL)
    {
        if (cdcInstance->interruptPipe != NULL)
        {
            status = USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->interruptPipe, NULL);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when Cancel pipe\r\n");
#endif
            }
            status = USB_HostClosePipe(cdcInstance->hostHandle, cdcInstance->interruptPipe);

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
            cdcInstance->interruptPipe = NULL;
        }

        (void)USB_HostCloseDeviceInterface(deviceHandle, cdcInstance->controlInterfaceHandle);

        if (cdcInstance->inPipe != NULL)
        {
            status = USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->inPipe, NULL);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when Cancel pipe\r\n");
#endif
            }
            status = USB_HostClosePipe(cdcInstance->hostHandle, cdcInstance->inPipe);

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
            cdcInstance->inPipe = NULL;
        }
        if (cdcInstance->outPipe != NULL)
        {
            status = USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->outPipe, NULL);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when Cancel pipe\r\n");
#endif
            }
            status = USB_HostClosePipe(cdcInstance->hostHandle, cdcInstance->outPipe);

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
            cdcInstance->outPipe = NULL;
        }
        if ((cdcInstance->controlPipe != NULL) && (cdcInstance->controlTransfer != NULL))
        {
            status =
                USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->controlPipe, cdcInstance->controlTransfer);
        }
        (void)USB_HostCloseDeviceInterface(deviceHandle, cdcInstance->dataInterfaceHandle);

        OSA_MemoryFree(cdcInstance);
    }
    else
    {
        (void)USB_HostCloseDeviceInterface(deviceHandle, NULL);
        /*add for misra*/
        status = kStatus_USB_Success;
    }

    return status;
}

/*!
 * @brief receive data.
 *
 * This function implements cdc receiving data.
 *
 * @param classHandle    the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength   the buffer length.
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        receive request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostRecv.
 */
usb_status_t USB_HostCdcDataRecv(usb_host_class_handle classHandle,
                                 uint8_t *buffer,
                                 uint32_t bufferLength,
                                 transfer_callback_t callbackFn,
                                 void *callbackParam)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (cdcInstance->inPipe == NULL)
    {
        return kStatus_USB_Error;
    }

    if (USB_HostMallocTransfer(cdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    cdcInstance->inCallbackFn    = callbackFn;
    cdcInstance->inCallbackParam = callbackParam;
    transfer->transferBuffer     = buffer;
    transfer->transferLength     = bufferLength;
    transfer->callbackFn         = USB_HostCdcDataInPipeCallback;
    transfer->callbackParam      = cdcInstance;

    if (USB_HostRecv(cdcInstance->hostHandle, cdcInstance->inPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failed to USB_HostRecv\r\n");
#endif
        (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }

    return kStatus_USB_Success;
}

/*!
 * @brief send data.
 *
 * This function implements cdc sending data.
 *
 * @param classHandle    the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength   the buffer length.
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success          receive request successfully.
 * @retval kStatus_USB_InvalidHandle    The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy             There is no idle transfer.
 * @retval kStatus_USB_Error            pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostSend.
 */
usb_status_t USB_HostCdcDataSend(usb_host_class_handle classHandle,
                                 uint8_t *buffer,
                                 uint32_t bufferLength,
                                 transfer_callback_t callbackFn,
                                 void *callbackParam)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (cdcInstance->outPipe == NULL)
    {
        return kStatus_USB_Error;
    }

    if (USB_HostMallocTransfer(cdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    cdcInstance->outCallbackFn    = callbackFn;
    cdcInstance->outCallbackParam = callbackParam;
    transfer->transferBuffer      = buffer;
    transfer->transferLength      = bufferLength;
    transfer->callbackFn          = USB_HostCdcDataOutPipeCallback;
    transfer->callbackParam       = cdcInstance;

    if (USB_HostSend(cdcInstance->hostHandle, cdcInstance->outPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failed to USB_HostSend\r\n");
#endif
        (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    return kStatus_USB_Success;
}

/*!
 * @brief interrupt receive data.
 *
 * This function implements interrupt receiving data.
 *
 * @param classHandle     the class handle.
 * @param buffer          the buffer pointer.
 * @param bufferLength    the buffer length.
 * @param callbackFn      this callback is called after this function completes.
 * @param callbackParam   the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success         receive request successfully.
 * @retval kStatus_USB_InvalidHandle   The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy            There is no idle transfer.
 * @retval kStatus_USB_Error           pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostRecv.
 */
usb_status_t USB_HostCdcInterruptRecv(usb_host_class_handle classHandle,
                                      uint8_t *buffer,
                                      uint32_t bufferLength,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (cdcInstance->interruptPipe == NULL)
    {
        return kStatus_USB_Error;
    }

    if (USB_HostMallocTransfer(cdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    cdcInstance->interruptCallbackFn    = callbackFn;
    cdcInstance->interruptCallbackParam = callbackParam;
    transfer->transferBuffer            = buffer;
    transfer->transferLength            = bufferLength;
    transfer->callbackFn                = USB_HostCdcInterruptPipeCallback;
    transfer->callbackParam             = cdcInstance;

    if (USB_HostRecv(cdcInstance->hostHandle, cdcInstance->interruptPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failed to usb_interrupt_recv\r\n");
#endif
        (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    return kStatus_USB_Success;
}
/*!
 * @brief get pipe max packet size.
 *
 * @param[in] classHandle the class handle.
 * @param[in] pipeType    It's value is USB_ENDPOINT_CONTROL, USB_ENDPOINT_ISOCHRONOUS, USB_ENDPOINT_BULK or
 * USB_ENDPOINT_INTERRUPT.
 *                        Please reference to usb_spec.h
 * @param[in] direction   pipe direction.
 *
 * @retval 0        The classHandle is NULL.
 * @retval max packet size.
 */
uint16_t USB_HostCdcGetPacketsize(usb_host_class_handle classHandle, uint8_t pipeType, uint8_t direction)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    if (classHandle == NULL)
    {
        return 0;
    }

    if (pipeType == USB_ENDPOINT_BULK)
    {
        if (direction == USB_IN)
        {
            return cdcInstance->bulkInPacketSize;
        }
        else
        {
            return cdcInstance->bulkOutPacketSize;
        }
    }

    return 0;
}

/*!
 * @brief cdc send control transfer common code.
 *
 * @param classHandle    the class handle.
 * @param request_type   setup packet request type.
 * @param request        setup packet request value.
 * @param wvalue_l       setup packet wvalue low byte.
 * @param wvalue_h       setup packet wvalue high byte.
 * @param wlength        setup packet wlength value.
 * @param data           data buffer pointer
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @return An error code or kStatus_USB_Success.
 */
usb_status_t USB_HostCdcControl(usb_host_class_handle classHandle,
                                uint8_t request_type,
                                uint8_t request,
                                uint8_t wvalue_l,
                                uint8_t wvalue_h,
                                uint16_t wlength,
                                uint8_t *data,
                                transfer_callback_t callbackFn,
                                void *callbackParam)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (USB_HostMallocTransfer(cdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    cdcInstance->controlCallbackFn    = callbackFn;
    cdcInstance->controlCallbackParam = callbackParam;

    transfer->transferBuffer             = data;
    transfer->transferLength             = wlength;
    transfer->callbackFn                 = USB_HostCdcControlPipeCallback;
    transfer->callbackParam              = cdcInstance;
    transfer->setupPacket->bmRequestType = request_type;
    transfer->setupPacket->bRequest      = request;
    transfer->setupPacket->wValue        = USB_SHORT_TO_LITTLE_ENDIAN(wvalue_l | (uint16_t)((uint16_t)wvalue_h << 8));
    transfer->setupPacket->wIndex        = USB_SHORT_TO_LITTLE_ENDIAN(
        ((usb_host_interface_t *)cdcInstance->controlInterfaceHandle)->interfaceDesc->bInterfaceNumber);
    transfer->setupPacket->wLength = USB_SHORT_TO_LITTLE_ENDIAN(wlength);

    if (USB_HostSendSetup(cdcInstance->hostHandle, cdcInstance->controlPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failed for USB_HostSendSetup\r\n");
#endif
        (void)USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    cdcInstance->controlTransfer = transfer;

    return kStatus_USB_Success;
}

/*!
 * @brief cdc get line coding.
 *
 * This function implements cdc GetLineCoding request.refer to pstn spec.
 *
 * @param classHandle    the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength   the buffer length.
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 */
usb_status_t USB_HostCdcGetAcmLineCoding(usb_host_class_handle classHandle,
                                         usb_host_cdc_line_coding_struct_t *uartLineCoding,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam)
{
    return USB_HostCdcControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_CDC_GET_LINE_CODING, 0, 0, 7, (uint8_t *)uartLineCoding, callbackFn, callbackParam);
}

/*!
 * @brief cdc set line coding.
 *
 * This function implements cdc SetLineCoding request.refer to pstn spec.
 *
 * @param classHandle    the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength   the buffer length.
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 */
usb_status_t USB_HostCdcSetAcmLineCoding(usb_host_class_handle classHandle,
                                         usb_host_cdc_line_coding_struct_t *uartLineCoding,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam)
{
    return USB_HostCdcControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_CDC_SET_LINE_CODING, 0, 0, 7, (uint8_t *)uartLineCoding, callbackFn, callbackParam);
}

/*!
 * @brief cdc setControlLineState.
 *
 * This function implements cdc etControlLineState request.refer to pstn spec.
 *
 * @param classHandle   the class handle.
 * @param buffer        the buffer pointer.
 * @param bufferLength  the buffer length.
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 */
usb_status_t USB_HostCdcSetAcmCtrlState(
    usb_host_class_handle classHandle, uint8_t dtr, uint8_t rts, transfer_callback_t callbackFn, void *callbackParam)
{
    uint16_t lineState = 0U;

    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;

    lineState = (0U != dtr) ? USB_HOST_CDC_CONTROL_LINE_STATE_DTR : 0U;
    lineState |= (0U != rts) ? USB_HOST_CDC_CONTROL_LINE_STATE_RTS : 0U;
    return USB_HostCdcControl(
        cdcInstance, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_CDC_SET_CONTROL_LINE_STATE, (uint8_t)USB_SHORT_GET_LOW(lineState), USB_SHORT_GET_HIGH(lineState), 0U,
        NULL, callbackFn, callbackParam);
}

/*!
 * @brief cdc send encapsulated command.
 *
 * This function implements cdc SEND_ENCAPSULATED_COMMAND request.refer to cdc 1.2 spec.
 *
 * @param classHandle    the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength   the buffer length.
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 */
usb_status_t USB_HostCdcSendEncapsulatedCommand(usb_host_class_handle classHandle,
                                                uint8_t *buffer,
                                                uint16_t bufferLength,
                                                transfer_callback_t callbackFn,
                                                void *callbackParam)
{
    return USB_HostCdcControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_CDC_SEND_ENCAPSULATED_COMMAND, 0U, 0U, bufferLength, buffer, callbackFn, callbackParam);
}

/*!
 * @brief cdc get encapsulated response.
 *
 * This function implements cdc GET_ENCAPSULATED_RESPONSE request.refer to cdc 1.2 spec.
 *
 * @param classHandle    the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength   the buffer length.
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 */
usb_status_t USB_HostCdcGetEncapsulatedResponse(usb_host_class_handle classHandle,
                                                uint8_t *buffer,
                                                uint16_t bufferLength,
                                                transfer_callback_t callbackFn,
                                                void *callbackParam)
{
    return USB_HostCdcControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_CDC_GET_ENCAPSULATED_RESPONSE, 0, 0, bufferLength, buffer, callbackFn, callbackParam);
}

/*!
 * @brief cdc get acm descriptor.
 *
 * This function is hunting for class specific acm decriptor in the configuration ,get the corresponding
 * descriptor .
 *
 * @param classHandle          the class handle.
 * @param headDesc             the head function descriptor pointer.
 * @param callManageDesc       the call management functional descriptor pointer.
 * @param abstractControlDesc  the abstract control management functional pointer.
 * @param unionInterfaceDesc   the union functional descriptor pointer.
 *
 * @retval kStatus_USB_Error          analyse descriptor error.
 */
usb_status_t USB_HostCdcGetAcmDescriptor(usb_host_class_handle classHandle,
                                         usb_host_cdc_head_function_desc_struct_t **headDesc,
                                         usb_host_cdc_call_manage_desc_struct_t **callManageDesc,
                                         usb_host_cdc_abstract_control_desc_struct_t **abstractControlDesc,
                                         usb_host_cdc_union_interface_desc_struct_t **unionInterfaceDesc)
{
    usb_status_t status;
    usb_descriptor_union_t *ptr1;
    uint32_t end_address;
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_cdc_func_desc_struct_t *cdc_common_ptr;
    usb_host_interface_t *interface_handle;
    void *temp;

    status           = kStatus_USB_Success;
    interface_handle = (usb_host_interface_t *)cdcInstance->controlInterfaceHandle;
    temp             = (void *)interface_handle->interfaceExtension;
    ptr1             = (usb_descriptor_union_t *)temp;
    end_address      = (uint32_t)(interface_handle->interfaceExtension + interface_handle->interfaceExtensionLength);

    while ((uint32_t)ptr1 < end_address)
    {
        temp           = (void *)&ptr1->common;
        cdc_common_ptr = (usb_cdc_func_desc_struct_t *)temp;
        switch (cdc_common_ptr->common.bDescriptorSubtype)
        {
            case USB_HOST_DESC_SUBTYPE_HEADER:
                *headDesc = &cdc_common_ptr->head;
                if ((((uint32_t)((*headDesc)->bcdCDC[1U]) << 8U) + (*headDesc)->bcdCDC[0U]) > 0x0110U)
                {
                    status = kStatus_USB_Error;
                }
                break;
            case USB_HOST_DESC_SUBTYPE_UNION:
                if (cdc_common_ptr->unionDesc.bControlInterface == interface_handle->interfaceDesc->bInterfaceNumber)
                {
                    *unionInterfaceDesc = &cdc_common_ptr->unionDesc;
                }
                else
                {
                    status = kStatus_USB_Error;
                }
                break;
            case USB_HOST_DESC_SUBTYPE_CM:
                *callManageDesc = &cdc_common_ptr->callManage;
                break;
            case USB_HOST_DESC_SUBTYPE_ACM:
                *abstractControlDesc = &cdc_common_ptr->acm;
                break;
            default:
                /*no action*/
                break;
        }

        if (kStatus_USB_Success != status)
        {
            break;
        }
        temp = (void *)((uint8_t *)ptr1 + ptr1->common.bLength);
        ptr1 = (usb_descriptor_union_t *)temp;
    }
    cdcInstance->headDesc            = *headDesc;
    cdcInstance->callManageDesc      = *callManageDesc;
    cdcInstance->abstractControlDesc = *abstractControlDesc;
    cdcInstance->unionInterfaceDesc  = *unionInterfaceDesc;
    return status;
}

usb_status_t USB_HostCdcGetEcmDescriptor(usb_host_class_handle classHandle,
                                         usb_host_cdc_head_function_desc_struct_t **headDesc,
                                         usb_host_cdc_union_interface_desc_struct_t **unionInterfaceDesc,
                                         usb_host_cdc_ethernet_networking_desc_struct_t **ethernetNetworkingDesc)
{
    usb_status_t status = kStatus_USB_Success;

    usb_host_cdc_head_function_desc_struct_t *ifHeadDesc                     = NULL;
    usb_host_cdc_union_interface_desc_struct_t *ifUnionInterfaceDesc         = NULL;
    usb_host_cdc_ethernet_networking_desc_struct_t *ifEthernetNetworkingDesc = NULL;

    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_host_interface_t *ifPointer             = (usb_host_interface_t *)cdcInstance->controlInterfaceHandle;
    usb_descriptor_union_t *ifBuffer            = (usb_descriptor_union_t *)(ifPointer->interfaceExtension);
    usb_cdc_func_desc_struct_t *funcBuffer;

    for (uint32_t ifIndex  = 0; ifIndex < ifPointer->interfaceExtensionLength; ifIndex += ifBuffer->common.bLength,
                  ifBuffer = (usb_descriptor_union_t *)((uint8_t *)ifBuffer + ifBuffer->common.bLength))
    {
        if (ifBuffer->common.bDescriptorType == 0x24U)
        {
            funcBuffer = (usb_cdc_func_desc_struct_t *)ifBuffer;
            switch (funcBuffer->common.bDescriptorSubtype)
            {
                case USB_HOST_DESC_SUBTYPE_HEADER:
                    ifHeadDesc = &funcBuffer->head;
                    if ((((uint32_t)(ifHeadDesc->bcdCDC[1U]) << 8U) + ifHeadDesc->bcdCDC[0U]) > 0x0110U)
                    {
                        status = kStatus_USB_Error;
                    }
                    break;

                case USB_HOST_DESC_SUBTYPE_UNION:
                    if (funcBuffer->unionDesc.bControlInterface == ifPointer->interfaceDesc->bInterfaceNumber)
                    {
                        ifUnionInterfaceDesc = &funcBuffer->unionDesc;
                    }
                    else
                    {
                        status = kStatus_USB_Error;
                    }
                    break;

                case USB_HOST_DESC_SUBTYPE_ECM:
                    ifEthernetNetworkingDesc            = &funcBuffer->ecm;
                    cdcInstance->headDesc               = ifHeadDesc;
                    cdcInstance->unionInterfaceDesc     = ifUnionInterfaceDesc;
                    cdcInstance->ethernetNetworkingDesc = ifEthernetNetworkingDesc;
                    if (headDesc)
                    {
                        *headDesc = cdcInstance->headDesc;
                    }
                    if (unionInterfaceDesc)
                    {
                        *unionInterfaceDesc = cdcInstance->unionInterfaceDesc;
                    }
                    if (ethernetNetworkingDesc)
                    {
                        *ethernetNetworkingDesc = cdcInstance->ethernetNetworkingDesc;
                    }
                    break;

                default:
                    break;
            }
            if (status != kStatus_USB_Success)
            {
                break;
            }
        }
    }
    return status;
}
#endif
