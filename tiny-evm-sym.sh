#!/bin/bash

set -Eeuxo pipefail

PROGRAM=tiny-evm.c
NUM_ACCOUNTS=2

echo "${PATH}"
pwd

/tmp/llvm-60-install_O_D_A/bin/clang  -emit-llvm -g -c "${PROGRAM}" -o "${PROGRAM}".bc
/home/klee/klee_build/bin/klee -write-sym-paths -readable-posix-inputs -write-cov \
                              -write-test-info -only-output-states-covering-new --optimize --libc=uclibc \
                              --posix-runtime "${PROGRAM}".bc ${NUM_ACCOUNTS}
cd klee-last
cp ../"${PROGRAM}" .
gcc --coverage "${PROGRAM}" -o "${PROGRAM}".cov.o

EXT=ktest
for KTEST_FILE in *."${EXT}"; do
    echo "Running ${KTEST_FILE}"
    echo
    ARGS=$(/home/klee/klee_build/bin/ktest-tool "${KTEST_FILE}" |  grep -v "object  0:" | grep ": int :" | awk '{print $5}' | tr '\n' ' ')
    ./"${PROGRAM}".cov.o $NUM_ACCOUNTS $ARGS
done
gcovr -r . --html --html-details -o gcovr.html
#pwd