#include "trace.h"
#include "lcd.h"

void Show_LCD_Obstacle_LEFT(robot* rbt_ptr) {
    switch (rbt_ptr->direction) {
    case FORWARD:
        LCD_ShowString(rbt_ptr->LR / ROBOT_TRACE_SPEED + OB_LCD_DST, rbt_ptr->FB / ROBOT_TRACE_SPEED - ROBOT_TRACE_SPEED, "O", RED, RED);
        break;

    case LEFT:
        LCD_ShowString(rbt_ptr->LR / ROBOT_TRACE_SPEED, rbt_ptr->FB / ROBOT_TRACE_SPEED - ROBOT_TRACE_SPEED - OB_LCD_DST * 2, "O", RED, RED);
        break;

    case RIGHT:
        LCD_ShowString(rbt_ptr->LR / ROBOT_TRACE_SPEED, rbt_ptr->FB / ROBOT_TRACE_SPEED - ROBOT_TRACE_SPEED + OB_LCD_DST * 2, "O", RED, RED);
        break;

    default:
        break;
    }
}

void Show_LCD_Obstacle_RIGHT(robot* rbt_ptr) {
    switch (rbt_ptr->direction) {
    case FORWARD:
        LCD_ShowString(rbt_ptr->LR / ROBOT_TRACE_SPEED - OB_LCD_DST * 2, rbt_ptr->FB / ROBOT_TRACE_SPEED - ROBOT_TRACE_SPEED, "O", RED, RED);
        break;

    case LEFT:
        LCD_ShowString(rbt_ptr->LR / ROBOT_TRACE_SPEED, rbt_ptr->FB / ROBOT_TRACE_SPEED - ROBOT_TRACE_SPEED + OB_LCD_DST * 2, "O", RED, RED);
        break;

    case RIGHT:
        LCD_ShowString(rbt_ptr->LR / ROBOT_TRACE_SPEED, rbt_ptr->FB / ROBOT_TRACE_SPEED - ROBOT_TRACE_SPEED - OB_LCD_DST * 2, "O", RED, RED);
        break;

    default:
        break;
    }
}

void Show_LCD_Obstacle_FORWARD(robot* rbt_ptr) {
    LCD_ShowString(rbt_ptr->LR / ROBOT_TRACE_SPEED, rbt_ptr->FB / ROBOT_TRACE_SPEED + ROBOT_TRACE_SPEED, "O", RED, RED);
}


void Show_LCD_Robot_Direction(robot* rbt_ptr) {
    switch (rbt_ptr->direction) {
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

void Record_Trace_Array(robot* rbt_ptr) {
    end = clock();
    rbt_ptr->trace[trace_index].time = (int)(end - start);

    trace_index++;

    rbt_ptr->trace[trace_index].dir = rbt_ptr->direction;
    rbt_ptr->trace[trace_index].time = 0;

    start = clock();
}

void Show_LCD_FB_Displacement(robot* rbt_ptr) {
    LCD_ShowNum(60, 70, rbt_ptr->FB, 6, BLACK, WHITE);
}

void Show_LCD_LR_Displacement(robot* rbt_ptr) {
    LCD_ShowNum(150, 70, rbt_ptr->LR, 6, BLACK, WHITE);
}
void Record_LCD_Robot_Trace(robot* rbt_ptr) {
    LCD_DrawCircle(rbt_ptr->LR / ROBOT_TRACE_SPEED, rbt_ptr->FB / ROBOT_TRACE_SPEED, 1);
}