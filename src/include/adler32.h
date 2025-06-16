/*
 * Copyright (c) 2025-Present
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <cstdlib>

typedef unsigned char byte;
typedef unsigned short word16;

/**
 * Calculates the Adler-32 checksum for a given data buffer
 * Adler-32 is a checksum algorithm that is faster than CRC-32 but less reliable
 * 
 * @param data Pointer to the input data buffer
 * @param len Length of the data buffer in bytes
 * @return uint32_t The calculated Adler-32 checksum value
 */
uint32_t adler32checksum(const unsigned char *data, uint32_t len);
