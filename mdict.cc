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

#include "mdict.h"
#include "fileutils.h"
#include "binutils.h"
#include "xmlutils.h"

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



namespace jsmdict {

	class Mdict{
		public:
			Mdict(std::string fn) noexcept;
			~Mdict();
			std::string lookup(const std::string word, int word_len);
			std::vector<std::string> prefix(const std::string prefix, int prefix_len);
			// read dictory file header
			void read_header();

			void printhead() {
				std::cout<<"version: "<<this->version<<std::endl<<"encoding: "<<this->encoding<<std::endl;
			}

		private:
			const std::string filename;

			long long offset;

			std::ifstream instream;

			int encrypt = 0;

			float version = 1.0;
			int number_width = 8;
      		int number_format = 0;

			int encoding = ENCODING_UTF8;

			// read offset and len bytes
			void readfile(int offset, int len, char *buf);

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
		// head size buffer
		char* head_size_buf = (char*) std::calloc(4, sizeof(char));
		putbytes(head_size_buf)
		readfile(0, 4, head_size_buf);
		uint32_t b_size = be_bin_to_u32((const unsigned char*) head_size_buf);
    std::free(head_size_buf);
    std::cout<<"len1:" <<b_size<<std::endl;

		// header buffer
		char* head_buffer = (char*) std::calloc(b_size, sizeof(char));
		readfile(4, b_size, head_buffer);
		putbytes(head_buffer, b_size);
		// alder32 checksum buffer
		char* head_checksum_buffer = (char*) std::calloc(4, sizeof(char));
		readfile(b_size + 4, 4, head_checksum_buffer);
		putbytes(head_checksum_buffer, 4);
		// skip head checksum for now TODO(terasum)
		std::free(head_checksum_buffer);

		// header text utf16
		std::string header_text = le_bin_utf16_to_utf8(head_buffer, 0, b_size - 2 );
		if (header_text == "") {
		  std::cout<<"len:" <<b_size<<std::endl;
			std::cout<<"this mdx file is invalid"<<std::endl;
			return;
		}
		std::cout<< header_text << std::endl;

		std::map<std::string, std::string> headinfo = parseXMLHeader(header_text);

		// encrypted flag
		// 0x00 - no encryption
		// 0x01 - encrypt record block
		// 0x02 - encrypt key info block
		if ( headinfo.find("Encrypted") == headinfo.end() ||
				headinfo["Encrypted"].compare("") == 0 ||
				headinfo["Encrypted"].compare("No") == 0) {
			this->encrypt = ENCRYPT_NO_ENC;
		} else if ( headinfo["Encrypted"].compare("Yes") == 0) {
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

    // before version 2.0, number is 4 bytes integer
    // version 2.0 and above use 8 bytes

    std::string sver = headinfo["GeneratedByEngineVersion"];
	std::string::size_type sz;     // alias of size_t

    float version = std::stof (sver,&sz);
	this->version = version;

    if (this->version >= 2.0) {
      this->number_width = 8;
      this->number_format = NUMFMT_BE_8BYTESQ;
    } else {
      this->number_format = NUMFMT_BE_4BYTESI;
      this->number_width = 4;
    }

	// encoding
	if (headinfo.find("Encoding") != headinfo.end() || headinfo["Encoding"].compare("") == 0 || headinfo["Encoding"].compare("UTF-8") == 0 ) {
		this->encoding = ENCODING_UTF8;
	} else if (headinfo["Encoding"].compare("GBK") == 0 || headinfo["Encoding"].compare("GB2312") == 0) {
		this->encoding = ENCODING_GB18030;
	} else if (headinfo["Encoding"].compare("Big5") == 0 || headinfo["Encoding"].compare("BIG5") == 0) {
		this->encoding = ENCODING_BIG5;
	} else if (headinfo["Encoding"].compare("utf16") == 0 || headinfo["Encoding"].compare("utf-16") == 0) {
		this->encoding = ENCODING_UTF16;
	} else {
		this->encoding = ENCODING_UTF8;
	}

	}

	void Mdict::readfile(int offset, int len,char* buf){
		instream.seekg(offset);
		instream.read(buf, len);
	}


	/***************************************
	 *             public part             *
	 ***************************************/

	std::string Mdict::lookup(const std::string word, int word_len){
		return "hello world" + this->filename + " ";
	}

	std::vector<std::string> Mdict::prefix(const std::string prefix, int prefix_len){
		std::vector<std::string> list;
		list.push_back("hello1");
		list.push_back("hello2");
		list.push_back("hello3");
		list.push_back("hello4");
		return list;
	}


}

int main(){

	jsmdict::Mdict mdict("mdx/oale8.mdx");
	mdict.read_header();
	std::cout<<mdict.lookup("word", 4) <<std::endl;
	std::vector<std::string> ss = mdict.prefix("prefix", 6);
	//for (std::vector<std::string>::const_iterator i = ss.begin(); i != ss.end(); ++i)
	for (auto i = ss.begin(); i != ss.end(); ++i)
		std::cout << *i << std::endl;
	mdict.printhead();

}
