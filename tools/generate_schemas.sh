#!/bin/bash

# Build header files for all Flatbuffers schemas
flatc --cpp --gen-all --scoped-enums -o ../platforms/linux/deps/protocole/ ../protocole/Packet.fbs
flatc --python --gen-all -o ../interfaces/python/ ../protocole/Packet.fbs
