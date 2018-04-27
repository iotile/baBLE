cmake_minimum_required(VERSION 3.10)
project(baBLE_linux)

set(CMAKE_CXX_STANDARD 14)

## Build executable from sources files
set(SOURCES_FILES
        src/main.cpp
        src/utils/colors.hpp
        src/utils/stream_formats.hpp
        src/Log/Log.cpp
        src/Log/Log.hpp
        src/Log/Loggable.hpp
        src/Serializer/AbstractSerializer.cpp
        src/Serializer/AbstractSerializer.hpp
        src/Serializer/Serializer.cpp
        src/Serializer/Serializer.hpp
        src/Serializer/Deserializer.cpp
        src/Serializer/Deserializer.hpp
        src/Socket/AbstractSocket.hpp
        src/Socket/SocketManager.hpp
        src/Socket/MGMT/MGMTSocket.cpp
        src/Socket/MGMT/MGMTSocket.hpp
        src/Socket/HCI/HCISocket.cpp
        src/Socket/HCI/HCISocket.hpp
        src/Socket/StdIO/StdIOSocket.cpp
        src/Socket/StdIO/StdIOSocket.hpp
        src/Packet/constants.hpp
        src/Packet/AbstractPacket.hpp
        src/Packet/Commands/GetMGMTInfo/GetMGMTInfo.cpp
        src/Packet/Commands/GetMGMTInfo/GetMGMTInfo.hpp
        src/Packet/Commands/CommandPacket.hpp
        src/Packet/Responses/ResponsePacket.hpp
        src/Packet/Responses/GetMGMTInfo/GetMGMTInfo.cpp
        src/Packet/Responses/GetMGMTInfo/GetMGMTInfo.hpp
        src/Builder/AbstractBuilder.hpp
        src/Builder/MGMT/MGMTBuilder.cpp
        src/Builder/MGMT/MGMTBuilder.hpp
        src/Builder/Ascii/AsciiBuilder.cpp
        src/Builder/Ascii/AsciiBuilder.hpp
        src/Poller/Poller.cpp
        src/Poller/Poller.hpp
        src/Poller/PipePoller.cpp
        src/Poller/PipePoller.hpp
        src/Packet/Commands/Scan/StartScan.cpp
        src/Packet/Commands/Scan/StartScan.hpp
        src/Packet/Responses/Scan/StartScan.cpp
        src/Packet/Responses/Scan/StartScan.hpp
        src/Packet/Events/EventPacket.hpp
        src/Packet/Events/DeviceFound/DeviceFound.cpp
        src/Packet/Events/DeviceFound/DeviceFound.hpp
        src/Packet/Events/Discovering/Discovering.cpp
        src/Packet/Events/Discovering/Discovering.hpp
        src/Packet/Commands/Scan/StopScan.cpp
        src/Packet/Commands/Scan/StopScan.hpp
        src/Packet/Responses/Scan/StopScan.cpp
        src/Packet/Responses/Scan/StopScan.hpp)
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
