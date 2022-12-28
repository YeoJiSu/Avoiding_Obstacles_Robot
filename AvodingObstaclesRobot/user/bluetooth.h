#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"

void BT_RCC_Configure(void);
void BT_GPIO_Configure(void);
void BT_Configure(void);

void USART2_Init(void);
void USART2_IRQHandler(void);
void NVIC_Configure(void);

void sendDataUART2(uint16_t data);