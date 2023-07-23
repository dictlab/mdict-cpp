//
// Created by 陈权 on 2019/2/7.
//

#include "mdict_extern.h"

#include <sys/time.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <cstring>
typedef long long int64;
class Timetool {
 public:
  static int64 getSystemTime() {
    timeval tv;
    gettimeofday(&tv, NULL);
    int64 t = tv.tv_sec;
    t *= 1000;
    t += tv.tv_usec / 1000;
    return t;
  }
};

int main(int argc, char** argv) {
  std::cout << "arg count: " << argc << std::endl;
  std::cout << "executable file name: " << argv[0] << std::endl;

  if (argc < 3) {
    std::cout << "please specific mdx file" << std::endl;
    return -1;
  }
  if (strcmp(argv[1], "") == 0) {
    std::cout << "please specific mdx file" << std::endl;
    return -1;
  }
  std::cout << argv[1] << std::endl;

  if (strcmp(argv[2], "") == 0) {
    std::cout << "please specific word" << std::endl;
    return -1;
  }

  int64 t1 = Timetool::getSystemTime();
  void* dict = mdict_init(argv[1], "en_US.aff", "en_US.dic");

  int64 t2 = Timetool::getSystemTime();
  std::cout << "init cost time: " << t2 - t1 << "ms" << std::endl;

  char* result[0];
  mdict_lookup(dict, argv[2], result);
  std::string difinition(*result);
  std::cout << difinition << std::endl;

  int64 t3 = Timetool::getSystemTime();
  std::cout << "lookup cost time: " << t3 - t2 << " ms" << std::endl;

  mdict_destory(dict);
  if (*result != nullptr) {
    free(*result);
  }
}
