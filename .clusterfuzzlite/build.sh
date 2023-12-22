#!/bin/bash -eu
# Copy all fuzzer executables to $OUT/
$CXX $CFLAGS $LIB_FUZZING_ENGINE -std=c++17 \
  $SRC/neujson/.clusterfuzzlite/doc_fuzzer.cpp \
  -o $OUT/doc_fuzzer \
  -I$SRC/neujson/include
