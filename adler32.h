#ifndef MDICT_ADLER32_H_
#define MDICT_ADLER32_H_

#include <stdint.h>

uint32_t adler32chksum(const unsigned char* data, uint32_t len);

#endif
