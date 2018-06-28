#!/bin/bash

DIR=`dirname "$0"`
ROOT_DIR=${DIR}/..

# Build header files for all Flatbuffers schemas
flatc --cpp --gen-all --scoped-enums -o ${ROOT_DIR}/platforms/linux/deps/protocol/ ${ROOT_DIR}/protocol/Packet.fbs
flatc --python --gen-all -o ${ROOT_DIR}/interfaces/python/bable_interface ${ROOT_DIR}/protocol/Packet.fbs
