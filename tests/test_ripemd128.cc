#include <gtest/gtest.h>

#include <cstring>
#include <string>

#include "include/ripemd128.h"

TEST(Ripemd128Test, BasicHash) {
  uint8_t msg[3] = {65, 66, 67};  // "ABC"
  const byte* hashcode = ripemd128bytes(msg, 3);
  const std::string expected = "c14a12199c66e4ba84636b0f69144c77";
  std::string actual;
  for (int i = 0; i < RMDsize / 8; i++) {
    char hex[3];
    snprintf(hex, sizeof(hex), "%02x", hashcode[i]);
    actual += hex;
  }
  EXPECT_EQ(actual, expected);
}

TEST(Ripemd128Test, BytesToDword) {
  uint8_t msg[4] = {65, 66, 67, 128};
  uint32_t x = (uint32_t)BYTES_TO_DWORD(msg);
  EXPECT_EQ(x, 2151891521);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}