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
#define Arrival (LCD_LENGTH_SIZE * ROBOT_TRACE_SPEED)

#define ROTATE_DELAY 14000000
#define SHORT_DELAY 100000

typedef struct robot {
    Direction direction; // 출발점의 시야
    bool isGo; // 정지 또는 출발
    Trace* trace; // 지나온 길 (MAP)
    int FB; // 앞뒤 변위 (결승점까지 남은 거리)
    int LR; // 좌우 변위 (좌회전, 우회전 결정요소)
}robot;

/* function definition */

void turnLeft(robot* rbt);
void turnRight(robot* rbt);
void turn_Head_To_End(robot* rbt);

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
void Show_LCD_msg_Arrived(void);

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
    if (rbt->LR > LCD_MID) {
        robotTurnRight(rbt);
        turnRight(rbt);
    }
    else if (rbt->LR < LCD_MID) {
        robotTurnLeft(rbt);
        turnLeft(rbt);
    }
    else {}
    while (rbt->LR != LCD_MID) {
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

robot_trace_array[arrSize] = { {FORWARD, 0, LCD_MID, 0}, };
trace_index = 0; 

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
    return rbt->FB >= Arrival;
}

void Delay(int value) {
    int i;
    for (i = 0; i < value; i++) {}
}

void robotStop() {
    MOTOR_SET_STOP();
}

void robotGo(robot* rbt) {
    MOTOR_SET_GO();
    switch (rbt->direction) {
    case FORWARD:
        rbt->FB++;
        break;
    case LEFT:
        rbt->LR++;
        break;
    case RIGHT:
        rbt->LR--;
        break;
    default:
        break;
    }
    Show_LCD_FB_Displacement(rbt);
    Show_LCD_LR_Displacement(rbt);
    Show_LCD_Robot_Direction(rbt);
    Record_LCD_Robot_Trace(rbt);
}

void robotTurnRight(robot* rbt) {
    MOTOR_SET_TURN_RIGHT();
    Delay(ROTATE_DELAY);
    robotGo(rbt);
    Delay(SHORT_DELAY);
}

void robotTurnLeft(robot* rbt) {
    MOTOR_SET_TURN_LEFT();
    Delay(ROTATE_DELAY);
    robotGo(rbt);
    Delay(SHORT_DELAY);
}
void Show_LCD_msg_Arrived(void) {
    LCD_ShowString(120, 300, "ARRIVED", BLUE, YELLOW);
}

FunctionalState isGo = DISABLE;

int main(void)
{
    MOTOR_Configure();
    SENSOR_Configure();
    BT_Configure(); // bluetooth
    LCD_Init();

    struct robot rbt = { FORWARD, GO, robot_trace_array, 0, LCD_MID }; // 로봇 상태 초기화;
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
            Record_Trace_Array(&rbt); // setTrace
        }
        else if (isRobotForward(&rbt) && frontRightObstacle()) {
            //Show_LCD_Obstacle_RIGHT(&rbt);
            Show_LCD_Obstacle_FORWARD(&rbt);
            robotStop();
            robotTurnLeft(&rbt);

            turnLeft(&rbt); // setStatus
            Record_Trace_Array(&rbt); // setTrace
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
            Record_Trace_Array(&rbt); // setTrace
        }
        else if (isRobotLeft(&rbt) && frontRightObstacle()) {
            //Show_LCD_Obstacle_RIGHT(&rbt);
            Show_LCD_Obstacle_FORWARD(&rbt);
            robotStop();
            robotTurnRight(&rbt);

            turnRight(&rbt); // setStatus
            Record_Trace_Array(&rbt); // setTrace
        }
        else if (isRobotRight(&rbt) && noObstacle()) {
            robotStop();
            robotTurnLeft(&rbt);

            turnLeft(&rbt); // setStatus
            Record_Trace_Array(&rbt); // setTrace
        }
        else if (isRobotRight(&rbt) && frontLeftObstacle()) {
            //Show_LCD_Obstacle_LEFT(&rbt);
            Show_LCD_Obstacle_FORWARD(&rbt);
            robotStop();
            robotTurnLeft(&rbt);

            turnLeft(&rbt); // setStatus
            Record_Trace_Array(&rbt); // setTrace
        }
        else if (isRobotForward(&rbt) && frontObstacle()) {
            Show_LCD_Obstacle_FORWARD(&rbt);
            robotStop();
            if (rbt.LR > LCD_MID) {
                robotTurnLeft(&rbt);
                turnLeft(&rbt); // setStatus
            }
            else {
                robotTurnRight(&rbt);
                turnRight(&rbt); // setStatus
            }
            Record_Trace_Array(&rbt); // setTrace
        }
        else {
            robotGo(&rbt);
        }
    }

    turn_Head_To_End(&rbt);

    robotStop();
    Show_LCD_msg_Arrived();

    return 0;
}