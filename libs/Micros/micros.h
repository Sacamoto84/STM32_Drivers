#include "main.h"
#include "gpio.h"
#include <cmath>
#include "tim.h"

#define    DWT_CYCCNT    *(volatile uint32_t*)0xE0001004
#define    DWT_CONTROL   *(volatile uint32_t*)0xE0001000
#define    SCB_DEMCR     *(volatile uint32_t*)0xE000EDFC

extern uint32_t us_count_tic;

#define delayMicroseconds(x)  DWT->CYCCNT = 0U; us_count_tic =  (x * ((SystemCoreClock)/1000000)) - 16; while(DWT->CYCCNT < us_count_tic);
#define delayMilis(x)         DWT->CYCCNT = 0U; us_count_tic =  x * (SystemCoreClock)/1000; while(DWT->CYCCNT < us_count_tic);
//DWT
void delay_micros(uint32_t us);
void DWT_Init(void);

void micros_init(TIM_HandleTypeDef * htim);
uint32_t micros(void);
