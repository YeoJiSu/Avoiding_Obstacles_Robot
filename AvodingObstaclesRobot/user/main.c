#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "stdio.h"
#include <stdbool.h>
#include <time.h>

#define arrSize 1000
#define GO 1
#define STOP 0
#define Arrival 17
#define TESTSIZE 36 

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void EXTI_Configure(void);
void NVIC_Configure(void);
void USART2_Init(void);
void USART2_IRQHandler();
void sendDataUART1(uint16_t data);
void sendDataUART2(uint16_t data);

typedef enum {
    BACK, LEFT, FORWARD, RIGHT
}Direction;

typedef struct Trace {
    Direction dir;
    int time;
}Trace;

typedef struct robot {
    Direction direction; // 출발점의 시야
    bool isGo; // 정지 또는 출발
    Trace* trace; // 지나온 길 (MAP)
    int forwardBackward; // 앞뒤 변위 (결승점까지 남은 거리)
    int leftRight; // 좌우 변위 (좌회전, 우회전 결정요소)
}robot;

void turnLeft(robot* rbt);
void turnRight(robot* rbt);
void setNewDirectionToTrace(robot* rbt);
bool leftOb();
bool rightOb();
bool frontOb();
bool noObstacle();
bool frontLeftObstacle();
bool frontRightObstacle();
bool leftObstacle();
bool rightObstacle();
bool frontObstacle();
bool isRobotForward(robot* rbt);
bool isRobotLeft(robot* rbt);
bool isRobotRight(robot* rbt);
bool isRobotBack(robot* rbt);
bool isRobotArrived(robot* rbt);
void robotStop();
void robotGo();
void robotTurnRight();
void robotTurnLeft();
void delay();

void turnLeft(robot* rbt)
{
    rbt->direction -= 1;
    if (rbt->direction < BACK)
        rbt->direction = RIGHT;
}

void turnRight(robot* rbt) {
    rbt->direction += 1;
    if (rbt->direction > RIGHT)
        rbt->direction = BACK;
}

Trace trace2[arrSize] = { {FORWARD, 0}, };
int rbt_index = 0; // 로봇 방향이 바뀔 때 증가하는 index. 결국 Trace 저장용!

clock_t start;
clock_t end;

void setNewDirectionToTrace(robot* rbt) {
    end = clock();
    rbt->trace[rbt_index].time = (int)(end - start);

    rbt->trace[rbt_index].dir = rbt->direction;
    rbt->trace[rbt_index].time = 0;
    start = clock();
}

bool leftOb() {
    return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == Bit_RESET;
}

bool rightOb() {
    return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3) == Bit_RESET;
}

bool frontOb() {
    return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == Bit_RESET;
}

bool noObstacle() {
    return !frontOb() && !leftOb() && !rightOb();
}

bool frontLeftObstacle() {
    return frontOb() && leftOb() && !rightOb();
}

bool frontRightObstacle() {
    return frontOb() && !leftOb() && rightOb();
}

bool leftObstacle() {
    return !frontOb() && leftOb() && !rightOb();
}

bool rightObstacle() {
    return !frontOb() && !leftOb() && rightOb();
}

bool frontObstacle() {
    return frontOb() && !leftOb() && !rightOb();
}

bool isRobotForward(robot* rbt) {
    return rbt->direction == FORWARD;
}

bool isRobotLeft(robot* rbt) {
    return rbt->direction == LEFT;
}

bool isRobotRight(robot* rbt) {
    return rbt->direction == RIGHT;
}

bool isRobotBack(robot* rbt) {
    return rbt->direction == BACK;
}

bool isRobotArrived(robot* rbt) {
    return rbt->forwardBackward >= Arrival;
}

void Delay(void) {
	int i;

	for (i = 0; i < 10000000; i++) {}
}

void robotStop() {
    GPIO_ResetBits(GPIOC, GPIO_Pin_8);
    GPIO_ResetBits(GPIOC, GPIO_Pin_9);
    GPIO_ResetBits(GPIOC, GPIO_Pin_10);
    GPIO_ResetBits(GPIOC, GPIO_Pin_11);
}

void robotGo() {
    GPIO_ResetBits(GPIOC, GPIO_Pin_8);
    GPIO_SetBits(GPIOC, GPIO_Pin_9);
    GPIO_ResetBits(GPIOC, GPIO_Pin_10);
    GPIO_SetBits(GPIOC, GPIO_Pin_11);
}

void robotTurnRight() {
    GPIO_ResetBits(GPIOC, GPIO_Pin_8);
    GPIO_SetBits(GPIOC, GPIO_Pin_9);
    GPIO_SetBits(GPIOC, GPIO_Pin_10);
    GPIO_ResetBits(GPIOC, GPIO_Pin_11);
    Delay();
}

void robotTurnLeft() {
    GPIO_SetBits(GPIOC, GPIO_Pin_8);
    GPIO_ResetBits(GPIOC, GPIO_Pin_9);
    GPIO_ResetBits(GPIOC, GPIO_Pin_10);
    GPIO_SetBits(GPIOC, GPIO_Pin_11);
    Delay();
}


void RCC_Configure(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    /* UART 2 enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    /* TIM 2 enable */
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void GPIO_Configure(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = 0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

     /* UART pin setting */ 
    //TX, Output
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ; // UART2 and UART1 for TX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //RX, Input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;  // UART2 and UART1 for RX
    GPIO_InitStructure.GPIO_Speed = 0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

FunctionalState isGo = DISABLE;

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

void USART2_Init(void) 
{
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

void USART2_IRQHandler() { 
    uint16_t word;
    if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET){
        word = USART_ReceiveData(USART2);
        if (word=='g'){
            isGo = ENABLE;
        }
        if (word=='s'){
            isGo = DISABLE;
        }
        // USART_SendData(USART2, word);
    	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
}

/*
void TIM_Configure(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 1200;
    TIM_TimeBaseStructure.TIM_Prescaler = 60000;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}
*/
void sendDataUART2(uint16_t data) { 
	USART_SendData(USART2, data);
}

int main(void)
{
    RCC_Configure();
    GPIO_Configure();
    USART2_Init();
    NVIC_Configure();

    struct robot rbt = { FORWARD, GO, trace2, 0, 0 }; // 로봇 상태 초기화;
    start = clock(); // 블루투스 모듈 작성시 수정해야함. 

    
    while (1)
    {
       while (1){
            if (isGo == ENABLE){
                robotGo();
                break;
            }
            
            if (isGo == DISABLE){
               robotStop();
            }
        }

        if (isRobotForward(&rbt) && noObstacle()) {
            robotGo();
        }
        else if (isRobotForward(&rbt) && leftObstacle()) {
            robotGo();
        }
        else if (isRobotForward(&rbt) && rightObstacle()) {
            robotGo();
        }
        else if (isRobotForward(&rbt) && frontLeftObstacle()) {

            // frontLeft가 없어질 때 까지 회전한 후, S/W로 turnRight()를 한번만 해야 Back상태로 가지 않음. 
            robotStop();
            robotTurnRight();

            turnRight(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
        else if (isRobotForward(&rbt) && frontRightObstacle()) {
            robotStop();
            robotTurnLeft();

            turnLeft(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }

        else if (isRobotRight(&rbt) && leftObstacle()) {
            // Go until noLeftObstacle;
            robotGo();

            rbt.leftRight++;
        }
        else if (isRobotLeft(&rbt) && rightObstacle()) {
            // Go until noRightObstacle;
            robotGo();

            rbt.leftRight--;
        }
        else if (isRobotLeft(&rbt) && noObstacle()) {
            robotStop();
            robotTurnRight();

            turnRight(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
        else if (isRobotLeft(&rbt) && frontRightObstacle()) {
            robotStop();
            robotTurnRight();

            turnRight(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
        else if (isRobotRight(&rbt) && noObstacle()) {
            robotStop();
            robotTurnLeft();

            turnLeft(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
        else if (isRobotRight(&rbt) && frontLeftObstacle()) {
            robotStop();
            robotTurnLeft();

            turnLeft(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
        else if (isRobotForward(&rbt) && frontObstacle()) {
            robotStop();
            if (rbt.leftRight > 0) {
                robotTurnLeft();
                turnLeft(&rbt); // setStatus
            }
            else {
                robotTurnRight();
                turnRight(&rbt); // setStatus
            }
            setNewDirectionToTrace(&rbt); // setTrace
        }
        else{
            robotGo();
        }

        // 직진 변위 계산하기 

    }

    for (int i = 0; i < TESTSIZE; i++) {
        if (rbt.trace[i].dir == LEFT) {
            printf("왼쪽으로 %d\n", rbt.trace[i].time);
        }
        if (rbt.trace[i].dir == FORWARD) {
            printf("앞으로 %d\n", rbt.trace[i].time);
        }
        if (rbt.trace[i].dir == RIGHT) {
            printf("오른쪽으로 %d\n", rbt.trace[i].time);

        }
    }
    
   return 0;
}