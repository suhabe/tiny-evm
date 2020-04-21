#!/bin/bash

set -Eeuxo -pipefail

docker pull klee/klee:2.0
docker rm my_first_klee_container
docker run -ti --name=my_first_klee_container \
                --ulimit='stack=-1:-1' \
                --mount type=bind,source=/Users/suhabebugrara/tiny-evm/,target=/home/klee/host \
                 klee/klee:2.0 || true
docker start -ai my_first_klee_container