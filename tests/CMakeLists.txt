enable_testing()
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/deps/googletest/googletest/include)
include_directories(${PROJECT_SOURCE_DIR}/deps/googletest/googlemock/include)

add_subdirectory(testdict)

add_executable(test_reduce0 test_lookup_01.cpp)
target_link_libraries(test_reduce0 GTest GTestMain mdict Miniz)
add_test(NAME test_reduce0 COMMAND test_reduce0)

add_executable(test_adler32 test_adler32.cpp)
target_link_libraries(test_adler32 GTest GTestMain mdict)
add_test(NAME test_adler32 COMMAND test_adler32)


add_executable(test_wordlist test_wordlist.cpp)
target_link_libraries(test_wordlist GTest GTestMain mdict Miniz)
add_test(NAME test_wordlist COMMAND test_wordlist)

add_executable(test_xmlutils test_xmlutils.cpp)
target_link_libraries(test_xmlutils GTest GTestMain mdict Miniz)
add_test(NAME test_xmlutils COMMAND test_xmlutils)