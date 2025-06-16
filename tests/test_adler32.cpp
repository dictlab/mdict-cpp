#include "include/adler32.h"
#include <iostream>
#include <gtest/gtest.h>

int adler32() {
    //  Adler32 adler32hasher;
    char *str = const_cast<char *>("helloworld");
    uint32_t hash = adler32checksum((unsigned char *)str, 10);
    return hash;
//    assert(hash == 0x1736043D);
}



int sum(int a, int b) {
    return a+b;
}

TEST(sum, testSum) {
    EXPECT_EQ(5, sum(2, 3));    // 求合2+3=5
    EXPECT_NE(3, sum(3, 4));    // 求合3+4 != 3
}

TEST(adler32, testAdler32){
    EXPECT_EQ(0x1736043D, adler32());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}