/*
 * spi_receiver.h
 *
 *  Created on: 08.09.2018
 *      Author: Julian Metzler
 */

#ifndef SPI_RECEIVER_H_
#define SPI_RECEIVER_H_

#define DEST_BUF_BITMAP			0
#define DEST_BUF_STATIC_DATA	1
#define DEST_BUF_STATIC_MASK	2

void setDestinationBuffer(uint8_t);
void startReceivingFrameSPI();

#endif /* SPI_RECEIVER_H_ */
