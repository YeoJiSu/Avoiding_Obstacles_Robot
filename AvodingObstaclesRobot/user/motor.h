#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

void MOTOR_RCC_Configure(void);
void MOTOR_GPIO_Configure(void);
void MOTOR_Configure(void);

void MOTOR_SET_GO(void);
void MOTOR_SET_STOP(void);
void MOTOR_SET_TURN_RIGHT(void);
void MOTOR_SET_TURN_LEFT(void);