//
// Created by 陈权 on 2019/2/7.
//

#include "mdict.h"
#include <sys/time.h>
#include <ctime>
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
  mdict::Mdict mdict("/Users/chenquan/Workspace/cpp/libmdict/mdx/oale8.mdx");
  mdict.init();
  int64 t2 = Timetool::getSystemTime();
  std::cout << "init cost time: " << t2 - t1 << "ms" << std::endl;
  std::cout << mdict.lookup("word") << std::endl;
  int64 t3 = Timetool::getSystemTime();
  std::cout << "lookup cost time: " << t3 - t2 << " ms" << std::endl;
}
