/*
 * util.c
 *
 *  Created on: 07.10.2018
 *      Author: Julian Metzler
 */

#include "util.h"

int16_t mod(int16_t x, int16_t N) {
    return (x % N + N) %N;
}
