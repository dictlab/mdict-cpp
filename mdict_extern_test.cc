//
// Created by 陈权 on 2019/2/7.
//

#include "mdict_extern.h"

#include <sys/time.h>

#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include "encode/base64.h"
#include "encode/api.h"
#include <vector>
#include <algorithm>

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

int main(int argc, char **argv) {
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
  void *dict = mdict_init(argv[1]);

  int64 t2 = Timetool::getSystemTime();
  std::cout << "init cost time: " << t2 - t1 << "ms" << std::endl;

  char *result[0];
  mdict_lookup(dict, argv[2], result);
  std::string difinition(*result);
  std::cout << difinition << std::endl;

  int64 t3 = Timetool::getSystemTime();
  std::cout << "lookup cost time: " << t3 - t2 << " ms" << std::endl;


  unsigned long key_list_len = 0;

  simple_key_item** key_list_result = mdict_keylist(dict, &key_list_len);
  std::cout << "key list length: " << key_list_len << std::endl;

  auto key0 = key_list_result[0];
  auto keylen = key_list_result[key_list_len-1];

  // Add this to process all keys
  std::cout << "\nProcessing all keys:\n";
  for (unsigned long i = 0; i < key_list_len; ++i) {
    auto key = key_list_result[i];
    char* definition = nullptr;
    
    // Get raw hex definition
    mdict_parse_definition(dict, key->key_word, key->record_start, &definition);
    
    if (definition) {
      // Convert using our function
      char* utf8_def = hex_string_to_utf8(definition);
        
      std::cout << "Key: " << key->key_word 
		<< "\nHex length: " << strlen(definition)
		<< "\nUTF-8 length: " << (utf8_def ? strlen(utf8_def) : 0)
		<< "\n-------------------------\n";
        
      if (utf8_def) {
	free(utf8_def);
      }
      free(definition);
    }
  }


  char* result_len_1 = nullptr;  // Use a single pointer instead of an array.

  mdict_parse_definition(dict, keylen->key_word, keylen->record_start, &result_len_1);

  // Ensure that result_len_1 is valid before using it.
  if (result_len_1 != nullptr) {
        std::string encoded = base64_from_hex(result_len_1);
        std::cout << "defintion encoded in base64:  " << encoded << std::endl;
        
    } 
    // Free allocated memory.
    free(result_len_1);
   

  free_simple_key_list(key_list_result, key_list_len);
  mdict_destory(dict);


  if (*result != nullptr) {
    free(*result);
  }


}
