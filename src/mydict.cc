/*
 * Copyright (c) 2025-Present
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#include "include/mdict_extern.h"

#include <sys/time.h>
#include <unistd.h>  // for getopt
#include <encode/base64.h>
#include <nanobench.h>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <iostream>
#include <string>

typedef long long int64;

// Helper function to check if a string ends with a specific suffix
bool ends_with(const std::string &str, const std::string &suffix) {
  if (str.length() < suffix.length()) {
    return false;
  }
  return str.compare(str.length() - suffix.length(), suffix.length(), suffix) ==
    0;
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

void print_usage(const char *program_name) {
  std::cout << "Usage: " << program_name
            << " [options] <dictionary_file> [query_key]\n"
            << "Options:\n"
            << "  -l, --list        List all keys in the dictionary\n"
            << "  -h, --help        Display this help message\n"
            << "  -v, --verbose     Enable verbose output\n"
            << "  -x, --hex         Output in hex format for MDD files\n"
            << "  -n, --no-content  Only show definition existence and length\n"
            << "  -t, --timing      Show lookup/locate timing in milliseconds\n"
            << "\n"
            << "Examples:\n"
            << "  " << program_name
            << " dictionary.mdx word        # Look up a word\n"
            << "  " << program_name
            << " -l dictionary.mdx          # List all keys\n"
            << "  " << program_name
            << " -x dictionary.mdd image    # Get image in hex format\n"
            << "  " << program_name
            << " -n dictionary.mdx word     # Show only definition length\n";
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
  bool hex_output = false;
  bool no_content = false;
  bool show_timing = false;
  bool bench_base64 = false;
  int opt;

  std::string definition;
  std::string definition_hex;

  // Parse command line options
  while ((opt = getopt(argc, argv, "blhvxnt")) != -1) {
    switch (opt) {
    case 'b':
      bench_base64 = true;
      hex_output = true; // required for benchmark
      break;  
    case 'l':
      list_keys = true;
      break;
    case 'v':
      verbose = true;
      break;
    case 'x':
      hex_output = true;
      break;
    case 'n':
      no_content = true;
      break;
    case 't':
      show_timing = true;
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
  SizedData dict_handle = mdict_init(dict_file);
  void *dict = const_cast<void*>(dict_handle.data);  // extract the pointer

  int64 t2 = Timetool::getSystemTime();
  if (verbose) {
    std::cout << "Init cost time: " << t2 - t1 << "ms" << std::endl;
  }

  bool is_mdd = is_mdd_file(dict_file);
  if (is_mdd) {
    std::cout << "mdd detected" << std::endl;
  }

  if (list_keys) {
    uint64_t key_list_len = 0;
    simple_key_item **key_list_result = mdict_keylist(dict, &key_list_len);

    if (key_list_len == 0) {
      std::cerr << "No keys in dictionary\n";
      mdict_destroy(dict);
      return 1;
    }

    std::cout << "Total keys: " << key_list_len << "\n";
    if (verbose) {
      std::cout << "File type: " << (is_mdd ? "MDD" : "MDX") << "\n\n";
    }

    for (unsigned long i = 0; i < key_list_len; ++i) {
      simple_key_item *key = key_list_result[i];
      if (!key || !key->key_word) continue;

      std::string original_str = key->key_word;

      if (verbose) {
        std::cout << "<================ start key index :[" << i
                  << "] =================>\n";
        std::cout << "Original string: " << original_str << "\n";
        std::cout << (is_mdd ? "Decoded (MDD): " : "Key (MDX): ")
                  << original_str << "\n";
        std::cout << "<================   end key index :[" << i
                  << "] =================>\n";
      } else {
        std::cout << original_str << "\n";
      }
    }

    free_simple_key_list(key_list_result, key_list_len);
  } else {
    SizedData lookup_result = { nullptr, 0 };  // default empty
    int64 lookup_start = Timetool::getSystemTime();

    if (!is_mdd) {
      lookup_result = mdict_lookup(dict, query_key);
    } else {
      // ternay operator, hex_output = true, then first option is used.
      mdict_encoding_t encoding = hex_output ? MDICT_ENCODING_HEX : MDICT_ENCODING_BASE64;
      lookup_result = mdict_locate(dict, query_key, encoding);
    }

    int64 lookup_end = Timetool::getSystemTime();

    if (show_timing) {
      std::cout << "Lookup/Locate time: " << (lookup_end - lookup_start) << "ms" << std::endl;
    }

    // Example: convert to string if needed
    std::string definition((const char*)lookup_result.data, lookup_result.size);

    if (verbose) {
      std::cout << ">>[definition start] [" << query_key
		<< "] length: " << definition.length() << " >>" << std::endl;
    }


    if (no_content) {
      if (definition.empty()) {
	std::cout << "-1" << std::endl;
      } else {
	std::cout << definition.length() << std::endl;
      }
    } else {
      std::string ext;
      const char *dot = strrchr(query_key, '.');
      if (dot && *(dot + 1)) {
	ext = dot + 1;
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
      }


      std::string mime_type = mime_detect(query_key);

      if (bench_base64) {
	ankerl::nanobench::Bench bench;
	bench.timeUnit(std::chrono::milliseconds(1), "ms");
	bench.output(nullptr);

	bench.run("base64_from_hex", [&] {
	  std::string hex_str(reinterpret_cast<const char*>(lookup_result.data),
			      lookup_result.size);
	  ankerl::nanobench::doNotOptimizeAway(base64_from_hex(hex_str));
	});

	auto const& r = bench.results()[0];
	double ms  = r.median(ankerl::nanobench::Result::Measure::elapsed) * 1000.0;
	double ops = 1.0 / r.median(ankerl::nanobench::Result::Measure::elapsed);
	double err = r.medianAbsolutePercentError(
						  ankerl::nanobench::Result::Measure::elapsed) * 100.0;

	char msbuf[32];
	snprintf(msbuf, sizeof(msbuf), "%8.3f", ms);

	printf("| %-18s | %8s | %10s | %7s |\n",
	       "benchmark", "ms/op", "op/s", "err");
	printf("|--------------------|---------:|-----------:|-------:|\n");
	printf("| %-18s | \033[32m%s\033[0m | %10.2f | %6.2f%% |\n",
	       "base64_from_hex", msbuf, ops, err);

	return 0;
      }

      if (is_mdd) {
	if (hex_output) {
	  std::cout << definition << std::endl;
	} else {
	  std::cout << "data:" << mime_type
		    << ";base64," << definition << std::endl;
	}
      } else {
	std::cout << "query key:" << query_key
		  << " | def:\n\n"
		  << definition << "\n\n";
      }
    }

    if (verbose) {
      std::cout << "<<[definition   end] [" << query_key << "] <<" << std::endl;
    }

    int64 t3 = Timetool::getSystemTime();
    if (verbose) {
      std::cout << "Lookup cost time: " << t3 - t2 << " ms" << std::endl;
    }
  }

  mdict_destroy(dict);
  return 0;
}
