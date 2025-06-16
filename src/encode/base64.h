/*
 * Copyright (c) 2025-Present
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#ifndef MDICT_BASE64_H
#define MDICT_BASE64_H

#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <cctype>
#include "turbobase64/turbob64.h"
#include <cmath>

constexpr char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::vector<uint8_t> hex_to_binary(const std::string& hex_str) {
    std::vector<uint8_t> binary;
    binary.reserve(hex_str.size());

    for(char c : hex_str) {
        uint8_t value;
        if(c >= 'a' && c <= 'f') {
            value = c - 'a' + 10;
        } else if(c >= 'A' && c <= 'F') {
            value = c - 'A' + 10;
        } else if(c >= '0' && c <= '9') {
            value = c - '0';
        } else {
            throw std::runtime_error("Invalid hex character");
        }
        binary.push_back(value);
    }

    return binary;
}

void fix_padding(std::string &base64, size_t orig_size) {
    // Remove existing padding
    while (!base64.empty() && base64.back() == '=') {
        base64.pop_back();
    }

    // Calculate required padding based on original data size
    size_t mod = orig_size % 3;
    if (mod == 1) {
        base64.append(2, '=');
    } else if (mod == 2) {
        base64.append(1, '=');
    }
}


std::string base64_from_hex(const std::string& hex_str) {
    std::vector<uint8_t> binary = hex_to_binary(hex_str);
    const size_t orig_size = binary.size();
    std::string base64;
    base64.reserve(static_cast<size_t>(std::ceil(orig_size * 4 / 6.0)) + 2);

    size_t i = 0;
    for(; i < orig_size; i += 3) {
        uint8_t byte1 = binary[i];
        uint8_t byte2 = (i + 1 < orig_size) ? binary[i + 1] : 0;
        uint8_t byte3 = (i + 2 < orig_size) ? binary[i + 2] : 0;

        uint8_t index1 = (byte1 << 2) | (byte2 >> 2);
        uint8_t index2 = ((byte2 & 0x3) << 4) | byte3;
        
        base64 += b64[index1];
        base64 += b64[index2];
    }

    //  fix_padding(base64, orig_size); doesnt work as by now, gives wrong output
    return base64;
}


// Base64 encode binary data to string
std::string encode_base64(const std::vector<uint8_t>& data) {
    return std::string(data.begin(), data.end());

    const size_t encoded_size = tb64enclen(data.size());
    std::string output;
    output.resize(encoded_size);
    
    tb64enc(
        data.data(),
        data.size(),
        reinterpret_cast<uint8_t*>(const_cast<char*>(output.data()))
    );
    
    return output;
}

// Base64 decode for string input
std::string decode_base64(const std::string& encoded_input) {


    const size_t decoded_size = tb64declen(
        reinterpret_cast<const uint8_t*>(encoded_input.data()),
        encoded_input.size()
    );
    std::string output;
    output.resize(decoded_size);

    tb64dec(
        reinterpret_cast<const uint8_t*>(encoded_input.data()),
        encoded_input.size(),
        reinterpret_cast<uint8_t*>(const_cast<char*>(output.data()))
    );
    return output;
}


// Overload for vector<uint8_t> input
std::string decode_base64(const std::vector<uint8_t>& encoded_input) {
    // Convert vector to string
    std::string encoded_str(encoded_input.begin(), encoded_input.end());
    return decode_base64(encoded_str);
}

// Hex string to binary conversion
std::vector<uint8_t> hex_to_bytes(const std::string& hex_str) {
    if (hex_str.length() % 2 != 0) {
        throw std::runtime_error("Invalid hex string: odd length");
    }

    std::vector<uint8_t> bytes;
    bytes.reserve(hex_str.size() / 2);

    for (size_t i = 0; i < hex_str.size(); i += 2) {
        auto char_to_nibble = [](char c) -> uint8_t {
            c = static_cast<char>(std::toupper(c));
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
            throw std::runtime_error("Invalid hex character");
        };

        uint8_t high = char_to_nibble(hex_str[i]);
        uint8_t low = char_to_nibble(hex_str[i+1]);
        bytes.push_back((high << 4) | low);
    }

    return bytes;
}

// Binary to hex string conversion (for verification)
std::string bytes_to_hex(const std::vector<uint8_t>& bytes) {
    static const char* hex_chars = "0123456789ABCDEF";
    std::string hex_str;
    hex_str.reserve(bytes.size() * 2);
    
    for (uint8_t byte : bytes) {
        hex_str.push_back(hex_chars[byte >> 4]);
        hex_str.push_back(hex_chars[byte & 0x0F]);
    }
    
    return hex_str;
}

#endif // MDICT_BASE64_H