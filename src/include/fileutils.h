/*
 * Copyright (c) 2020-2025
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#pragma once

#include <cstdio>

// get binary file size
inline static int fsizeof(const char* fpath) {
  FILE* f = std::fopen(fpath, "rb");
  std::fseek(f, 0, SEEK_END);  // seek to end of file
  auto size = std::ftell(f);   // get current file pointer
  std::fseek(f, 0, SEEK_SET);  // seek back to beginning of file
  std::fclose(f);
  return size;
}
