#include "robot.h"


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