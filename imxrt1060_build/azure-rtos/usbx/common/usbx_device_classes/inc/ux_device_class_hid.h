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
/**   HID Class                                                           */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    ux_device_class_hid.h                               PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains all the header and extern functions used by the  */
/*    USBX HID class.                                                     */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            used UX prefix to refer to  */
/*                                            TX symbols instead of using */
/*                                            them directly,              */
/*                                            resulting in version 6.1    */
/*  12-31-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added Get/Set Protocol      */
/*                                            request support,            */
/*                                            resulting in version 6.1.3  */
/*  08-02-2021     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added extern "C" keyword    */
/*                                            for compatibility with C++, */
/*                                            resulting in version 6.1.8  */
/*  01-31-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added standalone support,   */
/*                                            added interrupt OUT support,*/
/*                                            resulting in version 6.1.10 */
/*  04-25-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added receiver callback,    */
/*                                            resulting in version 6.1.11 */
/*  07-29-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added standalone int out,   */
/*                                            resulting in version 6.1.12 */
/*  10-31-2023     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added zero copy support,    */
/*                                            added a new mode to manage  */
/*                                            endpoint buffer in classes, */
/*                                            moved build option check,   */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/

#ifndef UX_DEVICE_CLASS_HID_H
#define UX_DEVICE_CLASS_HID_H

/* Determine if a C++ compiler is being used.  If so, ensure that standard
   C is used to process the API information.  */

#ifdef   __cplusplus

/* Yes, C++ compiler is present.  Use standard C.  */
extern   "C" {

#endif

/* Device HID Compile Options.  */

/* If defined, interrupt OUT transfer is supported.  */
/* #define UX_DEVICE_CLASS_HID_INTERRUPT_OUT_SUPPORT  */


/* Internal option: enable the basic USBX error checking. This define is typically used
   while debugging application.  */
#if defined(UX_ENABLE_ERROR_CHECKING) && !defined(UX_DEVICE_CLASS_HID_ENABLE_ERROR_CHECKING)
#define UX_DEVICE_CLASS_HID_ENABLE_ERROR_CHECKING
#endif


/* Use UX general thread stack size for receiver thread.  */
#define UX_DEVICE_CLASS_HID_RECEIVER_THREAD_STACK_SIZE              UX_THREAD_STACK_SIZE

/* Use UX general thread stack size for HID class thread.  */
#define UX_DEVICE_CLASS_HID_THREAD_STACK_SIZE                       UX_THREAD_STACK_SIZE

/* Interrupt out endpoint buffer size, must be larger than endpoint, and aligned in 4-bytes.  */
#define UX_DEVICE_CLASS_HID_INTERRUPTIN_BUFFER_SIZE                 UX_DEVICE_CLASS_HID_EVENT_BUFFER_LENGTH

/* Interrupt in endpoint buffer size, must be larger than endpoint, and aligned in 4-bytes.  */
#ifdef UX_DEVICE_CLASS_HID_INTERRUPT_OUT_SUPPORT
#define UX_DEVICE_CLASS_HID_INTERRUPTOUT_BUFFER_SIZE                8
#else
#define UX_DEVICE_CLASS_HID_INTERRUPTOUT_BUFFER_SIZE                0
#endif

/* Option: defined, it enables zero copy support (works if HID owns endpoint buffer).  */
/* #define UX_DEVICE_CLASS_HID_ZERO_COPY  */

/* Option: defined, it enables initialize parameters to set event queue size and event max length.
    If disabled, the default value is UX_DEVICE_CLASS_HID_MAX_EVENTS_QUEUE for queue size
     and UX_DEVICE_CLASS_HID_EVENT_BUFFER_LENGTH for event max length.
    If enabled, the parameter _event_max_number (2~UX_DEVICE_CLASS_HID_MAX_EVENTS_QUEUE) sets
     the queue size and _event_max_length (max UX_DEVICE_CLASS_HID_EVENT_BUFFER_LENGTH) sets
     event max length. If parameters are not valid, maximum values are used.
    It's enabled automatically if HID owns endpoint buffer and zero copy is enabled.
 */
#ifndef UX_DEVICE_CLASS_HID_FLEXIBLE_EVENTS_QUEUE
#if (UX_DEVICE_ENDPOINT_BUFFER_OWNER == 1) && defined(UX_DEVICE_CLASS_HID_ZERO_COPY)
#define UX_DEVICE_CLASS_HID_FLEXIBLE_EVENTS_QUEUE
#endif
#endif

/* Internal: check if class own endpoint buffer  */
#if (UX_DEVICE_ENDPOINT_BUFFER_OWNER == 1)
#define UX_DEVICE_CLASS_HID_OWN_ENDPOINT_BUFFER
#endif


/* Define HID Class constants.  */

#define UX_DEVICE_CLASS_HID_CLASS                                   0x03
#define UX_DEVICE_CLASS_HID_SUBCLASS                                0X00
#define UX_DEVICE_CLASS_HID_PROTOCOL                                0X00

/* Define HID Class commands.  */

#define UX_DEVICE_CLASS_HID_COMMAND_GET_REPORT                      0x01
#define UX_DEVICE_CLASS_HID_COMMAND_GET_IDLE                        0x02
#define UX_DEVICE_CLASS_HID_COMMAND_GET_PROTOCOL                    0x03
#define UX_DEVICE_CLASS_HID_COMMAND_SET_REPORT                      0x09
#define UX_DEVICE_CLASS_HID_COMMAND_SET_IDLE                        0x0A
#define UX_DEVICE_CLASS_HID_COMMAND_SET_PROTOCOL                    0x0B

/* Define HID Class Descriptor types.  */

#define UX_DEVICE_CLASS_HID_DESCRIPTOR_HID                          0x21
#define UX_DEVICE_CLASS_HID_DESCRIPTOR_REPORT                       0x22
#define UX_DEVICE_CLASS_HID_DESCRIPTOR_PHYSICAL                     0x23

/* Define HID Report Types.  */

#define UX_DEVICE_CLASS_HID_REPORT_TYPE_INPUT                       0x1
#define UX_DEVICE_CLASS_HID_REPORT_TYPE_OUTPUT                      0x2
#define UX_DEVICE_CLASS_HID_REPORT_TYPE_FEATURE                     0x3

/* Define HID Protocols.  */

#define UX_DEVICE_CLASS_HID_PROTOCOL_BOOT                           0
#define UX_DEVICE_CLASS_HID_PROTOCOL_REPORT                         1

/* Define HID standalone read/receiver states.  */

#define UX_DEVICE_CLASS_HID_READ_START                              (UX_STATE_STEP + 1)
#define UX_DEVICE_CLASS_HID_READ_WAIT                               (UX_STATE_STEP + 2)

#define UX_DEVICE_CLASS_HID_RECEIVER_START                          (UX_STATE_STEP + 3)
#define UX_DEVICE_CLASS_HID_RECEIVER_WAIT                           (UX_STATE_STEP + 4)
#define UX_DEVICE_CLASS_HID_RECEIVER_ERROR                          (UX_STATE_STEP + 5)


/* Define HID event info structure.  */

#ifndef UX_DEVICE_CLASS_HID_EVENT_BUFFER_LENGTH
#define UX_DEVICE_CLASS_HID_EVENT_BUFFER_LENGTH                     32
#endif

/* Ensure the event buffer can fit inside the control endpoint's data buffer.  */
#if UX_DEVICE_CLASS_HID_EVENT_BUFFER_LENGTH > UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH
/* #error "Error: the event buffer cannot fit inside the control endpoint's data buffer. Reduce UX_DEVICE_CLASS_HID_EVENT_BUFFER_LENGTH such that it is less than or equal to UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH." */
/* Build option checked runtime by UX_ASSERT  */
#endif

/* Ensure the event buffer can fit inside the interrupt endpoint's data buffer.  */
#if UX_DEVICE_CLASS_HID_EVENT_BUFFER_LENGTH > UX_SLAVE_REQUEST_DATA_MAX_LENGTH
/* #error "Error: the event buffer cannot fit inside the interrupt endpoint's data buffer. Reduce UX_DEVICE_CLASS_HID_EVENT_BUFFER_LENGTH such that it is less than or equal to UX_SLAVE_REQUEST_DATA_MAX_LENGTH." */
/* Build option checked runtime by UX_ASSERT  */
#endif

#ifndef UX_DEVICE_CLASS_HID_MAX_EVENTS_QUEUE
#define UX_DEVICE_CLASS_HID_MAX_EVENTS_QUEUE                        16
#endif

#define UX_DEVICE_CLASS_HID_NEW_EVENT                               1u
#define UX_DEVICE_CLASS_HID_NEW_IDLE_RATE                           2u
#define UX_DEVICE_CLASS_HID_EVENTS_MASK                             3u /* Mask _NEW_EVENT and _NEW_IDLE_RATE  */

#define UX_DEVICE_CLASS_HID_RECEIVER_RESTART                        4u

#define UX_DEVICE_CLASS_HID_EVENTS_ALL_MASK                         7u /* Mask all event flags.  */

typedef struct UX_SLAVE_CLASS_HID_EVENT_STRUCT
{
    ULONG                   ux_device_class_hid_event_report_id;
    ULONG                   ux_device_class_hid_event_report_type;
    ULONG                   ux_device_class_hid_event_length;
    UCHAR                   ux_device_class_hid_event_buffer[UX_DEVICE_CLASS_HID_EVENT_BUFFER_LENGTH];

} UX_SLAVE_CLASS_HID_EVENT;

/* Internal (transmit) event header.   */
typedef struct UX_DEVICE_CLASS_HID_EVENT_STRUCT
{
    ULONG                   ux_device_class_hid_event_report_id;
    ULONG                   ux_device_class_hid_event_report_type;
    ULONG                   ux_device_class_hid_event_length;
    UCHAR                   *ux_device_class_hid_event_buffer;
} UX_DEVICE_CLASS_HID_EVENT;

#if defined(UX_DEVICE_CLASS_HID_FLEXIBLE_EVENTS_QUEUE)
#define UX_DEVICE_CLASS_HID_EVENT_BUFFER(e)     ((e)->ux_device_class_hid_event_buffer)
#else
#define UX_DEVICE_CLASS_HID_EVENT_BUFFER(e)     (((UX_SLAVE_CLASS_HID_EVENT*)e)->ux_device_class_hid_event_buffer)
#endif


/* Define HID structure.  */

typedef struct UX_SLAVE_CLASS_HID_STRUCT
{

    UX_SLAVE_INTERFACE              *ux_slave_class_hid_interface;
    UX_SLAVE_ENDPOINT               *ux_device_class_hid_interrupt_endpoint;

#if defined(UX_DEVICE_CLASS_HID_OWN_ENDPOINT_BUFFER)
    UCHAR                           *ux_device_class_hid_endpoint_buffer;
#endif

    UINT                            ux_device_class_hid_state;
    UINT                            (*ux_device_class_hid_callback)(struct UX_SLAVE_CLASS_HID_STRUCT *hid, UX_SLAVE_CLASS_HID_EVENT *);
    UINT                            (*ux_device_class_hid_get_callback)(struct UX_SLAVE_CLASS_HID_STRUCT *hid, UX_SLAVE_CLASS_HID_EVENT *);
    VOID                            (*ux_slave_class_hid_instance_activate)(VOID *);
    VOID                            (*ux_slave_class_hid_instance_deactivate)(VOID *);
    UCHAR                           *ux_device_class_hid_report_address;
    ULONG                           ux_device_class_hid_report_id;
    ULONG                           ux_device_class_hid_report_length;
#if !defined(UX_DEVICE_STANDALONE)
    UX_EVENT_FLAGS_GROUP            ux_device_class_hid_event_flags_group;
#else
    UINT                            ux_device_class_hid_event_state;
    ULONG                           ux_device_class_hid_event_wait_start;
    UX_DEVICE_CLASS_HID_EVENT       ux_device_class_hid_event;
#endif
    ULONG                           ux_device_class_hid_event_idle_rate;
    ULONG                           ux_device_class_hid_event_wait_timeout;
    ULONG                           ux_device_class_hid_protocol;
    UX_DEVICE_CLASS_HID_EVENT       *ux_device_class_hid_event_array;
    UX_DEVICE_CLASS_HID_EVENT       *ux_device_class_hid_event_array_head;
    UX_DEVICE_CLASS_HID_EVENT       *ux_device_class_hid_event_array_tail;
    UX_DEVICE_CLASS_HID_EVENT       *ux_device_class_hid_event_array_end;
#if defined(UX_DEVICE_CLASS_HID_FLEXIBLE_EVENTS_QUEUE)
    ULONG                           ux_device_class_hid_event_max_length;
#endif

#if defined(UX_DEVICE_CLASS_HID_INTERRUPT_OUT_SUPPORT)
    UX_SLAVE_ENDPOINT               *ux_device_class_hid_read_endpoint;
    struct UX_DEVICE_CLASS_HID_RECEIVER_STRUCT
                                    *ux_device_class_hid_receiver;
#if !defined(UX_DEVICE_STANDALONE)
    UX_MUTEX                        ux_device_class_hid_read_mutex;
#else

    UCHAR                           *ux_device_class_hid_read_buffer;
    ULONG                           ux_device_class_hid_read_requested_length;
    ULONG                           ux_device_class_hid_read_actual_length;
    ULONG                           ux_device_class_hid_read_transfer_length;

    UINT                            ux_device_class_hid_read_state;
    UINT                            ux_device_class_hid_read_status;
#endif
#endif

} UX_SLAVE_CLASS_HID;

/* Define HID endpoint buffer settings (when HID owns buffer).  */
#define UX_DEVICE_CLASS_HID_ENDPOINT_BUFFER_SIZE_CALC_OVERFLOW                  \
    (UX_OVERFLOW_CHECK_ADD_ULONG(UX_DEVICE_CLASS_HID_INTERRUPTIN_BUFFER_SIZE,   \
                                 UX_DEVICE_CLASS_HID_INTERRUPTOUT_BUFFER_SIZE))
#define UX_DEVICE_CLASS_HID_ENDPOINT_BUFFER_SIZE        (UX_DEVICE_CLASS_HID_INTERRUPTOUT_BUFFER_SIZE + UX_DEVICE_CLASS_HID_INTERRUPTIN_BUFFER_SIZE)
#define UX_DEVICE_CLASS_HID_INTERRUPTOUT_BUFFER(hid)    ((hid)->ux_device_class_hid_endpoint_buffer)
#define UX_DEVICE_CLASS_HID_INTERRUPTIN_BUFFER(hid)     (UX_DEVICE_CLASS_HID_INTERRUPTOUT_BUFFER(hid) + UX_DEVICE_CLASS_HID_INTERRUPTOUT_BUFFER_SIZE)

#ifdef UX_DEVICE_CLASS_HID_FLEXIBLE_EVENTS_QUEUE
#define UX_DEVICE_CLASS_HID_EVENT_MAX_LENGTH(hid)       ((hid)->ux_device_class_hid_event_max_length)
#if (UX_DEVICE_ENDPOINT_BUFFER_OWNER == 1) && defined(UX_DEVICE_CLASS_HID_ZERO_COPY)
#define UX_DEVICE_CLASS_HID_EVENT_QUEUE_ITEM_SIZE(hid)  sizeof(UX_DEVICE_CLASS_HID_EVENT)
#else
#define UX_DEVICE_CLASS_HID_EVENT_QUEUE_ITEM_SIZE(hid)  (sizeof(UX_DEVICE_CLASS_HID_EVENT) - 4 + UX_DEVICE_CLASS_HID_EVENT_MAX_LENGTH(hid))
#endif
#else
#define UX_DEVICE_CLASS_HID_EVENT_MAX_LENGTH(hid)       UX_DEVICE_CLASS_HID_EVENT_BUFFER_LENGTH
#define UX_DEVICE_CLASS_HID_EVENT_QUEUE_ITEM_SIZE(hid)  sizeof(UX_SLAVE_CLASS_HID_EVENT)
#endif


/* HID interrupt OUT support extensions.  */

typedef struct UX_DEVICE_CLASS_HID_RECEIVED_EVENT_STRUCT
{
    ULONG                   ux_device_class_hid_received_event_length;
    UCHAR                   *ux_device_class_hid_received_event_data;
} UX_DEVICE_CLASS_HID_RECEIVED_EVENT;

typedef struct UX_DEVICE_CLASS_HID_RECEIVER_STRUCT
{

    VOID                    (*ux_device_class_hid_receiver_uninitialize)(struct UX_DEVICE_CLASS_HID_RECEIVER_STRUCT *receiver);
    VOID                    (*ux_device_class_hid_receiver_event_callback)(struct UX_SLAVE_CLASS_HID_STRUCT *hid);

    ULONG                   ux_device_class_hid_receiver_event_buffer_size;
    UX_DEVICE_CLASS_HID_RECEIVED_EVENT
                            *ux_device_class_hid_receiver_events;
    UX_DEVICE_CLASS_HID_RECEIVED_EVENT
                            *ux_device_class_hid_receiver_events_end;
    UX_DEVICE_CLASS_HID_RECEIVED_EVENT
                            *ux_device_class_hid_receiver_event_read_pos;
    UX_DEVICE_CLASS_HID_RECEIVED_EVENT
                            *ux_device_class_hid_receiver_event_save_pos;

#if !defined(UX_DEVICE_STANDALONE)
    UX_THREAD               ux_device_class_hid_receiver_thread;
#else
    UINT                    (*ux_device_class_hid_receiver_tasks_run)(struct UX_SLAVE_CLASS_HID_STRUCT *hid);
#endif
} UX_DEVICE_CLASS_HID_RECEIVER;

#if (UX_DEVICE_ENDPOINT_BUFFER_OWNER == 1) && defined(UX_DEVICE_CLASS_HID_ZERO_COPY)
#define UX_DEVICE_CLASS_HID_RECEIVED_QUEUE_ITEM_BUFFER(i)   ((i) -> ux_device_class_hid_received_event_data)
#define UX_DEVICE_CLASS_HID_RECEIVED_QUEUE_ITEM_SIZE(r)     sizeof(UX_DEVICE_CLASS_HID_RECEIVED_EVENT)
#else
#define UX_DEVICE_CLASS_HID_RECEIVED_QUEUE_ITEM_BUFFER(i)   ((UCHAR*)&(i) -> ux_device_class_hid_received_event_data)
#define UX_DEVICE_CLASS_HID_RECEIVED_QUEUE_ITEM_SIZE(r)     ((r) -> ux_device_class_hid_receiver_event_buffer_size + sizeof(ULONG))
#endif


/* Define HID initialization command structure.  */

typedef struct UX_SLAVE_CLASS_HID_PARAMETER_STRUCT
{

    VOID                    (*ux_slave_class_hid_instance_activate)(VOID *);
    VOID                    (*ux_slave_class_hid_instance_deactivate)(VOID *);
    UCHAR                   *ux_device_class_hid_parameter_report_address;
    ULONG                   ux_device_class_hid_parameter_report_id;
    ULONG                   ux_device_class_hid_parameter_report_length;
    UINT                    (*ux_device_class_hid_parameter_callback)(struct UX_SLAVE_CLASS_HID_STRUCT *hid, UX_SLAVE_CLASS_HID_EVENT *);
    UINT                    (*ux_device_class_hid_parameter_get_callback)(struct UX_SLAVE_CLASS_HID_STRUCT *hid, UX_SLAVE_CLASS_HID_EVENT *);
#if defined(UX_DEVICE_CLASS_HID_FLEXIBLE_EVENTS_QUEUE)
    ULONG                   ux_device_class_hid_parameter_event_max_number;
    ULONG                   ux_device_class_hid_parameter_event_max_length;
#endif
#if defined(UX_DEVICE_CLASS_HID_INTERRUPT_OUT_SUPPORT)
    UINT                    (*ux_device_class_hid_parameter_receiver_initialize)(UX_SLAVE_CLASS_HID *hid, struct UX_SLAVE_CLASS_HID_PARAMETER_STRUCT *parameter, UX_DEVICE_CLASS_HID_RECEIVER **receiver);
    ULONG                   ux_device_class_hid_parameter_receiver_event_max_number;
    ULONG                   ux_device_class_hid_parameter_receiver_event_max_length;
    VOID                    (*ux_device_class_hid_parameter_receiver_event_callback)(struct UX_SLAVE_CLASS_HID_STRUCT *hid);
#endif

} UX_SLAVE_CLASS_HID_PARAMETER;

#ifdef UX_DEVICE_CLASS_HID_FLEXIBLE_EVENTS_QUEUE
#define UX_DEVICE_CLASS_HID_PARAM_EVENT_QUEUE_SIZE(p)       ((p)->ux_device_class_hid_parameter_event_max_number)
#define UX_DEVICE_CLASS_HID_PARAM_EVENT_MAX_LENGTH(p)       ((p)->ux_device_class_hid_parameter_event_max_length)
#else
#define UX_DEVICE_CLASS_HID_PARAM_EVENT_QUEUE_SIZE(p)       UX_DEVICE_CLASS_HID_MAX_EVENTS_QUEUE
#define UX_DEVICE_CLASS_HID_PARAM_EVENT_MAX_LENGTH(p)       UX_DEVICE_CLASS_HID_EVENT_BUFFER_LENGTH
#endif


/* Define HID Class function prototypes.  */
UINT  _ux_device_class_hid_descriptor_send(UX_SLAVE_CLASS_HID *hid, ULONG descriptor_type,
                                            ULONG request_index, ULONG host_length);
UINT  _ux_device_class_hid_activate(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_hid_deactivate(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_hid_control_request(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_hid_entry(UX_SLAVE_CLASS_COMMAND *command);
VOID  _ux_device_class_hid_interrupt_thread(ULONG hid_class);
UINT  _ux_device_class_hid_initialize(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_hid_uninitialize(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_hid_event_set(UX_SLAVE_CLASS_HID *hid,
                                      UX_SLAVE_CLASS_HID_EVENT *hid_event);
UINT  _ux_device_class_hid_event_check(UX_SLAVE_CLASS_HID *hid,
                                       UX_DEVICE_CLASS_HID_EVENT **hid_event);
VOID  _ux_device_class_hid_event_free(UX_SLAVE_CLASS_HID *hid);
UINT  _ux_device_class_hid_event_get(UX_SLAVE_CLASS_HID *hid,
                                      UX_SLAVE_CLASS_HID_EVENT *hid_event);
UINT  _ux_device_class_hid_report_set(UX_SLAVE_CLASS_HID *hid, ULONG descriptor_type,
                                            ULONG request_index, ULONG host_length);
UINT  _ux_device_class_hid_report_get(UX_SLAVE_CLASS_HID *hid, ULONG descriptor_type,
                                            ULONG request_index, ULONG host_length);

UINT  _ux_device_class_hid_tasks_run(VOID *class_instance);

UINT  _ux_device_class_hid_read(UX_SLAVE_CLASS_HID *hid,
                                UCHAR *buffer, ULONG requested_length,
                                ULONG *actual_length);

VOID  _ux_device_class_hid_receiver_thread(ULONG hid_class);
UINT  _ux_device_class_hid_receiver_initialize(UX_SLAVE_CLASS_HID *hid,
                                    UX_SLAVE_CLASS_HID_PARAMETER *parameter,
                                    UX_DEVICE_CLASS_HID_RECEIVER **receiver);
VOID  _ux_device_class_hid_receiver_uninitialize(UX_DEVICE_CLASS_HID_RECEIVER *receiver);
UINT  _ux_device_class_hid_receiver_event_get(UX_SLAVE_CLASS_HID *hid,
                                UX_DEVICE_CLASS_HID_RECEIVED_EVENT *event);
UINT  _ux_device_class_hid_receiver_event_free(UX_SLAVE_CLASS_HID *hid);

UINT  _ux_device_class_hid_read_run(UX_SLAVE_CLASS_HID *hid,
                                UCHAR *buffer, ULONG requested_length,
                                ULONG *actual_length);
UINT  _ux_device_class_hid_receiver_tasks_run(UX_SLAVE_CLASS_HID *hid);


UINT  _uxe_device_class_hid_initialize(UX_SLAVE_CLASS_COMMAND *command);
UINT  _uxe_device_class_hid_event_set(UX_SLAVE_CLASS_HID *hid,
                                      UX_SLAVE_CLASS_HID_EVENT *hid_event);
UINT  _uxe_device_class_hid_event_get(UX_SLAVE_CLASS_HID *hid,
                                      UX_SLAVE_CLASS_HID_EVENT *hid_event);
UINT  _uxe_device_class_hid_read(UX_SLAVE_CLASS_HID *hid,
                                UCHAR *buffer, ULONG requested_length,
                                ULONG *actual_length);
UINT  _uxe_device_class_hid_read_run(UX_SLAVE_CLASS_HID *hid,
                                UCHAR *buffer, ULONG requested_length,
                                ULONG *actual_length);
UINT  _uxe_device_class_hid_receiver_initialize(UX_SLAVE_CLASS_HID *hid,
                                    UX_SLAVE_CLASS_HID_PARAMETER *parameter,
                                    UX_DEVICE_CLASS_HID_RECEIVER **receiver);
UINT  _uxe_device_class_hid_receiver_event_get(UX_SLAVE_CLASS_HID *hid,
                                UX_DEVICE_CLASS_HID_RECEIVED_EVENT *event);
UINT  _uxe_device_class_hid_receiver_event_free(UX_SLAVE_CLASS_HID *hid);


/* Define Device HID Class API prototypes.  */

#if defined(UX_DEVICE_CLASS_HID_ENABLE_ERROR_CHECKING)

#define ux_device_class_hid_entry               _ux_device_class_hid_entry
#define ux_device_class_hid_event_set           _uxe_device_class_hid_event_set
#define ux_device_class_hid_event_get           _uxe_device_class_hid_event_get
#define ux_device_class_hid_report_set          _ux_device_class_hid_report_set
#define ux_device_class_hid_report_get          _ux_device_class_hid_report_get

#define ux_device_class_hid_protocol_get(hid)   (((hid) == UX_NULL) ? UX_ERROR : (hid) -> ux_device_class_hid_protocol)

#define ux_device_class_hid_read                _uxe_device_class_hid_read
#define ux_device_class_hid_read_run            _uxe_device_class_hid_read_run

#define ux_device_class_hid_receiver_initialize _ux_device_class_hid_receiver_initialize
#define ux_device_class_hid_receiver_event_get  _uxe_device_class_hid_receiver_event_get
#define ux_device_class_hid_receiver_event_free _uxe_device_class_hid_receiver_event_free

#else

#define ux_device_class_hid_entry        _ux_device_class_hid_entry
#define ux_device_class_hid_event_set    _ux_device_class_hid_event_set
#define ux_device_class_hid_event_get    _ux_device_class_hid_event_get
#define ux_device_class_hid_report_set   _ux_device_class_hid_report_set
#define ux_device_class_hid_report_get   _ux_device_class_hid_report_get

#define ux_device_class_hid_protocol_get(hid)   ((hid) -> ux_device_class_hid_protocol)

#define ux_device_class_hid_read                _ux_device_class_hid_read
#define ux_device_class_hid_read_run            _ux_device_class_hid_read_run

#define ux_device_class_hid_receiver_initialize _ux_device_class_hid_receiver_initialize
#define ux_device_class_hid_receiver_event_get  _ux_device_class_hid_receiver_event_get
#define ux_device_class_hid_receiver_event_free _ux_device_class_hid_receiver_event_free

#endif

/* Determine if a C++ compiler is being used.  If so, complete the standard
   C conditional started above.  */
#ifdef __cplusplus
}
#endif

#endif
