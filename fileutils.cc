/*
 * =====================================================================================
 *
 *       Filename:  fileutils.cc
 *
 *    Description:  file utils implements
 *
 *        Version:  1.0
 *        Created:  01/24/2019 21:43:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#include <cstdio>

#include "fileutils.h"

int fsizeof(const char* fpath) {
  FILE* f = std::fopen(fpath, "rb");
  std::fseek(f, 0, SEEK_END);  // seek to end of file
  auto size = std::ftell(f);   // get current file pointer
  std::fseek(f, 0, SEEK_SET);  // seek back to beginning of file
  // proceed with allocating memory and reading the file
  std::fclose(f);
  return size;
}

#ifdef DEBUG
int main() {
  char* fn = "./vbc.cc";
  int s = fsizeof(fn);
  printf("%d", s);
}
#endif
