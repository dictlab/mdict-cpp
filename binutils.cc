/*
 * =====================================================================================
 *
 *       Filename:  binutils.h
 *
 *    Description:  binutils tools function
 *
 *        Version:  1.0
 *        Created:  01/24/2019 23:21:16
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  terasum (terasum@163.com)
 *
 * =====================================================================================
 */

#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "binutils.h"
//#include "deps/miniz/miniz.h"
#include <zlib.h>

using namespace std;

char const hex_chars[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                            '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

uint32_t be_bin_to_u32(const unsigned char* bin /* 4 bytes char array  */) {
  uint32_t n = 0;
  for (int i = 0; i < 3; i++) {
    n = n | (unsigned int)bin[i];
    n = n << 8;
  }
  n = n | (unsigned int)bin[3];
  return n;
}

uint64_t be_bin_to_u64(const unsigned char* bin /* 8 bytes char array  */) {
  uint64_t n = 0;
  for (int i = 0; i < 7; i++) {
    n = n | (unsigned int)bin[i];
    n = n << 8;
  }
  n = n | (unsigned int)bin[7];
  return n;
}

uint16_t be_bin_to_u16(const unsigned char* bin /* 8 bytes char array  */) {
  uint16_t n = 0;

  for (int i = 0; i < 1; i++) {
    n = n | (uint16_t)bin[i];
    n = n << 8;
  }
  n = n | (uint16_t)bin[1];
  return n;
}

uint8_t be_bin_to_u8(const unsigned char* bin /* 8 bytes char array  */) {
  return bin[0] & 255;
}

void putbytes(const char* bytes, int len, bool hex = true,
              unsigned long startofset) {
  if (hex) {
    std::printf("<Buffer ");
    for (int i = 0; i < len - 1; i++) {
      std::printf("%02x ", bytes[i] & 255);
      //        std::printf("%02x(%d) ", bytes[i] & 255,bytes[i] & 255);
    }
    std::printf("%02x", bytes[len - 1] & 255);

    std::printf("> (%ld,%d)\n", startofset, len);
    //    std::printf(">\n");
  } else {
    std::printf("<Buffer ");
    for (int i = 0; i < len - 1; i++) {
      std::printf("%d ", bytes[i] & 255);
    }
    std::printf("%d", bytes[len - 1] & 255);
    std::printf("> (%d)\n", len);
  }
}

/*****************************************************************
 *                                                               *
 *                        ENCODING METHODS                       *
 *                                                               *
 *****************************************************************/

// 工具包装器，用于字符转换 为wstring/wbuffer适配绑定到 locale 的平面
template <class Facet>
struct usable_facet : public Facet {
 public:
  using Facet::Facet;  // inherit constructors
  ~usable_facet() {}

  // workaround for compilers without inheriting constructors:
  // template <class ...Args> usable_facet(Args&& ...args) :
  // Facet(std::forward<Args>(args)...) {}
};

template <typename internT, typename externT, typename stateT>
using facet_codecvt = usable_facet<std::codecvt<internT, externT, stateT>>;

/*************************************************
* little-endian binary to utf16 to utf8 string   *
**************************************************/

// binary to utf16->utf8
std::string le_bin_utf16_to_utf8(const char* bytes, int offset, int len) {
  // std::wstring_convert<std::codecvt<char16_t,char,std::mbstate_t>,char16_t>
  // convert;
  std::wstring_convert<facet_codecvt<char16_t, char, std::mbstate_t>, char16_t>
      conv16;
  char* cbytes = (char*)calloc(len, sizeof(char));
  if (cbytes == nullptr) {
    return "";
  }
  // TODO insecure
  std::memcpy(cbytes, bytes + (offset * sizeof(char)), len * sizeof(char));
  // convert char* to char16_t*
  char16_t* wcbytes = reinterpret_cast<char16_t*>(cbytes);

  std::u16string u16 = std::u16string(wcbytes);

  std::string u8 = conv16.to_bytes(u16);
  if (len > 0) std::free(cbytes);
  return u8;
}

std::string be_bin_to_utf8(const char* bytes, unsigned long offset,
                           unsigned long len) {
  std::string u8(bytes + offset * sizeof(char), len);
  return u8;
}

// slice srcByte to distByte
// ensure srcByte.length > len
int bin_slice(const char* srcByte, int srcByteLen, int offset, int len,
              char* distByte) {
  if (offset < 0 || offset > srcByteLen - 1) {
    return -1;
  }
  if (offset + len > srcByteLen) {
    // invalid offset & length
    return -2;
  }
  // ensure distByte has malloced
  for (int i = 0; i < len; ++i) {
    (distByte)[i] = srcByte[i + offset];
  }
  return 0;
}

int bintohex(char* bin, int len, char* target) {
  int i = 0;
  for (i = 0; i < len; i++) {
    target[i] = hex_chars[bin[i] & 0xF0 >> 4];
    target[i + 1] = hex_chars[bin[i] & 0x0F >> 0];
  }
  return i;
}
