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
#include <cstdint>
#include <iostream>

#include "binutils.h"

using namespace std;

int main() {
//   unsigned char a[] = {0, 0, 0, 0, 0,0xff, 0xff, 0xff};
  unsigned char a[] = {0,0,4,166,1,2,3,4};
  uint64_t n = be_bin_to_u64(a);
  printf("0x%lx\n", n);
  printf("%lu\n", n);
//  assert(n == 1190);
}
