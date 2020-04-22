FROM klee/klee:2.0

RUN sudo -H pip install gcovr
WORKDIR /home/klee/host