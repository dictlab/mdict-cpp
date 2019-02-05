/*
 * zlib_wrapper.h
 *
 *  Created on: 2016年3月29日
 *      Author: guyadong
 */

#ifndef INCLUDE_ZLIB_WRAPPER_H_
#define INCLUDE_ZLIB_WRAPPER_H_
#include <vector>
#include "zlib.h"
//#define _DEF_STRING(x) #x
//#define DEF_TO_STRING(x) _DEF_STRING(x)
//#define SOURCE_AT __FILE__ ":" DEF_TO_STRING(__LINE__)
//#define ERROR_STR(msg) std::string(SOURCE_AT ":").append(msg)
//#define throw_except_if_msg(except,expression,msg) \
//    if(expression)\
//        throw except(ERROR_STR(msg));
//#define _CAS_ERROR_MSG_(ERR) case ERR:return #ERR;
///*
// * 以sting形式返回错误信息
// */
//inline std::string zlib_error_message(int err){
//  switch(err){
//    _CAS_ERROR_MSG_(Z_OK)
//    _CAS_ERROR_MSG_(Z_MEM_ERROR)
//    _CAS_ERROR_MSG_(Z_BUF_ERROR)
//    _CAS_ERROR_MSG_(Z_STREAM_ERROR)
//  };
//  return "unknow error";
//}
//#undef _CAS_ERROR_MSG_
/* zlib异常类 */
class zlib_exception:public std::logic_error{
    // 继承基类构造函数
    using std::logic_error::logic_error;
};


/*
 * 调用zlib解压缩数据
 * uncompress_bound为压缩前的数据长度,如果不知道数据源长度设置为0
 * */
inline std::vector<uint8_t> zlib_mem_uncompress (const void *source, size_t sourceLen,size_t uncompress_bound=0){
//   throw_except_if_msg(nullptr==source||0==sourceLen,"invalid source");
  //uncompress_bound为0时将缓冲区设置为sourceLen的8倍长度
  if (!uncompress_bound)
    uncompress_bound = sourceLen << 3;
  for(;;){
    std::vector<uint8_t> buffer(uncompress_bound);
    auto destLen=uLongf(buffer.size());
    auto err=uncompress(buffer.data(),&destLen,reinterpret_cast<const Bytef *>(source),uLong(sourceLen));
    if(Z_OK==err)
      return std::vector<uint8_t>(buffer.data(),buffer.data()+destLen);
    else if(Z_BUF_ERROR==err){
      // 缓冲区不足
      uncompress_bound<<=2;// 缓冲区放大4倍再尝试
      continue;
    }
    // 其他错误抛出异常
    return std::vector<uint8_t>();
  }
}

/*
 * 调用zlib解压缩数据
 * */
inline int zlib_mem_uncompress (void *dest,size_t *destLen,const void *source, size_t sourceLen){
//  throw_if(nullptr==source||0==sourceLen||nullptr==dest||nullptr==destLen||0==*destLen)
  auto len=uLongf(*destLen);
  auto err=uncompress(reinterpret_cast<Bytef *>(dest),&len,reinterpret_cast<const Bytef *>(source),uLong(sourceLen));
  *destLen=size_t(len);
  return err;
}
/*
 * 解压缩数据到对象
 */
template<typename T>
inline typename std::enable_if<std::is_class<T>::value>::type
uncompress_obj (T &dest,const void *source, size_t sourceLen){
  auto destLen=sizeof(T);
  zlib_mem_uncompress(std::addressof(dest),&destLen,source,sourceLen);
}
#endif /* INCLUDE_ZLIB_WRAPPER_H_ */
