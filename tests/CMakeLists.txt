enable_testing()
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/deps/googletest/googletest/include)
include_directories(${PROJECT_SOURCE_DIR}/deps/googletest/googlemock/include)

# Copy testdict and wordlist to binary directory
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/testdict)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
file(COPY testdict/testdict.mdx DESTINATION ${PROJECT_BINARY_DIR}/testdict)
file(COPY testdict/wordlist.txt DESTINATION ${PROJECT_BINARY_DIR}/testdict)
file(COPY text_xml.xml DESTINATION ${PROJECT_BINARY_DIR}/tests)


# Test executables
add_executable(test_lookup test_lookup.cc)
target_link_libraries(test_lookup GTest GTestMain mdict Miniz)
add_test(NAME test_lookup COMMAND test_lookup)

add_executable(test_adler32 test_adler32.cc)
target_link_libraries(test_adler32 GTest GTestMain mdict)
add_test(NAME test_adler32 COMMAND test_adler32)

add_executable(test_wordlist test_wordlist.cc)
target_link_libraries(test_wordlist GTest GTestMain mdict Miniz)
add_test(NAME test_wordlist COMMAND test_wordlist)

add_executable(test_xmlutils test_xmlutils.cc)
target_link_libraries(test_xmlutils GTest GTestMain mdict Miniz)
add_test(NAME test_xmlutils COMMAND test_xmlutils)

add_executable(test_base64 test_base64.cc)
target_link_libraries(test_base64 GTest GTestMain mdict)
add_test(NAME test_base64 COMMAND test_base64)

add_executable(test_binutils test_binutils.cc)
target_link_libraries(test_binutils GTest GTestMain mdict)
add_test(NAME test_binutils COMMAND test_binutils)

add_executable(test_ripemd128 test_ripemd128.cc)
target_link_libraries(test_ripemd128 GTest GTestMain mdict)
add_test(NAME test_ripemd128 COMMAND test_ripemd128)