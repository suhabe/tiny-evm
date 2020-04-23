#!/bin/bash

set -Eeuxo pipefail

PROGRAM=tiny-evm.c

docker build -t tiny-evm-klee:latest .
docker run -t -i --user=klee \
                --ulimit='stack=-1:-1' \
                --mount type=bind,source=/Users/suhabebugrara/tiny-evm/,target=/home/klee/host \
                 tiny-evm-klee:latest \
                /home/klee/host/tiny-evm-sym.sh "${PROGRAM}"
rm -rf *.temps