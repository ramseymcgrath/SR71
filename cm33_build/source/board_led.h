#ifndef BOARD_LED_H
#define BOARD_LED_H

#ifdef __cplusplus
extern "C" {
#endif

void BOARD_LED_Init(void);
void BOARD_LED_Toggle(void);
void BOARD_LED_On(void);
void BOARD_LED_Off(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_LED_H */
