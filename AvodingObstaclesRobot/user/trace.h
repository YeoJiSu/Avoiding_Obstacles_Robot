#include "direction.h"

// LCD에 표현되는 장애물과 지나온 길을 표현하는 모듈

/*######################################### DEFINE #########################################*/

#define arrSize 1000 // 저장할 trace 배열 크기
#define ROBOT_TRACE_SPEED 10 // LCD에 표현할 로봇의 진행 속도

/*######################################### DEFINE END #########################################*/


/*###################################### TYPEDEF STRUCT ######################################*/

typedef struct Trace {
    Direction dir; // 방향
    int time;
    int pos_x;
    int pos_y;
}Trace;

/*###################################### TYPEDEF STRUCT END ######################################*/


/*###################################### FUNCTION DEFINITION ######################################*/

// 인식된 장애물을 로봇의 방향의 상대적으로 표현함.
void Show_LCD_Obstacle_LEFT(robot* rbt);
void Show_LCD_Obstacle_RIGHT(robot* rbt);
void Show_LCD_Obstacle_FORWARD(robot* rbt);

// LCD에 robot이 현재 바라보는 방향을 글자로 출력
void Show_LCD_Robot_Direction(robot* rbt);

// LCD에 robot의 전후좌우 변위를 배율에 맞게 출력
void Show_LCD_FB_Displacement(robot* rbt);
void Show_LCD_LR_Displacement(robot* rbt);

// LCD에 robot이 지나온 길을 drawcircle 모듈을 이용하여 출력
void Record_LCD_Robot_Trace(robot* rbt);

void Record_Trace_Array(robot* rbt);

/*###################################### FUNCTION DEFINITION END ######################################*/


/*###################################### GLOBAL VARIABLE ######################################*/

Trace robot_trace_array[arrSize];
int trace_index;// 로봇 방향이 바뀔 때 증가하는 index. 결국 Trace 저장용!


clock_t start; // 방향 전환을 끝낸 직후 시점의 시각
clock_t end; // 방향 전환을 하려하는 현재의 시각

/*###################################### GLOBAL VARIABLE END ######################################*/