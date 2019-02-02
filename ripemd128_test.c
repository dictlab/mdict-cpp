/********************************************************************\
 *
 *      FILE:     hashtest.c
 *
 *      CONTENTS: test file for sample C-implementation of
 *                RIPEMD-160 and RIPEMD128
 *        * command line arguments:                                         
 *           filename  -- compute hash code of file read binary     
 *           -sstring  -- print string & hashcode                   
 *           -t        -- perform time trial                        
 *           -x        -- execute standard test suite, ASCII input
 *        * for linkage with rmd128.c: define RMDsize as 128
 *          for linkage with rmd160.c: define RMDsize as 160 (default)
 *      TARGET:   any computer with an ANSI C compiler
 *
 *      AUTHOR:   Antoon Bosselaers, ESAT-COSIC
 *      DATE:     18 April 1996
 *      VERSION:  1.1
 *      HISTORY:  bug in RMDonemillion() corrected
 *
 *      Copyright (c) Katholieke Universiteit Leuven
 *      1996, All Rights Reserved
 *
\********************************************************************/


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ripemd128.h"

/********************************************************************/


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

/********************** end of file hashtest.c **********************/

