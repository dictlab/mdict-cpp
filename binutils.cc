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
#include <cstdlib>
#include <iostream>

#include "binutils.h"
//#include "deps/miniz/miniz.h"
#include <zlib.h>

using namespace std;

// anouncement
gz_header* GZHeader();

uint32_t be_bin_to_u32(const unsigned char* bin /* 4 bytes char array  */){
    uint32_t n = 0;
    for (int i =0 ;i < 3; i++){
        n = n | (unsigned int) bin[i];
        n = n << 8;
    }
    n = n | (unsigned int) bin[3];
    return n;
}

uint64_t be_bin_to_u64(const unsigned char* bin /* 8 bytes char array  */){
    uint64_t n = 0;
    for (int i =0 ;i < 7; i++){
        n = n | (unsigned int) bin[i];
        n = n << 8;
    }
    n = n | (unsigned int) bin[7];
    return n;
}


void putbytes(const char* bytes,int len, bool hex = true){
  if (hex) {
    int maxlen = 10 > len ? len : 10;
    std::printf("Buffer[");
    for (int i = 0; i < maxlen - 1; i++) {
      std::printf("%02x, ", bytes[i] & 255);
//        std::printf("%02x(%d) ", bytes[i] & 255,bytes[i] & 255);
    }
    std::printf("%02x ", bytes[maxlen - 1] & 255);
//    std::printf("%02x(%d) ", bytes[maxlen - 1],bytes[maxlen - 1]);
    if (maxlen != len) {
      std::printf(", ..., ");
      for (int i = (len - maxlen < 10? maxlen: 10); i < len - 1; i++) {
        std::printf("%02x, ", bytes[i] & 255);
//          std::printf("%02x(%d) ", bytes[i] & 255,bytes[i] & 255);
      }
      std::printf("%02x", bytes[len - 1] & 255);
//      std::printf("%02x(%d) ", bytes[len - 1],bytes[len - 1]);
    }
    std::printf("](%d)\n", len);
  }else{
    int maxlen = 10 > len ? len : 10;
    std::printf("Buffer[");
    for (int i = 0; i < maxlen - 1; i++) {
      std::printf("%d, ", bytes[i] & 255);
    }
    std::printf("%d ", bytes[maxlen - 1] & 255);
    if (maxlen != len) {
      std::printf(", ..., ");
      //for (int i = len - 10; i < len - 1; i++) {
      for (int i = (len - maxlen < 10? maxlen: 10); i < len - 1; i++) {
        std::printf("%d, ", bytes[i] & 255);
      }
      std::printf("%d", bytes[len - 1] & 255);
    }
    std::printf("](%d)\n", len);
  }
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
	putbytes(cbytes, len, true);
    if(len > 0) std::free(cbytes);
	std::cout<<u8<<"|"<<u8.length()<<std::endl;
    return u8;
}


// slice srcByte to distByte
// ensure srcByte.length > len
int bin_slice(const char* srcByte, int srcByteLen, int offset, int len, char* distByte){
  if (offset <0 || offset > srcByteLen - 1) {
    return -1;
  }
  if (offset + len > srcByteLen) {
    // invalid offset & length
    return -2;
  }
  // ensure distByte has malloced
  for(int i = 0; i < len; ++i){
    (distByte)[i] =  srcByte[i + offset];
  }
}


/// gzip compress and uncompress

// gzCompress: do the compressing
int gzCompress(const char *src, int srcLen, char *dest, int destLen)
{
  z_stream c_stream;
  int err = 0;
  int windowBits = 15;
  int GZIP_ENCODING = 16;

  if(src && srcLen > 0)
  {
    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;
    if(deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                    windowBits | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
    c_stream.next_in  = (Bytef *)src;
    c_stream.avail_in  = srcLen;
    c_stream.next_out = (Bytef *)dest;
    c_stream.avail_out  = destLen;
    while (c_stream.avail_in != 0 && c_stream.total_out < destLen)
    {
      if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
    }
    if(c_stream.avail_in != 0) return c_stream.avail_in;
    for (;;) {
      if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
      if(err != Z_OK) return -1;
    }
    if(deflateEnd(&c_stream) != Z_OK) return -1;
    return c_stream.total_out;
  }
  return -1;
}

// gzDecompress: do the decompressing
int gzDecompress2(const char *src, int srcLen, const char *dst, int dstLen) {
  z_stream strm;
  strm.zalloc = NULL;
  strm.zfree = NULL;
  strm.opaque = NULL;

  strm.avail_in = srcLen;
  strm.avail_out = dstLen;
  strm.next_in = (Bytef *) src;
  strm.next_out = (Bytef *) dst;

  int err = -1, ret = -1;
  err = inflateInit2(&strm, MAX_WBITS + 16);
  if (err == Z_OK) {
    err = inflate(&strm, Z_FINISH);
    if (err == Z_STREAM_END) {
      ret = strm.total_out;
    } else {
      inflateEnd(&strm);
      return err;
    }
  } else {
    inflateEnd(&strm);
    return err;
  }
  inflateEnd(&strm);
  return err;
}

int gzipDecompress(const char *src, unsigned int srcLen, const char *dst, unsigned int dstLen){
  int chunkSize = 16384;
  int status = Z_OK;

  z_stream stream;
  stream.zalloc = NULL;
  stream.zfree = NULL;
  stream.opaque = NULL;



  stream.avail_in = static_cast<uInt>(srcLen);
  stream.avail_out = static_cast<uInt>(dstLen);
  stream.next_in = (Bytef *)src;
  stream.next_out = (Bytef *)dst;
//  uint infile_remaining = srcLen;

  if (inflateInit(&stream))
  {
    printf("inflateInit() failed!\n");
    return EXIT_FAILURE;
  }

    for ( ; ; ) {
      if (!stream.avail_in) {
        // Input buffer is empty, so read more bytes from input file.
//        uint n = my_min(BUF_SIZE, infile_remaining);
//
//        if (fread(s_inbuf, 1, n, pInfile) != n)
//        {
//          printf("Failed reading from input file!\n");
//          return EXIT_FAILURE;
//        }

        stream.next_in = (unsigned char *) src;
        stream.avail_in = srcLen;

//        infile_remaining -= n;
      }

//      status = inflate(&stream, Z_SYNC_FLUSH);
      status = inflate(&stream, Z_NO_FLUSH);

//      if ((status == Z_STREAM_END) || (!stream.avail_out))
//      {
//        // Output buffer is full, or decompression is done, so write buffer to output file.
////        uint n = BUF_SIZE - stream.avail_out;
////        if (fwrite(s_outbuf, 1, n, pOutfile) != n)
////        {
////          printf("Failed writing to output file!\n");
////          return EXIT_FAILURE;
////        }
////        stream.next_out = s_outbuf;
////        stream.avail_out = BUF_SIZE;
//
//      }

//      if (!stream.avail_out){
//        printf("inflate() output buffer error status %i!\n", status);
//      }

      if (status == Z_STREAM_END) {
        // break;
        printf("inflate() success with status %i!\n", status);
        (void) (inflateEnd(&stream));
        cout << "decompress succed, before decompress size is " << stream.total_in << " after decompress size is "
             << stream.total_out << endl;
        return 0;
      } else if (status == Z_OK) {
//        (void) (inflateEnd(&stream));
        printf("inflate() success with status round... %i!\n", status);
        // return EXIT_SUCCESS;
        continue;
      } else {
        (void) (inflateEnd(&stream));
        printf("inflate() failed with status2 %i!\n", status);
        return EXIT_FAILURE;
      }
    }
}


int UncompressData( const char* abSrc, int nLenSrc, char* abDst, int nLenDst )
{
  z_stream zInfo ={0};
  zInfo.total_in=  zInfo.avail_in=  nLenSrc;
  zInfo.total_out= zInfo.avail_out= nLenDst;
  zInfo.next_in= (Bytef*)abSrc;
  zInfo.next_out= (Bytef*)abDst;

  int nErr, nRet= -1;
  nErr= inflateInit( &zInfo );               // zlib function
  if ( nErr == Z_OK ) {
    nErr= inflate( &zInfo, Z_FINISH );     // zlib function
    if ( nErr == Z_STREAM_END ) {
      nRet= zInfo.total_out;
    }
  }
  inflateEnd( &zInfo );   // zlib function
  return( nRet ); // -1 or len of output
}

int zipUncompress(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen){
  return uncompress(dest, destLen, source, sourceLen);
}

int gzDecompress(const char * src, int srcLen,const char * dst, int dstLen)
{

  int chunkSize = 16384;
  int status = Z_OK;

  z_stream strm;
  strm.zalloc = NULL;
  strm.zfree = NULL;
  strm.opaque = NULL;



  strm.avail_in = srcLen;
  strm.avail_out = 0;
  strm.next_in = (Bytef *)src;
  strm.next_out = (Bytef *)dst;

  int err = -1;
//  err = inflateInit2(&strm, MAX_WBITS+16);
  err = inflateInit2(&strm, 32);

//  err = inflateInit(&strm);
  if (err != Z_OK) {
    inflateEnd(&strm);
    cout << "decompression initialization failed, quit!" << endl;
    return -1;
  }else{

    /// js ref code from pako
   ///
   /*
  Inflate.prototype.push = function (data, mode) {
  var strm = this.strm;
  var chunkSize = this.options.chunkSize;
  var dictionary = this.options.dictionary;
  var status, _mode;
  var next_out_utf8, tail, utf8str;

  // Flag to properly process Z_BUF_ERROR on testing inflate call
  // when we check that all output data was flushed.
  var allowBufError = false;

  if (this.ended) { return false; }
  _mode = (mode === ~~mode) ? mode : ((mode === true) ? c.Z_FINISH : c.Z_NO_FLUSH);

  // Convert data if needed
  if (typeof data === 'string') {
    // Only binary strings can be decompressed on practice
    strm.input = strings.binstring2buf(data);
  } else if (toString.call(data) === '[object ArrayBuffer]') {
    strm.input = new Uint8Array(data);
  } else {
    strm.input = data;
  }

  strm.next_in = 0;
  strm.avail_in = strm.input.length;

  do {
    if (strm.avail_out === 0) {
      strm.output = new utils.Buf8(chunkSize);
      strm.next_out = 0;
      strm.avail_out = chunkSize;
    }

    status = zlib_inflate.inflate(strm, c.Z_NO_FLUSH);    // no bad return value

    if (status === c.Z_NEED_DICT && dictionary) {
      status = zlib_inflate.inflateSetDictionary(this.strm, dictionary);
    }

    if (status === c.Z_BUF_ERROR && allowBufError === true) {
      status = c.Z_OK;
      allowBufError = false;
    }

    if (status !== c.Z_STREAM_END && status !== c.Z_OK) {
      this.onEnd(status);
      this.ended = true;
      return false;
    }

    if (strm.next_out) {
      if (strm.avail_out === 0 || status === c.Z_STREAM_END || (strm.avail_in === 0 && (_mode === c.Z_FINISH || _mode === c.Z_SYNC_FLUSH))) {

        if (this.options.to === 'string') {

          next_out_utf8 = strings.utf8border(strm.output, strm.next_out);

          tail = strm.next_out - next_out_utf8;
          utf8str = strings.buf2string(strm.output, next_out_utf8);

          // move tail
          strm.next_out = tail;
          strm.avail_out = chunkSize - tail;
          if (tail) { utils.arraySet(strm.output, strm.output, next_out_utf8, tail, 0); }

          this.onData(utf8str);

        } else {
          this.onData(utils.shrinkBuf(strm.output, strm.next_out));
        }
      }
    }

    // When no more input data, we should check that internal inflate buffers
    // are flushed. The only way to do it when avail_out = 0 - run one more
    // inflate pass. But if output data not exists, inflate return Z_BUF_ERROR.
    // Here we set flag to process this error properly.
    //
    // NOTE. Deflate does not return error in this case and does not needs such
    // logic.
    if (strm.avail_in === 0 && strm.avail_out === 0) {
      allowBufError = true;
    }

  } while ((strm.avail_in > 0 || strm.avail_out === 0) && status !== c.Z_STREAM_END);

  if (status === c.Z_STREAM_END) {
    _mode = c.Z_FINISH;
  }

  // Finalize on the last chunk.
  if (_mode === c.Z_FINISH) {
    status = zlib_inflate.inflateEnd(this.strm);
    this.onEnd(status);
    this.ended = true;
    return status === c.Z_OK;
  }

  // callback interim results if Z_SYNC_FLUSH.
  if (_mode === c.Z_SYNC_FLUSH) {
    this.onEnd(c.Z_OK);
    strm.avail_out = 0;
    return true;
  }

  return true;
};



    */


    cout << "init OK" << endl;
    gz_headerp gzheader = GZHeader();

    err = inflateGetHeader(&strm, gzheader);
    if (err != Z_OK){
      cout << "decompression set header failed, inflate return: " << err << endl;
      return -1;
    }

    do{
      if (strm.avail_out == 0) {
//        strm.ou
      }


    } while((strm.avail_in > 0 || strm.avail_out == 0) && status != Z_STREAM_END);



    err = inflate(&strm, Z_FINISH);
    if (err == Z_STREAM_END)
    {
      (void)inflateEnd(&strm);
      cout << "decompress succed, before decompress size is " << strm.total_in  << " after decompress size is " << strm.total_out << endl;
      return strm.total_out;
    }
    else
    {
      (void)inflateEnd(&strm);
      cout << "decompression failed, inflate return: " << err << endl;
      return -1;
    }
  }

}

gz_header* GZHeader() {
    gz_header* header =(gz_header*)malloc(1 * sizeof(gz_header));

    /* true if compressed data believed to be text */
    header->text       = 0;
    /* modification time */
    header->time       = 0;
    /* extra flags (not used when writing a gzip file) */
    header->xflags     = 0;
    /* operating system */
    header->os         = 0;
    /* pointer to extra field or Z_NULL if none */
    header->extra      = Z_NULL;
    /* extra field length (valid if extra != Z_NULL) */
    header->extra_len  = 0; // Actually, we don't need it in JS,
    // but leave for few code modifications

    //
    // Setup limits is not necessary because in js we should not preallocate memory
    // for inflate use constant limit in 65536 bytes
    //

    /* space at extra (only when reading header) */
    // this.extra_max  = 0;
    /* pointer to zero-terminated file name or Z_NULL */
    header->name       = Z_NULL;
    /* space at name (only when reading header) */
    // this.name_max   = 0;
    /* pointer to zero-terminated comment or Z_NULL */
    header->comment    = Z_NULL;
    /* space at comment (only when reading header) */
    // this.comm_max   = 0;
    /* true if there was or will be a header crc */
    header->hcrc       = 0;
    /* true when done reading gzip header (not used when writing a gzip file) */
    header->done       = false;
}
