/*
 * Copyright (c) 2025-Present
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#include <stdlib.h>
#include <string.h>
#include <string>
#pragma once
#include "char_decoder.h"

// Safe casting wrapper
static inline unsigned char* malloc_uc(size_t size) {
    return reinterpret_cast<unsigned char*>(malloc(size));
}

// Adaptor for utf16le_to_utf8
static inline ssize_t utf16le_to_utf8_compat(const char* src, size_t src_len,
                                            char* dst, size_t dst_len) {
    return utf16le_to_utf8(reinterpret_cast<const unsigned char*>(src),
                          src_len,
                          reinterpret_cast<unsigned char*>(dst),
                          dst_len);
}

static inline char* hex_string_to_utf8(const char* hex_input) {
    if (!hex_input) return NULL;

    size_t hex_len = strlen(hex_input);
    unsigned char* utf16_buffer = NULL;
    char* utf8_output = NULL;
    
    // Handle empty input case
    if (hex_len == 0) {
        char* result = reinterpret_cast<char*>(malloc_uc(1));
        if (result) result[0] = '\0';
        return result;
    }

    // Convert hex to UTF-16LE bytes
    size_t utf16_buf_size = hex_len / 2;
    utf16_buffer = malloc_uc(utf16_buf_size);  // Changed
    if (!utf16_buffer) return NULL;

    ssize_t utf16_bytes = hex_to_bytes(hex_input, utf16_buffer, utf16_buf_size);
    if (utf16_bytes < 0) {
        free(utf16_buffer);
        return NULL;
    }

    // Convert UTF-16LE to UTF-8
    size_t utf8_buf_size = (size_t)utf16_bytes * 3 + 1;
    utf8_output = reinterpret_cast<char*>(malloc_uc(utf8_buf_size));  // Changed
    if (!utf8_output) {
        free(utf16_buffer);
        return NULL;
    }

    // Modified function call
    ssize_t utf8_bytes = utf16le_to_utf8_compat(
        reinterpret_cast<const char*>(utf16_buffer),  // Cast input
        (size_t)utf16_bytes,
        utf8_output,  // Already char* through reinterpret_cast
        utf8_buf_size
    );
    
    free(utf16_buffer);

    if (utf8_bytes < 0) {
        free(utf8_output);
        return NULL;
    }

    // Ensure null-termination
    if ((size_t)utf8_bytes < utf8_buf_size) {
        utf8_output[utf8_bytes] = '\0';
    } else {
        free(utf8_output);
        return NULL;
    }

    return utf8_output;
}

inline std::string trim_nulls(const std::string& s) {
    size_t end = s.find_last_not_of('\0');
    if (end == std::string::npos) return ""; // all nulls
    return s.substr(0, end + 1);
}
