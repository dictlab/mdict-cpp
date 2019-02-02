/*
 * =====================================================================================
 *
 *       Filename:  binutils.h
 *
 *    Description:  binutils header file
 *
 *        Version:  1.0
 *        Created:  01/24/2019 23:21:16
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  terasum (terasum@163.com)
 *
 * =====================================================================================
 */


/*
 * convert BigEndian 4 bytes unsigned char array to uint32 integer
 *
 **/
uint32_t be_bin_to_u32(const unsigned char* bin /* 4 bytes char array  */);


void putbytes(const char* bytes,int len);


// ================================================
// code convert part
// ===============================================
std::string le_bin_utf16_to_utf8(const char* bytes, int offset, int len);

