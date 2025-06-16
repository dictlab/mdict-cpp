/*
 * Copyright (c) 2025-Present
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#pragma once
#include <vector>

#include "miniz/miniz.h"

/**
 * Decompresses zlib-compressed data into a vector
 * This function automatically handles buffer sizing and retries with larger buffers if needed
 * 
 * @param source Pointer to the compressed data
 * @param sourceLen Length of the compressed data in bytes
 * @param uncompress_bound Expected size of decompressed data (if known). If 0, will estimate based on source length
 * @return std::vector<uint8_t> The decompressed data, or empty vector if decompression fails
 */
inline std::vector<uint8_t> zlib_mem_uncompress(const void *source,
                                                size_t sourceLen,
                                                size_t uncompress_bound = 0) {
  //   throw_except_if_msg(nullptr==source||0==sourceLen,"invalid source");
  // uncompress_bound为0时将缓冲区设置为sourceLen的8倍长度
  if (!uncompress_bound)
    uncompress_bound = sourceLen << 3;
  for (;;) {
    std::vector<uint8_t> buffer(uncompress_bound);
    auto destLen = uLongf(buffer.size());
    auto err =
        uncompress(buffer.data(), &destLen,
                   reinterpret_cast<const Bytef *>(source), uLong(sourceLen));
    if (Z_OK == err)
      return std::vector<uint8_t>(buffer.data(), buffer.data() + destLen);
    else if (Z_BUF_ERROR == err) {
      // 缓冲区不足
      uncompress_bound <<= 2; // 缓冲区放大4倍再尝试
      continue;
    }
    // 其他错误抛出异常
    printf("ZLIBERR %d\n", err);
    return std::vector<uint8_t>();
  }
}

/**
 * Decompresses zlib-compressed data into a pre-allocated buffer
 * 
 * @param dest Pointer to the destination buffer where decompressed data will be stored
 * @param destLen Pointer to the size of the destination buffer. Will be updated with actual decompressed size
 * @param source Pointer to the compressed data
 * @param sourceLen Length of the compressed data in bytes
 * @return int Z_OK on success, or a zlib error code on failure
 */
inline int zlib_mem_uncompress(void *dest, size_t *destLen, const void *source,
                               size_t sourceLen) {
  //  throw_if(nullptr==source||0==sourceLen||nullptr==dest||nullptr==destLen||0==*destLen)
  auto len = uLongf(*destLen);
  auto err =
      uncompress(reinterpret_cast<Bytef *>(dest), &len,
                 reinterpret_cast<const Bytef *>(source), uLong(sourceLen));
  *destLen = size_t(len);
  return err;
}
