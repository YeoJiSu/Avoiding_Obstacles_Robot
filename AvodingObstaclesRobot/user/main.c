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

void setNewDirectionToTrace(robot* rbt) {
    end = clock();
    rbt->trace[rbt_index].time = (int)(end - start);
    rbt_index++;
    rbt->trace[rbt_index].dir = rbt->direction;
    rbt->trace[rbt_index].time = 0;
    start = clock();
}


Trace trace2[arrSize] = { {FORWARD, 0}, };
int rbt_index = 0; // 로봇 방향이 바뀔 때 증가하는 index. 결국 Trace 저장용!

clock_t start;
clock_t end;


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


int main(void)
{
    RCC_Configure();
    GPIO_Configure();
    EXTI_Configure();

    struct robot rbt = { FORWARD, GO, trace2, 0, 0 }; // 로봇 상태 초기화;
    start = clock(); // 블루투스 모듈 작성시 수정해야함. 

    
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