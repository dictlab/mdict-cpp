/*
 * Copyright (c) 2025-Present
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#pragma once

#include <cstdio>

/**
 * Gets the size of a binary file in bytes
 * Opens the file in binary read mode, seeks to the end to determine size,
 * then closes the file
 * 
 * @param fpath Path to the file to measure
 * @return int The size of the file in bytes, or -1 if the file cannot be opened
 */
inline static int fsizeof(const char* fpath) {
  FILE* f = std::fopen(fpath, "rb");
  std::fseek(f, 0, SEEK_END);  // seek to end of file
  auto size = std::ftell(f);   // get current file pointer
  std::fseek(f, 0, SEEK_SET);  // seek back to beginning of file
  std::fclose(f);
  return size;
}
