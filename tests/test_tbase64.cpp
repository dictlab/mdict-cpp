// test turbobase64

#include "deps/turbobase64/turbob64.h"
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <iostream>

// compile this file with clang++
// cd ${workspace}
// clang++ -std=c++11 -I. -L./deps/turbobase64/build -lbase64 -o test_tbase64 tests/test_tbase64.cpp
int main() {
    // read file from build/xxx.bin
    const std::string filename = "build/_img_019_s.bin";
    // detect file exists
    if (!std::ifstream(filename)) {
        std::cerr << "File " << filename << " does not exist" << std::endl;
        return 1;
    }

    std::ifstream input_file(filename, std::ios::binary);
    std::vector<uint8_t> input_data(std::istreambuf_iterator<char>(input_file), {});

    // encode to base64
    const size_t base64_len = tb64enclen(input_data.size());
    unsigned char *base64_buf = new unsigned char[base64_len];
    tb64enc(input_data.data(), input_data.size(), base64_buf);
    std::string base64_str(reinterpret_cast<char *>(base64_buf), base64_len);
    // output the base64 string
    std::cout << base64_str << std::endl;
    return 0;
}