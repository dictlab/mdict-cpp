/*
 * Copyright (c) 2025-Present
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#pragma once
#include <stdint.h>

/**
 * Structure representing a single entry in a simple key list
 * Used for storing dictionary entries with their corresponding file positions
 */
typedef struct simple_key_list_item {
  uint64_t record_start;  // Supports files >4GB
  char* key_word;
} simple_key_item;
