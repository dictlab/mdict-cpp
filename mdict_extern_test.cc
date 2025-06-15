//
// Created by 陈权 on 2019/2/7.
//

#include "mdict_extern.h"

#include <sys/time.h>
#include <unistd.h> // for getopt

#include "encode/api.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <iostream>
#include <string>
#include "encode/api.h"
#include "encode/base64.h"
#include <algorithm>
#include <functional>

typedef long long int64;

// Helper function to check if a string ends with a specific suffix
bool ends_with(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

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

void for_each_key(void *dict, std::function<void(simple_key_item *)> on_key) {
  uint64_t len = 0;
  simple_key_item **list = mdict_keylist(dict, &len);
  std::cerr << "[for_each_key] got len=" << len << "\n"; // diagnostic

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

void print_usage(const char *program_name) {
  std::cout << "Usage: " << program_name
            << " [options] <dictionary_file> [query_key]\n"
            << "Options:\n"
            << "  -l, --list        List all keys in the dictionary\n"
            << "  -h, --help        Display this help message\n"
            << "  -v, --verbose     Enable verbose output\n"
            << "\n"
            << "Examples:\n"
            << "  " << program_name
            << " dictionary.mdx word        # Look up a word\n"
            << "  " << program_name
            << " -l dictionary.mdx          # List all keys\n";
}

bool is_mdd_file(const std::string &filename) {
  std::string ext;
  size_t dot_pos = filename.find_last_of('.');
  if (dot_pos != std::string::npos) {
    ext = filename.substr(dot_pos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
  }
  return ext == ".mdd";
}



int main(int argc, char **argv) {
    bool list_keys = false;
    bool verbose = false;
    int opt;

    std::string definition;
    std::string definition_hex;

  // Parse command line options
  while ((opt = getopt(argc, argv, "lhv")) != -1) {
    switch (opt) {
    case 'l':
      list_keys = true;
      break;
    case 'v':
      verbose = true;
      break;
    case 'h':
      print_usage(argv[0]);
      return 0;
    default:
      print_usage(argv[0]);
      return 1;
    }
  }

  // Check remaining arguments
  if (optind >= argc) {
    std::cerr << "Error: Dictionary file is required\n";
    print_usage(argv[0]);
    return 1;
  }

  const char *dict_file = argv[optind++];
  const char *query_key = nullptr;

  // If not listing keys, require a query key
  if (!list_keys) {
    if (optind >= argc) {
      std::cerr << "Error: Query key is required when not listing keys\n";
      print_usage(argv[0]);
      return 1;
    }
    query_key = argv[optind];
  }

  if (verbose) {
    std::cout << "Dictionary file: " << dict_file << std::endl;
    if (query_key) {
      std::cout << "Query key: " << query_key << std::endl;
    }
  }

  int64 t1 = Timetool::getSystemTime();
  void *dict = mdict_init(dict_file);

  int64 t2 = Timetool::getSystemTime();
  if (verbose) {
    std::cout << "Init cost time: " << t2 - t1 << "ms" << std::endl;
  }

  bool is_mdd = is_mdd_file(dict_file);

  if (list_keys) {
    uint64_t key_list_len = 0;
    simple_key_item **key_list_result = mdict_keylist(dict, &key_list_len);

    if (key_list_len == 0) {
      std::cerr << "No keys in dictionary\n";
      mdict_destory(dict);
      return 1;
    }

    std::cout << "Total keys: " << key_list_len << "\n";
    if (verbose) {
      std::cout << "File type: " << (is_mdd ? "MDD" : "MDX") << "\n\n";
    }

    for (unsigned long i = 0; i < key_list_len; ++i) {
      simple_key_item *key = key_list_result[i];
      if (!key || !key->key_word)
        continue;

      std::string original_str = key->key_word;

      if (verbose) {
        std::cout << "<================ start key index :[" << i
                  << "] =================>\n";
        std::cout << "Original string: " << original_str << "\n";
        std::cout << (is_mdd ? "Decoded (MDD): " : "Key (MDX): ") << original_str
                  << "\n";
        std::cout << "<================   end key index :[" << i
                  << "] =================>\n";
      } else {
        std::cout << original_str << "\n";
      }
    }

    free_simple_key_list(key_list_result, key_list_len);
  } else {
    char *result[0];
    if (!is_mdd) {
      mdict_lookup(dict, query_key, result);
    } else {
      mdict_locate(dict, query_key, result);
    }
    std::string definition(*result);
    if (verbose) {
      std::cout << ">>[definition start] [" << query_key
                << "] length: " << definition.length() << " >>" << std::endl;
    }
    // if query_key ends with png, output data:image/png;base64, for html
    if (ends_with(query_key, "png")) {
      std::cout << "data:image/png;base64," << definition << std::endl;
    } else if (ends_with(query_key, "jpg")) {
      std::cout << "data:image/jpeg;base64," << definition << std::endl;
    } else if (ends_with(query_key, "gif")) {
      std::cout << "data:image/gif;base64," << definition << std::endl;
    } else if (ends_with(query_key, "ico")) {
      std::cout << "data:image/x-icon;base64," << definition << std::endl;
    } else if (ends_with(query_key, "webp")) {
      std::cout << "data:image/webp;base64," << definition << std::endl;
    } else if (ends_with(query_key, "svg")) {
      std::cout << "data:image/svg+xml;base64," << definition << std::endl;
    } else if (ends_with(query_key, "mp3")) {
      std::cout << "data:audio/mpeg;base64," << definition << std::endl;
    } else if (ends_with(query_key, "mp4")) {
      std::cout << "data:video/mp4;base64," << definition << std::endl;
    } else if (ends_with(query_key, "wav")) {
      std::cout << "data:audio/wav;base64," << definition << std::endl;
    } else if (ends_with(query_key, "m4a")) {
      std::cout << "data:audio/m4a;base64," << definition << std::endl;
    } else if (ends_with(query_key, "m4v")) {
      std::cout << "data:video/m4v;base64," << definition << std::endl;
    } else if (ends_with(query_key, "m4b")) {
      std::cout << "data:audio/m4b;base64," << definition << std::endl;
    } else if (ends_with(query_key, "js")) {
      std::cout << "data:application/javascript;base64," << definition << std::endl;
    } else if (ends_with(query_key, "css")) {
      std::cout << "data:text/css;base64," << definition << std::endl;
    } else if (ends_with(query_key, "html")) {
      std::cout << "data:text/html;base64," << definition << std::endl;
    } else if (ends_with(query_key, "txt")) {
      std::cout << "data:text/plain;base64," << definition << std::endl;
    } else if (ends_with(query_key, "ttf")) {
      std::cout << "data:font/ttf;base64," << definition << std::endl;
    } else if (ends_with(query_key, "otf")) {
      std::cout << "data:font/otf;base64," << definition << std::endl;
    } else if (ends_with(query_key, "woff")) {
      std::cout << "data:font/woff;base64," << definition << std::endl;
    } else if (ends_with(query_key, "woff2")) {
      std::cout << "data:font/woff2;base64," << definition << std::endl;
    }
    else {
      std::cout << "query key:" <<query_key << "| def:" << definition << std::endl;
    }
    
    if (verbose) {
      std::cout << "<<[definition   end] [" << query_key << "]" << " <<"
                << std::endl;
    }

    int64 t3 = Timetool::getSystemTime();
    if (verbose) {
      std::cout << "Lookup cost time: " << t3 - t2 << " ms" << std::endl;
    }
  }

  mdict_destory(dict);
  return 0;
}
