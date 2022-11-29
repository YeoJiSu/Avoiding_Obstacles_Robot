#include <stdio.h> // merge 충돌 실험
#include <stdbool.h>

#define arrSize 1000
#define GO 1
#define STOP 0

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
int index = 0; // 로봇 방향이 바뀔 때 증가하는 index. 결국 Trace 저장용!

int exampleF[15] = { 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 };
int exampleL[15] = { 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0 };
int exampleR[15] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0 };
//                      if(o, Dirction == LEFT)

void setNewDirectionToTrace(robot* rbt) {
    rbt->trace[index].dir = rbt->direction;
    rbt->trace[index].time = 0;
}

bool noObstacle(int idx) {
    return !exampleF[idx] && !exampleL[idx] && !exampleR[idx];
}

bool frontLeftObstacle(int idx) {
    return exampleF[idx] && exampleL[idx] && !exampleR[idx];
}

bool frontObstacle() {
    return false;
}

void timePassed(robot* rbt) {
    rbt->trace[index].time++;
}

int main(void) {
    struct robot rbt = { FORWARD, GO, trace2, 0, 0 }; // 로봇 상태 초기화;

    // robot* rbt_ptr = &rbt;
    // turnLeft(&rbt);
    // setNewDirectionToTrace(rbt_ptr);

    for (int currPos = 0; currPos < 15; currPos++)
    {
        if (noObstacle(currPos) && (rbt.direction == FORWARD)) { // 현재 위치의 장애물이 없는 경우
            // time ++;
            // setNewDir 사용 안 함;
            // increase Trace[index].time;
            timePassed(&rbt);
        }
        else if (frontLeftObstacle(currPos) && (rbt.direction == FORWARD)) {
            // frontLeft가 없어질 때 까지 회전한 후, S/W로 turnRight()를 한번만 해야 Back상태로 가지 않음. 

            while (frontLeftObstacle(currPos)) {
                // Rotate Robot() : H/W
                currPos++;
            }

            turnRight(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace

        }
        else if (leftObstacle() && rbt.direction == RIGHT) {
            // Go until noLeftObstacle;
        }
        else {

        }
    }
    // 
    /*for (int i = 0; i < index; i++)
    {
        printf()
    }*/
}