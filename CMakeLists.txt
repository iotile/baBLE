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
        src/Packet/Commands/CommandPacket.hpp
        src/Packet/Commands/GetMGMTInfo/GetMGMTInfo.cpp
        src/Packet/Commands/GetMGMTInfo/GetMGMTInfo.hpp
        src/Packet/Commands/Scan/StartScan.cpp
        src/Packet/Commands/Scan/StartScan.hpp
        src/Packet/Commands/Scan/StopScan.cpp
        src/Packet/Commands/Scan/StopScan.hpp
        src/Packet/Commands/AddDevice/AddDevice.cpp
        src/Packet/Commands/AddDevice/AddDevice.hpp
        src/Packet/Commands/RemoveDevice/RemoveDevice.cpp
        src/Packet/Commands/RemoveDevice/RemoveDevice.hpp
        src/Packet/Commands/Disconnect/Disconnect.cpp
        src/Packet/Commands/Disconnect/Disconnect.hpp
        src/Packet/Commands/SetPowered/SetPowered.cpp
        src/Packet/Commands/SetPowered/SetPowered.hpp
        src/Packet/Commands/SetDiscoverable/SetDiscoverable.cpp
        src/Packet/Commands/SetDiscoverable/SetDiscoverable.hpp
        src/Packet/Commands/SetConnectable/SetConnectable.cpp
        src/Packet/Commands/SetConnectable/SetConnectable.hpp
        src/Packet/Commands/GetControllersList/GetControllersList.cpp
        src/Packet/Commands/GetControllersList/GetControllersList.hpp
        src/Packet/Commands/GetControllerInfo/GetControllerInfo.cpp
        src/Packet/Commands/GetControllerInfo/GetControllerInfo.hpp
        src/Packet/Commands/GetConnectedDevices/GetConnectedDevices.cpp
        src/Packet/Commands/GetConnectedDevices/GetConnectedDevices.hpp
        src/Packet/Events/EventPacket.hpp
        src/Packet/Events/DeviceFound/DeviceFound.cpp
        src/Packet/Events/DeviceFound/DeviceFound.hpp
        src/Packet/Events/Discovering/Discovering.cpp
        src/Packet/Events/Discovering/Discovering.hpp
        src/Packet/Events/DeviceConnected/DeviceConnected.cpp
        src/Packet/Events/DeviceConnected/DeviceConnected.hpp
        src/Packet/Events/DeviceDisconnected/DeviceDisconnected.cpp
        src/Packet/Events/DeviceDisconnected/DeviceDisconnected.hpp
        src/Packet/Events/ClassOfDeviceChanged/ClassOfDeviceChanged.cpp
        src/Packet/Events/ClassOfDeviceChanged/ClassOfDeviceChanged.hpp
        src/Packet/Events/NewSettings/NewSettings.cpp
        src/Packet/Events/NewSettings/NewSettings.hpp
        src/Packet/Events/ControllerAdded/ControllerAdded.cpp
        src/Packet/Events/ControllerAdded/ControllerAdded.hpp
        src/Packet/Events/ControllerRemoved/ControllerRemoved.cpp
        src/Packet/Events/ControllerRemoved/ControllerRemoved.hpp
        src/Packet/BaBLEError/BaBLEErrorPacket.cpp
        src/Packet/BaBLEError/BaBLEErrorPacket.hpp
        src/Format/AbstractFormat.hpp
        src/Format/MGMT/constants.hpp
        src/Format/MGMT/MGMTFormat.hpp
        src/Format/MGMT/MGMTFormatBuilder.cpp
        src/Format/MGMT/MGMTFormatBuilder.hpp
        src/Format/MGMT/MGMTFormatExtractor.cpp
        src/Format/MGMT/MGMTFormatExtractor.hpp
        src/Format/Flatbuffers/FlatbuffersFormat.hpp
        src/Format/Flatbuffers/FlatbuffersFormatBuilder.cpp
        src/Format/Flatbuffers/FlatbuffersFormatBuilder.hpp
        src/Format/Flatbuffers/FlatbuffersFormatExtractor.cpp
        src/Format/Flatbuffers/FlatbuffersFormatExtractor.hpp
        src/Format/Ascii/constants.hpp
        src/Format/Ascii/AsciiFormat.hpp
        src/Format/Ascii/AsciiFormatBuilder.cpp
        src/Format/Ascii/AsciiFormatBuilder.hpp
        src/Format/Ascii/AsciiFormatExtractor.cpp
        src/Format/Ascii/AsciiFormatExtractor.hpp
        src/Builder/PacketBuilder.cpp
        src/Builder/PacketBuilder.hpp
        src/Transport/AbstractSocket.hpp
        src/Transport/SocketContainer/SocketContainer.cpp
        src/Transport/SocketContainer/SocketContainer.hpp
        src/Transport/Socket/MGMT/MGMTSocket.cpp
        src/Transport/Socket/MGMT/MGMTSocket.hpp
        src/Transport/Socket/StdIO/StdIOSocket.cpp
        src/Transport/Socket/StdIO/StdIOSocket.hpp
        src/Exceptions/constants.hpp
        src/Exceptions/AbstractException.hpp
        src/Exceptions/NotFound/NotFoundException.hpp
        src/Exceptions/WrongFormat/WrongFormatException.hpp
        src/Exceptions/Socket/SocketException.hpp
        src/Exceptions/InvalidCommand/InvalidCommandException.hpp
        src/bootstrap.hpp)
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
