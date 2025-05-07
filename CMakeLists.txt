# Set the project name
CMAKE_POLICY(SET CMP0048 NEW)
PROJECT(MDictCpp VERSION 1.0.0)

# Set the minimum required CMake version
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Set build type and compilation flags
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb --std=c++17 ")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall --std=c++17 ")

# Set output directories
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

add_compile_options(-fpermissive)

# Add include directories
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/deps/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/turbobase64)

# Add library search directories
LINK_DIRECTORIES(/usr/local/lib ${CMAKE_CURRENT_BINARY_DIR}/lib)

# Add subdirectories
ADD_SUBDIRECTORY(deps)

# Library target: mdict
ADD_LIBRARY(mdict STATIC mdict.cc binutils.cc xmlutils.cc ripemd128.c adler32.cc mdict_extern.cc)
target_link_libraries(mdict PRIVATE miniz base64)

# Executable target: mydict (for development/testing purposes only)
ADD_EXECUTABLE(mydict mdict.cc binutils.cc xmlutils.cc ripemd128.c adler32.cc mdict_extern.cc mdict_extern_test.cc)
target_link_libraries(mydict PRIVATE miniz base64)

# Test executables
ADD_EXECUTABLE(adler32_test adler32.cc adler32_test.cc binutils.cc)
ADD_EXECUTABLE(ripemd128_test ripemd128_test.c ripemd128.c)
ADD_EXECUTABLE(xmlutils_test xmlutils.cc xmlutils_test.cc)
ADD_EXECUTABLE(binutils_test binutils.cc binutils_test.cc)

# Define installation behavior for the mdict library
OPTION(INSTALL_TO_SYSTEM "Install the mdict library to the system" OFF)

if(INSTALL_TO_SYSTEM)
    # Detect operating system and set library installation paths accordingly
    if(UNIX AND NOT APPLE)
        # For Linux
        set(LIB_INSTALL_DIR "/usr/lib")
        set(INCLUDE_INSTALL_DIR "/usr/include")
    elseif(APPLE)
        # For macOS
        set(LIB_INSTALL_DIR "/usr/local/lib")
        set(INCLUDE_INSTALL_DIR "/usr/local/include")
    elseif(${CMAKE_SYSTEM_NAME} MATCHES ".*BSD")
        # For BSD systems
        set(LIB_INSTALL_DIR "/usr/local/lib")
        set(INCLUDE_INSTALL_DIR "/usr/local/include")
    else()
        message(FATAL_ERROR "Unsupported operating system for installation")
    endif()

    # Install the mdict library
    install(TARGETS mdict
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    )
    # Install headers
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include/mdict-cpp
        FILES_MATCHING PATTERN "*.h")
    
endif()
