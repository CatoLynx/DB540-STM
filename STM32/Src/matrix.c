/*
 * matrix.c
 *
 *  Created on: 07.09.2018
 *      Author: Julian Metzler
 */

#include <stdint.h>
#include "matrix.h"
#include "gpio.h"
#include "spi.h"
#include "tim.h"

volatile uint8_t currentFrameBuffer = 0;
volatile uint8_t currentActiveRow = 0;
volatile uint8_t frameBufferSwitchPending = 0;

void setRowData(uint8_t* lineData) {
	HAL_SPI_Transmit_DMA(&hspi1, lineData, MATRIX_TOTAL_WIDTH_BYTES);
}

void enableRow(uint8_t rowIndex) {
	HAL_GPIO_WritePin(GPIOB, (1 << (9 - rowIndex)), 1);
}

void disableRow(uint8_t rowIndex) {
	HAL_GPIO_WritePin(GPIOB, (1 << (9 - rowIndex)), 0);
}

void setReset(uint8_t state) {
	HAL_GPIO_WritePin(GPIOA, (1 << 3), !state);
}

void switchFrameBuffer() {
	frameBufferSwitchPending = 1;
}

uint8_t* getActiveFrameBuffer() {
	if(currentFrameBuffer == 0) {
		return frameBuffer1;
	} else {
		return frameBuffer2;
	}
}

uint8_t* getInactiveFrameBuffer() {
	if(currentFrameBuffer == 0) {
		return frameBuffer2;
	} else {
		return frameBuffer1;
	}
}

void startDisplayMultiplex() {
	currentActiveRow++;
	currentActiveRow %= MATRIX_HEIGHT;
	currentFrameBufferPointer = getActiveFrameBuffer();
	setRowData(currentFrameBufferPointer + (currentActiveRow * MATRIX_TOTAL_WIDTH_BYTES));
	//uint8_t* buf = getActiveFrameBuffer();
	/*for(uint8_t rowIndex = 0; rowIndex < MATRIX_HEIGHT; rowIndex++) {
		setRowData(buf + (rowIndex * MATRIX_TOTAL_WIDTH_BYTES));
		enableRow(rowIndex);
		HAL_Delay(1);
		disableRow(rowIndex);
	}*/
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
	if(hspi -> Instance == SPI1) {
		enableRow(currentActiveRow);
		HAL_TIM_Base_Start_IT(&htim1);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	if(htim -> Instance == TIM1) {
		disableRow(currentActiveRow);
		HAL_TIM_Base_Stop_IT(&htim1);
		if(frameBufferSwitchPending == 1 && currentActiveRow == MATRIX_HEIGHT - 1) {
			if(currentFrameBuffer == 0) {
				currentFrameBuffer = 1;
			} else {
				currentFrameBuffer = 0;
			}
			frameBufferSwitchPending = 0;
		}
		startDisplayMultiplex();
	}
}
