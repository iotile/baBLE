#!/bin/bash

# Build header files for all Flatbuffers schemas
flatc --cpp --gen-all --scoped-enums -o ./schemas/generated/ ./schemas/Packet.fbs
flatc --python --gen-all -o ./ ./schemas/Packet.fbs
