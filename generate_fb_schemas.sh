#!/bin/bash

# Build header files for all Flatbuffers schemas
flatc --cpp --gen-all -o ./schemas/generated/ ./schemas/Packet.fbs