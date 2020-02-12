//
//  mdict_extern.h
//  medict
//
//  Created by 陈权 on 2019/11/3.
//  Copyright © 2019 chenquan. All rights reserved.
//

#ifndef mdict_extern_h
#define mdict_extern_h

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------

/**
 init the dictionary
 */
void* mdict_init(const char* dictionary_path,const char* aff_path,const char* dic_path);

/**
 lookup a word
 */
void mdict_lookup(void* dict,const char* word, char** result);

/**
suggest  a word
*/
void mdict_suggest(void* dict, char* word, char** suggested_words, int length);

/**
 return a stem
 */
void mdict_stem(void* dict, char* word, char** suggested_words, int length);


void mdict_destory(void* dict);


//-------------------------


#ifdef __cplusplus
}
#endif


#endif /* mdict_extern_h */
