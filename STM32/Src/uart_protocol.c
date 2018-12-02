/*
 * uart_protocol.c
 *
 *  Created on: 08.09.2018
 *      Author: Julian Metzler
 */

#include <stdint.h>
#include <string.h>
#include "bitmap.h"
#include "spi_receiver.h"
#include "uart_protocol.h"

static uint8_t uartRxRingBuffer[UART_RX_RING_BUFFER_SIZE];
static UART_HandleTypeDef *uartRx;
static uint8_t uartRxRingBufferReadPtr;

static uint8_t currentCommand;
static uint8_t currentParameters[MAX_PAYLOAD_LENGTH-1];

void startUartRxRingBuffer(UART_HandleTypeDef *huart)
{
	uartRx = huart;
	HAL_UART_Receive_DMA(uartRx, uartRxRingBuffer, UART_RX_RING_BUFFER_SIZE);
	uartRxRingBufferReadPtr = 0;
}

static uint8_t uartRxRingBufferEmpty() {
	if(uartRxRingBufferReadPtr == DMA_WRITE_PTR) {
		return 1;
	}
	return 0;
}

static uint8_t uartRxRingBufferWaiting() {
	if(DMA_WRITE_PTR >= uartRxRingBufferReadPtr) {
		return DMA_WRITE_PTR - uartRxRingBufferReadPtr;
	} else {
		return (DMA_WRITE_PTR + UART_RX_RING_BUFFER_SIZE) - uartRxRingBufferReadPtr;
	}
}

static uint8_t uartRxRingBufferRead() {
	uint8_t c = 0;
	if(uartRxRingBufferReadPtr != DMA_WRITE_PTR) {
		c = uartRxRingBuffer[uartRxRingBufferReadPtr++];
		uartRxRingBufferReadPtr &= (UART_RX_RING_BUFFER_SIZE - 1);
	}
	return c;
}

void handleUartProtocol() {
	// Skip if there's nothing to read
	if(uartRxRingBufferEmpty()) return;

	// Check if the minimum number of bytes for a command is available (otherwise reading is pointless)
	uint8_t inWaiting = uartRxRingBufferWaiting();
	if(inWaiting < MIN_COMMAND_LENGTH) return;

	// Check for valid command start byte
	if(uartRxRingBufferRead() != START_BYTE) return;

	// Check if the target ID matches
	if(uartRxRingBufferRead() != DEVICE_ID) return;

	// Check how many more bytes there are to be expected
	uint8_t length = uartRxRingBufferRead();

	// Cancel if the number of expected bytes is too large
	if(length > MAX_PAYLOAD_LENGTH) return;

	// Check the actual command byte
	currentCommand = uartRxRingBufferRead();

	// There are additional bytes to read after the command byte
	if(length > 1) {
		// Blocking wait if not enough bytes are in the buffer
		while(uartRxRingBufferWaiting() < length-1);

		// Read all the remaining bytes
		for(uint8_t i = 0; i < length-1; i++) {
			currentParameters[i] = uartRxRingBufferRead();
		}
	}

	// Call the command handler
	handleCommand();
}

void handleCommand() {
	switch(currentCommand) {
		case CMD_CLEAR_SCREEN: {
			memset(BITMAP, 0x00, BITMAP_SIZE);
			break;
		}

		case CMD_SINGLE_SCROLL_X: {
			uint8_t lineIndex = currentParameters[0];
			int16_t amount = currentParameters[1];
			if(currentParameters[2] != 0) amount *= -1;
			scrollX(lineIndex, amount);
			break;
		}

		case CMD_SET_SCROLL_STEP_X: {
			uint8_t lineIndex = currentParameters[0];
			int16_t amount = currentParameters[1];
			if(currentParameters[2] != 0) amount *= -1;
			setScrollStepX(lineIndex, amount);
			break;
		}

		case CMD_SET_SCROLL_INTERVAL_X: {
			uint8_t lineIndex = currentParameters[0];
			uint16_t interval = ((uint16_t)currentParameters[1] << 8) | currentParameters[2];
			setScrollIntervalX(lineIndex, interval);
			break;
		}

		case CMD_SET_SCROLL_WIDTH: {
			uint8_t lineIndex = currentParameters[0];
			int16_t width = ((uint16_t)currentParameters[1] << 8) | currentParameters[2];
			if(currentParameters[3] != 0) width *= -1;
			setScrollWidth(lineIndex, width);
			break;
		}

		case CMD_SET_SCROLL_POSITION_X: {
			uint8_t lineIndex = currentParameters[0];
			int16_t position = ((int16_t)currentParameters[1] << 8) | currentParameters[2];
			if(currentParameters[3] != 0) position *= -1;
			setScrollPositionX(lineIndex, position);
			break;
		}

		case CMD_SET_SCROLL_STOP_POS_X: {
			uint8_t lineIndex = currentParameters[0];
			int16_t position = ((int16_t)currentParameters[1] << 8) | currentParameters[2];
			if(currentParameters[3] != 0) position *= -1;
			setScrollStopPositionX(lineIndex, position);
			break;
		}

		case CMD_SET_AUTO_SCROLL_ENABLED: {
			uint8_t lineIndex = currentParameters[0];
			uint8_t enabled = currentParameters[1];
			setAutoScrollEnabledX(lineIndex, enabled);
			break;
		}

		case CMD_SET_AUTO_SCROLL_RESET_ENABLED: {
			uint8_t lineIndex = currentParameters[0];
			uint8_t enabled = currentParameters[1];
			setAutoScrollResetEnabledX(lineIndex, enabled);
			break;
		}

		case CMD_SET_SCROLL_ENABLED: {
			uint8_t lineIndex = currentParameters[0];
			uint8_t enabled = currentParameters[1];
			setScrollEnabledX(lineIndex, enabled);
			break;
		}

		case CMD_SET_BLINK_INTERVAL: {
			uint8_t lineIndex = currentParameters[0];
			uint16_t interval = ((uint16_t)currentParameters[1] << 8) | currentParameters[2];
			setBlinkInterval(lineIndex, interval);
			break;
		}

		case CMD_SET_BLINK_INTERVAL_ON: {
			uint8_t lineIndex = currentParameters[0];
			uint16_t interval = ((uint16_t)currentParameters[1] << 8) | currentParameters[2];
			setBlinkIntervalOn(lineIndex, interval);
			break;
		}

		case CMD_SET_BLINK_INTERVAL_OFF: {
			uint8_t lineIndex = currentParameters[0];
			uint16_t interval = ((uint16_t)currentParameters[1] << 8) | currentParameters[2];
			setBlinkIntervalOff(lineIndex, interval);
			break;
		}

		case CMD_SET_DESTINATION_BUFFER: {
			uint8_t bufIndex = currentParameters[0];
			setDestinationBuffer(bufIndex);
			break;
		}
	}
}
