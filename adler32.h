/*
 * Copyright (c) 2020-2025
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#ifndef MDICT_ADLER32_H_
#define MDICT_ADLER32_H_

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <cstdlib>

typedef unsigned char byte;
typedef unsigned short word16;

uint32_t adler32checksum(const unsigned char *data, uint32_t len);

#endif
