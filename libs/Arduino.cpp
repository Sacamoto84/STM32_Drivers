/*
 * Arduino.cpp
 *
 *  Created on: 14 мая 2023 г.
 *      Author: Ivan
 */

#include "Arduino.h"

uint32_t microTick;
uint16_t HMictotick;

uint32_t micros1(void)
{
  microTick = (HMictotick << 16) + TIM4->CNT;
  return microTick;
}

extern "C" void TIM4_IRQHandler(void)
{
  HMictotick++;
}


