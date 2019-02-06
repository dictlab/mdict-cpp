mdict: bin mdict.cc mdict.h src/binutils.cc src/binutils.h xmlutils.h xmlutils.cc
	g++ --std=c++11 mdict.cc binutils.cc xmlutils.cpp -o ./bin/mdict && ./bin/mdict

binutils: bin src/binutils.cc src/binutils.h src/binutils_test.cc
	g++ --std=c++11 binutils.cc binutils_test.cc -o bin/binutils && ./bin/binutils

bin:
	mkdir -p bin

xml: tinyxml2.cpp tinyxml2.h xmlutils.cc
	g++ --std=c++11 -DJSMDICT_XML_TEST tinyxml2.cpp xmlutils.cpp  -o ./bin/xmltest && ./bin/xmltest

.PHONY: mdict binutils

