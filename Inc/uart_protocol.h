/*
 * uart_protocol.h
 *
 *  Created on: 08.09.2018
 *      Author: micro
 */

#ifndef UART_PROTOCOL_H_
#define UART_PROTOCOL_H_

#include "usart.h"

#define START_BYTE					0xFF
#define DEVICE_ID					0x01 // Hardcoded ID in case something like RS485 with multiple devices is ever used
#define MIN_COMMAND_LENGTH			   4 // Start byte, ID byte, length byte, command byte
#define MAX_PAYLOAD_LENGTH			 255

#define CMD_CLEAR_SCREEN			0x01
#define CMD_SINGLE_SCROLL_X			0x10
#define CMD_SET_SCROLL_STEP_X		0x11
#define CMD_SET_SCROLL_INTERVAL_X	0x12
#define CMD_SET_SCROLL_WIDTH		0x13
#define CMD_SET_SCROLL_POSITION_X	0x14
#define CMD_SET_DESTINATION_BUFFER	0x20 // Set which buffer the next data received via SPI should go to (bitmap, static data or static mask)

#define UART_RX_RING_BUFFER_SIZE 128  /* must be power of two */
#define DMA_WRITE_PTR ( (UART_RX_RING_BUFFER_SIZE - uartRx->hdmarx->Instance->CNDTR) & (UART_RX_RING_BUFFER_SIZE - 1) )

void startUartRxRingBuffer(UART_HandleTypeDef*);
void handleUartProtocol();
void handleCommand();

#endif /* UART_PROTOCOL_H_ */
