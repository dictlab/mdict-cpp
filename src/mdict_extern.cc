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
void *mdict_init(const char *dictionary_path) {
  std::string dict_file_path(dictionary_path);
  auto *mydict = new mdict::Mdict(dict_file_path);
  mydict->init();
  return mydict;
}

/**
 lookup a word
 */

void mdict_lookup(void *dict, const char *word, char **result) {
    auto *self = (mdict::Mdict *)dict;
    std::string queryWord(word);

    std::string s = self->lookup(queryWord);

    // Create vector with null terminator
    std::vector<char> buf(s.begin(), s.end());
    buf.push_back('\0');

    // Allocate result buffer once, copy vector content
    *result = (char*)malloc(buf.size());
    if (!*result) {
        perror("malloc");
        return;
    }
    memcpy(*result, buf.data(), buf.size());
}


/**
 locate a word
 */
void mdict_locate(void *dict, const char *word, char **result, mdict_encoding_t encoding) {
    auto *self = (mdict::Mdict *)dict;
    std::string queryWord(word);

    std::string s = self->locate(queryWord, encoding);

    std::vector<char> buf(s.begin(), s.end());
    buf.push_back('\0');

    *result = (char*)malloc(buf.size());
    if (!*result) {
        perror("malloc");
        return;
    }
    memcpy(*result, buf.data(), buf.size());
}


void mdict_parse_definition(void *dict, const char *word,
                            unsigned long record_start, char **result) {
  auto *self = (mdict::Mdict *)dict;
  std::string queryWord(word);
  std::string s = self->parse_definition(queryWord, record_start);

  (*result) = (char *)calloc(sizeof(char), s.size() + 1);
  std::copy(s.begin(), s.end(), (*result));
  (*result)[s.size()] = '\0';
}

simple_key_item **mdict_keylist(void *dict, uint64_t *len) {
  auto *self = (mdict::Mdict *)dict;
  auto keylist = self->keyList();

  *len = keylist.size();
  auto *items = new simple_key_item *[keylist.size()];

  for (auto i = 0; i < keylist.size(); i++) {
    items[i] = new simple_key_item;
    auto key_word = (const char *)keylist[i]->key_word.c_str();
    auto key_size = keylist[i]->key_word.size() + 1;
    items[i]->key_word = (char *)malloc(sizeof(char) * key_size);
    strcpy(items[i]->key_word, key_word);
    items[i]->record_start = keylist[i]->record_start;
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

int mdict_destory(void *dict) {
  auto *self = (mdict::Mdict *)dict;
  delete self;
  return 0;
}


const char* c_mime_detect(const char* filename) {
    static std::string result;       // keep it alive after return
    result = mime_detect(filename);  
    return result.c_str();
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
