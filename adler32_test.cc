// adler32.cpp - originally written and placed in the public domain by Wei Dai

#include "adler32.h"
#include <gtest/gtest.h>




int main() {
//  Adler32 adler32hasher;
  char* str = const_cast<char *>("helloworld");
  uint32_t hash = adler32checksum((unsigned char *) str, 10);
  assert(hash == 0x1736043D);
}
