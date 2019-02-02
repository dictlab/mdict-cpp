// adler32.cpp - originally written and placed in the public domain by Wei Dai

#include <exception>
#include <assert.h>
#include <stddef.h>
#include <cstdio>
#include <cstdlib>
#include "binutils.h"

typedef unsigned char byte;
typedef unsigned short word16;
typedef unsigned int word32;



/// ADLER-32 checksum calculations
class Adler32 {
	public:
	static const int DIGESTSIZE = 4;

	Adler32() { Reset(); }

	void Update(const byte *input, size_t length);

	void TruncatedFinal(byte *hash, size_t size);

	unsigned int DigestSize() const { return DIGESTSIZE; }

	/// \brief Computes the hash of the current message
	/// \param digest a pointer to the buffer to receive the hash
	/// \details Final() restarts the hash for a new message.
	/// \pre <tt>COUNTOF(digest) == DigestSize()</tt> or <tt>COUNTOF(digest) == HASH::DIGESTSIZE</tt> ensures
	///   the output byte buffer is large enough for the digest.
	void Final(byte *digest)
	{TruncatedFinal(digest, DigestSize());}


	private:
	void Reset() {
		m_s1 = 1;
		m_s2 = 0;
	}

	word16 m_s1, m_s2;
};

void Adler32::Update(const byte *input, size_t length) {
	const unsigned long BASE = 65521;

	unsigned long s1 = m_s1;
	unsigned long s2 = m_s2;

	if (length % 8 != 0) {
		do {
			s1 += *input++;
			s2 += s1;
			length--;
		} while (length % 8 != 0);

		if (s1 >= BASE)
			s1 -= BASE;
		s2 %= BASE;
	}

	while (length > 0) {
		s1 += input[0];
		s2 += s1;
		s1 += input[1];
		s2 += s1;
		s1 += input[2];
		s2 += s1;
		s1 += input[3];
		s2 += s1;
		s1 += input[4];
		s2 += s1;
		s1 += input[5];
		s2 += s1;
		s1 += input[6];
		s2 += s1;
		s1 += input[7];
		s2 += s1;

		length -= 8;
		input += 8;

		if (s1 >= BASE)
			s1 -= BASE;
		if (length % 0x8000 == 0)
			s2 %= BASE;
	}


	assert(s1 < BASE);
	assert(s2 < BASE);

	m_s1 = (word16) s1;
	m_s2 = (word16) s2;
}

void Adler32::TruncatedFinal(byte *hash, size_t size) {
	// ThrowIfInvalidTruncatedSize(size);
	if (size != DIGESTSIZE) {
		// TODO enhance exception logic
		throw std::exception();
	}

	switch (size) {
		default:
			hash[3] = byte(m_s1);
			// fall through
		case 3:
			hash[2] = byte(m_s1 >> 8);
			// fall through
		case 2:
			hash[1] = byte(m_s2);
			// fall through
		case 1:
			hash[0] = byte(m_s2 >> 8);
			// fall through
		case 0:;;
			// fall through
	}

	Reset();
}

uint32_t alder32chksum(const char* data, uint32_t len) {
  Adler32 adler32hasher;
//  char* str = const_cast<char *>("helloworld");
  adler32hasher.Update(reinterpret_cast<const byte *>(data), len);
  char* hash = (char*)calloc(4, sizeof(char));
  adler32hasher.Final(reinterpret_cast<byte *>(hash));
//  for (int i = 0; i < 4; i++) {
//    printf("%x ", hash[i]);
//  }
  
  uint32_t chksum = be_bin_to_u32(hash);
  if(hash) free(hash);
  return chksum;
}


#ifdef ADLER32_TEST
int main() {
  Adler32 adler32hasher;
  char* str = const_cast<char *>("helloworld");
  adler32hasher.Update(reinterpret_cast<const byte *>(str), 10);
  char* hash = (char*)calloc(4, sizeof(char));
  adler32hasher.Final(reinterpret_cast<byte *>(hash));
  for (int i = 0; i < 4; i++) {
    printf("%x ", hash[i]);
  }
}
#endif