#ifndef MDICT_ADLER32_H_
#define MDICT_ADLER32_H_

#include <stdint.h>
#include <assert.h>
#include <stddef.h>

#include <exception>
#include <cstdio>
#include <cstdlib>

typedef unsigned char byte;
typedef unsigned short word16;

uint32_t adler32checksum(const unsigned char *data, uint32_t len);

#endif
