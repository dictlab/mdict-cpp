//
// Created by 陈权 on 2020/12/5.
//

#include "mdict_extern.h"

#include <algorithm>

#include "mdict.h"

/**
  实现 mdict_extern.h中的方法
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 init the dictionary
 */
void *mdict_init(const char *dictionary_path, const char *aff_path,
                 const char *dic_path) {
#ifdef ENABLE_HUNSPELL
  std::string dict_file_path(dictionary_path);
  std::string aff_file_path(aff_path);
  std::string dic_file_path(dic_path);
  auto *mydict = new mdict::Mdict(dict_file_path, aff_file_path, dic_file_path);
  mydict->init();
  return mydict;
#else
  std::string dict_file_path(dictionary_path);
  auto *mydict = new mdict::Mdict(dict_file_path);
  mydict->init();
  return mydict;
#endif
}

/**
 lookup a word
 */
void mdict_lookup(void *dict, const char *word, char **result) {
  auto *self = (mdict::Mdict *)dict;
  std::string queryWord(word);
  std::string s = self->lookup(queryWord);

  (*result) = (char *)calloc(sizeof(char), s.size() + 1);
  std::copy(s.begin(), s.end(), (*result));
  (*result)[s.size()] = '\0';
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

void mdict_destory(void *dict) {
  auto *self = (mdict::Mdict *)dict;
  delete self;
}

#ifdef __cplusplus
}
#endif
