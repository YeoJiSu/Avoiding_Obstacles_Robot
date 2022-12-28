#include "obstacle.h"

bool __OB_LEFT() {
    return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == Bit_RESET;
}

bool __OB_RIGHT() {
    return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3) == Bit_RESET;
}

bool __OB_FRONT() {
    return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == Bit_RESET;
}

bool OB_NONE() {
    return !__OB_FRONT() && !__OB_LEFT() && !__OB_RIGHT();
}

bool OB_FRONT_LEFT() {
    return __OB_FRONT() && __OB_LEFT() && !__OB_RIGHT();
}

bool OB_FRONT_RIGHT() {
    return __OB_FRONT() && !__OB_LEFT() && __OB_RIGHT();
}

bool OB_LEFT() {
    return !__OB_FRONT() && __OB_LEFT() && !__OB_RIGHT();
}

bool OB_RIGHT() {
    return !__OB_FRONT() && !__OB_LEFT() && __OB_RIGHT();
}
bool OB_LEFT_RIGHT() {
    return !__OB_FRONT() && __OB_LEFT() && __OB_RIGHT();
}
bool OB_FRONT() {
    return __OB_FRONT() && !__OB_LEFT() && !__OB_RIGHT();
}