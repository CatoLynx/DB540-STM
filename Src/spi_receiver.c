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

void startReceivingFrameSPI() {
	//uint8_t* buf = getInactiveFrameBuffer();
	HAL_SPI_Receive_DMA(&hspi2, BITMAP, BITMAP_SIZE);
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
	if(hspi -> Instance == SPI2) {
		HAL_GPIO_TogglePin(GPIOC, (1 << 13));
		//switchFrameBuffer();
	}
}
