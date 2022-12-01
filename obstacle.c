#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "stdio.h"
#include <stdbool.h>
#include <time.h>

#define arrSize 1000
#define GO 1
#define STOP 0
#define Arrival 17
#define TESTSIZE 36 

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
    rbt_index++;
    rbt->trace[rbt_index].dir = rbt->direction;
    rbt->trace[rbt_index].time = 0;
    start = clock();
}

bool leftOb() {
    return GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14) == Bit_RESET;
}

bool rightOb() {
    return GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13) == Bit_RESET;
}

bool frontOb() {
    return GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12) == Bit_RESET;
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


void Delay(vu32 cnt) {
    for (; cnt != 0; cnt--);
}

void RCC_Configure(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = 0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void EXTI_Configure(void) {
    EXTI_InitTypeDef EXTI_InitStructure;

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource12);
    EXTI_InitStructure.EXTI_Line = EXTI_Line12;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

/*
void NVIC_Configure(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure,NVIC_IRQChannel = EXTI
}
*/

void robotStop() {
    GPIO_ResetBits(GPIOE, GPIO_Pin_8);
    GPIO_ResetBits(GPIOE, GPIO_Pin_9);
    GPIO_ResetBits(GPIOE, GPIO_Pin_10);
    GPIO_ResetBits(GPIOE, GPIO_Pin_11);
}

void robotGo() {
    GPIO_ResetBits(GPIOE, GPIO_Pin_8);
    GPIO_SetBits(GPIOE, GPIO_Pin_9);
    GPIO_ResetBits(GPIOE, GPIO_Pin_10);
    GPIO_SetBits(GPIOE, GPIO_Pin_11);
}

void robotTurnRight() {
    GPIO_ResetBits(GPIOE, GPIO_Pin_8);
    GPIO_SetBits(GPIOE, GPIO_Pin_9);
    GPIO_SetBits(GPIOE, GPIO_Pin_10);
    GPIO_ResetBits(GPIOE, GPIO_Pin_11);
    Delay(1000);
}

void robotTurnLeft() {
    GPIO_SetBits(GPIOE, GPIO_Pin_8);
    GPIO_ResetBits(GPIOE, GPIO_Pin_9);
    GPIO_ResetBits(GPIOE, GPIO_Pin_10);
    GPIO_SetBits(GPIOE, GPIO_Pin_11);
    Delay(1000);
}

int main(void)
{
    RCC_Configure();
    GPIO_Configure();
    EXTI_Configure();

    struct robot rbt = { FORWARD, GO, trace2, 0, 0 }; // 로봇 상태 초기화;
    start = clock(); // 블루투스 모듈 작성시 수정해야함. 

    while (1)
    {

        if (isRobotArrived(&rbt)) { // 도착지에 도착했으면 멈춰라 
            robotStop();
            break;
        }

        if (isRobotForward(&rbt) && noObstacle()) { // 현재 위치의 장애물이 없는 경우
            // time ++;
            // setNewDir 사용 안 함;
            // increase Trace[index].time;
            robotGo();

        }
        if (isRobotForward(&rbt) && leftObstacle()) {
            robotGo();

        }
        if (isRobotForward(&rbt) && rightObstacle()) {
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

        // 직진 변위 계산하기 
        if (isRobotForward(&rbt) && (noObstacle() || rightObstacle() || leftObstacle())) {
            rbt.forwardBackward++;
        }
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

    /*
    while(1){

      if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12) == Bit_RESET)  {
        GPIO_ResetBits(GPIOE, GPIO_Pin_8);
        GPIO_SetBits(GPIOE, GPIO_Pin_9);

      }
      else {
        GPIO_ResetBits(GPIOE, GPIO_Pin_9);
        GPIO_SetBits(GPIOE, GPIO_Pin_8);
      }

    }
  */


}