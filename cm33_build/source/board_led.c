/*
 * board_led.c
 *
 * Implements LED functions using the macros defined in board.h.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "board_led.h"

void BOARD_LED_Init(void)
{
    /* Initialize all board LEDs to OFF (LOGIC_LED_OFF) */
    LED_RED_INIT(LOGIC_LED_OFF);
    LED_BLUE_INIT(LOGIC_LED_OFF);
    LED_GREEN_INIT(LOGIC_LED_OFF);
}

void BOARD_LED_Toggle(void)
{
    /* Toggle the red LED; modify if you wish to toggle others */
    LED_RED_TOGGLE();
}

void BOARD_LED_On(void)
{
    LED_RED_ON();
}

void BOARD_LED_Off(void)
{
    LED_RED_OFF();
}
