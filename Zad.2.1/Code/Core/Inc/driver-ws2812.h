#ifndef WS2812_DRIVER_H
#define WS2812_DRIVER_H

#include "stm32g4xx_hal.h"
#include <string.h>

/*
 *	CONFIG
*/
#define WS2812_LED_COUNT 	1

#define WS2812_TIMER 		htim1
#define WS2812_CHANNEL 		TIM_CHANNEL_1
extern TIM_HandleTypeDef WS2812_TIMER;

/*
 * 	HIGH - 0.8us on time (0,8/(1,25/100)=64% duty cycle
 * 	LOW - 0.4us on time (0,4/(1,25/100)=32% duty cycle
 * 	Since ARR = 100 - 1, values for CCR register will be the same as duty cycle
 *
 * 	Reset > 50us, so 50/1,25>40
 * */
#define WS2812_HIGH			64
#define WS2812_LOW			32
#define WS2812_RESET		50
#define WS2812_DMA_BUF_LEN	(WS2812_LED_COUNT * 24) + WS2812_RESET

/*
 * FUNCTIONS
 * */
HAL_StatusTypeDef	WS2812_Init();
void				WS2812_SetLED(uint8_t idx, uint8_t r, uint8_t g, uint8_t b);
HAL_StatusTypeDef	WS2812_Update();
void				WS2812_FinishedCallback();

#endif
