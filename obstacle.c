#include <stdio.h>
#define arrSize 200
#define FORWARD 1
#define RIGHT 2
#define LEFT 3
#define SPEED 1

#define ROBOT_STOP 0
#define ROBOT_GO 1
#define ROBOT_EXIT 2

int robot_status = 0;
int direction[arrSize] = {0}; // 이동 방향
int distance[arrSize] = {0};

// 적외선 센서 좌, 우 감지에 대한  example
int exampleL[15] = {0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int exampleR[15] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0};

int main(void) {
  int time = 0;
  robot_status = ROBOT_GO;

  int index = 0;
  

  for (int i = 0; i <= 14; i++) {
    if (exampleL[i] == 0 && exampleR[i] == 0) {
      time += 1;
      // direction[index] = FORWARD;
      if (!(direction[index] == LEFT || direction[index] == RIGHT)){           direction[index] = FORWARD;
        }
    }
    if (exampleL[i] == 0 && exampleR[i] == 1) {
      int j = i;
      while (1) {
        j++;
        if (exampleL[j] == 0 && exampleR[j] == 1)
          i++;
        else
          break;
      }
      // 이전단계 저장하기
      distance[index] = time*SPEED;
      time = 0;
      direction[index+1] = LEFT;
      index += 1;
    }

    if (exampleL[i] == 1 && exampleR[i] == 0) {

      int j = i;
      while (1) {
        j++;
        if (exampleL[j] == 1 && exampleR[j] == 0)
          i++;
        else
          break;
      }
      // 이전단계 저장하기
      distance[index] = time*SPEED;
      time = 0;
      direction[index+1] = RIGHT;
      index += 1;
    }
    if (exampleL[i] == 1 && exampleR[i] == 1) {

      int j = i;
      while (1) {
        j++;
        if (exampleL[j] == 1 && exampleR[j] == 1)
          i++;
        else
          break;
      }
      // 이전단계 저장하기
      direction[index] = time*SPEED;
      time = 0;
    }
    // 직진 거리 저장
    if (time != 0){
      distance[index] = time*SPEED;
    }
    
  }

  robot_status = ROBOT_EXIT;
  for (int i = 0; i <= 15; i++) {
    printf("방향: %d, 거리: %d\n", direction[i], distance[i]);
  }
  return 0;
}