/*
 * =====================================================================================
 *
 *       Filename:  binutils_test.cc
 *
 *    Description:  binutils test file
 *
 *        Version:  1.0
 *        Created:  01/24/2019 23:49:13
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  terasum (terasum@163.com)
 *
 * =====================================================================================
 */
#include <iostream>
#include <cstdint>

#include "binutils.h"

using namespace std;

int main() {
    unsigned char a[] = {0,0,4,166};
    uint32_t n = be_bin_to_u32(a);
    assert(n == 1190);
}
