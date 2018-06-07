#!/bin/bash

# Build header files for all Flatbuffers schemas
flatc --cpp --gen-all --scoped-enums -o ../platforms/linux/deps/protocol/ ../protocol/Packet.fbs
flatc --python --gen-all -o ../interfaces/python/bable_interface ../protocol/Packet.fbs
