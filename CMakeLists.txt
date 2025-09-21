# Set the minimum required CMake version
CMAKE_MINIMUM_REQUIRED(VERSION 3.10) # for windows XP compatibility
# Set the project name
PROJECT(mdict-cpp VERSION 1.0.0)

# Suppress developer warnings
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS TRUE)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 17) # for windows XP compatibility
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Set build type and compilation flags
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb --std=c++17 ")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall --std=c++17 ")

# Set output directories
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

ADD_COMPILE_OPTIONS(-Wno-unused-variable -fpermissive -Wno-sign-compare)

# Add cmake modules path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

INCLUDE(cmake/ProjectMiniz.cmake)
INCLUDE(cmake/ProjectMinilzo.cmake)
INCLUDE(cmake/ProjectGTest.cmake)
INCLUDE(cmake/ProjectTurbobase64.cmake)

# Add include directories
INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/b64/include
)


# Add library search directories
LINK_DIRECTORIES(/usr/local/lib ${CMAKE_CURRENT_BINARY_DIR}/lib)

# Add subdirectories
ADD_SUBDIRECTORY(tests)

# Library target: mdict
ADD_LIBRARY(mdict STATIC src/mdict.cc src/binutils.cc src/ripemd128.c src/adler32.cc src/mdict_extern.cc)
TARGET_LINK_LIBRARIES(mdict PRIVATE mdictminiz mdictbase64)

# Executable target: mydict (for development/testing purposes only)
ADD_EXECUTABLE(mydict src/mydict.cc)
TARGET_LINK_LIBRARIES(mydict PRIVATE mdict mdictminiz mdictbase64)

# Define installation behavior for the mdict library
OPTION(INSTALL_TO_SYSTEM "Install the mdict library to the system" OFF)

if(INSTALL_TO_SYSTEM)
    # Prevent subdirectories from installing their targets
    set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)
    # Detect operating system and set library installation paths accordingly
    if(UNIX AND NOT APPLE)
        # For Linux
        set(LIB_INSTALL_DIR "/usr/lib")
        set(INCLUDE_INSTALL_DIR "/usr/include")
        set(BIN_INSTALL_DIR "/usr/bin")
    elseif(APPLE)
        # For macOS
        set(LIB_INSTALL_DIR "/usr/local/lib")
        set(INCLUDE_INSTALL_DIR "/usr/local/include")
        set(BIN_INSTALL_DIR "/usr/local/bin")
    elseif(${CMAKE_SYSTEM_NAME} MATCHES ".*BSD")
        # For BSD systems
        set(LIB_INSTALL_DIR "/usr/local/lib")
        set(INCLUDE_INSTALL_DIR "/usr/local/include")
        set(BIN_INSTALL_DIR "/usr/local/bin")
    else()
        message(FATAL_ERROR "Unsupported operating system for installation")
    endif()

    # Install the mdict library
    install(TARGETS mdict
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
        COMPONENT mdict
    )

    install(FILES ${LIBRARY_OUTPUT_PATH}/libmdictminiz${CMAKE_STATIC_LIBRARY_SUFFIX} DESTINATION ${LIB_INSTALL_DIR})
    install(FILES ${LIBRARY_OUTPUT_PATH}/libmdictbase64${CMAKE_STATIC_LIBRARY_SUFFIX} DESTINATION ${LIB_INSTALL_DIR})

    # Install headers
    install(FILES ${CMAKE_SOURCE_DIR}/src/include/mdict.h DESTINATION include/mdict)
    install(FILES ${CMAKE_SOURCE_DIR}/src/include/mdict_extern.h DESTINATION include/mdict)



    # Install mydict executable
    install(TARGETS mydict
        RUNTIME DESTINATION bin
        COMPONENT mydict
    )
endif()


