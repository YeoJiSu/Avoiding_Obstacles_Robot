#include "stm32f10x.h"
#include "stdio.h"
#include "core_cm3.h"
#include "lcd.h"
#include "obstacle.h"
#include "trace.h"
#include "motor.h"
#include "sensor.h"
#include "bluetooth.h"
#include <stdbool.h>
#include <time.h>

#define GO 1
#define STOP 0

#define LCD_WIDTH_SIZE 240
#define LCD_LENGTH_SIZE 310
#define LCD_MID ((LCD_WIDTH_SIZE / 2) * ROBOT_TRACE_SPEED)

#define ARRIVAL (LCD_LENGTH_SIZE * ROBOT_TRACE_SPEED)

#define ROTATE_90DEG_DLY 14000000
#define SHORT_DLY 100000

typedef struct robot {
    Direction direction; // 출발점의 시야
    bool isGo; // 정지 또는 출발
    Trace* trace; // 지나온 길 (MAP)
    int FB; // 앞뒤 변위 (결승점까지 남은 거리)
    int LR; // 좌우 변위 (좌회전, 우회전 결정요소)
}robot;

/* function definition */

void ROBOT_ROTATE_DIR_L(robot* rbt_ptr);
void ROBOT_ROTATE_DIR_R(robot* rbt_ptr);

bool ROBOT_CHK_DIR_FW(robot* rbt_ptr);
bool ROBOT_CHK_DIR_B(robot* rbt_ptr);
bool ROBOT_CHK_DIR_L(robot* rbt_ptr);
bool ROBOT_CHK_DIR_R(robot* rbt_ptr);

void ROBOT_STOP();
void ROBOT_GO(robot* rbt_ptr);
void ROBOT_TURN_R(robot* rbt_ptr);
void ROBOT_TURN_L(robot* rbt_ptr);
void ROBOT_TURN_HEAD_TO_END(robot* rbt_ptr);

void Delay(int value);
void show_LCD_msg_Arrived(void);
bool isRobotArrived(robot* rbt_ptr);

void ROBOT_ROTATE_DIR_L(robot* rbt_ptr) {
    rbt_ptr->direction -= 1;
    if (rbt_ptr->direction < BACK)
        rbt_ptr->direction = RIGHT;
}

void ROBOT_ROTATE_DIR_R(robot* rbt_ptr) {
    rbt_ptr->direction += 1;
    if (rbt_ptr->direction > RIGHT)
        rbt_ptr->direction = BACK;
}

void ROBOT_TURN_HEAD_TO_END(robot* rbt_ptr) {
    if (rbt_ptr->LR > LCD_MID) {
        ROBOT_TURN_R(rbt);
    }
    else if (rbt_ptr->LR < LCD_MID) {
        ROBOT_TURN_L(rbt);
    }
    else {}
    while (rbt_ptr->LR != LCD_MID) {
        ROBOT_GO(rbt);
    }

    switch (rbt_ptr->direction) {
    case LEFT:
        ROBOT_TURN_R(rbt);
        break;

    case RIGHT:
        ROBOT_TURN_L(rbt);
        break;

    default:
        break;
    }
}

robot_trace_array[arrSize] = { {FORWARD, 0, LCD_MID, 0}, };
trace_index = 0; 

bool ROBOT_CHK_DIR_FW(robot* rbt_ptr) {
    return rbt_ptr->direction == FORWARD;
}

bool ROBOT_CHK_DIR_L(robot* rbt_ptr) {
    return rbt_ptr->direction == LEFT;
}

bool ROBOT_CHK_DIR_R(robot* rbt_ptr) {
    return rbt_ptr->direction == RIGHT;
}

bool ROBOT_CHK_DIR_B(robot* rbt_ptr) {
    return rbt_ptr->direction == BACK;
}

bool isRobotArrived(robot* rbt_ptr) {
    return rbt_ptr->FB >= ARRIVAL;
}

void Delay(int value) {
    int i;
    for (i = 0; i < value; i++) {}
}

void ROBOT_STOP() {
    MOTOR_SET_STOP();
}

void ROBOT_GO(robot* rbt_ptr) {
    MOTOR_SET_GO();
    switch (rbt_ptr->direction) {
    case FORWARD:
        rbt_ptr->FB++;
        break;
    case LEFT:
        rbt_ptr->LR++;
        break;
    case RIGHT:
        rbt_ptr->LR--;
        break;
    default:
        break;
    }
    Show_LCD_FB_Displacement(rbt_ptr);
    Show_LCD_LR_Displacement(rbt_ptr);
    Show_LCD_Robot_Direction(rbt_ptr);
    Record_LCD_Robot_Trace(rbt_ptr);
}

void ROBOT_TURN_R(robot* rbt_ptr) {
    MOTOR_SET_TURN_RIGHT();
    Delay(ROTATE_90DEG_DLY);
    ROBOT_GO(rbt_ptr);
    Delay(SHORT_DLY);
    ROBOT_ROTATE_DIR_R(rbt_ptr);
}

void ROBOT_TURN_L(robot* rbt_ptr) {
    MOTOR_SET_TURN_LEFT();
    Delay(ROTATE_90DEG_DLY);
    ROBOT_GO(rbt_ptr);
    Delay(SHORT_DLY);
    ROBOT_ROTATE_DIR_L(rbt_ptr);
}
void show_LCD_msg_Arrived(void) {
    LCD_ShowString(120, 300, "ARRIVED", BLUE, YELLOW);
}

int main(void)
{
    MOTOR_Configure();
    SENSOR_Configure();
    BT_Configure(); // bluetooth
    LCD_Init();

    struct robot rbt = { FORWARD, GO, robot_trace_array, 0, LCD_MID }; // 로봇 상태 초기화;
    start = clock(); // 블루투스 모듈 작성시 수정해야함. 

    LCD_ShowString(1, 1, "START", BLUE, WHITE);

    while (1) {
        if (isRobotArrived(&rbt)) { // 도착지에 도착했으면 멈춰라 
            ROBOT_STOP();
            Delay(1000000);
            break;
        }

        while (1) {
            if (isGo == ENABLE) {
                ROBOT_GO(&rbt);
                break;
            }

            if (isGo == DISABLE) {
                ROBOT_STOP();
            }
        }

        if (ROBOT_CHK_DIR_FW(&rbt) && OB_NONE()) {
            ROBOT_GO(&rbt);
        }
        else if (ROBOT_CHK_DIR_FW(&rbt) && OB_LEFT_RIGHT()) {
            Show_LCD_Obstacle_LEFT(&rbt);
            Show_LCD_Obstacle_RIGHT(&rbt);
            ROBOT_GO(&rbt);
        }
        else if (ROBOT_CHK_DIR_FW(&rbt) && OB_LEFT()) {
            Show_LCD_Obstacle_LEFT(&rbt);
            ROBOT_GO(&rbt);
        }
        else if (ROBOT_CHK_DIR_FW(&rbt) && OB_RIGHT()) {
            Show_LCD_Obstacle_RIGHT(&rbt);
            ROBOT_GO(&rbt);
        }
        else if (ROBOT_CHK_DIR_FW(&rbt) && OB_FRONT_LEFT()) {
            Show_LCD_Obstacle_FORWARD(&rbt);
            ROBOT_STOP();
            ROBOT_TURN_R(&rbt);
            Record_Trace_Array(&rbt); // setTrace
        }
        else if (ROBOT_CHK_DIR_FW(&rbt) && OB_FRONT_RIGHT()) {
            Show_LCD_Obstacle_FORWARD(&rbt);
            ROBOT_STOP();
            ROBOT_TURN_L(&rbt);
            Record_Trace_Array(&rbt); // setTrace
        }
        else if (ROBOT_CHK_DIR_R(&rbt) && OB_LEFT_RIGHT()) {
            Show_LCD_Obstacle_LEFT(&rbt);
            Show_LCD_Obstacle_RIGHT(&rbt);
            ROBOT_GO(&rbt);
        }
        else if (ROBOT_CHK_DIR_R(&rbt) && OB_LEFT()) {
            Show_LCD_Obstacle_LEFT(&rbt);
            ROBOT_GO(&rbt);
        }
        else if (ROBOT_CHK_DIR_L(&rbt) && OB_LEFT_RIGHT()) {
            Show_LCD_Obstacle_LEFT(&rbt);
            Show_LCD_Obstacle_RIGHT(&rbt);
            ROBOT_GO(&rbt);
        }
        else if (ROBOT_CHK_DIR_L(&rbt) && OB_RIGHT()) {
            Show_LCD_Obstacle_RIGHT(&rbt);
            ROBOT_GO(&rbt);
        }
        else if (ROBOT_CHK_DIR_L(&rbt) && OB_NONE()) {
            ROBOT_STOP();
            ROBOT_TURN_R(&rbt);
            Record_Trace_Array(&rbt); // setTrace
        }
        else if (ROBOT_CHK_DIR_L(&rbt) && OB_FRONT_RIGHT()) {
            Show_LCD_Obstacle_FORWARD(&rbt);
            ROBOT_STOP();
            ROBOT_TURN_R(&rbt);
            Record_Trace_Array(&rbt); // setTrace
        }
        else if (ROBOT_CHK_DIR_R(&rbt) && OB_NONE()) {
            ROBOT_STOP();
            ROBOT_TURN_L(&rbt);
            Record_Trace_Array(&rbt); // setTrace
        }
        else if (ROBOT_CHK_DIR_R(&rbt) && OB_FRONT_LEFT()) {
            Show_LCD_Obstacle_FORWARD(&rbt);
            ROBOT_STOP();
            ROBOT_TURN_L(&rbt);
            Record_Trace_Array(&rbt); // setTrace
        }
        else if (ROBOT_CHK_DIR_FW(&rbt) && OB_FRONT()) {
            Show_LCD_Obstacle_FORWARD(&rbt);
            ROBOT_STOP();
            if (rbt.LR > LCD_MID) {
                ROBOT_TURN_L(&rbt);
            }
            else {
                ROBOT_TURN_R(&rbt);
            }
            Record_Trace_Array(&rbt); // setTrace
        }
        else {
            ROBOT_GO(&rbt);
        }
    }

    ROBOT_TURN_HEAD_TO_END(&rbt);
    ROBOT_STOP();
    show_LCD_msg_Arrived();

    return 0;
}