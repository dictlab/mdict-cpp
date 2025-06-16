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

uint32_t adler32checksum(const unsigned char *data, uint32_t len);
