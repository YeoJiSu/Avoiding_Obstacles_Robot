#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "stdio.h"
#include "core_cm3.h"
#include "lcd.h"
#include <stdbool.h>
#include <time.h>

#define arrSize 1000
#define GO 1
#define STOP 0
#define LCD_SPEED 10
#define OBSTACLE_DST 5
#define LCD_MID (120 * LCD_SPEED)
#define Arrival (310 * LCD_SPEED)
#define ROTATE_DELAY 14000000
#define SHORT_DELAY 100000
/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void NVIC_Configure(void);
void USART2_Init(void);
void USART2_IRQHandler();
void sendDataUART2(uint16_t data);

typedef enum {
    BACK, LEFT, FORWARD, RIGHT
}Direction;

typedef struct Trace {
    Direction dir;
    int time;
    int pos_x;
    int pos_y;
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
void turn_Head_To_End(robot* rbt);
void ShowDirection(robot* rbt);
bool leftOb();
bool rightOb();
bool frontOb();
bool noObstacle();
bool frontLeftObstacle();
bool frontRightObstacle();
bool leftObstacle();
bool rightObstacle();
bool frontObstacle();
bool leftRightObstacle();
bool isRobotForward(robot* rbt);
bool isRobotLeft(robot* rbt);
bool isRobotRight(robot* rbt);
bool isRobotBack(robot* rbt);
bool isRobotArrived(robot* rbt);
void robotStop();
void robotGo(robot* rbt);
void robotTurnRight(robot* rbt);
void robotTurnLeft(robot* rbt);
void Delay(int value);

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

void turn_Head_To_End(robot* rbt) {
    if (rbt->leftRight > LCD_MID) {
        robotTurnRight(rbt);
        turnRight(rbt);
    }
    else if (rbt->leftRight < LCD_MID) {
        robotTurnLeft(rbt);
        turnLeft(rbt);
    }
    else {}
    while (rbt->leftRight != LCD_MID) {
        robotGo(rbt);
    }

    switch (rbt->direction) {
    case LEFT:
        robotTurnRight(rbt);
        turnRight(rbt);
        break;

    case RIGHT:
        robotTurnLeft(rbt);
        turnLeft(rbt);
        break;

    default:
        break;
    }

}

void ShowDirection(robot* rbt) {
    switch (rbt->direction) {
    case FORWARD:
        LCD_ShowString(1, 20, "FORWARD", BLUE, WHITE);
        break;

    case LEFT:
        LCD_ShowString(1, 20, "LEFT       ", BLUE, WHITE);
        break;

    case RIGHT:
        LCD_ShowString(1, 20, "RIGHT       ", BLUE, WHITE);
        break;

    default:

        break;
    }
}

Trace trace2[arrSize] = { {FORWARD, 0, LCD_MID, 0}, };
int rbt_index = 0; // 로봇 방향이 바뀔 때 증가하는 index. 결국 Trace 저장용!

clock_t start;
clock_t end;

void setNewDirectionToTrace(robot* rbt) {
    end = clock();
    rbt->trace[rbt_index].time = (int)(end - start);

    rbt_index++;

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
bool leftRightObstacle() {
    return !frontOb() && leftOb() && rightOb();
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


void Delay(int value) {
    int i;
    for (i = 0; i < value; i++) {}
}

void robotStop() {
    GPIO_ResetBits(GPIOC, GPIO_Pin_8);
    GPIO_ResetBits(GPIOC, GPIO_Pin_9);
    GPIO_ResetBits(GPIOC, GPIO_Pin_10);
    GPIO_ResetBits(GPIOC, GPIO_Pin_12);
}

void robotGo(robot* rbt) {
    GPIO_ResetBits(GPIOC, GPIO_Pin_8);
    GPIO_SetBits(GPIOC, GPIO_Pin_9);
    GPIO_ResetBits(GPIOC, GPIO_Pin_10);
    GPIO_SetBits(GPIOC, GPIO_Pin_12);
    switch (rbt->direction) {
    case FORWARD:
        rbt->forwardBackward++;
        break;
    case LEFT:
        rbt->leftRight++;
        break;
    case RIGHT:
        rbt->leftRight--;
        break;
    default:
        break;
    }
    LCD_ShowNum(60, 70, rbt->forwardBackward, 6, BLACK, WHITE);
    LCD_ShowNum(150, 70, rbt->leftRight, 6, BLACK, WHITE);
    LCD_DrawCircle(rbt->leftRight / LCD_SPEED, rbt->forwardBackward / LCD_SPEED, 1);
    ShowDirection(rbt);
}

void robotTurnRight(robot* rbt) {
    GPIO_ResetBits(GPIOC, GPIO_Pin_8);
    GPIO_SetBits(GPIOC, GPIO_Pin_9);
    GPIO_SetBits(GPIOC, GPIO_Pin_10);
    GPIO_ResetBits(GPIOC, GPIO_Pin_12);
    Delay(ROTATE_DELAY);
    robotGo(rbt);
    Delay(SHORT_DELAY);
}

void robotTurnLeft(robot* rbt) {
    GPIO_SetBits(GPIOC, GPIO_Pin_8);
    GPIO_ResetBits(GPIOC, GPIO_Pin_9);
    GPIO_ResetBits(GPIOC, GPIO_Pin_10);
    GPIO_SetBits(GPIOC, GPIO_Pin_12);
    Delay(ROTATE_DELAY);
    robotGo(rbt);
    Delay(SHORT_DELAY);
}

void Show_LCD_Obstacle_LEFT(robot* rbt) {
    switch (rbt->direction) {
    case FORWARD:
        LCD_ShowString(rbt->leftRight / LCD_SPEED + OBSTACLE_DST, rbt->forwardBackward / LCD_SPEED - LCD_SPEED, "O", RED, RED);
        break;

    case LEFT:
        LCD_ShowString(rbt->leftRight / LCD_SPEED, rbt->forwardBackward / LCD_SPEED - LCD_SPEED - OBSTACLE_DST * 2, "O", RED, RED);
        break;

    case RIGHT:
        LCD_ShowString(rbt->leftRight / LCD_SPEED, rbt->forwardBackward / LCD_SPEED - LCD_SPEED + OBSTACLE_DST * 2, "O", RED, RED);
        break;

    default:
        break;
    }

}
void Show_LCD_Obstacle_RIGHT(robot* rbt) {

    switch (rbt->direction) {
    case FORWARD:
        LCD_ShowString(rbt->leftRight / LCD_SPEED - OBSTACLE_DST * 2, rbt->forwardBackward / LCD_SPEED - LCD_SPEED, "O", RED, RED);
        break;

    case LEFT:
        LCD_ShowString(rbt->leftRight / LCD_SPEED, rbt->forwardBackward / LCD_SPEED - LCD_SPEED + OBSTACLE_DST * 2, "O", RED, RED);
        break;

    case RIGHT:
        LCD_ShowString(rbt->leftRight / LCD_SPEED, rbt->forwardBackward / LCD_SPEED - LCD_SPEED - OBSTACLE_DST * 2, "O", RED, RED);
        break;

    default:
        break;
    }

}
void Show_LCD_Obstacle_FORWARD(robot* rbt) {
    LCD_ShowString(rbt->leftRight / LCD_SPEED, rbt->forwardBackward / LCD_SPEED + LCD_SPEED, "O", RED, RED);
}

void RCC_Configure(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    /* UART 2 enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}

void GPIO_Configure(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = 0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

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

void sendDataUART2(uint16_t data) {
    USART_SendData(USART2, data);
}

int main(void)
{
    RCC_Configure();
    GPIO_Configure();
    USART2_Init();
    NVIC_Configure();
    LCD_Init();

    struct robot rbt = { FORWARD, GO, trace2, 0, LCD_MID }; // 로봇 상태 초기화;
    start = clock(); // 블루투스 모듈 작성시 수정해야함. 

    LCD_ShowString(1, 1, "START", BLUE, WHITE);

    while (1)
    {
        if (isRobotArrived(&rbt)) { // 도착지에 도착했으면 멈춰라 
            robotStop();
            Delay(1000000);
            break;
        }

        while (1) {
            if (isGo == ENABLE) {
                robotGo(&rbt);
                break;
            }

            if (isGo == DISABLE) {
                robotStop();
            }
        }

        if (isRobotForward(&rbt) && noObstacle()) {
            robotGo(&rbt);

        }
        else if (isRobotForward(&rbt) && leftRightObstacle()) {
            Show_LCD_Obstacle_LEFT(&rbt);
            Show_LCD_Obstacle_RIGHT(&rbt);
            robotGo(&rbt);
        }
        else if (isRobotForward(&rbt) && leftObstacle()) {
            Show_LCD_Obstacle_LEFT(&rbt);
            robotGo(&rbt);

        }
        else if (isRobotForward(&rbt) && rightObstacle()) {
            Show_LCD_Obstacle_RIGHT(&rbt);
            robotGo(&rbt);
        }
        else if (isRobotForward(&rbt) && frontLeftObstacle()) {
            //Show_LCD_Obstacle_LEFT(&rbt);
            Show_LCD_Obstacle_FORWARD(&rbt);
            // frontLeft가 없어질 때 까지 회전한 후, S/W로 turnRight()를 한번만 해야 Back상태로 가지 않음. 
            robotStop();
            robotTurnRight(&rbt);

            turnRight(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
        else if (isRobotForward(&rbt) && frontRightObstacle()) {
            //Show_LCD_Obstacle_RIGHT(&rbt);
            Show_LCD_Obstacle_FORWARD(&rbt);
            robotStop();
            robotTurnLeft(&rbt);

            turnLeft(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
        else if (isRobotRight(&rbt) && leftRightObstacle()) {
            Show_LCD_Obstacle_LEFT(&rbt);
            Show_LCD_Obstacle_RIGHT(&rbt);
            robotGo(&rbt);
        }
        else if (isRobotRight(&rbt) && leftObstacle()) {
            Show_LCD_Obstacle_LEFT(&rbt);
            // Go until noLeftObstacle;
            robotGo(&rbt);
        }
        else if (isRobotLeft(&rbt) && leftRightObstacle()) {
            Show_LCD_Obstacle_LEFT(&rbt);
            Show_LCD_Obstacle_RIGHT(&rbt);
            robotGo(&rbt);
        }
        else if (isRobotLeft(&rbt) && rightObstacle()) {
            Show_LCD_Obstacle_RIGHT(&rbt);
            // Go until noRightObstacle;
            robotGo(&rbt);
        }
        else if (isRobotLeft(&rbt) && noObstacle()) {
            robotStop();
            robotTurnRight(&rbt);
            turnRight(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
        else if (isRobotLeft(&rbt) && frontRightObstacle()) {
            //Show_LCD_Obstacle_RIGHT(&rbt);
            Show_LCD_Obstacle_FORWARD(&rbt);
            robotStop();
            robotTurnRight(&rbt);

            turnRight(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
        else if (isRobotRight(&rbt) && noObstacle()) {
            robotStop();
            robotTurnLeft(&rbt);

            turnLeft(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
        else if (isRobotRight(&rbt) && frontLeftObstacle()) {
            //Show_LCD_Obstacle_LEFT(&rbt);
            Show_LCD_Obstacle_FORWARD(&rbt);
            robotStop();
            robotTurnLeft(&rbt);

            turnLeft(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
        else if (isRobotForward(&rbt) && frontObstacle()) {
            Show_LCD_Obstacle_FORWARD(&rbt);
            robotStop();
            if (rbt.leftRight > LCD_MID) {
                robotTurnLeft(&rbt);
                turnLeft(&rbt); // setStatus
            }
            else {
                robotTurnRight(&rbt);
                turnRight(&rbt); // setStatus
            }
            setNewDirectionToTrace(&rbt); // setTrace
        }
        else {
            robotGo(&rbt);
        }
    }

    turn_Head_To_End(&rbt);

    robotStop();
    LCD_ShowString(120, 300, "ARRIVED", BLUE, YELLOW);

    return 0;
}