/*
 * =====================================================================================
 *
 *       Filename:  binutils.h
 *
 *    Description:  binutils tools function
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


#include <cstdint>
#include <iostream>

#include "binutils.h"

using namespace std;

uint32_t be_bin_to_u32(const unsigned char* bin /* 4 bytes char array  */){
    uint32_t n = 0;
    for (int i =0 ;i < 3; i++){
        n = n | (unsigned int) bin[i];
        n = n << 8;
    }
    n = n | (unsigned int) bin[3];
    return n;
}


void putbytes(const char* bytes,int len){
    int maxlen = 10 > len ? len : 10;
    std::printf("Buffer[");
    for (int i = 0; i < maxlen - 1; i++) {
        std::printf("%x, ", bytes[i] & 255);
    }
    std::printf("%x", bytes[maxlen - 1]);
    if (maxlen != len){
        std::printf(", ..., ");
        for (int i = len - 10; i < len-1; i++) {
            std::printf("%x, ", bytes[i] & 255);
        }
        std::printf("%x", bytes[len - 1]);
    }
    std::printf("](%d)\n", len);
}



/*****************************************************************
 *                                                               *
 *                        ENCODING METHODS                       *
 *                                                               *
 *****************************************************************/



// 工具包装器，用于字符转换 为wstring/wbuffer适配绑定到 locale 的平面
template <class Facet>
struct usable_facet : public Facet {
public:
    using Facet::Facet; // inherit constructors
    ~usable_facet() {}

    // workaround for compilers without inheriting constructors:
    // template <class ...Args> usable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
};

template<typename internT, typename externT, typename stateT>
using facet_codecvt = usable_facet<std::codecvt<internT, externT, stateT>>;

/*************************************************
* little-endian binary to utf16 to utf8 string   *
**************************************************/


// binary to utf16->utf8
std::string le_bin_utf16_to_utf8(const char* bytes, int offset, int len){
    // std::wstring_convert<std::codecvt<char16_t,char,std::mbstate_t>,char16_t> convert;
	std::wstring_convert<facet_codecvt<char16_t, char,std::mbstate_t>, char16_t> conv16;
    char* cbytes = (char*) calloc(len, sizeof(char));
    if (cbytes == nullptr){
      return "";
    }
    // TODO insecure
    std::memcpy(cbytes, bytes + (offset * sizeof(char)), len * sizeof(char));
	// convert char* to char16_t*
	char16_t* wcbytes = reinterpret_cast<char16_t*>(cbytes);

    std::u16string u16 = std::u16string(wcbytes);
	std::cout<<u16.length()<<std::endl;

    std::string u8 = conv16.to_bytes(u16);
	putbytes(cbytes, len);
    if(len > 0) std::free(cbytes);
	std::cout<<u8<<"|"<<u8.length()<<std::endl;
    return u8;
}
