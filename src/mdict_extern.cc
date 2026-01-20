/*
 * Copyright (c) 2025-Present
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#include "include/mdict_extern.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include <cstdio>
#include <type_traits>
#include "include/mdict.h"



/**
  实现 mdict_extern.h中的方法
 */

std::string mime_detect(const std::string &filename) {
  static const std::unordered_map<std::string, std::string> mime_map = {
      {"png", "image/png"},     {"jpg", "image/jpeg"},
      {"gif", "image/gif"},     {"ico", "image/x-icon"},
      {"jpeg", "image/jpeg"},   {"webp", "image/webp"},
      {"svg", "image/svg+xml"}, {"mp3", "audio/mpeg"},
      {"mp4", "video/mp4"},     {"wav", "audio/wav"},
      {"m4a", "audio/m4a"},     {"m4v", "video/m4v"},
      {"m4b", "audio/m4b"},     {"js", "application/javascript"},
      {"css", "text/css"},      {"html", "text/html"},
      {"txt", "text/plain"},    {"ttf", "font/ttf"},
      {"otf", "font/otf"},      {"woff", "font/woff"},
      {"woff2", "font/woff2"}};

  // Find the last dot in the filename
  size_t dot_pos = filename.find_last_of('.');
  if (dot_pos == std::string::npos) {
    return "application/octet-stream";  // Default MIME type for unknown
                                        // extensions
  }

  // Get the extension and convert to lowercase
  std::string ext = filename.substr(dot_pos + 1);
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  // Look up the MIME type
  auto it = mime_map.find(ext);
  if (it != mime_map.end()) {
    return it->second;
  }

  return "application/octet-stream";  // Default MIME type for unknown
                                      // extensions
}

#ifdef __cplusplus
extern "C" {
#endif

/**
 init the dictionary
 */
SizedData mdict_init(const char *dictionary_path) {
    if (!dictionary_path) {
        fprintf(stderr, "mdict_init: null dictionary path\n");
        RETURN_SIZED(nullptr, 0);
    }
    std::string dict_file_path(dictionary_path);
    // Allocate dictionary safely on the heap
    mdict::Mdict *dict = new (std::nothrow) mdict::Mdict(dict_file_path);
    if (!dict) {
        perror("mdict_init: allocation failed");
        RETURN_SIZED(nullptr, 0);
    }
    // Initialize the dictionary
    try {
        dict->init();
    } catch (const std::exception &e) {
        fprintf(stderr, "mdict_init: exception during init: %s\n", e.what());
        delete dict;
        RETURN_SIZED(nullptr, 0);
    } catch (...) {
        fprintf(stderr, "mdict_init: unknown exception during init\n");
        delete dict;
        RETURN_SIZED(nullptr, 0);
    }
    // No copy; return the single allocated handle safely
    RETURN_SIZED(dict, sizeof(mdict::Mdict*));
}

  
/**
 lookup a word
 */

SizedData mdict_lookup(void *dict, const char *word) {
    auto *self = (mdict::Mdict *)dict;
    std::string queryWord(word);

    // Get the lookup result as std::string
    std::string s = self->lookup(queryWord);

    // Allocate a buffer that survives after return
    char *buf = (char *)malloc(s.size() + 1);
    if (!buf) {
        perror("malloc");
        RETURN_SIZED(NULL, 0); // return empty SizedData on allocation failure
    }

    // Copy string data and null-terminate
    std::copy(s.begin(), s.end(), buf);
    buf[s.size()] = '\0';

    // Wrap pointer + size into SizedData and return
    RETURN_SIZED(buf, s.size());
}


/**
 locate a word
 */
SizedData mdict_locate(void *dict, const char *word, mdict_encoding_t encoding) {
    auto *self = (mdict::Mdict *)dict;
    std::string queryWord(word);

    // Call the original locate function
    std::string s = self->locate(queryWord, encoding);

    // Allocate a buffer that survives after function returns
    char *buf = (char *)malloc(s.size() + 1);
    if (!buf) {
        perror("malloc");
        RETURN_SIZED(NULL, 0); // return empty SizedData on error
    }

    // Copy string data into the buffer and null-terminate
    std::copy(s.begin(), s.end(), buf);
    buf[s.size()] = '\0';

    // Wrap pointer + size into SizedData and return
    RETURN_SIZED(buf, s.size());
}


SizedData mdict_parse_definition(void *dict, const char *word, unsigned long record_start) {
    auto *self = (mdict::Mdict *)dict;
    std::string queryWord(word);
    std::string s = self->parse_definition(queryWord, record_start);

    // Allocate memory for a C string copy so it survives after function returns
    char *buf = (char *)malloc(s.size() + 1);
    std::copy(s.begin(), s.end(), buf);
    buf[s.size()] = '\0';

    // Wrap pointer + size into SizedData and return
    RETURN_SIZED(buf, s.size());
}

simple_key_item **mdict_keylist(void *dict, uint64_t *len) {
    auto *self = reinterpret_cast<mdict::Mdict*>(dict);
    auto keylist = self->keyList();              // copy of whatever keyList() returns
    const std::size_t n = keylist.size();
    *len = static_cast<uint64_t>(n);

    // allocate array of pointers (new T[0] is fine in C++).
    simple_key_item **items = new simple_key_item*[n];

    // safe_strdup: always returns a pointer you may free() or nullptr on catastrophic failure.
    auto safe_strdup = [](const char* s, std::size_t len) -> char* {
        if (!s || len == 0) {
            char* p = static_cast<char*>(std::malloc(1));
            if (p) p[0] = '\0';
            return p;
        }

        if (len >= SIZE_MAX / 2) len = 0; // paranoia guard
        std::size_t alloc_size = len + 1;
        char* p = static_cast<char*>(std::malloc(alloc_size));
        if (!p) {
            // Try to return an allocated marker string "<OOM>" (so caller can free it)
            const char oom_marker[] = "<OOM>";
            std::size_t oml = sizeof(oom_marker) - 1;
            char* q = static_cast<char*>(std::malloc(oml + 1));
            if (q) {
                std::memcpy(q, oom_marker, oml + 1);
                return q;
            }
            // Last-ditch: allocate 1 byte and return empty string (may still fail)
            p = static_cast<char*>(std::malloc(1));
            if (p) p[0] = '\0';
            return p; // maybe nullptr if even this failed
        }

        std::memcpy(p, s, len);
        p[len] = '\0';
        return p;
    };

    // make_item expects a pointer to a source element type with fields:
    //  - record_start (uint64_t)
    //  - key_word (something with .data() and .size())
    // We'll accept a pointer (const auto*) so it works uniformly.
    auto make_item = [&](const auto* src) -> simple_key_item* {
        simple_key_item* item = new simple_key_item;
        item->record_start = src ? src->record_start : 0;

        if (!src) {
            item->key_word = safe_strdup("<NULL>", 6);
            return item;
        }

        constexpr std::size_t MAX_KEY = 10 * 1024; // 10 KB per key
        std::size_t ksize = src->key_word.size();
        if (ksize > MAX_KEY) {
            item->key_word = safe_strdup("<TOO_LONG>", 10);
            return item;
        }

        item->key_word = safe_strdup(src->key_word.data(), ksize);
        if (!item->key_word) {
            // catastrophic; safe_strdup already tried to return "<OOM>" or "".
            // To be safe, allocate an empty string if possible:
            item->key_word = static_cast<char*>(std::malloc(1));
            if (item->key_word) item->key_word[0] = '\0';
        }
        return item;
    };

    // Detect element type of keylist and call make_item appropriately.
    using KeyListType = decltype(keylist);
    using Elem = typename KeyListType::value_type;
    constexpr bool elem_is_ptr = std::is_pointer_v<Elem>;

    for (std::size_t i = 0; i < n; ++i) {
      items[i] = make_item(keylist[i]); 
}


    return items;
}


int free_simple_key_list(simple_key_item **key_items, uint64_t len) {
  if (key_items == nullptr) {
    return 0;
  }

  for (unsigned long i = 0; i < len; i++) {
    if (key_items[i]->key_word != nullptr) {
      free(key_items[i]->key_word);
    }
    delete key_items[i];
  }

  return 0;
}

int mdict_filetype(void *dict) {
  auto *self = (mdict::Mdict *)dict;
  if (self->filetype == "MDX") {
    return 0;
  }
  return 1;
}

/**
suggest  a word
*/
void mdict_suggest(void *dict, char *word, char **suggested_words, int length) {

}

/**
 return a stem
 */
void mdict_stem(void *dict, char *word, char **suggested_words, int length) {}

int mdict_destroy(void *dict) {
    if (!dict) return -1; // nothing to free

    auto *self = (mdict::Mdict *)dict;
    delete self;
    return 0;
}

SizedData c_mime_detect(const char* filename) {
    static std::string result;       // keep it alive after return
    result = mime_detect(filename);
    // Wrap pointer + size into SizedData
    RETURN_SIZED(result.c_str(), result.size());
}

  
// this is a variant of mdict_lookup with does "atomic" lookups.
// by atomic, we mean that each lookup is done isolated from each other, preventing memory bugs
extern "C" char* mdict_atomic_lookup(const char* dictPath, const char* key) {
    try {
        mdict::Mdict dict(dictPath);
        dict.init();

        std::string html = dict.lookup(key);
      
        // Copy to heap 
        char* result = (char*)malloc(html.size() + 1);
        std::memcpy(result, html.c_str(), html.size() + 1);
        return result;

    } catch (const std::exception& e) {
        // Catch anything derived from std::exception
        const std::string msg = std::string("ERROR: ") + e.what();
        char* result = (char*)malloc(msg.size() + 1);
        std::memcpy(result, msg.c_str(), msg.size() + 1);
        result[msg.size()] = '\0';
        return result;

    } catch (...) {
        // Catch everything else
        const char* msg = "ERROR: unknown exception";
        char* result = (char*)malloc(strlen(msg) + 1);
        strcpy(result, msg);
        return result;
    }
}
  


#ifdef __cplusplus
}
#endif
