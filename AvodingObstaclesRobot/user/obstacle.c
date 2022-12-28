#include "obstacle.h"

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