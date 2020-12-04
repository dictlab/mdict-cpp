#include "ripemd128.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>


int main (int argc, char *argv[])
{
   char msg[3] = {65,66,67};
   byte* hashcode = ripemd128bytes(msg, 3);
   for(int i=0;i < RMDsize/8;i++) {
     printf("%02x", hashcode[i]);
   }
   printf("\n");

   uint8_t msg2[4] = {65,66,67,128};
   uint32_t  x = (uint32_t) BYTES_TO_DWORD(msg2);
   assert(x == 2151891521);
   return 0;
}


