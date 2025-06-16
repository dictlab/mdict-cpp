#include <gtest/gtest.h>
#include "include/xmlutils.h"
#include <map>
#include <string>
#include <fstream>

class XMLUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Test empty or invalid XML headers
TEST_F(XMLUtilsTest, EmptyAndInvalidHeaders) {
    std::map<std::string, std::string> result;
    
    // Test empty string
    parse_xml_header("", result);
    EXPECT_TRUE(result.empty());
    
    // Test string too short
    result.clear();
    parse_xml_header("a", result);
    EXPECT_TRUE(result.empty());
    
    // Test string without proper ending
    result.clear();
    parse_xml_header("abc", result);
    EXPECT_TRUE(result.empty());
    
    // Test string with wrong ending
    result.clear();
    parse_xml_header("abc>", result);
    EXPECT_TRUE(result.empty());
}

// Test basic XML header parsing
TEST_F(XMLUtilsTest, BasicHeaderParsing) {
    std::string xml = "version=\"1.0\" encoding=\"UTF-8\"/>";
    std::map<std::string, std::string> result;
    parse_xml_header(xml, result);
    
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result["version"], "1.0");
    EXPECT_EQ(result["encoding"], "UTF-8");
}

// Test XML header with spaces
TEST_F(XMLUtilsTest, HeaderWithSpaces) {
    std::string xml = "  version=\"1.0\"  encoding=\"UTF-8\"  />";
    std::map<std::string, std::string> result;
    parse_xml_header(xml, result);
    
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result["version"], "1.0");
    EXPECT_EQ(result["encoding"], "UTF-8");
}

// Test XML header with multiple attributes
TEST_F(XMLUtilsTest, MultipleAttributes) {
    std::string xml = "version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" />";
    std::map<std::string, std::string> result;
    parse_xml_header(xml, result);
    
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result["version"], "1.0");
    EXPECT_EQ(result["encoding"], "UTF-8");
    EXPECT_EQ(result["standalone"], "yes");
}

// Test XML header with special characters in values
TEST_F(XMLUtilsTest, SpecialCharacters) {
    std::string xml = "version=\"1.0\" encoding=\"UTF-8\" description=\"Test & Special > Characters < />";
    std::map<std::string, std::string> result;
    parse_xml_header(xml, result);
    
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result["version"], "1.0");
    EXPECT_EQ(result["encoding"], "UTF-8");
    EXPECT_EQ(result["description"], "Test & Special > Characters < ");
}

// Test XML header with empty values
TEST_F(XMLUtilsTest, EmptyValues) {
    std::string xml = "version=\"\" encoding=\"UTF-8\" />";
    std::map<std::string, std::string> result;
    parse_xml_header(xml, result);
    
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result["version"], "");
    EXPECT_EQ(result["encoding"], "UTF-8");
}

// Test XML header with malformed quotes
TEST_F(XMLUtilsTest, MalformedQuotes) {
    std::string xml = "version=1.0 encoding=\"UTF-8\" />";
    std::map<std::string, std::string> result;
    parse_xml_header(xml, result);
    
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result["encoding"], "UTF-8");
}

// Test large dictionary XML header parsing
TEST_F(XMLUtilsTest, DictionaryHeaderParsing) {
    std::string dicxml =
        "<Dictionary GeneratedByEngineVersion=\"2.0\" "
        "RequiredEngineVersion=\"2.0\" Format=\"Html\" KeyCaseSensitive=\"No\" "
        "StripKey=\"Yes\" Encrypted=\"2\" RegisterBy=\"EMail\" "
        "Description=\"Oxford Advanced Learner's English-Chinese Dictionary "
        "Eighth edition Based on Langheping&apos;s version Modified by "
        "EarthWorm&lt;br/&gt;Headwords: 41969 &lt;br/&gt; Entries: 109473 "
        "&lt;br/&gt; Version: 3.0.0 &lt;br/&gt;Date: 2018.02.18 &lt;br/&gt; Last "
        "Modified By roamlog&lt;br/&gt;\" Title=\"\" Encoding=\"UTF-8\" "
        "CreationDate=\"2018-2-18\" Compact=\"Yes\" Compat=\"Yes\" "
        "Left2Right=\"Yes\" DataSourceFormat=\"106\" StyleSheet=\"a\"/>";
    std::map<std::string, std::string> headinfo;
    parse_xml_header(dicxml, headinfo);
    
    // Verify all attributes are present and correct
    EXPECT_EQ(headinfo["GeneratedByEngineVersion"], "2.0");
    EXPECT_EQ(headinfo["RequiredEngineVersion"], "2.0");
    EXPECT_EQ(headinfo["Format"], "Html");
    EXPECT_EQ(headinfo["KeyCaseSensitive"], "No");
    EXPECT_EQ(headinfo["StripKey"], "Yes");
    EXPECT_EQ(headinfo["Encrypted"], "2");
    EXPECT_EQ(headinfo["RegisterBy"], "EMail");
    EXPECT_EQ(headinfo["Title"], "");
    EXPECT_EQ(headinfo["Encoding"], "UTF-8");
    EXPECT_EQ(headinfo["CreationDate"], "2018-2-18");
    EXPECT_EQ(headinfo["Compact"], "Yes");
    EXPECT_EQ(headinfo["Compat"], "Yes");
    EXPECT_EQ(headinfo["Left2Right"], "Yes");
    EXPECT_EQ(headinfo["DataSourceFormat"], "106");
    EXPECT_EQ(headinfo["StyleSheet"], "a");
    
    // Verify the Description attribute (which contains special characters)
    std::string expected_desc = "Oxford Advanced Learner's English-Chinese Dictionary "
                              "Eighth edition Based on Langheping&apos;s version Modified by "
                              "EarthWorm&lt;br/&gt;Headwords: 41969 &lt;br/&gt; Entries: 109473 "
                              "&lt;br/&gt; Version: 3.0.0 &lt;br/&gt;Date: 2018.02.18 &lt;br/&gt; Last "
                              "Modified By roamlog&lt;br/&gt;";
    EXPECT_EQ(headinfo["Description"], expected_desc);
    
    // Verify total number of attributes
    EXPECT_EQ(headinfo.size(), 16);
}

// Test XML header with multiple attributes
TEST_F(XMLUtilsTest, MultipleAttributes2) {
    // read the text_xml.xml file relative to the current file
    std::ifstream file("../tests/text_xml.xml");
    if (!file.is_open()) {
        FAIL() << "Failed to open text_xml.xml";
    }
    std::string xml((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::map<std::string, std::string> result;
    parse_xml_header(xml, result);
    
    EXPECT_EQ(result["Title"], "旺文社漢字典 第四版");
    EXPECT_EQ(result["RegisterBy"], "");
    EXPECT_EQ(result["GeneratedByEngineVersion"], "2.0");
    EXPECT_EQ(result["RequiredEngineVersion"], "2.0");
    EXPECT_EQ(result["Encrypted"], "No");
    EXPECT_EQ(result["Encoding"], "");
    EXPECT_EQ(result["Format"], "");
    EXPECT_EQ(result["CreationDate"], "2025-5-17");
    EXPECT_EQ(result["KeyCaseSensitive"], "No");
    EXPECT_EQ(result["Stripkey"], "No");
    EXPECT_EQ(result.size(), 11);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
