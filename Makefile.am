ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = \
    -Wall \
    -O3 \
    -Wextra \
    -Wpedantic \
    -pipe \
    -fvisibility=hidden \
    -fvisibility-inlines-hidden

AM_CXXFLAGS = $(AM_CFLAGS) -std=c++14

lib_LTLIBRARIES = libblake2.la

libblake2_la_SOURCES = \
    src/Blake2b.cpp \
    src/Blake2b.hpp \
    src/blake2b-capi.cpp \
    src/blake2b.h \
    src/VectorRange.hpp

pkginclude_HEADERS = src/blake2b.h
