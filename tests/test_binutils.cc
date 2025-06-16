#include <gtest/gtest.h>

#include <cstdint>

#include "include/binutils.h"

TEST(BinUtilsTest, BigEndianConversion) {
  unsigned char a[] = {0, 0, 4, 166, 1, 2, 3, 4};
  uint64_t n = be_bin_to_u64(a);
  EXPECT_EQ(n, 0x000004A601020304);
}

TEST(BinUtilsTest, ZeroBytes) {
  unsigned char a[] = {0, 0, 0, 0, 0, 0, 0, 0};
  uint64_t n = be_bin_to_u64(a);
  EXPECT_EQ(n, 0);
}

TEST(BinUtilsTest, MaxValue) {
  unsigned char a[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  uint64_t n = be_bin_to_u64(a);
  EXPECT_EQ(n, UINT64_MAX);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}