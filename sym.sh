#!/bin/bash

set -Eeuxo pipefail

PROGRAM=$1

echo "${PATH}"
pwd

/tmp/llvm-60-install_O_D_A/bin/clang  -emit-llvm -g -c "${PROGRAM}" -o "${PROGRAM}".bc
/home/klee/klee_build/bin/klee --optimize --libc=uclibc --posix-runtime "${PROGRAM}".bc
cd klee-last
cp ../"${PROGRAM}" .
gcc --coverage "${PROGRAM}" -o "${PROGRAM}".cov.o
/home/klee/klee_build/bin/klee-replay "${PROGRAM}".cov.o *.ktest
gcovr -r . --html --html-details -o gcovr.html
pwd