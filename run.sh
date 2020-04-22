#!/bin/bash

set -Eeuxo pipefail

if [ "$#" -ne 1 ]; then
  PROGRAM=tiny-evm.c
else
  PROGRAM=$1
fi

docker build -t tiny-evm-klee:latest .
docker run -t -i --user=klee \
                --ulimit='stack=-1:-1' \
                --mount type=bind,source=/Users/suhabebugrara/tiny-evm/,target=/home/klee/host \
                 tiny-evm-klee:latest \
                /home/klee/host/sym.sh "${PROGRAM}"
rm -rf *.temps