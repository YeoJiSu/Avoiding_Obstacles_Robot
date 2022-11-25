#include "stm32f10x.h"
#include "stdio.h"

#define UP 0x20
#define DOWN 0x4
#define LEFT 0x8
#define RIGHT 0x10

// Enable Port REGISTER
#define RCC_APB2ENR (*(volatile unsigned int*) 0x40021018)

//  I/O MODE REGISTER
#define GPIOC_CRL (*(volatile unsigned int *) 0x40011000)
#define GPIOD_CRL (*(volatile unsigned int *) 0x40011400)
#define GPIOD_CRH (*(volatile unsigned int *) 0x40011404)

// INPUT PORT 
#define GPIOC_IDR (*(volatile unsigned int *) 0x40011008)
#define GPIOD_IDR  (*(volatile unsigned int *) 0x40011408)

// OUTPUT PORT D
#define GPIOD_BRR (*(volatile unsigned int *) 0x40011414)
#define GPIOD_BSRR (*(volatile unsigned int *) 0x40011410)

void Delay(vu32 cnt){
  for(; cnt!=0; cnt--);
}

int main(void)
{
  // PORT D Enable
  RCC_APB2ENR |= 0x20;
  
  // PORT C EN
  RCC_APB2ENR |= 0x10;
  
  // PORT C Pull Up-Down: 초기화, Pin 설정
  GPIOC_CRL &= !0x00FFFF00;
  GPIOC_CRL |= 0x00888800;
  
  // PORT D Pull Up-Down: 초기화, Pin 설정
  GPIOD_CRL &= ~0xFFFFFFFF;
  GPIOD_CRL |= 0x33333333;
  GPIOD_CRH &= ~0xFFFFFFFF;//0x000F0F00;
  GPIOD_CRH |=  0x33333333;//0x00030300;
  
  // IDR 초기화
  GPIOC_IDR &= 0x00000000;
  GPIOD_IDR &= 0x00000000;
  
  // BSRR, BRR의 초기화
  GPIOD_BSRR &= 0x00000000;
  GPIOD_BRR &= 0x00000000;
  
  int count =0;
  while(1){  
 
    if (!(GPIOC_IDR & 0x2000)) { // S5 TAMPER (PC13) 

      if (count==2) {
        GPIOD_BRR |= 0x4000; //PD 14
        GPIOD_BRR |=  0x2000; //PD 13 
      }
       else if (count==1) {
          GPIOD_BSRR |= 0x4000; 
          GPIOD_BSRR |=  0x2000; 
       }
    }

      else if(!(GPIOC_IDR & UP) ) { 
        count = 1;
        GPIOD_BSRR |= 0x4000; //PD 14 set
        GPIOD_BRR |=  0x2000; //PD 13 reset
         
       }
       
      else if(!(GPIOC_IDR & DOWN) ) { 
        count =2;
        GPIOD_BSRR |= 0x2000; //PD 13 set    
        GPIOD_BRR |= 0x4000; //PD 14 reset 
      }
    
      Delay(1000);

    }
  
  
}