//
// Created by 陈权 on 2019/2/7.
//

#include "mdict_extern.h"

#include <iostream>
#include <sys/time.h>
#include <ctime>
#include <cstdlib>
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

int main() {
  int64 t1 = Timetool::getSystemTime();
  void* dict = mdict_init("/Users/chenquan/Workspace/cpp/libmdict/mdx/oale8.mdx","/Users/chenquan/Workspace/cpp/libmdict/deps/hunspell/dictionary/en_US.aff","/Users/chenquan/Workspace/cpp/libmdict/deps/hunspell/dictionary/en_US.dic" );

  int64 t2 = Timetool::getSystemTime();
  std::cout << "init cost time: " << t2 - t1 << "ms" << std::endl;


  char* result[0];
  mdict_lookup(dict, "word", result);
  std::string difinition(*result);
  std::cout << difinition << std::endl;

  int64 t3 = Timetool::getSystemTime();
  std::cout << "lookup cost time: " << t3 - t2 << " ms" << std::endl;


  mdict_destory(dict);
  if(*result != nullptr) {
    free(*result);
  }

}
