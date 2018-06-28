#!/usr/bin/env bash


git clone --branch v1.9.0 https://github.com/google/flatbuffers.git
cd flatbuffers
cmake -G "Unix Makefiles"
make -j `nproc`
