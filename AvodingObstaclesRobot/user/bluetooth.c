#include "bluetooth.h"

void BT_RCC_Configure(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    /* UART 2 enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}

void BT_GPIO_Configure(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    /* UART pin setting */
   //TX, Output
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // UART2 and UART1 for TX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //RX, Input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;  // UART2 and UART1 for RX
    GPIO_InitStructure.GPIO_Speed = 0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void USART2_Init(void) {
    USART_InitTypeDef USART2_InitStructure;
    USART_Cmd(USART2, ENABLE);
    USART2_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART2_InitStructure.USART_BaudRate = 9600;
    USART2_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART2_InitStructure.USART_Parity = USART_Parity_No;
    USART2_InitStructure.USART_StopBits = USART_StopBits_1;
    USART2_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART2_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void NVIC_Configure(void) {
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    // UART2 에 대한 우선순위 
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_EnableIRQ(USART2_IRQn);
}

void USART2_IRQHandler(void) {
    uint16_t word;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        word = USART_ReceiveData(USART2);
        if (word == 'g') {
            isGo = ENABLE;
        }
        if (word == 's') {
            isGo = DISABLE;
        }
        // USART_SendData(USART2, word);
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

void BT_Configure(void) {
    BT_RCC_Configure();
    BT_GPIO_Configure();
    USART2_Init();
    NVIC_Configure();
}

void sendDataUART2(uint16_t data) {
    USART_SendData(USART2, data);
}
