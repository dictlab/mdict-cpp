#include <gtest/gtest.h>

#include <cstring>

#include "include/ripemd128.h"

TEST(Ripemd128Test, ripemd128bytes_for_empty_string) {
  char *input = "";
  const byte *hashcode =
      ripemd128bytes(reinterpret_cast<uint8_t *>(input), strlen(input));

  const byte expected_bytes[16] = {0xcd, 0xf2, 0x62, 0x13, 0xa1, 0x50,
                                   0xdc, 0x3e, 0xcb, 0x61, 0x0f, 0x18,
                                   0xf6, 0xb3, 0x8b, 0x46};

  for (int i = 0; i < 16; i++) {
    EXPECT_EQ(hashcode[i], expected_bytes[i]);
  }
}

TEST(Ripemd128Test, ripemd128bytes_for_a) {
  char *input = "a";
  const byte *hashcode =
      ripemd128bytes(reinterpret_cast<uint8_t *>(input), strlen(input));

  const byte expected_bytes[16] = {0x86, 0xbe, 0x7a, 0xfa, 0x33, 0x9d,
                                   0x0f, 0xc7, 0xcf, 0xc7, 0x85, 0xe7,
                                   0x2f, 0x57, 0x8d, 0x33};

  for (int i = 0; i < 16; i++) {
    EXPECT_EQ(hashcode[i], expected_bytes[i]);
  }
}

TEST(Ripemd128Test, ripemd128bytes_for_abc) {
  char *input = "abc";
  const byte *hashcode =
      ripemd128bytes(reinterpret_cast<uint8_t *>(input), strlen(input));

  const byte expected_bytes[16] = {0xc1, 0x4a, 0x12, 0x19, 0x9c, 0x66,
                                   0xe4, 0xba, 0x84, 0x63, 0x6b, 0x0f,
                                   0x69, 0x14, 0x4c, 0x77};

  for (int i = 0; i < 16; i++) {
    EXPECT_EQ(hashcode[i], expected_bytes[i]);
  }
}

TEST(Ripemd128Test, ripemd128bytes_for_message_digest) {
  char *input = "message digest";
  const byte *hashcode =
      ripemd128bytes(reinterpret_cast<uint8_t *>(input), strlen(input));

  const byte expected_bytes[16] = {0x9e, 0x32, 0x7b, 0x3d, 0x6e, 0x52,
                                   0x30, 0x62, 0xaf, 0xc1, 0x13, 0x2d,
                                   0x7d, 0xf9, 0xd1, 0xb8};

  for (int i = 0; i < 16; i++) {
    EXPECT_EQ(hashcode[i], expected_bytes[i]);
  }
}

TEST(Ripemd128Test, ripemd128bytes_for_abcdefghijklmnopqrstuvwxyz) {
  char *input = "abcdefghijklmnopqrstuvwxyz";
  const byte *hashcode =
      ripemd128bytes(reinterpret_cast<uint8_t *>(input), strlen(input));

  const byte expected_bytes[16] = {0xfd, 0x2a, 0xa6, 0x07, 0xf7, 0x1d,
                                   0xc8, 0xf5, 0x10, 0x71, 0x49, 0x22,
                                   0xb3, 0x71, 0x83, 0x4e};

  for (int i = 0; i < 16; i++) {
    EXPECT_EQ(hashcode[i], expected_bytes[i]);
  }
}

TEST(
    Ripemd128Test,
    ripemd128bytes_for_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789) {
  char *input =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  const byte *hashcode =
      ripemd128bytes(reinterpret_cast<uint8_t *>(input), strlen(input));

  const byte expected_bytes[16] = {0xd1, 0xe9, 0x59, 0xeb, 0x17, 0x9c,
                                   0x91, 0x1f, 0xae, 0xa4, 0x62, 0x4c,
                                   0x60, 0xc5, 0xc7, 0x02};

  for (int i = 0; i < 16; i++) {
    EXPECT_EQ(hashcode[i], expected_bytes[i]);
  }
}

TEST(Ripemd128Test, BytesToDword) {
  uint8_t msg[4] = {65, 66, 67, 128};
  uint32_t x = (uint32_t)BYTES_TO_DWORD(msg);
  EXPECT_EQ(x, 2151891521);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}