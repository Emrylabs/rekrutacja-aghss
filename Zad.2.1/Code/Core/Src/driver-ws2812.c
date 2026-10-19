/*
 * driver-ws2812.c
 *
 *  Created on: 18 paź 2026
 *      Author: Eryk Bujak
 */
#include "driver-ws2812.h"

/*
 * WS2812 expects data in PWM-like format with period of 1,25us (800kHz)
 * Bits are encoded by changing duty cycle whereas:
 * HIGH - 0.8us on time
 * LOW - 0.4us on time
 *
 * To send it we are going to use a timer peripheral in PWM generation mode with DMA
 *
 * */
static uint8_t WS2812_DMA_BUF[WS2812_DMA_BUF_LEN];
static uint8_t volatile WS2812_DMA_RDY_FLAG;

HAL_StatusTypeDef WS2812_Init() {
	HAL_StatusTypeDef status = HAL_TIM_PWM_Init(&WS2812_TIMER);

	memset(WS2812_DMA_BUF, 0x00, WS2812_DMA_BUF_LEN);
	WS2812_DMA_RDY_FLAG = 1;

	return status;
}

void WS2812_SetLED(uint8_t idx, uint8_t r, uint8_t g, uint8_t b) {
	/*
	 * WS2812 expects green, red, blue color order
	 * MSB first
	 * */
	for(uint8_t i = 0; i < 8; i++) {
		WS2812_DMA_BUF[(idx*24)+i] = ((g >> (7 - i)) & 0x01) ? WS2812_HIGH : WS2812_LOW;
		WS2812_DMA_BUF[(idx*24)+(i+8)] = ((r >> (7 - i)) & 0x01) ? WS2812_HIGH : WS2812_LOW;
		WS2812_DMA_BUF[(idx*24)+(i+16)] = ((b >> (7 - i)) & 0x01) ? WS2812_HIGH : WS2812_LOW;
	}
}

HAL_StatusTypeDef WS2812_Update() {
	// Do not update if we are sending data already
	if(!WS2812_DMA_RDY_FLAG) {
		return HAL_BUSY;
	}

	HAL_StatusTypeDef status = HAL_TIM_PWM_Start_DMA(
			&WS2812_TIMER,
			WS2812_CHANNEL,
			(uint32_t *)WS2812_DMA_BUF,
			WS2812_DMA_BUF_LEN);

	if(status == HAL_OK) {
		WS2812_DMA_RDY_FLAG = 0;
	}

	return status;
}

void WS2812_FinishedCallback() {
	HAL_TIM_PWM_Stop_DMA(&WS2812_TIMER, WS2812_CHANNEL);
	WS2812_DMA_RDY_FLAG = 1;
}
