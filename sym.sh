#!/bin/bash

set -Eeuxo pipefail

if [ "$#" -ne 1 ]; then
  PROGRAM=tiny-evm.c
else
  PROGRAM=$1
fi

clang -emit-llvm -g -c "${PROGRAM}" -o "${PROGRAM}".bc
klee --libc=uclibc --posix-runtime "${PROGRAM}".bc