/* Copyright © 2025-present MrPetals */
// this is written in pure C and must not be modified. 
// the api.h and other headers can be used to modify the behavior
// of other functions if needed.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// In Windows ssize_t is not standard.
#ifdef _WIN32
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#else
#include <sys/types.h> // POSIX ssize_t
#endif

// --- Helper Functions ---

// Converts a single hex character to its integer value. Returns -1 on error.
static int hex_char_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1; // Invalid hex char
}

// --- Conversion Functions ---

/**
 * Converts a hexadecimal string to a byte array.
 * Return is The number of bytes written on success, or -1 on any error. */
inline ssize_t hex_to_bytes(const char *hex_str, unsigned char *byte_buf, size_t buf_len) {
    if (!hex_str || !byte_buf) {
        fprintf(stderr, "Error: Null pointer passed to hex_to_bytes.\n");
        return -1;
    }

    size_t hex_len = strlen(hex_str);
    if (hex_len % 2 != 0) {
        fprintf(stderr, "Error: Hex string must have an even number of characters.\n");
        return -1;
    }

    size_t bytes_to_write = hex_len / 2;
    if (bytes_to_write > buf_len) {
        fprintf(stderr, "Error: Output buffer (size %zu) too small for hex conversion (%zu bytes needed).\n", buf_len, bytes_to_write);
        return -1;
    }

    for (size_t i = 0; i < bytes_to_write; ++i) {
        int high_nibble = hex_char_to_int(hex_str[i * 2]);
        int low_nibble = hex_char_to_int(hex_str[i * 2 + 1]);

        if (high_nibble == -1 || low_nibble == -1) {
            fprintf(stderr, "Error: Invalid hex character '%c' or '%c' found in input string.\n",
                    hex_str[i * 2], hex_str[i * 2 + 1]);
            return -1;
        }
        byte_buf[i] = (unsigned char)((high_nibble << 4) | low_nibble);
    }

    return (ssize_t)bytes_to_write;
}

/**
 * Converts a UTF-16 Little Endian byte sequence to UTF-8.
 * I treat any invalid sequences as errors.
 * Return is the number of UTF-8 bytes written (ignoring null bytes), or -1 on any error.
 */
inline ssize_t utf16le_to_utf8(const unsigned char *utf16le_data, size_t utf16le_len_bytes,
                        unsigned char *utf8_buf, size_t utf8_buf_len)
{
    if (!utf16le_data || !utf8_buf) {
         fprintf(stderr, "Error: Null pointer passed to utf16le_to_utf8.\n");
         return -1;
    }

    // UTF-16 must consist of 16-bit (2-byte) units
    if (utf16le_len_bytes % 2 != 0) {
        fprintf(stderr, "Error: UTF-16LE data length (%zu bytes) must be even.\n", utf16le_len_bytes);
        return -1;
    }

    size_t utf16_idx = 0;
    size_t utf8_idx = 0;

    while (utf16_idx < utf16le_len_bytes) {
        // Read 16-bit code unit (Little Endian: LSB first)
        uint16_t u16_char = utf16le_data[utf16_idx] | ((uint16_t)utf16le_data[utf16_idx + 1] << 8);
        utf16_idx += 2;

        uint32_t codepoint; // The 32-bit Unicode code point
        
        // Check for surrogate pairs (U+D800 to U+DFFF) or decides it is BMP
        if (u16_char >= 0xD800 && u16_char <= 0xDBFF) { // High surrogate (lead)
            if (utf16_idx < utf16le_len_bytes) {
                uint16_t next_u16_char = utf16le_data[utf16_idx] | ((uint16_t)utf16le_data[utf16_idx + 1] << 8);
                if (next_u16_char >= 0xDC00 && next_u16_char <= 0xDFFF) { // Low surrogate (trail)
                    // Valid surrogate pair found. Combine them.
                    codepoint = 0x10000 + (((uint32_t)u16_char - 0xD800) << 10) + ((uint32_t)next_u16_char - 0xDC00);
                    utf16_idx += 2; // We need to consume the low surrogate as well. 16+16 = 32
                } else {
                    fprintf(stderr, "Error: Invalid UTF-16 sequence: High surrogate U+%04X not followed by low surrogate.\n", u16_char);
                    return -1; // Invalid sequence
                }
            } else {
                fprintf(stderr, "Error: Invalid UTF-16 sequence: Dangling high surrogate U+%04X at end of input.\n", u16_char);
                return -1; // Dangling surrogate
            }
        } else if (u16_char >= 0xDC00 && u16_char <= 0xDFFF) {
             fprintf(stderr, "Error: Invalid UTF-16 sequence: Lone low surrogate U+%04X found.\n", u16_char);
             return -1; // Lone low surrogate
        } else {
            // Basic Multilingual Plane (BMP) character
            codepoint = u16_char;
        }

        // Encode the resulting codepoint to UTF-8
        size_t bytes_needed;
        if (codepoint <= 0x7F)        bytes_needed = 1;
        else if (codepoint <= 0x7FF)  bytes_needed = 2;
        else if (codepoint <= 0xFFFF) bytes_needed = 3;
        else if (codepoint <= 0x10FFFF) bytes_needed = 4;
        else {
            fprintf(stderr, "Error: Invalid Unicode code point U+%X generated.\n", codepoint);
             return -1; // Invalid codepoint (e.g., > U+10FFFF)
        }

        // Check buffer space *before* writing
        // Need space for the bytes + 1 for potential null terminator later
        if (utf8_idx + bytes_needed >= utf8_buf_len) { // Use >= to ensure space for null term
             fprintf(stderr, "Error: Output UTF-8 buffer (size %zu) too small. Needs at least %zu bytes (+ null).\n",
                     utf8_buf_len, utf8_idx + bytes_needed + 1);
             return -1; // Buffer overflow
        }

        // Write the UTF-8 bytes
        if (bytes_needed == 1) {
            // Only 7 bits are needed, this is identical to ASCII
            utf8_buf[utf8_idx++] = (unsigned char)codepoint;
        } else if (bytes_needed == 2) {
            // May need up to 11 bits
            // Pattern is 110yyyyy 10xxxxxx
                  
            // 0xC0 sets the 110 prefix
            // We shift the codepoint by 6 bits, so the 5 MSB are in the lower positions (yyyyy)
            // Then the bitwise OR to combine 11000000 with 000yyyyy
            utf8_buf[utf8_idx++] = (unsigned char)(0xC0 | (codepoint >> 6));

            // 0x80 sets the 10 prefix
            // The 0x3F mask gets the lowest 6 bits (00xxxxxx)
            // Then the bitwise OR to combine 10000000 with 00xxxxxx
            utf8_buf[utf8_idx++] = (unsigned char)(0x80 | (codepoint & 0x3F));
        } else if (bytes_needed == 3) {
            utf8_buf[utf8_idx++] = (unsigned char)(0xE0 | (codepoint >> 12));
            utf8_buf[utf8_idx++] = (unsigned char)(0x80 | ((codepoint >> 6) & 0x3F));
            utf8_buf[utf8_idx++] = (unsigned char)(0x80 | (codepoint & 0x3F));
        } else /* if bytes_needed == 4 */ { 
            utf8_buf[utf8_idx++] = (unsigned char)(0xF0 | (codepoint >> 18));
            utf8_buf[utf8_idx++] = (unsigned char)(0x80 | ((codepoint >> 12) & 0x3F));
            utf8_buf[utf8_idx++] = (unsigned char)(0x80 | ((codepoint >> 6) & 0x3F));
            utf8_buf[utf8_idx++] = (unsigned char)(0x80 | (codepoint & 0x3F));
        }
    } // End while loop

    // Ensure null termination (we checked for space earlier)
    utf8_buf[utf8_idx] = '\0';

    return (ssize_t)utf8_idx; // Return number of bytes written (excluding null)
}

int example_main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hex_string>\n", argv[0]);
        return 1;
    }

    const char *hex_input = argv[1];
    size_t hex_len = strlen(hex_input);

    if (hex_len == 0) {
        printf("Input Hex:    (empty)\n");
        printf("Output UTF-8: (empty)\n");
        return 0;
    }

    // Hex to Bytes
    // Allocate buffer for intermediate UTF-16LE data.
    // Need hex_len / 2 bytes. If hex_len is odd, hex_to_bytes will error.
    size_t utf16le_buf_size = (hex_len / 2) + 1; // Add 1 just in case (though hex_to_bytes checks evenness)
    unsigned char *utf16le_bytes = (unsigned char*)malloc(utf16le_buf_size);
    if (!utf16le_bytes) {
        perror("Error allocating memory for UTF-16LE buffer");
        return 1;
    }

    ssize_t utf16_bytes_written = hex_to_bytes(hex_input, utf16le_bytes, utf16le_buf_size);
    if (utf16_bytes_written < 0) {
        free(utf16le_bytes);
        return 1;
    }

    // UTF-16LE Bytes to UTF-8
    // Allocate buffer for UTF-8 output.
    // Estimate: Max 3 bytes UTF-8 per 1 byte UTF-16LE is generous and safe.
    // (Max is 4 UTF-8 bytes per 4 UTF-16LE bytes for surrogates, which is 1x).
    // (Max is 3 UTF-8 bytes per 2 UTF-16LE bytes for BMP, which is 1.5x).
    // So, 3x the number of *UTF-16 bytes* is very safe. Add 1 for null terminator.
    size_t utf8_buf_size = ((size_t)utf16_bytes_written * 3) + 1;
    unsigned char *utf8_output = (unsigned char*)malloc(utf8_buf_size);
    if (!utf8_output) {
        perror("Error allocating memory for UTF-8 output buffer");
        free(utf16le_bytes);
        return 1;
    }

    ssize_t utf8_bytes_written = utf16le_to_utf8(utf16le_bytes, (size_t)utf16_bytes_written,
                                                 utf8_output, utf8_buf_size);

    if (utf8_bytes_written < 0) {
        free(utf16le_bytes);
        free(utf8_output);
        return 1;
    }

    // Print Result
    printf("Input Hex:        %s\n", hex_input);
    printf("Output UTF-8:     %s\n", utf8_output);
    printf("Output UTF-8 (hex): ");
    for(ssize_t i = 0; i < utf8_bytes_written; ++i) {
        printf("%02X", utf8_output[i]);
    }
    printf("\n");

    // Cleanup
    free(utf16le_bytes);
    free(utf8_output);

    return 0; // Nothing imploded
}
