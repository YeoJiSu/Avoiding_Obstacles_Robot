#include "direction.h"
#include "trace.h"
#include "lcd2.h"
#include "motor.h"
#include <stdbool.h>

#define ROTATE_90DEG_DLY 14000000
#define SHORT_DLY 100000

typedef struct robot {
    Direction direction; // 출발점의 시야
    bool isGo; // 정지 또는 출발
    Trace* trace; // 지나온 길 (MAP)
    int FB; // 앞뒤 변위 (결승점까지 남은 거리)
    int LR; // 좌우 변위 (좌회전, 우회전 결정요소)
}robot;

// 로봇이 바라보는 방향 상태를 기준으로 좌회전한 상태로 변경
void ROBOT_ROTATE_DIR_L(robot* rbt_ptr);
// 로봇이 바라보는 방향 상태를 기준으로 우회전한 상태로 변경
void ROBOT_ROTATE_DIR_R(robot* rbt_ptr);


// 로봇이 전방을 바라보는 지 체크
bool ROBOT_CHK_DIR_FW(robot* rbt_ptr);

// 로봇이 후방을 바라보는 지 체크 (쓰진 않음)
bool ROBOT_CHK_DIR_B(robot* rbt_ptr);

// 로봇이 좌측을 바라보는 지 체크
bool ROBOT_CHK_DIR_L(robot* rbt_ptr);

// 로봇이 우측을 바라보는 지 체크
bool ROBOT_CHK_DIR_R(robot* rbt_ptr);


// 로봇의 움직임 제어 _ 멈춤
void ROBOT_STOP();
// 로봇의 움직임 제어 _ 전진
void ROBOT_GO(robot* rbt_ptr);


// 로봇이 우회전하는 프로세스
void ROBOT_TURN_R(robot* rbt_ptr);

// 로봇이 좌회전하는 프로세스
void ROBOT_TURN_L(robot* rbt_ptr);

// 종료시점에서 처리하는 프로세스
void ROBOT_TURN_HEAD_TO_END(robot* rbt_ptr);


// 딜레이 함수
void Delay(int value);