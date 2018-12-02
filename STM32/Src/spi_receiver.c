/*
 * spi_receiver.c
 *
 *  Created on: 08.09.2018
 *      Author: Julian Metzler
 */

#include <stdint.h>
#include "bitmap.h"
#include "matrix.h"
#include "spi.h"
#include "spi_receiver.h"

static uint8_t destinationBuffer = 0;

void setDestinationBuffer(uint8_t bufIndex) {
	// Set which buffer the received data should go to.
	// 0x00 - bitmap (scrolling data) - default
	// 0x01 - static data (non-scrolling data)
	// 0x02 - static mask (mask for which data should be sourced from which buffer)
	if(bufIndex > 2) bufIndex = 0;
	destinationBuffer = bufIndex;
	HAL_SPI_Abort(&hspi2);
}

void startReceivingFrameSPI() {
	//uint8_t* buf = getInactiveFrameBuffer();
	switch(destinationBuffer) {
		case DEST_BUF_BITMAP: {
			HAL_SPI_Receive_DMA(&hspi2, BITMAP, BITMAP_SIZE);
			break;
		}

		case DEST_BUF_STATIC_DATA: {
			HAL_SPI_Receive_DMA(&hspi2, staticData, FB_SIZE);
			break;
		}

		case DEST_BUF_STATIC_MASK: {
			HAL_SPI_Receive_DMA(&hspi2, staticMask, FB_SIZE);
			break;
		}
	}
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
	if(hspi -> Instance == SPI2) {
		HAL_GPIO_TogglePin(GPIOC, (1 << 13));
		parameterUpdatePending = 1;
		//switchFrameBuffer();
	}
}
