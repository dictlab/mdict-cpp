#include "adler32.h"
#include <iostream>
#include <gtest/gtest.h>
#include "mdict.h"

#define LENTH 255

int getpwd() {
    char pwd[LENTH];
    char *wd;
//getcwd函数测试
    if (!getcwd(pwd, LENTH)) {
        perror("getcwd");
        return 1;
    }
    printf("\ngetcwd pwd is %s\n", pwd);
}

std::string test_reduce0() {
    auto myDict = new mdict::Mdict("../testdict/testdict.mdx");
    return myDict->lookup("case");
}


TEST(mdict, lookup) {
    EXPECT_EQ("case", test_reduce0());    // 求合2+3=5
}


int main(int argc, char **argv) {
    getpwd();

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

