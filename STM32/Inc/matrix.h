/*
 * matrix.h
 *
 *  Created on: 07.09.2018
 *      Author: micro
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#define NUM_LINES 5
#define MATRIX_HEIGHT 7
#define MATRIX_WIDTH 240
#define MATRIX_WIDTH_BYTES MATRIX_WIDTH / 8
#define MATRIX_TOTAL_WIDTH NUM_LINES * MATRIX_WIDTH
#define MATRIX_TOTAL_WIDTH_BYTES MATRIX_TOTAL_WIDTH / 8
#define FB_SIZE MATRIX_TOTAL_WIDTH_BYTES * MATRIX_HEIGHT

uint8_t frameBuffer1[FB_SIZE];
uint8_t frameBuffer2[FB_SIZE];
volatile uint8_t currentFrameBuffer;
volatile uint8_t* currentFrameBufferPointer;
volatile uint8_t currentActiveRow;
volatile uint8_t frameBufferSwitchPending;

void setRowData(uint8_t*);
void enableRow(uint8_t);
void disableRow(uint8_t);
void setReset(uint8_t);
void switchFrameBuffer();
uint8_t* getActiveFrameBuffer();
uint8_t* getInactiveFrameBuffer();
void startDisplayMultiplex();

#endif /* MATRIX_H_ */
