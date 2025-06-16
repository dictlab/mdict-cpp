#include <gtest/gtest.h>
#include "deps/turbobase64/turbob64.h"
#include <string>
#include <vector>

// Test fixture for base64 tests
class Base64Test : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize the base64 library
        tb64ini(0, 0);
    }
    void TearDown() override {}
};

// Helper function to encode string to base64
std::string encode_base64(const std::string& input) {
    size_t outlen = tb64enclen(input.length());
    std::vector<unsigned char> output(outlen);
    size_t actual_len = tb64enc(
        reinterpret_cast<const unsigned char*>(input.c_str()),
        input.length(),
        output.data()
    );
    return std::string(output.begin(), output.begin() + actual_len);
}

// Helper function to decode base64 to string
std::string decode_base64(const std::string& input) {
    size_t outlen = tb64declen(
        reinterpret_cast<const unsigned char*>(input.c_str()),
        input.length()
    );
    if (outlen == 0) return "";
    
    std::vector<unsigned char> output(outlen);
    size_t actual_len = tb64dec(
        reinterpret_cast<const unsigned char*>(input.c_str()),
        input.length(),
        output.data()
    );
    return std::string(output.begin(), output.begin() + actual_len);
}

// Test basic encoding
TEST_F(Base64Test, BasicEncode) {
    std::string input = "Hello, World!";
    std::string expected = "SGVsbG8sIFdvcmxkIQ==";
    std::string encoded = encode_base64(input);
    EXPECT_EQ(encoded, expected);
}

// Test basic decoding
TEST_F(Base64Test, BasicDecode) {
    std::string input = "SGVsbG8sIFdvcmxkIQ==";
    std::string expected = "Hello, World!";
    std::string decoded = decode_base64(input);
    EXPECT_EQ(decoded, expected);
}

// Test special characters
TEST_F(Base64Test, SpecialCharacters) {
    std::string input = "!@#$%^&*()_+";
    std::string encoded = encode_base64(input);
    std::string decoded = decode_base64(encoded);
    EXPECT_EQ(decoded, input);
}

// Test binary data
TEST_F(Base64Test, BinaryData) {
    std::vector<unsigned char> binary_data = {0x00, 0xFF, 0x42, 0x13, 0x37};
    std::string input(binary_data.begin(), binary_data.end());
    std::string encoded = encode_base64(input);
    std::string decoded = decode_base64(encoded);
    EXPECT_EQ(decoded, input);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
