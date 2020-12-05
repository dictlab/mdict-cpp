//
// Created by 陈权 on 2019/2/7.
//

#include "mdict.h"
#include "mdict_extern.h"

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

    auto* mdict = static_cast<mdict::Mdict*>(mdict_init(
                  "../example_data/mdx/macmillan2.mdx",
                       "../example_data/dictionary/en_US.aff",
                       "../example_data/dictionary/en_US.dic"));

  int64 t2 = Timetool::getSystemTime();
  std::cout << "init cost time: " << t2 - t1 << "ms" << std::endl;
  std::cout << mdict->lookup("hello") << std::endl;
  int64 t3 = Timetool::getSystemTime();
  std::cout << "lookup cost time: " << t3 - t2 << " ms" << std::endl;

  //
  //  int64 t4 = Timetool::getSystemTime();
  //  std::vector<std::string> wordl = mdict.suggest("hell");
  //  for(auto w1=wordl.begin(); w1 != wordl.end(); w1++){
  //    std::cout<<(*w1)<<std::endl;
  //  }
  //  int64 t5 = Timetool::getSystemTime();
  //  std::cout << "suggest cost time: " << t5 - t4 << " ms" << std::endl;
  //
  //
  //
  //  int64 t6 = Timetool::getSystemTime();
  //  std::vector<std::string> wordl2 = mdict.stem("hell");
  //  for(auto w2=wordl2.begin(); w2 != wordl2.end(); w2++){
  //    std::cout<<(*w2)<<std::endl;
  //  }
  //  int64 t7 = Timetool::getSystemTime();
  //  std::cout << "stem cost time: " << t7 - t6 << " ms" << std::endl;
}
