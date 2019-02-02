/**
 *
 * Author: terasum<terasum@163.com>
 * Date: 2019-01-24
 * Desc: Mdict mdd/mdx parser
 * License: LGPL TODO
 *
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <codecvt>
#include <map>
#include <string> // std::stof

#include <stdexcept>

#include "mdict.h"
#include "fileutils.h"
#include "binutils.h"
#include "xmlutils.h"
#include "ripemd128.h"

//#include "deps/miniz/miniz.h"
#include <zlib.h>
#include "adler32.h"

#define ENCRYPT_NO_ENC 0
#define ENCRYPT_RECORD_ENC 1
#define ENCRYPT_KEY_INFO_ENC 2


#define NUMFMT_BE_8BYTESQ 0;
#define NUMFMT_BE_4BYTESI 1;


#define ENCODING_UTF8 0;
#define ENCODING_UTF16 1;
#define ENCODING_BIG5 2;
#define ENCODING_GBK 3;
#define ENCODING_GB2312 4;
#define ENCODING_GB18030 5;


/**
 * mdx struct analysis
 * mdx file:
 *#| dictionary header
 *    | [0:4]: the length of dictionary header (header_bytes_size)
 *      | uint32 integer
 *    | [4:header_bytes_size + 4]: dictionary header info bytes (header_info_bytes)
 *      | header info bytes, little-endian utf16 bytes, xml format
 *    | [header_bytes_size + 4: header_bytes_size + 8]: dictionary header info adler32 checksum (alder32_checksum)
 *      | assert(zlib.alder32(header_bytes) & 0xffffffff, alder32_checksum)
 *#| key blocks (starts with header_bytes_size + 8, key block offset)
 *    | note: num bytes format, if version >= 2.0, number format is uint64 (8bytes), otherwise, number format is uint32
 *    | note: encrypt == 1 && passcode != nil, TODO
 *   #| key block header:
 *      | for version >= 2.0
 *      | [0:8]    - number of key blocks (key_block_num)
 *      | [8:16]   - number of entries (entries_num)
 *      | [16:24]  - key block info decompressed size (if version < 2.0, this part does not exist)
 *      | [24:32]  - key block info size (key_block_info_size)
 *      | [32:40]  - key block size (key_block_size)
 *      | [40:44]  - key info (key block header) alder32 checksum *
 *                  (*: this part only include ver>2.0, and not include in key block info length)
 *      | for version < 2.0
 *      | [0:4]    - number of key blocks
 *      | [4:8]    - number of entries
 *      | [8:12]   - key block info size
 *      | [12:16]  - key block size*
 *      | summary: if version >= 2.0, the key block header buffer size is 5 * 8 = 40 bytes, and actually, with 4 bytes checksum,
 *      |          so, if version >= 2.0, key block header length = 44 bytes
 *      |          else if version < 2.0, the key block header buffer size is 4 * 4 = 16 bytes
 *      |
 *   #| key block info
 *      | note: offset, if version >= 2.0, start with header_bytes_size + 8 + (44 = key_block_header_length),
 *      |               else, version < 2.0, start with header_bytes_size + 8 + (16 = key_block_header_length)
 *      | key block info size equals the number read from key block header (key_block_size)
 *      | key_block_info_buffer = dict_file.read(header_bytes_size + 8 + key_block_header_length, key_block_info_size);
 *      | typedef: key_block_info_list [{
 *      |    compressed_size
 *      |    decompressed_size
 *      | },...]
 *      | key_block_info_list = decode_key_block_info(key_block_info_buffer)
 *          | deceode_key_block_info:
 *
 *      | assert(key_block_info_list.length, key_block_num);
 *      | key_block_compressed = dict_file.read(header_bytes_size + 8 + key_block_header_length + key_block_info_size, key_block_size)
 *      | key_list = decode_key_block(key_block_compressed, key_block_info_list)
 *      | note: record_block_offset = header_bytes_size + 8 + key_block_header_length + key_block_info_size + key_block_size
 *
 *#| record block:
 *   #| record block header (recoder_block_header)
 *    | if version >= 2.0:
 *      | [0:8]   - record block number (record_block_num)
 *      | [8:16]  - number of the key-value entries (entries_num)
 *      | [16:24] - record block info size (record_block_info_size)
 *      | [24:32] - record block size (record_block_size)
 *    | else if version < 2.0:
 *      | [0:4]   - record block number (record_block_num)
 *      | [4:8]  - number of the key-value entries (entries_num)
 *      | [8:12] - record block info size (record_block_info_size)
 *      | [12:16] - record block size (record_block_size)
 *    | note: every record block contains a lot of entries
 *    | typedef: record_block_info_list [{
 *    |   compressed_size,
 *    |   decompressed_size
 *    | },...]
 *  #| record block info
 *    | decode record_block_info_list
 *      | for i =0; i < record_block_num; ++i
 *      |    compressed_size = readnumber(dict_file.readbyte(number_width)) // number_width = 8 (ver >= 2.0), 4(ver < 2.0)
 *      |    decompresed_size = readnumber(dict_file.readbyte(number_width))
 *      |    size_counter += 2 * number_width
 *      | assert(size_counter, record_block_info_size)
 *  #| record block starts at: record_block_offset + record_block_header_size + record_block_info_size
 *    | decode record block
 *      | for i = 0; i < record_block_info_list.length(record_block_num); ++i
 *      |   compressed_size = record_block_info_list[i].compressed_size
 *      |   decompressed_size = record_block_info_list[i].decompressed_size
 *      |   #record_block_compressed = dict_file.readbytes(compressed_size)
 *      |     | decode and decrypt record_block_compressed, and gets block's keys list
 *      |
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */





namespace jsmdict {

    class Mdict{
    public:
        Mdict(std::string fn) noexcept;
        ~Mdict();
        std::string lookup(const std::string word, int word_len);
        std::vector<std::string> prefix(const std::string prefix, int prefix_len);

        // read dictionary file header
        void read_header();

        void read_key_block_header();

        void read_key_block_info();

        int decode_key_block_info(char* key_block_info_buffer, unsigned long kb_info_buff_len ,int key_block_num, int entries_num);

        void printhead() {
          std::cout<<"version: "<<this->version<<std::endl<<"encoding: "<<this->encoding<<std::endl;
        }

    private:
        // general part
        const std::string filename;

        // file read offset
        long long offset;

        // file input stream
        std::ifstream instream;

        // ---------------------
        //     header part
        // ---------------------

        // offset part (important)
        // dictionary header part
        // | dictionary header
        //    | [0:4]: the length of dictionary header (header_bytes_size)
        //      | uint32 integer
        //    | [4:header_bytes_size + 4]: dictionary header info bytes (header_info_bytes)
        //      | header info bytes, little-endian utf16 bytes, xml format
        //    | [header_bytes_size + 4: header_bytes_size + 8]: dictionary header info adler32 checksum (alder32_checksum)
        //      | assert(zlib.alder32(header_bytes) & 0xffffffff, alder32_checksum)
        uint32_t header_bytes_size = 0;

        // key block start offset
        // key_block_start_offset = header_bytes_size + 8;
        uint32_t key_block_start_offset = 0;

        // key_block_info_start_offset = key_block_start_offset + info_size (>=2.0: 40+4, <2.0: 16)
        uint32_t key_block_info_start_offset = 0;
        // key block compressed start offset = this->key_block_info_start_offset + key_block_info_size
        uint32_t key_block_compressed_start_offset = 0;

        // ---------------------
        //     block key info part
        // ---------------------
        uint64_t key_block_num = 0;
        uint64_t entries_num = 0;
        uint64_t key_block_info_decompress_size = 0;
        uint64_t key_block_info_size = 0;
        uint64_t key_block_size = 0;


        // head info part
        int encrypt = 0;
        float version = 1.0;

        int number_width = 8;
        int number_format = 0;

        int encoding = ENCODING_UTF8;

        // read offset and len bytes
        void readfile(uint64_t offset, uint64_t len, char *buf);

        // contains key or not
        bool contains(char* word, int word_len);


    };


    // constructor
    Mdict::Mdict(std::string fn)noexcept: filename(fn) {
      offset = 0;
      instream = std::ifstream(filename, std::ios::binary);
      /// infile = std::fopen(fn.substr, "rb"); // read fn as read||binary mode
      std::cout<<"constructor"<<std::endl;
    }
    // distructor
    Mdict::~Mdict() {
      // close instream
      instream.close();
    }


    /***************************************
     *             private part            *
     ***************************************/


    void Mdict::read_header(){
      // -----------------------------------------
      // 1. [0:4] dictionary header length 4 byte
      // -----------------------------------------

      // header size buffer
      char* head_size_buf = (char*) std::calloc(4, sizeof(char));
      readfile(0, 4, head_size_buf);
      putbytes(head_size_buf, 4, true);
      // header byte size convert
      uint32_t header_bytes_size = be_bin_to_u32((const unsigned char*) head_size_buf);
      std::free(head_size_buf);
      std::cout<<"len1:" <<header_bytes_size<<std::endl;
      // assign key block start offset
      this->header_bytes_size = header_bytes_size;
      this->key_block_start_offset = this->header_bytes_size + 8;

      // -----------------------------------------
      // 2. [4: header_bytes_size+4], header buffer
      // -----------------------------------------

      // header buffer
      char* head_buffer = (char*) std::calloc(header_bytes_size, sizeof(char));
      readfile(4, header_bytes_size, head_buffer);
      putbytes(head_buffer, header_bytes_size, true);

      // -----------------------------------------
      // 3. alder32 checksum
      // -----------------------------------------

      // TODO  version < 2.0 needs to checksum?
      // alder32 checksum buffer
      char* head_checksum_buffer = (char*) std::calloc(4, sizeof(char));
      readfile(header_bytes_size + 4, 4, head_checksum_buffer);
      putbytes(head_checksum_buffer, 4, true);

      // TODO skip head checksum for now
      std::free(head_checksum_buffer);

      // -----------------------------------------
      // 4. convert header buffer into utf16 text
      // -----------------------------------------

      // header text utf16
      std::string header_text = le_bin_utf16_to_utf8(head_buffer, 0, header_bytes_size - 2 );
      if (header_text.empty()) {
        std::cout<<"len:" <<header_bytes_size<<std::endl;
        std::cout<<"this mdx file is invalid"<<std::endl;
        return;
      }
      std::cout<< header_text << std::endl;

      // -----------------------------------------
      // 5. parse xml string into map
      // -----------------------------------------

      std::map<std::string, std::string> headinfo = parseXMLHeader(header_text);

      // -----------------------------------------
      // 6. handle header message, set flags
      // -----------------------------------------

      // encrypted flag
      // 0x00 - no encryption
      // 0x01 - encrypt record block
      // 0x02 - encrypt key info block
      if ( headinfo.find("Encrypted") == headinfo.end() ||
           headinfo["Encrypted"].empty() ||
           headinfo["Encrypted"] == "No") {
        this->encrypt = ENCRYPT_NO_ENC;
      } else if ( headinfo["Encrypted"] == "Yes" ) {
        this->encrypt = ENCRYPT_RECORD_ENC;
      } else {
        std::string s = headinfo["Encrypted"];
        if (s.at(0) == '2' ) {
          this->encrypt = ENCRYPT_KEY_INFO_ENC;
        } else if (s.at(0) == '1') {
          this->encrypt = ENCRYPT_RECORD_ENC;
        } else{
          this->encrypt = ENCRYPT_NO_ENC;
        }
      }



      // -------- stylesheet ----------
      // stylesheet attribute if present takes from of:
      // style_number # 1-255
      // style_begin # or ''
      // style_end # or ''
      // TODO: splitstyle info

      // header_info['_stylesheet'] = {}
      // if header_tag.get('StyleSheet'):
      //   lines = header_tag['StyleSheet'].splitlines()
      //   for i in range(0, len(lines), 3):
      //        header_info['_stylesheet'][lines[i]] = (lines[i + 1], lines[i + 2])




      // ---------- version ------------
      // before version 2.0, number is 4 bytes integer
      // version 2.0 and above use 8 bytes
      std::string sver = headinfo["GeneratedByEngineVersion"];
      std::string::size_type sz;     // alias of size_t

      float version = std::stof (sver,&sz);
      this->version = version;

      if (this->version >= 2.0) {
        this->number_width = 8;
        this->number_format = NUMFMT_BE_8BYTESQ;
        this->key_block_info_start_offset = this->key_block_start_offset + 40 + 4;
      } else {
        this->number_format = NUMFMT_BE_4BYTESI;
        this->number_width = 4;
        this->key_block_info_start_offset = this->key_block_start_offset + 16;
      }

      // ---------- encoding ------------
      if (headinfo.find("Encoding") != headinfo.end() || headinfo["Encoding"] == "" || headinfo["Encoding"] == "UTF-8" ) {
        this->encoding = ENCODING_UTF8;
      } else if (headinfo["Encoding"] == "GBK" || headinfo["Encoding"] == "GB2312") {
        this->encoding = ENCODING_GB18030;
      } else if (headinfo["Encoding"] == "Big5" || headinfo["Encoding"] == "BIG5") {
        this->encoding = ENCODING_BIG5;
      } else if (headinfo["Encoding"] == "utf16" || headinfo["Encoding"] == "utf-16") {
        this->encoding = ENCODING_UTF16;
      } else {
        this->encoding = ENCODING_UTF8;
      }

    }


    void Mdict::read_key_block_header() {
      // key block header part
      int key_block_info_bytes_num = 0;
      if (this->version >= 2.0) {
        key_block_info_bytes_num = 8 * 5;
      } else {
        key_block_info_bytes_num = 4 * 4;
      }

      // key block info buffer
      char* key_block_info_buffer = (char*) calloc(static_cast<size_t>(key_block_info_bytes_num), sizeof(char));
      // read buffer
      this->readfile(this->key_block_start_offset, key_block_info_bytes_num, key_block_info_buffer);
      // TODO PASSED
      putbytes(key_block_info_buffer, key_block_info_bytes_num, true);


      // TODO key block info encrypted file not support yet
      if(this->encrypt == ENCRYPT_RECORD_ENC) {
        std::cout<< "user identification is needed to read encrypted file"<<std::endl;
        throw std::invalid_argument("invalid encrypted file");
      }

      // keyblock header info struct:
      // [0:8]/[0:4]   - number of key blocks
      // [8:16]/[4:8]  - number of entries
      // [16:24]/nil - key block info decompressed size (if version >= 2.0, otherwise, this section does not exist)
      // [24:32]/[8:12] - key block info size
      // [32:40][12:16] - key block size
      // note: if version <2.0, the key info buffer size is 4 * 4
      //       otherwise, ths key info buffer size is 5 * 8
      // <2.0  the order of number is same

      // 1. [0:8]([0:4]) number of key blocks
      char * key_block_nums_bytes = (char*)calloc(static_cast<size_t>(this->number_width), sizeof(char));
      int eno = bin_slice(key_block_info_buffer, key_block_info_bytes_num, 0, this->number_width, key_block_nums_bytes);
      if (eno != 0){
        std::cerr<<"binslice err:"<< eno <<std::endl;
        // TODO throw error
      }
      putbytes(key_block_nums_bytes, this->number_width, true);

      uint64_t key_block_num = 0;
      if (this->number_width == 8) key_block_num = be_bin_to_u64((const unsigned char*)key_block_nums_bytes);
      else if(this->number_width == 4) key_block_num = be_bin_to_u32((const unsigned char*)key_block_nums_bytes);
      std::cout<<"keyblocknum:"<< key_block_num <<std::endl;
      // TODO PASSED

      // 2. [8:16]  - number of entries
      char * entries_num_bytes = (char*)calloc(static_cast<size_t>(this->number_width), sizeof(char));
      eno = bin_slice(key_block_info_buffer, key_block_info_bytes_num, this->number_width, this->number_width, entries_num_bytes);
      if (eno != 0){
        std::cerr<<"binslice err2:"<< eno <<std::endl;
        // TODO throw error
      }

      putbytes(entries_num_bytes, this->number_width, true);

      uint64_t entries_num = 0;
      if (this->number_width == 8) entries_num = be_bin_to_u64((const unsigned char*)entries_num_bytes);
      else if(this->number_width == 4) key_block_num = be_bin_to_u32((const unsigned char*)entries_num_bytes);
      std::cout<<"entries_num:"<< entries_num <<std::endl;
      // TODO PASSED

      int key_block_info_size_start_offset = 0;

      // 3. [16:24] - key block info decompressed size (if version >= 2.0, otherwise, this section does not exist)
      if (this->version >= 2.0)  {
        char * key_block_info_decompress_size_bytes = (char*)calloc(static_cast<size_t>(this->number_width), sizeof(char));
        eno = bin_slice(key_block_info_buffer, key_block_info_bytes_num, this->number_width*2, this->number_width, key_block_info_decompress_size_bytes);
        if (eno != 0){
          std::cerr<<"binslice err3:"<< eno <<std::endl;
          // TODO throw error
        }
        putbytes(key_block_info_decompress_size_bytes, this->number_width, true);

        uint64_t key_block_info_decompress_size = 0;
        if (this->number_width == 8) key_block_info_decompress_size = be_bin_to_u64((const unsigned char*)key_block_info_decompress_size_bytes);
        else if(this->number_width == 4) key_block_info_decompress_size = be_bin_to_u32((const unsigned char*)key_block_info_decompress_size_bytes);
        std::cout<<"keyblockcompress size:"<< key_block_info_decompress_size <<std::endl;
        // TODO PASSED
        if (key_block_info_decompress_size_bytes) std::free(key_block_info_decompress_size_bytes);
        // key block info size (number) start at 24 ([24:32])
        key_block_info_size_start_offset = this->number_width * 3;
      }else{
        // key block info size (number) start at 24 ([8:12])
        key_block_info_size_start_offset = this->number_width * 2;
      }

      // 4. [24:32] - key block info size
      char * key_block_info_size_buffer = (char*)calloc(static_cast<size_t>(this->number_width), sizeof(char));
      eno = bin_slice(key_block_info_buffer, key_block_info_bytes_num, key_block_info_size_start_offset, this->number_width, key_block_info_size_buffer);
      if (eno != 0){
        std::cerr<<"binslice err3:"<< eno <<std::endl;
        // TODO throw error
      }
      putbytes(key_block_info_size_buffer, this->number_width, true);

      uint64_t key_block_info_size = 0;
      if (this->number_width == 8) key_block_info_size = be_bin_to_u64((const unsigned char*)key_block_info_size_buffer);
      else if(this->number_width == 4) key_block_info_size = be_bin_to_u32((const unsigned char*)key_block_info_size_buffer);
      std::cout<<"key block info size:"<< key_block_info_size <<std::endl;
      // TODO PASSED

      // 5. [32:40] - key block size
      char * key_block_size_buffer = (char*)calloc(static_cast<size_t>(this->number_width), sizeof(char));
      eno = bin_slice(key_block_info_buffer, key_block_info_bytes_num, key_block_info_size_start_offset + this->number_width, this->number_width, key_block_size_buffer);
      if (eno != 0){
        std::cerr<<"binslice err3:"<< eno <<std::endl;
        // TODO throw error
      }
      putbytes(key_block_size_buffer, this->number_width, true);

      uint64_t key_block_size = 0;
      if (this->number_width == 8) key_block_size = be_bin_to_u64((const unsigned char*)key_block_size_buffer);
      else if(this->number_width == 4) key_block_size = be_bin_to_u32((const unsigned char*)key_block_size_buffer);
      std::cout<<"key block size:"<< key_block_size <<std::endl;
      // TODO PASSED

      // 6. [40:44] - 4bytes checksum
      // TODO if version > 2.0, skip 4bytes checksum

      // free key block info buffer
      if (key_block_nums_bytes != nullptr) std::free(key_block_nums_bytes);
      if (entries_num_bytes != nullptr) std::free(entries_num_bytes);
      if (key_block_info_size_buffer) std::free(key_block_info_size_buffer);
      if (key_block_size_buffer) std::free(key_block_size_buffer);
      if (key_block_info_buffer != nullptr) std::free(key_block_info_buffer);

      this->key_block_num = key_block_num;
      this->entries_num = entries_num;
      this->key_block_info_size = key_block_info_size;
      this->key_block_size = key_block_size;
      if (this->version >= 2.0) {
        this->key_block_info_start_offset = this->key_block_start_offset + 40 + 4;
        this->key_block_info_decompress_size = 0;
      }else{
        this->key_block_info_start_offset = this->key_block_start_offset + 16;
      }

    }




    void Mdict::read_key_block_info() {
      // start at this->key_block_info_start_offset
      char* key_block_info_buffer = (char*)calloc(static_cast<size_t>(this->key_block_info_size), sizeof(char));
      readfile(this->key_block_info_start_offset, static_cast<int>(this->key_block_info_size), key_block_info_buffer);

      // ------------------------------------
      // TODO decode key_block_info
      // ------------------------------------
      decode_key_block_info(key_block_info_buffer, this->key_block_info_size, this->key_block_num, this->entries_num);


      // key block compressed start offset = this->key_block_info_start_offset + key_block_info_size
      this->key_block_compressed_start_offset = static_cast<uint32_t>(this->key_block_info_start_offset + this->key_block_info_size);

      char * key_block_compressed_buffer = (char*)calloc(static_cast<size_t>(this->key_block_size), sizeof(char));
      readfile(this->key_block_compressed_start_offset, static_cast<int>(this->key_block_info_size), key_block_compressed_buffer);

      // ------------------------------------
      // TODO decode key_block_compressed
      // ------------------------------------


      if (key_block_info_buffer != nullptr) std::free(key_block_info_buffer);
      if (key_block_compressed_buffer != nullptr) std::free(key_block_compressed_buffer);

    }


    void fast_decrypt(byte* data, const byte* k, int data_len, int key_len){
      std::cout<<"fast decrypt key"<<std::endl;
      const byte* key = k;
      putbytes((char*)data, 16, true);
      byte* b = data;
      std::cout<<"fast decrypt"<<std::endl;
      putbytes((char*)data,data_len, true);
      byte previous = 0x36;

      for (int i =0; i < data_len; ++i){
       byte t = static_cast<byte>(((b[i] >> 4) | (b[i] << 4)) & 0xff);
       t = t ^ previous ^ ((byte)(i & 0xff)) ^ key[i % key_len];
       previous = b[i];
       b[i] = t;
      }
//      const b = new Uint8Array(data);
//      const key = new Uint8Array(k);
//      let previous = 0x36;
//      for (let i = 0; i < b.length; ++i) {
//        let t = ((b[i] >> 4) | (b[i] << 4)) & 0xff;
//        t = t ^ previous ^ (i & 0xff) ^ key[i % key.length];
//        previous = b[i];
//        b[i] = t;
//      }
//      return new BufferList(b);

    }

    /* note: don't forget free comp_block */
    byte* mdx_decrypt(byte* comp_block, const int comp_block_len) {
      std::cout<<"--- compblock buff --- "<< std::endl;
      putbytes((char*) comp_block, comp_block_len, true);
      byte* key_buffer = (byte*) calloc(8, sizeof(byte));
      memcpy(key_buffer, comp_block + 4 * sizeof(char), 4 * sizeof(char));
      key_buffer[4] = 0x95; // comp_block[4:8] + [0x95,0x36,0x00,0x00]
      key_buffer[5] = 0x36;

      std::cout<<"--- key buff --- "<< std::endl;
      putbytes((char*) key_buffer, 8,true);
      byte* key = ripemd128bytes(key_buffer, 8);
      std::cout<<"--- key --- "<< std::endl;
      putbytes((char*) key, 16, false);


      fast_decrypt(comp_block + 8 * sizeof(byte), key, comp_block_len - 8, 16 /* key length*/);

      // finally
      std::free(key_buffer);
      return comp_block;
      /// passed
    }


    // note: kb_info_buff_len == key_block_info_compressed_size
    int Mdict::decode_key_block_info(char* key_block_info_buffer, unsigned long kb_info_buff_len, int key_block_num, int entries_num){
        char* kb_info_buff = key_block_info_buffer;
        std::cout<<"-----------\n kb_info_buffer [02 00 00 00 e7 a4 02 e4 a2 e4 b8 b9 e4 dd d1 71 5e a2 7f 0c ](1508)"<<std::endl;
        putbytes(kb_info_buff, kb_info_buff_len, true);

        if (this->version >= 2.0) {
          // if version >= 2.0, use zlib compression
          assert(kb_info_buff[0] == 2);
          assert(kb_info_buff[1] == 0);
          assert(kb_info_buff[2] == 0);
          assert(kb_info_buff[3] == 0);
          byte* kb_info_decrypted = (unsigned char *)key_block_info_buffer;
          if(this->encrypt == ENCRYPT_KEY_INFO_ENC){
            kb_info_decrypted = mdx_decrypt((byte*)kb_info_buff, kb_info_buff_len);
          }
          std::cout<<"-----------\n kb_info_decrypted [02 00 00 00 e7 a4 02 e4 78 da 75 96 4b 88 1c 45 18 c7 3f dd cc 7b 66 77 03 01 41 cf 5e 36 d9 dd 59 21 17 41 f4 e2 d9 1c 3c 78 88 8f ea ee ea ee 72 ba ab 7a ab ab 67 b2 11 dc ... > ] 1508" <<std::endl;
          putbytes((char*)kb_info_decrypted, kb_info_buff_len, true);
          std::cout<<"==" <<std::endl;
          putbytes((char*)kb_info_decrypted, kb_info_buff_len, false);

          unsigned char expect[1500] = {120, 218, 117, 150, 75, 136, 28, 69, 24, 199, 63, 221, 204, 123, 102, 119, 3, 1, 65, 207, 94, 54, 217, 221, 89, 33, 23, 65, 244, 226, 217, 28, 60, 120, 136, 143, 234, 238, 234, 238, 114, 186, 171, 122, 171, 171, 103, 178, 17, 220, 5, 37, 224, 65, 16, 73, 240, 164, 248, 34,136, 34, 232, 73, 240, 224, 193, 120, 16, 65, 196, 87, 98, 76, 64, 205, 37, 66, 162, 70, 19, 205, 195, 231, 87, 221, 85, 61, 51, 21, 156, 203, 236, 176, 191, 127, 125, 95, 125, 207, 2, 208, 159, 230, 81, 88, 222, 24, 14, 63, 124, 109, 255, 144, 42, 159, 168, 88, 138, 9, 64, 135, 68, 146, 72, 70, 120, 14, 213, 103, 120, 160, 250, 222, 254, 181, 250, 110, 110, 66, 31, 25, 74, 148, 224, 52, 20, 18, 96, 145, 112, 45, 206, 68, 34, 34, 150, 43, 163, 187, 235, 14, 163, 251, 221, 232, 238, 131, 165, 121, 16, 45, 244, 72, 174, 164, 224, 34, 165, 178, 182, 247, 188, 209, 253, 98, 116, 25, 116, 45, 197, 124, 252, 237, 17, 30, 37, 212, 208, 27, 71, 12, 253, 91, 245, 221, 56, 5, 173, 10, 192, 243, 58, 30, 141, 105, 42, 84, 108, 207, 94, 31, 26, 250, 130, 161, 95, 129, 190, 102, 36, 85, 34, 87, 100, 11, 240, 39, 147, 65, 78, 183,232, 152, 209, 137, 85, 253, 101, 84, 231, 141, 71, 103, 97, 169, 196, 68, 72, 66, 140, 3, 234, 1, 118, 121, 98, 146, 216, 43, 124, 111, 4, 255, 24, 193, 14, 122, 141, 255, 166, 129, 118, 191, 144, 145, 22, 148, 159, 181, 219, 29, 242, 125, 116, 191, 4, 208, 229, 150, 79, 228, 152, 202, 45, 235, 134, 48, 236, 159, 134, 61, 2, 205, 146, 40, 209, 152, 69, 51, 199, 30, 172, 190, 119, 204, 239, 230, 9, 104, 27, 2, 225, 166, 47, 130, 160, 142, 224, 218, 89, 115, 236, 117, 195, 222, 131, 167, 149, 0, 122, 59, 240, 5, 15, 153, 76, 137, 98, 162, 174, 136, 149, 75, 70, 97, 190, 155, 99, 232, 24, 78, 107, 186, 190, 208, 55, 80, 138, 90, 193, 222, 59, 141, 224, 154, 17, 156, 212, 38, 10, 201, 180, 195, 75, 254, 150, 194, 252, 71, 36, 17, 99, 38, 11, 171, 89, 251, 110, 62, 177, 205, 203, 176, 219, 33, 245, 249, 237, 128, 102, 146, 141, 107, 83, 171, 135, 156, 170, 123, 21, 22, 45, 226, 137, 48, 87, 49, 22, 93, 192, 242, 148, 166, 30, 166, 61, 48, 178, 125, 119, 59, 1, 123, 18, 250, 53, 197, 120, 132, 183, 10, 36, 153, 4, 98, 194, 5, 183, 166, 142, 27, 205, 45, 70, 243, 37, 12, 166, 80, 105, 169, 69, 19, 22, 148, 242, 170, 46, 140, 98, 251, 15, 163, 184, 8, 61, 36, 34, 230, 177, 132, 41, 204, 115, 151, 234, 124, 79, 116, 65, 218, 48, 44, 204, 87, 108, 243, 3, 24, 24, 136, 249, 241, 68, 151, 236, 162, 169, 65, 143, 6, 51, 182, 86, 77, 0, 182, 127, 54, 194, 23, 160, 55, 5, 49, 92, 75, 97, 66, 15, 109, 97, 22, 210, 34, 247, 147, 58, 128, 107, 159, 58, 229, 240, 186, 38, 153, 135, 113, 80, 218, 83, 143, 98, 131, 119, 67, 73, 55, 11, 202, 85, 29, 192, 117, 223, 233, 218, 111, 208, 156, 129, 170, 248, 97, 69, 144, 84, 140, 105, 29, 191, 245, 31, 230, 83, 213, 152, 64, 79, 67, 28, 187, 47, 44, 18, 45, 73, 10, 133, 83, 70, 212, 69, 177, 242, 128, 211, 2, 7, 161, 109, 32, 221, 238, 145, 20, 5, 159, 141, 247, 25, 131, 255, 107, 240, 99, 120, 166, 101, 80, 176, 39, 38, 99, 74, 50, 34, 241, 128, 44, 33, 91, 172, 202, 218, 252, 168, 184, 98, 188, 251, 10, 58, 21, 158, 232, 160, 55, 98, 198, 85, 29, 179, 253, 134, 189, 81, 199, 108, 33, 102, 25, 222, 32, 222, 202, 176, 229, 50, 226, 219, 124, 14, 31, 116, 208, 123, 161, 95, 66, 138, 242, 156, 233, 216, 116, 25, 247, 139, 4, 231, 241, 180, 166, 95, 50, 154, 203, 70, 115, 63, 244, 44, 84, 94, 22, 127, 41, 42, 19, 198, 41, 177, 19, 96, 159, 77, 227, 79, 198, 253, 75, 208, 183, 84, 84, 16, 140, 110, 231, 113, 138, 141, 84, 228, 137, 157, 47, 171, 129, 19, 173, 16, 218, 134, 65, 164, 61, 194, 56, 31, 102, 245, 136, 89, 127, 212, 185, 200, 1, 232, 24, 68, 175, 128, 86, 66, 67, 53, 93, 6, 107, 220, 73, 220, 179, 208, 54, 132, 222, 2, 137, 72, 18, 150, 137, 44, 37, 182, 56, 134, 143, 25, 193, 143, 70, 176, 2, 183, 205, 80, 246, 207, 132, 4, 218, 183, 148, 140, 104, 206, 252, 145, 181, 22, 205, 95, 190, 249, 34, 116, 75, 68, 120, 34, 193, 91, 182, 82, 42, 38, 211, 50, 89, 123, 206, 224, 102, 206, 55, 79, 67, 67, 19, 186, 164, 82, 44, 63, 180, 194, 235, 126, 60, 229, 176, 215, 241, 104, 203, 160, 160, 203, 41, 139, 98, 79, 200, 58, 172, 27, 143, 204, 55, 125, 227, 13, 232, 24, 168, 192, 96, 46, 115, 129, 213, 231, 209, 156, 179, 48, 164, 1, 177, 17, 91, 111, 57, 217, 120, 8, 101, 26, 29, 113, 189, 166, 7, 130, 235, 222, 141, 11, 62, 154, 174, 206, 141, 167, 230, 107, 182, 249, 38, 44, 86, 92, 130, 27, 52, 161, 186, 230, 219, 25, 9, 2, 225, 143, 242, 255, 91, 21, 239, 66, 67, 35, 232, 254, 82, 134, 10, 15, 91, 226, 220, 145, 163, 30, 153, 70, 247, 138, 179, 179, 222, 131, 174, 38, 85, 92, 65, 11, 25, 171, 211, 254, 133, 51, 240, 222, 134, 93, 248, 95, 130, 113, 202, 112, 225, 234, 206, 43, 82, 55, 178, 23, 13, 187, 13, 123, 166, 144, 158, 225, 52, 175, 58, 164, 149, 225, 27, 130, 38, 185, 91, 234, 87, 77, 128, 175, 65, 191, 36, 176, 161, 20, 211, 73, 233, 103, 56, 34, 48, 223, 248, 106, 161, 182, 192, 246, 158, 118, 92, 123, 11, 22, 13, 134, 51, 50, 143, 5, 222, 161, 143, 175, 156, 60, 47, 36, 225, 62, 189, 169, 215, 237, 46, 251, 24, 218, 6, 195, 26, 89, 148, 165, 159, 56, 244, 202, 141, 105, 199, 214, 85, 39, 151, 159, 192, 242, 60, 168, 27, 178, 43, 177, 168, 113, 73, 141, 235, 30, 91, 253, 214, 217, 48, 39, 97, 80, 65, 152, 145, 138, 235, 229, 164, 200, 73, 68, 83, 90, 87, 206, 144, 58, 30, 102, 53, 165, 197, 120, 173, 156, 70, 169, 227, 226, 234, 9, 39, 250, 155, 48, 152, 197, 116, 229, 96, 80, 252, 209, 204, 134, 121, 217, 121, 24, 190, 3, 93, 139, 232, 234, 223, 157, 39, 36, 203, 61, 130, 117, 174, 72, 57, 117, 221, 210, 182, 195, 224, 60, 186, 84, 177, 6, 107, 229, 66, 170, 169, 165, 245, 135, 157, 7, 199, 14, 58, 87, 17, 34, 12, 153, 30, 174, 61, 124, 190, 69, 52, 148, 216, 87, 54, 14, 251, 46, 220, 60, 161, 74, 42, 38, 218, 70, 27, 223, 148, 84, 39, 219, 94, 231, 232, 124, 25, 97, 173, 118, 45, 162, 171, 166, 147, 79, 112, 188, 226, 12, 21, 86, 176, 49, 191, 242, 26, 95, 227, 149, 53, 147, 176, 17, 102, 67, 10, 44, 65, 204, 80, 23, 55, 154, 34, 9, 203, 235, 93, 185, 242, 185, 243, 180, 105, 205, 64, 186, 92, 209, 160, 126, 113, 249, 36, 169, 167, 200, 176, 152, 15, 65, 227, 153, 41, 134, 89, 197, 8, 180, 48, 28, 124, 102, 168, 173, 58, 245, 253, 52, 180, 75, 66, 233, 109, 218, 81, 250, 1, 95, 153, 171, 66, 236, 62, 128, 240, 157, 81, 50, 250, 50, 1, 147, 122, 233, 23, 220, 151, 172, 116, 203, 138, 6, 198, 198, 223, 70, 244, 17, 244, 167, 144, 246, 189, 61, 38, 156, 4, 172, 110, 242, 225, 177, 249, 152, 181, 110, 133, 1, 34, 248, 122, 196, 106, 174, 186, 187, 51, 193, 213, 39, 61, 108, 69, 107, 229, 140, 211, 5, 49, 244, 74, 198, 39, 188, 124, 141, 117, 39, 229, 102, 83, 44, 181, 83, 122, 253, 134, 35, 217, 143, 199, 34, 20, 227, 128, 69, 175, 154, 135, 89, 116, 152, 212, 175, 132, 207, 230, 111, 1, 155, 208, 169, 128, 72, 167, 172, 117, 110, 231, 184, 144, 185, 205, 250, 130, 233, 152, 214, 19, 255, 1, 231, 164, 2, 228};

          for (int i = 0; i < 1500; i++) {
            if (expect[i] != (kb_info_decrypted [i+8] & 255)){
              std::cout<<"byte not equal i: " <<i<<" expect char: "<<(int) expect[i]<<" real char: "<<(int)kb_info_decrypted[i+8]<<std::endl;
              abort();
            }
          }


          // finally, we needs to check adler32 checksum
          // key_block_info_compressed[4:8] => adler32 checksum
//          uint32_t chksum = be_bin_to_u32((unsigned char*) (kb_info_buff + 4));
//          uint32_t adlercs = adler32chksum(key_block_info_uncomp, static_cast<uint32_t>(key_block_info_uncomp_len)) & 0xffffffff;
//
//          assert(chksum == adlercs);


          /// here passed, key block info is corrected
          // TODO decode key block info compressed into keys list



          // for version 2.0, will compress by zlib, lzo just just for 1.0
          // key_block_info_buff[0:8] => compress_type
          // TODO zlib decompress
          // TODO:
          // if the size of compressed data original data is unknown,
          // we malloc 8 size of source data len, we cannot estimate the original data size
          // but currently, we know the size of key_block_info decompress size, so we use this

          auto * key_block_info_uncomp = (unsigned char*) calloc(this->key_block_info_decompress_size * 2 , sizeof(char));

          if (!key_block_info_uncomp) {
            std::cout<<"key_block_info_uncomp failed"<<std::endl;
            return EXIT_FAILURE;
          }
          unsigned long key_block_info_uncomp_len = 0;

          std::cout<<"<BBBBBBBBB"<<std::endl;
          putbytes((char*)(kb_info_decrypted + 8 * sizeof(char)), kb_info_buff_len - 8, false);

//          int uncpm_status = gzipDecompress((const char*)(kb_info_decrypted +8 * sizeof(char)), (int)(kb_info_buff_len - 8), (const char*)key_block_info_uncomp, this->key_block_info_decompress_size);
//          int uncpm_status = gzipDecompress((const char*)(kb_info_decrypted +8 * sizeof(char)), (int)(kb_info_buff_len - 8), (char*) key_block_info_uncomp, this->key_block_info_decompress_size);
          int uncpm_status = zipUncompress((Bytef*)key_block_info_uncomp, (uLongf*) &key_block_info_uncomp_len, (Bytef*)(kb_info_decrypted + 8), (uLongf)(kb_info_buff_len - 8));
          std::cout<<"<CCCCCCCCC"<<std::endl;
          putbytes((char*)kb_info_decrypted, (int)this->key_block_info_decompress_size,true);
























          // note: we should uncompress key_block_info_buffer[8:] data, so we need (decrypted + 8, and length -8)
//          int uncpm_status = uncompress(key_block_info_uncomp, &key_block_info_uncomp_len, kb_info_decrypted + 8 * sizeof(char), kb_info_buff_len - 8);
//          int uncpm_status = 0;
          if (uncpm_status != 0) {
            std::cout<<"key_block_info_uncmp uncompress failed, exit code: "<<uncpm_status<<std::endl;
            return EXIT_FAILURE;
          }

          std::cout<<"key_block_info_uncmp decompress success, exit code: "<<uncpm_status<<std::endl;










//      key_block_info = new BufferList(pako.inflate(key_block_info_compressed.slice(
//            8,
//            key_block_info_compressed.length,
//          )));


        }else {
          // doesn't compression
        }


      }

    void Mdict::readfile(uint64_t offset, uint64_t len, char* buf){
      std::cout<<"offset: " <<offset<<"len: "<<len<<std::endl;
      instream.seekg(offset);
      instream.read(buf, static_cast<std::streamsize>(len));
    }


    /***************************************
     *             public part             *
     ***************************************/

    std::string Mdict::lookup(const std::string word, int word_len){
      return "hello world" + this->filename + " ";
    }

    std::vector<std::string> Mdict::prefix(const std::string prefix, int prefix_len){
      std::vector<std::string> list;
      list.emplace_back("hello1");
      return list;
    }


}

int main(){

  jsmdict::Mdict mdict("/Users/chenquan/Workspace/cpp/libmdict/mdx/oale8.mdx");
  mdict.read_header();
  std::cout<<mdict.lookup("word", 4) <<std::endl;
  std::vector<std::string> ss = mdict.prefix("prefix", 6);
  //for (std::vector<std::string>::const_iterator i = ss.begin(); i != ss.end(); ++i)
  for (auto i = ss.begin(); i != ss.end(); ++i)
    std::cout << *i << std::endl;
  mdict.printhead();
  mdict.read_key_block_header();
  mdict.read_key_block_info();

}
