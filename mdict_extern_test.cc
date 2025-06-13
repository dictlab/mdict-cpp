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
#include <functional>

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

void for_each_key(void* dict,
                  std::function<void(simple_key_item*)> on_key) {
    unsigned long len = 0;
    simple_key_item** list = mdict_keylist(dict, &len);
    std::cerr << "[for_each_key] got len=" << len << "\n";  // diagnostic

    if (!list || len == 0) {
        std::cerr << "[for_each_key] nothing to do\n";
        return;
    }

    for (unsigned long i = 0; i < len; ++i) {
        if (!list[i]) {
            std::cerr << "[for_each_key] skipping null at i=" << i << "\n";
            continue;
        }
        on_key(list[i]);
    }
    free_simple_key_list(list, len);
}

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


uint64_t key_list_len = 0; // If the number of keys can exceed 2^32

simple_key_item** key_list_result = mdict_keylist(dict, &key_list_len);

if (key_list_len == 0) {
    std::cerr << "No keys in dictionary\n";
    mdict_destory(dict);
    return 1;
}

simple_key_item* first_key = key_list_result[0];
simple_key_item* last_key  = key_list_result[key_list_len - 1];

// Process all keys
std::cout << "\nProcessing all keys:\n";
for (unsigned long i = 0; i < key_list_len; ++i) {
    simple_key_item* key = key_list_result[i];

    // If the library inserted a NULL sentinel, stop right there
    if (!key) {
        std::cerr << "Stopping at key[" << i << "] = nullptr\n";
        break;
    }

    // Skip any entries with no key_word
    if (!key->key_word) {
        std::cerr << "Skipping key[" << i << "] with null key_word\n";
        continue;
    }

    // Safe: key->key_word is a NUL-terminated hex string
    std::string utf8 = hex_string_to_utf8(key->key_word);

  
    char* definition = nullptr;
    mdict_parse_definition(dict,
                           key->key_word,
                           key->record_start,
                           &definition);
if (definition) {
    constexpr size_t MAX_SAFE_LEN = 1024 * 1024; // 1 MB max read , still cant figure out the segfault...
    size_t len = 0;

    // Manually scan for null-byte OR cap at MAX_SAFE_LEN
    while (len < MAX_SAFE_LEN && definition[len] != '\0') ++len;

    std::cout
        << "Key: " << utf8
        << "\nHex length: " << len
        << "\nUTF-8 length: " << len
        << "\nDefiniton: \n" << base64_from_hex(definition)
        << "\n-------------------------\n";

    free(definition);
}


}

// Parse the last key’s definition
char* result_len_1 = nullptr;
mdict_parse_definition(dict, last_key->key_word, last_key->record_start, &result_len_1);
if (result_len_1) {
    free(result_len_1);
}

// Clean up
free_simple_key_list(key_list_result, key_list_len);
mdict_destory(dict);
}
