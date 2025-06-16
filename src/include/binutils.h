/*
 * Copyright (c) 2025-Present
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#pragma once

#include <cstdint>
#include <string>

/*
 * Utility functions for binary data manipulation and conversion
 * All functions assume big-endian byte order unless specified otherwise
 */

/**
 * Converts a big-endian 1-byte binary array to an unsigned 8-bit integer
 * @param bin Pointer to a 1-byte unsigned char array
 * @return uint8_t The converted unsigned 8-bit integer
 */
uint8_t be_bin_to_u8(const unsigned char* bin /* 8 bytes char array*/);

/**
 * Converts a big-endian 2-byte binary array to an unsigned 16-bit integer
 * @param bin Pointer to a 2-byte unsigned char array
 * @return uint16_t The converted unsigned 16-bit integer
 */
uint16_t be_bin_to_u16(const unsigned char* bin /* 4 bytes char array  */);

/**
 * Converts a big-endian 4-byte binary array to an unsigned 32-bit integer
 * @param bin Pointer to a 4-byte unsigned char array
 * @return uint32_t The converted unsigned 32-bit integer
 */
uint32_t be_bin_to_u32(const unsigned char* bin /* 4 bytes char array  */);

/**
 * Converts a big-endian 8-byte binary array to an unsigned 64-bit integer
 * @param bin Pointer to an 8-byte unsigned char array
 * @return uint64_t The converted unsigned 64-bit integer
 */
uint64_t be_bin_to_u64(const unsigned char* bin /* 8 bytes char array*/);

/**
 * Extracts a slice of bytes from a source byte array
 * @param srcByte Source byte array
 * @param srcByteLen Length of the source byte array
 * @param offset Starting position in the source array
 * @param len Number of bytes to extract
 * @param distByte Destination byte array to store the extracted bytes
 * @return int Number of bytes successfully copied
 */
int bin_slice(const char* srcByte, int srcByteLen, int offset, int len,
              char* distByte);

/**
 * Prints bytes in either binary or hexadecimal format
 * @param bytes Source byte array to print
 * @param len Length of the byte array
 * @param hex If true, prints in hexadecimal format; if false, prints in binary
 * format
 * @param start_ofset Starting offset for display (default: 0)
 */
void putbytes(const char* bytes, int len, bool hex,
              unsigned long start_ofset = 0);

// ================================================
// code convert part
// ===============================================

/**
 * Converts a little-endian UTF-16 encoded binary array to UTF-8 string
 * @param bytes Source byte array containing UTF-16 data
 * @param offset Starting position in the byte array
 * @param len Length of the UTF-16 data in bytes
 * @return std::string The converted UTF-8 string
 */
std::string le_bin_utf16_to_utf8(const char* bytes, int offset, int len);

/**
 * Converts a big-endian binary array to UTF-8 string
 * @param bytes Source byte array
 * @param offset Starting position in the byte array
 * @param len Length of the data in bytes
 * @return std::string The converted UTF-8 string
 */
std::string be_bin_to_utf8(const char* bytes, unsigned long offset,
                           unsigned long len);

/**
 * Converts a big-endian binary array to UTF-16 string
 * @param bytes Source byte array
 * @param offset Starting position in the byte array
 * @param len Length of the data in bytes
 * @return std::string The converted UTF-16 string
 */
std::string be_bin_to_utf16(const char* bytes, unsigned long offset,
                            unsigned long len);

/**
 * Converts binary data to hexadecimal string representation
 * @param bin Source binary data
 * @param len Length of the binary data
 * @param target Destination buffer to store the hexadecimal string
 * @return int Number of characters written to the target buffer
 */
int bintohex(const char* bin, unsigned long len, char* target);
