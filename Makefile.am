ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = \
    -Wall \
    -O3 \
    -Wextra \
    -Wpedantic \
    -pipe \
    -fvisibility=hidden \
    -fvisibility-inlines-hidden \
    -flto \
    -ffat-lto-objects

AM_LDFLAGS = \
	-Wl,--as-needed \
	-Wl,--no-undefined \
	-Wl,--gc-sections \
	-Wl,-z,relro \
	-Wl,-z,now \
	-Wl,-fuse-ld=gold \
	-pie

AM_CXXFLAGS = $(AM_CFLAGS) -std=c++14

lib_LTLIBRARIES = libblake2.la
bin_PROGRAMS = blake2b

libblake2_la_SOURCES = \
    src/Blake2b.cpp \
    src/Blake2b.hpp \
    src/blake2b-capi.cpp \
    src/blake2b.h \
    src/SubrangeAdaptor.hpp

blake2b_SOURCES = \
    src/blake2b.c

blake2b_CFLAGS = -I/usr/include/libblake2
blake2b_LDFLAGS = -lblake2

pkginclude_HEADERS = src/blake2b.h

tests = \
    test-blake2b

check_PROGRAMS = $(tests)
TESTS = $(tests)

test_blake2b_SOURCES = src/test-blake2b.cpp
test_blake2b_LDADD = libblake2.la
test_blake2b_LDFLAGS = -static -lgtest
