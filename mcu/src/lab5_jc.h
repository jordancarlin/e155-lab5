// main.h
// Josh Brake
// jbrake@hmc.edu
// 10/31/22

#ifndef LAB5_JC_H
#define LAB5_JC_H

#include "STM32L432KC.h"
#include <stm32l432xx.h>
#include <stdio.h>
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
// Custom defines
///////////////////////////////////////////////////////////////////////////////

#define ENCODER_A_NUM 6
#define ENCODER_B_NUM 8

#define ENCODER_A PA6
#define ENCODER_B PA8
#define DELAY_TIM TIM2

#endif // MAIN_H