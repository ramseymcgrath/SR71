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
/** NetX Secure Component                                                 */
/**                                                                       */
/**    Transport Layer Security (TLS)                                     */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  PORT SPECIFIC C INFORMATION                            RELEASE        */
/*                                                                        */
/*    nx_secure_port.h                                    PORTABLE C      */
/*                                                           6.1          */
/*                                                                        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Timothy Stapko, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains data type definitions for generic.               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Timothy Stapko           Initial Version 6.0           */
/*  09-30-2020     Timothy Stapko           Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/

#ifndef NX_SECURE_PORT_H
#define NX_SECURE_PORT_H


/* Determine if the optional NetX Secure user define file should be used.  */

/*
   #define NX_SECURE_INCLUDE_USER_DEFINE_FILE
 */
#ifdef NX_SECURE_INCLUDE_USER_DEFINE_FILE


/* Yes, include the user defines in nx_user.h. The defines in this file may
   alternately be defined on the command line.  */

#include "nx_secure_user.h"
#endif


/* Define the version ID of NetX Secure.  This may be utilized by the application.  */

#ifdef NX_SECURE_SYSTEM_INIT
CHAR                            _nx_secure_version_id[] =
                                    "Copyright (c) 2024 Microsoft Corporation.  *  NetX Secure Generic Version G6.4.1 *";
#else
extern  CHAR                    _nx_secure_version_id[];
#endif

#endif

