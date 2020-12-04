CFLAGS=-O3 -Wall -fPIC
CXXFLAGS=--std=c++11 -O3 -Wall
HEADER=-I/deps/miniz
LIBRARY=-Llib

libmdict.a: lib lib/libminiz.a lib/mdict.o lib/ripemd128.o lib/binutils.o lib/adler32.o lib/xmlutils.o
	ar rvs lib/libmdict.a lib/miniz.o lib/miniz_zip.o lib/miniz_tdef.o lib/miniz_tinfl.o lib/mdict.o lib/ripemd128.o lib/binutils.o lib/adler32.o lib/xmlutils.o

libmdict.so: lib lib/libminiz.a lib/mdict.o lib/ripemd128.o lib/binutils.o lib/adler32.o lib/xmlutils.o
	g++ -shared lib/miniz.o lib/miniz_zip.o lib/miniz_tdef.o lib/miniz_tinfl.o lib/mdict.o lib/ripemd128.o lib/binutils.o lib/adler32.o lib/xmlutils.o -o lib/libmdict.so

libmdict.dylib: lib lib/libminiz.a lib/mdict.o lib/ripemd128.o lib/binutils.o lib/adler32.o lib/xmlutils.o
	g++ -shared lib/miniz.o lib/miniz_zip.o lib/miniz_tdef.o lib/miniz_tinfl.o lib/mdict.o lib/ripemd128.o lib/binutils.o lib/adler32.o lib/xmlutils.o -o lib/libmdict.dylib

lib/mdict.o: lib mdict.cc
	g++ ${CXXFLAGS} -c ${HEADER} mdict.cc -o lib/mdict.o

lib/ripemd128.o: lib ripemd128.c
	gcc ${CFLAGS} -c ${HEADER} ripemd128.c -o lib/ripemd128.o

lib/binutils.o: lib binutils.cc
	g++ ${CXXFLAGS} -c ${HEADER} binutils.cc -o lib/binutils.o

lib/adler32.o: lib adler32.cc
	g++ ${CXXFLAGS} -c ${HEADER} adler32.cc -o lib/adler32.o

lib/xmlutils.o: lib xmlutils.cc
	g++ ${CXXFLAGS} -c ${HEADER} xmlutils.cc -o lib/xmlutils.o


mdict: bin mdict.cc mdict.h lib/libminiz.a
	g++ ${CXXFLAGS} ${HEADER} ${LIBRARY} -lminiz mdict.cc mdict_test.cc ripemd128.c binutils.cc adler32.cc xmlutils.cc  -o ./bin/mdict

mdict_test: bin libmdict.a mdict.h mdict_test.cc
	g++ ${CXXFLAGS} ${HEADER} ${LIBRARY} -lmdict mdict_test.cc -o bin/mdict_test

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
	gcc ${CFLAGS} -c ${HEADER} -Ideps/miniz/ deps/miniz/miniz.c deps/miniz/miniz_zip.c deps/miniz/miniz_tinfl.c deps/miniz/miniz_tdef.c

bin:
	mkdir -p bin

lib:
	mkdir -p lib

xml: tinyxml2.cpp tinyxml2.h xmlutils.cc
	g++ --std=c++11 -DJSMDICT_XML_TEST tinyxml2.cpp xmlutils.cpp  -o ./bin/xmltest && ./bin/xmltest

.PHONY: mdict binutils

