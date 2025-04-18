/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** USBX Component                                                        */
/**                                                                       */
/**   Device Audio Class                                                  */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    ux_device_class_audio10.h                           PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains all the header and extern functions used by the  */
/*    USBX audio class version 1.0.                                       */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*  04-02-2021     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added volume RES support,   */
/*                                            resulting in version 6.1.6  */
/*  08-02-2021     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added extern "C" keyword    */
/*                                            for compatibility with C++, */
/*                                            resulting in version 6.1.8  */
/*  07-29-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added sampling control,     */
/*                                            resulting in version 6.1.12 */
/*  03-08-2023     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added error checks support, */
/*                                            fixed a macro name,         */
/*                                            resulting in version 6.2.1  */
/*  10-31-2023     Chaoqiong Xiao           Modified comment(s),          */
/*                                            optimized USB descriptors,  */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/

#ifndef UX_DEVICE_CLASS_AUDIO10_H
#define UX_DEVICE_CLASS_AUDIO10_H

/* Determine if a C++ compiler is being used.  If so, ensure that standard 
   C is used to process the API information.  */ 

#ifdef   __cplusplus 

/* Yes, C++ compiler is present.  Use standard C.  */ 
extern   "C" { 

#endif  


/* Internal option: enable the basic USBX error checking. This define is typically used
   while debugging application.  */
#if defined(UX_ENABLE_ERROR_CHECKING) && !defined(UX_DEVICE_CLASS_AUDIO_ENABLE_ERROR_CHECKING)
#define UX_DEVICE_CLASS_AUDIO_ENABLE_ERROR_CHECKING
#endif


/* Define Audio Class specific AC interface descriptor subclasses.  */

#define UX_DEVICE_CLASS_AUDIO10_AC_UNDEFINED                      0x00
#define UX_DEVICE_CLASS_AUDIO10_AC_HEADER                         0x01
#define UX_DEVICE_CLASS_AUDIO10_AC_INPUT_TERMINAL                 0x02
#define UX_DEVICE_CLASS_AUDIO10_AC_OUTPUT_TERMINAL                0x03
#define UX_DEVICE_CLASS_AUDIO10_AC_MIXER_UNIT                     0x04
#define UX_DEVICE_CLASS_AUDIO10_AC_SELECTOR_UNIT                  0x05
#define UX_DEVICE_CLASS_AUDIO10_AC_FEATURE_UNIT                   0x06
#define UX_DEVICE_CLASS_AUDIO10_AC_PROCESSING_UNIT                0x07
#define UX_DEVICE_CLASS_AUDIO10_AC_EXTENSION_UNIT                 0x08


/* Define Audio Class specific AS interface descriptor subclasses.  */

#define UX_DEVICE_CLASS_AUDIO10_AS_UNDEFINED                      0x00
#define UX_DEVICE_CLASS_AUDIO10_AS_GENERAL                        0x01
#define UX_DEVICE_CLASS_AUDIO10_AS_FORMAT_TYPE                    0x02
#define UX_DEVICE_CLASS_AUDIO10_AS_FORMAT_SPECIFIC                0x03


/* Define Audio Class specific endpoint descriptor subtypes.  */

#define UX_DEVICE_CLASS_AUDIO10_EP_UNDEFINED                      0x00
#define UX_DEVICE_CLASS_AUDIO10_EP_GENERAL                        0x01


/* Define Audio Class specific request codes.  */

#define UX_DEVICE_CLASS_AUDIO10_REQUEST_CODE_UNDEFINED            0x00
#define UX_DEVICE_CLASS_AUDIO10_SET_CUR                           0x01
#define UX_DEVICE_CLASS_AUDIO10_GET_CUR                           0x81
#define UX_DEVICE_CLASS_AUDIO10_SET_MIN                           0x02
#define UX_DEVICE_CLASS_AUDIO10_GET_MIN                           0x82
#define UX_DEVICE_CLASS_AUDIO10_SET_MAX                           0x03
#define UX_DEVICE_CLASS_AUDIO10_GET_MAX                           0x83
#define UX_DEVICE_CLASS_AUDIO10_SET_RES                           0x04
#define UX_DEVICE_CLASS_AUDIO10_GET_RES                           0x84
#define UX_DEVICE_CLASS_AUDIO10_SET_MEM                           0x05
#define UX_DEVICE_CLASS_AUDIO10_GET_MEM                           0x85
#define UX_DEVICE_CLASS_AUDIO10_GET_STAT                          0xFF


/* Define Audio Class specific terminal control selectors.  */

#define UX_DEVICE_CLASS_AUDIO10_TE_CONTROL_UNDEFINED              0x00
#define UX_DEVICE_CLASS_AUDIO10_TE_COPY_PROTECT_CONTROL           0x01


/* Define Audio Class specific feature unit control selectors.  */

#define UX_DEVICE_CLASS_AUDIO10_FU_CONTROL_UNDEFINED              0x00
#define UX_DEVICE_CLASS_AUDIO10_FU_MUTE_CONTROL                   0x01
#define UX_DEVICE_CLASS_AUDIO10_FU_VOLUME_CONTROL                 0x02
#define UX_DEVICE_CLASS_AUDIO10_FU_BASS_CONTROL                   0x03
#define UX_DEVICE_CLASS_AUDIO10_FU_MID_CONTROL                    0x04
#define UX_DEVICE_CLASS_AUDIO10_FU_TREBLE_CONTROL                 0x05
#define UX_DEVICE_CLASS_AUDIO10_FU_GRAPHIC_EQUALIZER_CONTROL      0x06
#define UX_DEVICE_CLASS_AUDIO10_FU_AUTOMATIC_GAIN_CONTROL         0x07
#define UX_DEVICE_CLASS_AUDIO10_FU_DELAY_CONTROL                  0x08
#define UX_DEVICE_CLASS_AUDIO10_FU_BASS_BOOST_CONTROL             0x09
#define UX_DEVICE_CLASS_AUDIO10_FU_LOUNDNESS_CONTROL              0x0A


/* Define Audio Class specific endpoint control selectors.  */

#define UX_DEVICE_CLASS_AUDIO10_EP_CONTROL_UNDEFINED              0x00
#define UX_DEVICE_CLASS_AUDIO10_EP_SAMPLING_FREQ_CONTROL          0x01
#define UX_DEVICE_CLASS_AUDIO10_EP_PITCH_CONTROL                  0x02


/* Define Audio Class encoding format types.  */

#define UX_DEVICE_CLASS_AUDIO10_FORMAT_PCM                        1
#define UX_DEVICE_CLASS_AUDIO10_FORMAT_PCM8                       2
#define UX_DEVICE_CLASS_AUDIO10_FORMAT_IEEE_FLOAT                 3
#define UX_DEVICE_CLASS_AUDIO10_FORMAT_ALAW                       4
#define UX_DEVICE_CLASS_AUDIO10_FORMAT_MULAW                      5


/* Audio Class Control interface structures.  */

typedef struct UX_DEVICE_CLASS_AUDIO10_AC_HEADER1_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDescriptorSubtype;
    UCHAR           _align_bcdADC[1];
    USHORT          bcdADC;
    USHORT          wTotalLength;
    UCHAR           bInCollection;
    UCHAR           baInterfaceNr1;
    UCHAR           _align_size[2];
} UX_DEVICE_CLASS_AUDIO10_AC_HEADER1_DESCRIPTOR;

typedef struct UX_DEVICE_CLASS_AUDIO10_AC_HEADER2_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDescriptorSubtype;
    UCHAR           _align_bcdADC[1];
    USHORT          bcdADC;
    USHORT          wTotalLength;
    UCHAR           bInCollection;
    UCHAR           baInterfaceNr1;
    UCHAR           baInterfaceNr2;
    UCHAR           _align_size[1];
} UX_DEVICE_CLASS_AUDIO10_AC_HEADER2_DESCRIPTOR;

typedef struct UX_DEVICE_CLASS_AUDIO10_AC_HEADER3_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDescriptorSubtype;
    UCHAR           _align_bcdADC[1];
    USHORT          bcdADC;
    USHORT          wTotalLength;
    UCHAR           bInCollection;
    UCHAR           baInterfaceNr1;
    UCHAR           baInterfaceNr2;
    UCHAR           baInterfaceNr3;
    UCHAR           _align_size[2];
} UX_DEVICE_CLASS_AUDIO10_AC_HEADER3_DESCRIPTOR;

typedef struct UX_DEVICE_CLASS_AUDIO10_AC_HEADER6_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDescriptorSubtype;
    UCHAR           _align_bcdADC[1];
    USHORT          bcdADC;
    USHORT          wTotalLength;
    UCHAR           bInCollection;
    UCHAR           baInterfaceNr1;
    UCHAR           baInterfaceNr2;
    UCHAR           baInterfaceNr3;
    UCHAR           baInterfaceNr4;
    UCHAR           baInterfaceNr5;
    UCHAR           baInterfaceNr6;
    UCHAR           _align_size[1];
} UX_DEVICE_CLASS_AUDIO10_AC_HEADER6_DESCRIPTOR;

typedef struct UX_DEVICE_CLASS_AUDIO10_AC_HEADER7_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDescriptorSubtype;
    UCHAR           _align_bcdADC[1];
    USHORT          bcdADC;
    USHORT          wTotalLength;
    UCHAR           bInCollection;
    UCHAR           baInterfaceNr1;
    UCHAR           baInterfaceNr2;
    UCHAR           baInterfaceNr3;
    UCHAR           baInterfaceNr4;
    UCHAR           baInterfaceNr5;
    UCHAR           baInterfaceNr6;
    UCHAR           baInterfaceNr7;
} UX_DEVICE_CLASS_AUDIO10_AC_HEADER7_DESCRIPTOR;

typedef struct UX_DEVICE_CLASS_AUDIO10_AC_HEADER8_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDescriptorSubtype;
    UCHAR           _align_bcdADC[1];
    USHORT          bcdADC;
    USHORT          wTotalLength;
    UCHAR           bInCollection;
    UCHAR           baInterfaceNr1;
    UCHAR           baInterfaceNr2;
    UCHAR           baInterfaceNr3;
    UCHAR           baInterfaceNr4;
    UCHAR           baInterfaceNr5;
    UCHAR           baInterfaceNr6;
    UCHAR           baInterfaceNr7;
    UCHAR           baInterfaceNr8;
    UCHAR           _align_size[3];
} UX_DEVICE_CLASS_AUDIO10_AC_HEADER8_DESCRIPTOR;


/* Define Audio Class specific input terminal interface descriptor.  */

typedef struct UX_DEVICE_CLASS_AUDIO10_AC_INPUT_TERMINAL_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDescriptorSubType;
    UCHAR           bTerminalID;
    USHORT          wTerminalType;
    UCHAR           bAssocTerminal;
    UCHAR           bNrChannels;
    USHORT          wChannelConfig;
    UCHAR           iChannelNames;
    UCHAR           iTerminal;
} UX_DEVICE_CLASS_AUDIO10_AC_INPUT_TERMINAL_DESCRIPTOR;


/* Define Audio Class specific output terminal interface descriptor.  */

typedef struct UX_DEVICE_CLASS_AUDIO10_AC_OUTPUT_TERMINAL_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDescriptorSubType;
    UCHAR           bTerminalID;
    USHORT          wTerminalType;
    UCHAR           bAssocTerminal;
    UCHAR           bSourceID;
    UCHAR           iTerminal;
    UCHAR          _align_size[3];
} UX_DEVICE_CLASS_AUDIO10_AC_OUTPUT_TERMINAL_DESCRIPTOR;


/* Define Audio Class specific feature unit descriptor.  */

typedef struct UX_DEVICE_CLASS_AUDIO10_AC_FEATURE_UNIT_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDescriptorSubType;
    UCHAR           bUnitID;
    UCHAR           bSourceID;
    UCHAR           bControlSize;
    UCHAR           _align_bmaControls[2];
    ULONG           bmaControls;
} UX_DEVICE_CLASS_AUDIO10_AC_FEATURE_UNIT_DESCRIPTOR;


/* Define Audio Class streaming interface descriptor.  */

typedef struct UX_DEVICE_CLASS_AUDIO10_AS_INTERFACE_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDescriptorSubtype;
    UCHAR           bTerminalLink;
    UCHAR           bDelay;
    UCHAR           _align_wFormatTag[1];
    USHORT          wFormatTag;
} UX_DEVICE_CLASS_AUDIO10_AS_INTERFACE_DESCRIPTOR;


/* Define Audio Class type I format type descriptor.  */

typedef struct UX_DEVICE_CLASS_AUDIO10_AS_TYPE_I_FORMAT_TYPE_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDescriptorSubtype;
    UCHAR           bFormatType;
    UCHAR           bNrChannels;
    UCHAR           bSubframeSize;
    UCHAR           bBitResolution;
    UCHAR           bSamFreqType;
    ULONG           SamplingFrequency;
} UX_DEVICE_CLASS_AUDIO10_AS_TYPE_I_FORMAT_TYPE_DESCRIPTOR;

#define UX_DEVICE_CLASS_AUDIO_AS_TYPE_I_FORMAT_TYPE_DESCRIPTOR_FORMAT_TYPE    3
#define UX_DEVICE_CLASS_AUDIO_AS_TYPE_I_FORMAT_TYPE_DESCRIPTOR_NR_CHANNELS    4
#define UX_DEVICE_CLASS_AUDIO_AS_TYPE_I_FORMAT_TYPE_DESCRIPTOR_SUBFRAME_SIZE  5
#define UX_DEVICE_CLASS_AUDIO_AS_TYPE_I_FORMAT_TYPE_DESCRIPTOR_BIT_RESOLUTION 6
#define UX_DEVICE_CLASS_AUDIO_AS_TYPE_I_FORMAT_TYPE_DESCRIPTOR_SAM_FREQ_TYPE  7
#define UX_DEVICE_CLASS_AUDIO_AS_TYPE_I_FORMAT_TYPE_DESCRIPTOR_SAM_FREQ_TABLE 8


/* Define Audio Class specific streaming endpoint descriptor.  */

typedef struct UX_DEVICE_CLASS_AUDIO10_AS_ENDPOINT_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDescriptorSubtype;
    UCHAR           bmAttributes;
    USHORT          wMaxPacketSize;
    UCHAR           bInterval;
    UCHAR           bRefresh;
    UCHAR           bSynchAddress;
    UCHAR          _align_size[3];
} UX_DEVICE_CLASS_AUDIO10_AS_ENDPOINT_DESCRIPTOR;

typedef struct UX_DEVICE_CLASS_AUDIO10_AS_DATA_ENDPOINT_DESCRIPTOR_STRUCT
{

    UCHAR           bLength;
    UCHAR           bDescriptorType;
    UCHAR           bDescriptorSubtype;
    UCHAR           bmAttributes;
    UCHAR           bLockDelayUnits;
    UCHAR          _align_wLockDelay[1];
    USHORT          wLockDelay;
} UX_DEVICE_CLASS_AUDIO10_AS_DATA_ENDPOINT_DESCRIPTOR;

typedef struct UX_DEVICE_CLASS_AUDIO10_CONTROL_STRUCT
{
    ULONG           ux_device_class_audio10_control_changed;

    ULONG           ux_device_class_audio10_control_ep_addr;        /* Endpoint address for sampling frequencies control.  */
    UCHAR           *ux_device_class_audio10_control_sam_freq_types;/* Format Type I Descriptor - bSamFreqType and followings.  */
    ULONG           ux_device_class_audio10_control_sam_freq;       /* Current sampling frequency.  */

    ULONG           ux_device_class_audio10_control_fu_id;
    USHORT          ux_device_class_audio10_control_mute[1];
    SHORT           ux_device_class_audio10_control_volume_min[1];
    SHORT           ux_device_class_audio10_control_volume_max[1];
    USHORT          ux_device_class_audio10_control_volume_res[1];
    SHORT           ux_device_class_audio10_control_volume[1];
} UX_DEVICE_CLASS_AUDIO10_CONTROL;

#define UX_DEVICE_CLASS_AUDIO10_CONTROL_MUTE_CHANGED                1u
#define UX_DEVICE_CLASS_AUDIO10_CONTROL_VOLUME_CHANGED              2u
#define UX_DEVICE_CLASS_AUDIO10_CONTROL_FREQUENCY_CHANGED           4u

typedef struct UX_DEVICE_CLASS_AUDIO10_CONTROL_GROUP_STRUCT
{
    ULONG                            ux_device_class_audio10_control_group_controls_nb;
    UX_DEVICE_CLASS_AUDIO10_CONTROL *ux_device_class_audio10_control_group_controls;
} UX_DEVICE_CLASS_AUDIO10_CONTROL_GROUP;

UINT _ux_device_class_audio10_control_process(UX_DEVICE_CLASS_AUDIO *audio,
                                              UX_SLAVE_TRANSFER *transfer_request,
                                              UX_DEVICE_CLASS_AUDIO10_CONTROL_GROUP *group);

UINT _uxe_device_class_audio10_control_process(UX_DEVICE_CLASS_AUDIO *audio,
                                              UX_SLAVE_TRANSFER *transfer_request,
                                              UX_DEVICE_CLASS_AUDIO10_CONTROL_GROUP *group);

#if defined(UX_DEVICE_CLASS_AUDIO_ENABLE_ERROR_CHECKING)

#define ux_device_class_audio10_control_process _uxe_device_class_audio10_control_process

#else

#define ux_device_class_audio10_control_process _ux_device_class_audio10_control_process

#endif

/* Determine if a C++ compiler is being used.  If so, complete the standard 
   C conditional started above.  */   
#ifdef __cplusplus
} 
#endif 

#endif /* ifndef UX_DEVICE_CLASS_AUDIO10_H */
