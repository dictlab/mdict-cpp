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
//#include <zlib.h>
#include "zlib_wrapper.h"
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

    class key_block_info{
    public:
       std::string first_key;
       std::string last_key;
       unsigned long key_block_start_offset;
       unsigned long key_block_comp_size;
       unsigned long key_block_decomp_size;
       key_block_info(std::string first_key, std::string last_key, unsigned long kb_start_ofset, unsigned long kb_comp_size,
                      unsigned long kb_decomp_size){
         this->key_block_comp_size = kb_comp_size;
         this->key_block_decomp_size = kb_decomp_size;
         this->key_block_start_offset = kb_start_ofset;
         this->first_key = first_key;
         this->last_key = last_key;
       }
       ~key_block_info(){
         delete this;
       }
    };

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

        int decode_key_block(unsigned char* key_block_buffer, unsigned long kb_buff_len);

        void printhead() {
//          std::cout<<"version: "<<this->version<<std::endl<<"encoding: "<<this->encoding<<std::endl;
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

        // ---------------------
        //  key block body offset
        // ---------------------

        uint64_t key_block_body_start = 0;

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


        void split_key_block(unsigned char* key_block, unsigned long key_block_len);


        // key block info list
        std::vector<key_block_info*> key_block_info_list;


    };


    // constructor
    Mdict::Mdict(std::string fn)noexcept: filename(fn) {
      offset = 0;
      instream = std::ifstream(filename, std::ios::binary);
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
//      putbytes(head_size_buf, 4, true);
      // header byte size convert
      uint32_t header_bytes_size = be_bin_to_u32((const unsigned char*) head_size_buf);
      std::free(head_size_buf);
      // assign key block start offset
      this->header_bytes_size = header_bytes_size;
      this->key_block_start_offset = this->header_bytes_size + 8;

      // -----------------------------------------
      // 2. [4: header_bytes_size+4], header buffer
      // -----------------------------------------

      // header buffer
      char* head_buffer = (char*) std::calloc(header_bytes_size, sizeof(char));
      readfile(4, header_bytes_size, head_buffer);
//      putbytes(head_buffer, header_bytes_size, true);

      // -----------------------------------------
      // 3. alder32 checksum
      // -----------------------------------------

      // TODO  version < 2.0 needs to checksum?
      // alder32 checksum buffer
      char* head_checksum_buffer = (char*) std::calloc(4, sizeof(char));
      readfile(header_bytes_size + 4, 4, head_checksum_buffer);
//      putbytes(head_checksum_buffer, 4, true);

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
//      std::cout<< header_text << std::endl;

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
//      putbytes(key_block_info_buffer, key_block_info_bytes_num, true);


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
//      putbytes(key_block_nums_bytes, this->number_width, true);

      uint64_t key_block_num = 0;
      if (this->number_width == 8) key_block_num = be_bin_to_u64((const unsigned char*)key_block_nums_bytes);
      else if(this->number_width == 4) key_block_num = be_bin_to_u32((const unsigned char*)key_block_nums_bytes);
      // TODO PASSED

      // 2. [8:16]  - number of entries
      char * entries_num_bytes = (char*)calloc(static_cast<size_t>(this->number_width), sizeof(char));
      eno = bin_slice(key_block_info_buffer, key_block_info_bytes_num, this->number_width, this->number_width, entries_num_bytes);
      if (eno != 0){
        std::cerr<<"binslice err2:"<< eno <<std::endl;
        // TODO throw error
      }

//      putbytes(entries_num_bytes, this->number_width, true);

      uint64_t entries_num = 0;
      if (this->number_width == 8) entries_num = be_bin_to_u64((const unsigned char*)entries_num_bytes);
      else if(this->number_width == 4) key_block_num = be_bin_to_u32((const unsigned char*)entries_num_bytes);
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
//        putbytes(key_block_info_decompress_size_bytes, this->number_width, true);

        uint64_t key_block_info_decompress_size = 0;
        if (this->number_width == 8) key_block_info_decompress_size = be_bin_to_u64((const unsigned char*)key_block_info_decompress_size_bytes);
        else if(this->number_width == 4) key_block_info_decompress_size = be_bin_to_u32((const unsigned char*)key_block_info_decompress_size_bytes);
        this->key_block_info_decompress_size = key_block_info_decompress_size;
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
//      putbytes(key_block_info_size_buffer, this->number_width, true);

      uint64_t key_block_info_size = 0;
      if (this->number_width == 8) key_block_info_size = be_bin_to_u64((const unsigned char*)key_block_info_size_buffer);
      else if(this->number_width == 4) key_block_info_size = be_bin_to_u32((const unsigned char*)key_block_info_size_buffer);
      // TODO PASSED

      // 5. [32:40] - key block size
      char * key_block_size_buffer = (char*)calloc(static_cast<size_t>(this->number_width), sizeof(char));
      eno = bin_slice(key_block_info_buffer, key_block_info_bytes_num, key_block_info_size_start_offset + this->number_width, this->number_width, key_block_size_buffer);
      if (eno != 0){
        std::cerr<<"binslice err3:"<< eno <<std::endl;
        // TODO throw error
      }
//      putbytes(key_block_size_buffer, this->number_width, true);

      uint64_t key_block_size = 0;
      if (this->number_width == 8) key_block_size = be_bin_to_u64((const unsigned char*)key_block_size_buffer);
      else if(this->number_width == 4) key_block_size = be_bin_to_u32((const unsigned char*)key_block_size_buffer);
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

      // here passed

      char* key_block_compressed_buffer = (char*) calloc(static_cast<size_t>(this->key_block_size), sizeof(char));

      readfile(this->key_block_compressed_start_offset, static_cast<int>(this->key_block_size), key_block_compressed_buffer);

      // ------------------------------------
      // TODO decode key_block_compressed
      // ------------------------------------
      unsigned  long kb_len = this->key_block_size;

      int err = decode_key_block((unsigned char*) key_block_compressed_buffer ,kb_len);
      if (err != 0) {
        throw std::runtime_error("decode key block error");
      }




      if (key_block_info_buffer != nullptr) std::free(key_block_info_buffer);
      if (key_block_compressed_buffer != nullptr) std::free(key_block_compressed_buffer);

    }


    void fast_decrypt(byte* data, const byte* k, int data_len, int key_len){
      const byte* key = k;
//      putbytes((char*)data, 16, true);
      byte* b = data;
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
      byte* key_buffer = (byte*) calloc(8, sizeof(byte));
      memcpy(key_buffer, comp_block + 4 * sizeof(char), 4 * sizeof(char));
      key_buffer[4] = 0x95; // comp_block[4:8] + [0x95,0x36,0x00,0x00]
      key_buffer[5] = 0x36;

      byte* key = ripemd128bytes(key_buffer, 8);


      fast_decrypt(comp_block + 8 * sizeof(byte), key, comp_block_len - 8, 16 /* key length*/);

      // finally
      std::free(key_buffer);
      return comp_block;
      /// passed
    }

    void Mdict::split_key_block(unsigned char* key_block, unsigned long key_block_len){
      int key_start_idx = 0;
      int key_end_idx = 0;

      while(key_start_idx < key_block_len){
        // # the corresponding record's offset in record block
        unsigned long key_id = 0;
        int width = 0;
        if(this->version >= 2.0) {
          key_id = be_bin_to_u64(key_block + key_start_idx);
          width = 2;
        }else{
          key_id = be_bin_to_u32(key_block + key_start_idx);
          width = 1;
        }
       // key text ends with '\x00'
       // version >= 2.0 delimiter == '0x0000'
       // else delimiter == '0x00'  (< 2.0)
       int i = key_start_idx + number_width;// ver > 2.0, move 8, else move 4
       while(i< key_block_len){
         if(version >= 2.0){
          if (key_block[i] == 0 && key_block[i+1] == 0 /* delimiter = '0000' */){
            key_end_idx = i;
            break;
          }
         }else{
           if (key_block[i] == 0 /* delimiter == '0' */){
            key_end_idx = i;
            break;
          }
         }

         i += 1;

       }

       std::string key_text = "";
       if (this->encoding == 1 /* ENCODING_UTF16 */){
         // TODO
         throw std::runtime_error("error");
       }else if (this->encoding == 0 /* ENCODING_UTF8 */){
         key_text = be_bin_to_utf8(
           (const char*)key_block,
           (key_start_idx + this->number_width),
           static_cast<unsigned long>(key_end_idx - key_start_idx - this->number_width));
       }

       // next round
       key_start_idx = key_end_idx + width;

//       break;

      }

    }


    int Mdict::decode_key_block(unsigned char* key_block_buffer, unsigned long kb_buff_len) {
      std::vector<char*> key_list;
      int i = 0;
      for (; i < this->key_block_info_list.size(); i++){
        unsigned long comp_size = this->key_block_info_list[i]->key_block_comp_size;
        unsigned long decomp_size = this->key_block_info_list[i]->key_block_decomp_size;
        unsigned long start_ofset = i;
        unsigned long end_ofset = i + comp_size;
        // 4 bytes comp type
        char* key_block_comp_type = (char*) calloc(4 , sizeof(char));
        memcpy(key_block_comp_type, key_block_buffer, 4 * sizeof(char));
        // 4 bytes adler checksum of decompressed key block
        // TODO  adler32 = unpack('>I', key_block_compressed[start + 4:start + 8])[0]
        unsigned char* key_block = nullptr;
        if((key_block_comp_type[0] & 255) == 0) {
          // none compressed
          key_block = key_block_buffer + 8 * sizeof(char);
        }else if((key_block_comp_type[0] & 255) == 1) {
          // 01000000
          // TODO lzo decompress

        }else if((key_block_comp_type[0] & 255) == 2){
          // zlib compress

          std::vector<uint8_t> kb_uncompressed = zlib_mem_uncompress(key_block_buffer + start_ofset + 8, comp_size - 8 );
          key_block = kb_uncompressed.data();
        }else{
          throw std::runtime_error("cannot determine the key block compress type");
        }

//        putbytes((char*)(key_block_buffer + start_ofset + 8), static_cast<int>(comp_size - 8), true);
//        putbytes((char*)key_block, static_cast<int>(comp_size - 8), true);
        // split key
        split_key_block(key_block, decomp_size);
        // TODO HERE append keys



        // next round
        i += comp_size;


      }
      return 0;
    }


    // note: kb_info_buff_len == key_block_info_compressed_size
    int Mdict::decode_key_block_info(char* key_block_info_buffer, unsigned long kb_info_buff_len, int key_block_num, int entries_num){
        char* kb_info_buff = key_block_info_buffer;

      // key block info offset indicator
      unsigned long data_offset = 0;

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


          // finally, we needs to check adler32 checksum
          // key_block_info_compressed[4:8] => adler32 checksum
//          uint32_t chksum = be_bin_to_u32((unsigned char*) (kb_info_buff + 4));
//          uint32_t adlercs = adler32checksum(key_block_info_uncomp, static_cast<uint32_t>(key_block_info_uncomp_len)) & 0xffffffff;
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


          // note: we should uncompress key_block_info_buffer[8:] data, so we need (decrypted + 8, and length -8)
          std::vector<uint8_t> decompress_buff = zlib_mem_uncompress(kb_info_decrypted+8, kb_info_buff_len - 8, this->key_block_info_decompress_size);
          /// uncompress successed
          assert(decompress_buff.size() == this->key_block_info_decompress_size);

          // get key block info list
//          std::vector<key_block_info*> key_block_info_list;
          /// entries summary, every block has a lot of entries, the sum of entries should equals entries_number
          unsigned long num_entries_counter = 0;
          // key number counter
          unsigned long counter = 0;

          // current block entries
          unsigned long current_entries = 0;

          unsigned long previous_start_offset = 0;


          int byte_width = 1;
          int text_term = 0;
          if (this->version >= 2.0) {
            byte_width = 2;
            text_term = 1;
          }

          while(counter < this->key_block_num){
            if (this->version >= 2.0) {
              current_entries = be_bin_to_u64(decompress_buff.data() + data_offset * sizeof(uint8_t));
            }else{
              current_entries = be_bin_to_u32(decompress_buff.data() + data_offset * sizeof(uint8_t));
            }
            num_entries_counter += current_entries;

            // move offset
            // if version>= 2.0 move forward 8 bytes

            data_offset += this->number_width * sizeof(uint8_t);

            // first key size
            unsigned long first_key_size = 0;

            if (this->version >= 2.0) {
              first_key_size = be_bin_to_u16(decompress_buff.data() + data_offset * sizeof(uint8_t));
            }else{
              first_key_size = be_bin_to_u8(decompress_buff.data() + data_offset * sizeof(uint8_t));
            }
            data_offset += byte_width;

            // step_gap means first key start offset to first key end;
            int step_gap = 0;

            if ( this->encoding == 1 /* encoding utf16 equals 1*/ ) {
              step_gap = (first_key_size + text_term) * 2;
            } else {
              step_gap = first_key_size + text_term;
            }

            // DECODE first CODE
            // TODO here minus the terminal character size(1), but we still not sure should minus this or not
            std::string fkey = be_bin_to_utf8((char*)(decompress_buff.data() + data_offset), 0, (unsigned long)step_gap - text_term);
//            std::cout<<"first key: "<<fkey<<std::endl;
            // move forward
            data_offset += step_gap;


            // the last key
            unsigned long last_key_size = 0;

            if (this->version >= 2.0) {
              last_key_size = be_bin_to_u16(decompress_buff.data() + data_offset * sizeof(uint8_t));
            }else{
              last_key_size = be_bin_to_u8(decompress_buff.data() + data_offset * sizeof(uint8_t));
            }
            data_offset += byte_width;

            if (this->encoding == 1 /* ENCODING_UTF16 */) {
              step_gap = (last_key_size + text_term) * 2;
            } else {
              step_gap = last_key_size + text_term;
            }

            std::string last_key = be_bin_to_utf8((char*)(decompress_buff.data() + data_offset), 0, (unsigned long)step_gap - text_term);

            // move forward
            data_offset += step_gap;


            // ------------
            // key block part
            // ------------

            uint64_t key_block_compress_size = 0;
            if (version >= 2.0) {
              key_block_compress_size = be_bin_to_u64(decompress_buff.data() + data_offset);
            }else{
              key_block_compress_size = be_bin_to_u32(decompress_buff.data() + data_offset);
            }

            data_offset += this->number_width;

            uint64_t key_block_decompress_size = 0;

            if (version >= 2.0) {
              key_block_decompress_size = be_bin_to_u64(decompress_buff.data() + data_offset);
            }else{
              key_block_decompress_size = be_bin_to_u32(decompress_buff.data() + data_offset);
            }

            // entries offset move forward
            data_offset += this->number_width;


            key_block_info* kbinfo = new key_block_info(fkey, last_key, previous_start_offset, key_block_compress_size, key_block_decompress_size);
            // adjust ofset
            previous_start_offset += key_block_compress_size;
            key_block_info_list.push_back(kbinfo);

            // key block counter
            counter += 1;
//          break;

          }
          assert(counter == this->key_block_num);
          assert(num_entries_counter == this->entries_num);

//          std::vector<key_block_info*>::iterator it;

          //TODO WORKING HERE
          for (auto it = key_block_info_list.begin(); it != key_block_info_list.end(); it++){
            std::cout<<"fkey : "<<(*it)->first_key<<std::endl;
            std::cout<<"lkey : "<<(*it)->last_key<<std::endl;
            std::cout<<"comp_size : "<<(*it)->key_block_comp_size<<std::endl;
            std::cout<<"decomp_size : "<<(*it)->key_block_decomp_size<<std::endl;
            std::cout<<"offset : "<<(*it)->key_block_start_offset<<std::endl;
            break;
          }

        }else {
          // doesn't compression
        }

//        std::cout<<"data offset: " << data_offset<<std::endl;
//        assert(data_offset == this->key_block_info_decompress_size);
        this->key_block_body_start = this->key_block_info_start_offset + this->key_block_info_size;
//        std::cout<<"key_block_body offset: " << this->key_block_body_start<<std::endl;
        /// here passed


      }

    void Mdict::readfile(uint64_t offset, uint64_t len, char* buf){
      instream.seekg(offset);
      instream.read(buf, static_cast<std::streamsize>(len));
    }


    /***************************************
     *             public part             *
     ***************************************/

    std::string Mdict::lookup(const std::string word, int word_len){
      return "lookup.. -? " + this->filename + " ";
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
  mdict.printhead();
  mdict.read_key_block_header();
  mdict.read_key_block_info();

}
