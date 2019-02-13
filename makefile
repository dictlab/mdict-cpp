CFLAGS=-O3 -Wall -fPIC
CXXFLAGS=--std=c++11 -O3 -Wall
HEADER=-I/deps/miniz
LIBRARY=-Llib

mdict: bin mdict.cc mdict.h lib/libminiz.a
	g++ ${CXXFLAGS} ${HEADER} ${LIBRARY} -lminiz mdict.cc mdict_test.cc ripemd128.c binutils.cc adler32.cc xmlutils.cc  -o ./bin/mdict

libminiz: deps/miniz/miniz.c deps/miniz/miniz_zip.c deps/miniz/miniz_tinfl.c deps/miniz/miniz_tdef.c
	gcc  -O3 -Wall deps/miniz/miniz.c deps/miniz/miniz_zip.c deps/miniz/miniz_tinfl.c deps/miniz/miniz_tdef.c

lib/miniz.o: deps/miniz/miniz.c lib
	gcc ${CFLAGS} -c ${HEADER} deps/miniz/miniz.c -o lib/miniz.o

lib/miniz_zip.o: deps/miniz/miniz_zip.c
	gcc ${CFLAGS} -c ${HEADER} deps/miniz/miniz_zip.c -o lib/miniz_zip.o

lib/miniz_tinfl.o: deps/miniz/miniz_tinfl.c
	gcc ${CFLAGS} -c ${HEADER} deps/miniz/miniz_tinfl.c -o lib/miniz_tinfl.o

lib/miniz_tdef.o: deps/miniz/miniz_tdef.c
	gcc ${CFLAGS} -c ${HEADER} deps/miniz/miniz_tdef.c -o lib/miniz_tdef.o

lib/libminiz.a: lib/miniz.o lib/miniz_zip.o lib/miniz_tdef.o lib/miniz_tinfl.o
	ar rvs lib/libminiz.a lib/miniz.o lib/miniz_zip.o lib/miniz_tdef.o lib/miniz_tinfl.o


binutils: bin src/binutils.cc src/binutils.h src/binutils_test.cc
	g++ --std=c++11 binutils.cc binutils_test.cc -o bin/binutils && ./bin/binutils

libmdict: deps/miniz/miniz.c deps/miniz/miniz_zip.c deps/miniz/miniz_tinfl.c deps/miniz/miniz_tdef.c
	g++ --std=c++11 -Ideps/miniz/ deps/miniz/miniz.c deps/miniz/miniz_zip.c deps/miniz/miniz_tinfl.c deps/miniz/miniz_tdef.c

bin:
	mkdir -p bin

lib:
	mkdir -p lib

xml: tinyxml2.cpp tinyxml2.h xmlutils.cc
	g++ --std=c++11 -DJSMDICT_XML_TEST tinyxml2.cpp xmlutils.cpp  -o ./bin/xmltest && ./bin/xmltest

.PHONY: mdict binutils

