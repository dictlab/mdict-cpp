#设置工程名称
PROJECT(libmdict)
#自己定制
#SET(MY_TARGET demo)
#SET(MY_LINK test1 test2)


#设置CMAKE最小版本
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
#设置编译器
SET(CMAKE_CXX_COMPILER "g++")
#设置构建类型，及相应的编译选项
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb --std=c++11")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall --std=c++11")

#设置执行文件输出目录
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#设置库输出路径
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

MESSAGE("++++++++++++++Start Build+++++++++++++++++")

#当前目录的文件
#添加子目录
#ADD_SUBDIRECTORY(src)

#头文件搜索目录
#INCLUDE_DIRECTORIES(inc)
#链接库搜索路径
#LINK_DIRECTORIES(lib)
#设置链接库
#SET(EXTRA_LIBS ${EXTRA_LIBS} test1 test2)
#SET(EXTRA_LIBS ${EXTRA_LIBS} ${MY_LINK})



# mdict


#执行文件链接属性
ADD_EXECUTABLE(mdict mdict.cc binutils.cc xmlutils.cpp)
# TARGET_LINK_LIBRARIES(${MY_TARGET} ${EXTRA_LIBS})
