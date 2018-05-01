cmake_minimum_required(VERSION 3.10)
project(baBLE_linux)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-funsigned-char")

## Build executable from sources files
set(SOURCES_FILES
        src/main.cpp
        src/utils/colors.hpp
        src/utils/stream_formats.hpp
        src/Log/Log.cpp
        src/Log/Log.hpp
        src/Log/Loggable.hpp
        src/Packet/constants.hpp
        src/Packet/AbstractPacket.hpp
        src/Packet/Commands/GetMGMTInfo/GetMGMTInfo.cpp
        src/Packet/Commands/GetMGMTInfo/GetMGMTInfo.hpp
#        src/Packet/Commands/CommandPacket.hpp
#        src/Packet/Commands/Scan/StartScan.cpp
#        src/Packet/Commands/Scan/StartScan.hpp
#        src/Packet/Events/EventPacket.hpp
#        src/Packet/Events/DeviceFound/DeviceFound.cpp
#        src/Packet/Events/DeviceFound/DeviceFound.hpp
#        src/Packet/Events/Discovering/Discovering.cpp
#        src/Packet/Events/Discovering/Discovering.hpp
#        src/Packet/Commands/Scan/StopScan.cpp
#        src/Packet/Commands/Scan/StopScan.hpp
        src/Format/MGMT/MGMTFormat.cpp
        src/Format/MGMT/MGMTFormat.hpp
        src/Format/MGMT/MGMTFormatBuilder.cpp
        src/Format/MGMT/MGMTFormatBuilder.hpp
        src/Format/MGMT/MGMTFormatExtractor.cpp
        src/Format/MGMT/MGMTFormatExtractor.hpp
        src/Format/Flatbuffers/FlatbuffersFormat.cpp
        src/Format/Flatbuffers/FlatbuffersFormat.hpp
        src/Format/Ascii/AsciiFormat.cpp
        src/Format/Ascii/AsciiFormat.hpp
        src/Format/Ascii/AsciiFormatBuilder.cpp
        src/Format/Ascii/AsciiFormatBuilder.hpp
        src/Format/Ascii/AsciiFormatExtractor.cpp
        src/Format/Ascii/AsciiFormatExtractor.hpp
        src/Builder/PacketBuilder.cpp
        src/Builder/PacketBuilder.hpp
        src/SocketContainer/SocketContainer.cpp
        src/SocketContainer/SocketContainer.hpp
        src/Transports/AbstractSocket.hpp
        src/Transports/MGMT/MGMTSocket.cpp
        src/Transports/MGMT/MGMTSocket.hpp
        src/Transports/StdIO/StdIOSocket.cpp
        src/Transports/StdIO/StdIOSocket.hpp
        src/Format/AbstractFormat.hpp
        src/Packet/Commands/CommandPacket.hpp
        src/Packet/Events/EventPacket.hpp)
add_executable(baBLE_linux ${SOURCES_FILES})

## Include dependencies
### libuv
add_subdirectory(deps/uv)
target_link_libraries(baBLE_linux uv)
### uvw
add_subdirectory(deps/uvw)
target_link_libraries(baBLE_linux uvw)
### flatbuffers
add_subdirectory(deps/flatbuffers)
target_link_libraries(baBLE_linux flatbuffers)

## Include generated flatbuffers schemas
include_directories(schemas/generated)

## Install
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/baBLE_linux
        DESTINATION bin)
