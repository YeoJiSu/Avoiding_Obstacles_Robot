#include "trace.h"
#include "lcd.h"

void Show_LCD_Obstacle_LEFT(robot* rbt) {
    switch (rbt->direction) {
    case FORWARD:
        LCD_ShowString(rbt->leftRight / ROBOT_TRACE_SPEED + OB_LCD_DST, rbt->forwardBackward / ROBOT_TRACE_SPEED - ROBOT_TRACE_SPEED, "O", RED, RED);
        break;

    case LEFT:
        LCD_ShowString(rbt->leftRight / ROBOT_TRACE_SPEED, rbt->forwardBackward / ROBOT_TRACE_SPEED - ROBOT_TRACE_SPEED - OB_LCD_DST * 2, "O", RED, RED);
        break;

    case RIGHT:
        LCD_ShowString(rbt->leftRight / ROBOT_TRACE_SPEED, rbt->forwardBackward / ROBOT_TRACE_SPEED - ROBOT_TRACE_SPEED + OB_LCD_DST * 2, "O", RED, RED);
        break;

    default:
        break;
    }
}

void Show_LCD_Obstacle_RIGHT(robot* rbt) {
    switch (rbt->direction) {
    case FORWARD:
        LCD_ShowString(rbt->leftRight / ROBOT_TRACE_SPEED - OB_LCD_DST * 2, rbt->forwardBackward / ROBOT_TRACE_SPEED - ROBOT_TRACE_SPEED, "O", RED, RED);
        break;

    case LEFT:
        LCD_ShowString(rbt->leftRight / ROBOT_TRACE_SPEED, rbt->forwardBackward / ROBOT_TRACE_SPEED - ROBOT_TRACE_SPEED + OB_LCD_DST * 2, "O", RED, RED);
        break;

    case RIGHT:
        LCD_ShowString(rbt->leftRight / ROBOT_TRACE_SPEED, rbt->forwardBackward / ROBOT_TRACE_SPEED - ROBOT_TRACE_SPEED - OB_LCD_DST * 2, "O", RED, RED);
        break;

    default:
        break;
    }
}

void Show_LCD_Obstacle_FORWARD(robot* rbt) {
    LCD_ShowString(rbt->leftRight / ROBOT_TRACE_SPEED, rbt->forwardBackward / ROBOT_TRACE_SPEED + ROBOT_TRACE_SPEED, "O", RED, RED);
}


void Show_LCD_Robot_Direction(robot* rbt) {
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

void Record_Trace_Array(robot* rbt) {
    end = clock();
    rbt->trace[trace_index].time = (int)(end - start);

    trace_index++;

    rbt->trace[trace_index].dir = rbt->direction;
    rbt->trace[trace_index].time = 0;

    start = clock();
}

void Show_LCD_FB_Displacement(robot* rbt) {
    LCD_ShowNum(60, 70, rbt->forwardBackward, 6, BLACK, WHITE);
}

void Show_LCD_LR_Displacement(robot* rbt) {
    LCD_ShowNum(150, 70, rbt->leftRight, 6, BLACK, WHITE);
}
void Record_LCD_Robot_Trace(robot* rbt) {
    LCD_DrawCircle(rbt->leftRight / ROBOT_TRACE_SPEED, rbt->forwardBackward / ROBOT_TRACE_SPEED, 1);
}