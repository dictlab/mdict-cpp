//
// Created by 陈权 on 2019/2/7.
//

#include "mdict.h"

int main() {
  mdict::Mdict mdict("/Users/chenquan/Workspace/cpp/libmdict/mdx/oale8.mdx");

  mdict.init();

  std::cout << mdict.lookup("word") << std::endl;
  std::vector<std::string> ss = mdict.prefix("prefix");
  // for (std::vector<std::string>::const_iterator i = ss.begin(); i !=
  // ss.end(); ++i)
}
