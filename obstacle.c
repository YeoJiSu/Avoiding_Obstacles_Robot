
#include <stdio.h> // merge 충돌 실험 같이해보기
#include <stdbool.h>

#define arrSize 1000
#define GO 1
#define STOP 0
#define Arrival 17
#define TESTSIZE 36 // 디버깅용으로 앞으로 자료추가는 여기 숫자만 고치면 됩니다!!

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

int exampleF[TESTSIZE] = { 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};
int exampleL[TESTSIZE] = { 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,};
int exampleR[TESTSIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};
//                      if(o, Dirction == LEFT)

void setNewDirectionToTrace(robot* rbt) {
    rbt_index++;
    rbt->trace[rbt_index].dir = rbt->direction;
    rbt->trace[rbt_index].time = 0;
}

bool noObstacle(int idx) {
    return !exampleF[idx] && !exampleL[idx] && !exampleR[idx];
}

bool frontLeftObstacle(int idx) {
    return exampleF[idx] && exampleL[idx] && !exampleR[idx];
}

bool frontRightObstacle(int idx) {
    return exampleF[idx] && !exampleL[idx] && exampleR[idx];
}

bool leftObstacle(int idx) {
    return !exampleF[idx] && exampleL[idx] && !exampleR[idx];
}

bool rightObstacle(int idx) {
    return !exampleF[idx] && !exampleL[idx] && exampleR[idx];
}

bool frontObstacle(int idx) {
    return exampleF[idx] && !exampleL[idx] && !exampleR[idx];
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
void timePassed(robot* rbt) {
    rbt->trace[rbt_index].time++;
}

int main(void) {
    struct robot rbt = { FORWARD, GO, trace2, 0, 0 }; // 로봇 상태 초기화;

    // robot* rbt_ptr = &rbt;
    // turnLeft(&rbt);
    // setNewDirectionToTrace(rbt_ptr);

    for (int currPos = 0; currPos < TESTSIZE; currPos++)
    {
        if (isRobotArrived(&rbt)) { // 도착지에 도착했으면 멈춰라 
            break;
        }
		if (isRobotForward(&rbt) && noObstacle(currPos)) { // 현재 위치의 장애물이 없는 경우
            // time ++;
            // setNewDir 사용 안 함;
            // increase Trace[index].time;
            timePassed(&rbt);
        }
        if (isRobotForward(&rbt) && leftObstacle(currPos)) {
            timePassed(&rbt);
        }
        if (isRobotForward(&rbt) && rightObstacle(currPos)) {
            timePassed(&rbt);
        }
        else if (isRobotForward(&rbt) && frontLeftObstacle(currPos)) {
            // frontLeft가 없어질 때 까지 회전한 후, S/W로 turnRight()를 한번만 해야 Back상태로 가지 않음. 

            while (frontLeftObstacle(currPos)) {
                // Rotate Robot() : H/W
                currPos++;
            }

            turnRight(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
		else if (isRobotForward(&rbt) && frontRightObstacle(currPos)) {
            while (frontRightObstacle(currPos)) {
                // Rotate Robot() : H/W
                currPos++;
            }

            turnLeft(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }

		else if (isRobotRight(&rbt) && leftObstacle(currPos)) {
            // Go until noLeftObstacle;
            timePassed(&rbt);
        }
		else if (isRobotLeft(&rbt) && rightObstacle(currPos)) {
            // Go until noRightObstacle;
            timePassed(&rbt);
        }
        else if (isRobotLeft(&rbt)&& noObstacle(currPos)) {
            timePassed(&rbt);
            turnRight(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
        else if (isRobotLeft(&rbt)&& frontRightObstacle(currPos)) {
            timePassed(&rbt);
            turnRight(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
		else if (isRobotRight(&rbt) && noObstacle(currPos)) {
            timePassed(&rbt);
            turnLeft(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
		else if (isRobotRight(&rbt) && frontLeftObstacle(currPos)) {
            timePassed(&rbt);
            turnLeft(&rbt); // setStatus
            setNewDirectionToTrace(&rbt); // setTrace
        }
		else if (isRobotForward(&rbt) && frontObstacle(currPos)) {
            timePassed(&rbt);
            if (rbt.leftRight > 0) {
                while (frontObstacle(currPos)) {
                    // Rotate Robot() : H/W -> 로봇을 왼쪽으로 돌리기 
                    currPos++;
                }
                turnLeft(&rbt); // setStatus
            }
            else{
                while (frontObstacle(currPos)) {
                    // Rotate Robot() : H/W -> 로봇을 오른쪽으로 돌리기 
                    currPos++;
                }
                turnRight(&rbt); // setStatus
            }
            setNewDirectionToTrace(&rbt); // setTrace
        }

        // 직진 변위 계산하기 
        if (isRobotForward(&rbt) && (noObstacle(currPos) || rightObstacle(currPos) || leftObstacle(currPos))) {
            rbt.forwardBackward++;
        }

        // 좌우 변위 계산하기
        if (isRobotRight(&rbt) && leftObstacle(currPos)) {
            rbt.leftRight++;
        }
        if (isRobotLeft(&rbt) && rightObstacle(currPos)) {
            rbt.leftRight--;
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
}