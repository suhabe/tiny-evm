#!/bin/bash

set -Eeuxo pipefail

docker build -t tiny-evm-klee:latest .
docker run -t -i --user=klee \
                --ulimit='stack=-1:-1' \
                --mount type=bind,source=/Users/suhabebugrara/tiny-evm/,target=/home/klee/host \
                 tiny-evm-klee:latest \
                /bin/bash