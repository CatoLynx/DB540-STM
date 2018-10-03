/*
 * bitmap.c
 *
 *  Created on: 08.09.2018
 *      Author: Julian Metzler
 */

#include <stdlib.h>
#include <string.h>
#include "bitmap.h"

uint8_t parameterUpdatePending = 1;

void initBitmap() {
	memset(BITMAP, 0x00, BITMAP_SIZE);
}

void scrollX(uint8_t lineIndex, int16_t amount) {
	if(lineIndex >= NUM_LINES) return;
	scrollOffsetsX[lineIndex] += amount;
	if(scrollOffsetsX[lineIndex] < 0) scrollOffsetsX[lineIndex] += scrollWidths[lineIndex];
	scrollOffsetsX[lineIndex] %= scrollWidths[lineIndex];
}

void setScrollStepX(uint8_t lineIndex, int16_t amount) {
	if(lineIndex >= NUM_LINES) return;
	scrollStepsX[lineIndex] = amount;
}

void setScrollIntervalX(uint8_t lineIndex, uint16_t interval) {
	if(lineIndex >= NUM_LINES) return;
	scrollIntervalsX[lineIndex] = interval;
}

void setScrollWidth(uint8_t lineIndex, int16_t width) {
	if(lineIndex >= NUM_LINES) return;
	userScrollWidths[lineIndex] = width;
	parameterUpdatePending = 1;
}

void setScrollPositionX(uint8_t lineIndex, int16_t position) {
	if(lineIndex >= NUM_LINES) return;
	scrollOffsetsX[lineIndex] = position;
	scrollOffsetsX[lineIndex] %= scrollWidths[lineIndex];
}

uint16_t getLineWidth(uint8_t lineIndex) {
	// Returns the rightmost nonzero coordinate of the specified line
	uint16_t baseByteIndex = lineIndex * MATRIX_HEIGHT * BITMAP_WIDTH_BYTES;
	uint16_t byteIndex;
	uint16_t pixelOffset;
	uint16_t maxPixelOffset = 0;
	for(uint16_t byteOffset = BITMAP_WIDTH_BYTES - 1; byteOffset >= 0; byteOffset--) {
		// Scan through the relevant part of the bitmap byte by byte,
		// checking the last byte of each row, then the second-to last of each row etc.
		for(uint8_t rowOffset = 0; rowOffset < MATRIX_HEIGHT; rowOffset++) {
			byteIndex = baseByteIndex + rowOffset * BITMAP_WIDTH_BYTES + byteOffset;
			if(BITMAP[byteIndex] != 0x00) {
				for(uint8_t bitPos = 0; bitPos < 8; bitPos++) {
					// Check for the rightmost nonzero bit
					if((BITMAP[byteIndex] & (1 << bitPos)) != 0) {
						pixelOffset = (byteOffset * 8) - bitPos + 8;
						if(pixelOffset > maxPixelOffset) maxPixelOffset = pixelOffset;
					}
				}
			}
		}
		if(maxPixelOffset != 0) return maxPixelOffset;
		if(byteOffset == 0) break; // Prevent underflow
	}
	return 0;
}

void calculateScrollWidths() {
	for(uint8_t lineIndex = 0; lineIndex < NUM_LINES; lineIndex++) {
		if(userScrollWidths[lineIndex] <= 0) {
			uint16_t lineWidth = getLineWidth(lineIndex);
			scrollWidths[lineIndex] = lineWidth + abs(userScrollWidths[lineIndex]);
		} else {
			scrollWidths[lineIndex] = userScrollWidths[lineIndex];
		}
		if(scrollWidths[lineIndex] < MATRIX_WIDTH) {
			scrollWidths[lineIndex] = MATRIX_WIDTH;
		}
		scrollOffsetsX[lineIndex] %= scrollWidths[lineIndex];
	}
}

void updateParameters() {
	calculateScrollWidths();
	parameterUpdatePending = 0;
}

void writeFrameBuffer(uint8_t* buf) {
	if(parameterUpdatePending == 1) {
		updateParameters();
	}

	uint8_t matrixRowIndex, bitmapRowIndex;
	int16_t baseColIndex, matrixColIndex, bitmapColIndex;
	uint16_t matrixByteIndex, bitmapByteIndex;
	uint8_t matrixBitIndex, bitmapBitIndex;
	uint8_t matrixBitMask;
	for(matrixRowIndex = 0; matrixRowIndex < MATRIX_HEIGHT; matrixRowIndex++) {
		for(uint8_t lineIndex = 0; lineIndex < NUM_LINES; lineIndex++) {
			bitmapRowIndex = lineIndex * MATRIX_HEIGHT + matrixRowIndex;
			baseColIndex = scrollOffsetsX[lineIndex];
			for(matrixColIndex = 0; matrixColIndex < MATRIX_WIDTH; matrixColIndex++) {
				bitmapColIndex = baseColIndex + matrixColIndex;
				while(bitmapColIndex >= scrollWidths[lineIndex]) bitmapColIndex %= scrollWidths[lineIndex];
				matrixByteIndex = matrixRowIndex * MATRIX_TOTAL_WIDTH_BYTES + lineIndex * MATRIX_WIDTH_BYTES + matrixColIndex / 8;
				matrixBitIndex = matrixColIndex % 8;
				bitmapByteIndex = bitmapRowIndex * BITMAP_WIDTH_BYTES + bitmapColIndex / 8;
				bitmapBitIndex = bitmapColIndex % 8;
				matrixBitMask = 1 << (7 - matrixBitIndex);
				if(staticMask[matrixByteIndex] & matrixBitMask) {
					// If the static mask is set at this pixel, copy data from the static buffer
					if(staticData[matrixByteIndex] & matrixBitMask) {
						buf[matrixByteIndex] |= matrixBitMask;
					} else {
						buf[matrixByteIndex] &= ~matrixBitMask;
					}
				} else {
					// Otherwise copy data from the bitmap
					if(BITMAP[bitmapByteIndex] & (1 << (7 - bitmapBitIndex))) {
						buf[matrixByteIndex] |= matrixBitMask;
					} else {
						buf[matrixByteIndex] &= ~matrixBitMask;
					}
				}
			}
		}
	}
}

void handleScrolling() {
	int32_t counterDiff;
	for(uint8_t lineIndex = 0; lineIndex < NUM_LINES; lineIndex++) {
		if(scrollStepsX[lineIndex] == 0) continue;
		counterDiff = (int32_t)frameCounter - (int32_t)lastScrollFrameCountsX[lineIndex];
		if(counterDiff < 0) counterDiff += MAX_FRAME_COUNTER;
		if(counterDiff >= scrollIntervalsX[lineIndex]) {
			scrollX(lineIndex, scrollStepsX[lineIndex]);
			lastScrollFrameCountsX[lineIndex] = frameCounter;
		}
	}
}
