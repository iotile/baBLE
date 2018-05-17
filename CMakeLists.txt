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
        src/Application/Packets/constants.hpp
        src/Application/AbstractPacket.hpp
        src/Application/Packets/Commands/CommandPacket.hpp
        src/Application/Packets/Commands/GetMGMTInfo/GetMGMTInfo.cpp
        src/Application/Packets/Commands/GetMGMTInfo/GetMGMTInfo.hpp
        src/Application/Packets/Commands/Scan/StartScan.cpp
        src/Application/Packets/Commands/Scan/StartScan.hpp
        src/Application/Packets/Commands/Scan/StopScan.cpp
        src/Application/Packets/Commands/Scan/StopScan.hpp
        src/Application/Packets/Commands/AddDevice/AddDevice.cpp
        src/Application/Packets/Commands/AddDevice/AddDevice.hpp
        src/Application/Packets/Commands/RemoveDevice/RemoveDevice.cpp
        src/Application/Packets/Commands/RemoveDevice/RemoveDevice.hpp
        src/Application/Packets/Commands/Disconnect/Disconnect.cpp
        src/Application/Packets/Commands/Disconnect/Disconnect.hpp
        src/Application/Packets/Commands/SetPowered/SetPowered.cpp
        src/Application/Packets/Commands/SetPowered/SetPowered.hpp
        src/Application/Packets/Commands/SetDiscoverable/SetDiscoverable.cpp
        src/Application/Packets/Commands/SetDiscoverable/SetDiscoverable.hpp
        src/Application/Packets/Commands/SetConnectable/SetConnectable.cpp
        src/Application/Packets/Commands/SetConnectable/SetConnectable.hpp
        src/Application/Packets/Meta/GetControllersList/GetControllersList.cpp
        src/Application/Packets/Meta/GetControllersList/GetControllersList.hpp
        src/Application/Packets/Commands/GetControllersIds/GetControllersIds.cpp
        src/Application/Packets/Commands/GetControllersIds/GetControllersIds.hpp
        src/Application/Packets/Commands/GetControllerInfo/GetControllerInfo.cpp
        src/Application/Packets/Commands/GetControllerInfo/GetControllerInfo.hpp
        src/Application/Packets/Commands/GetConnectedDevices/GetConnectedDevices.cpp
        src/Application/Packets/Commands/GetConnectedDevices/GetConnectedDevices.hpp
        src/Application/Packets/Events/EventPacket.hpp
        src/Application/Packets/Events/DeviceFound/DeviceFound.cpp
        src/Application/Packets/Events/DeviceFound/DeviceFound.hpp
        src/Application/Packets/Events/Discovering/Discovering.cpp
        src/Application/Packets/Events/Discovering/Discovering.hpp
        src/Application/Packets/Events/DeviceConnected/DeviceConnected.cpp
        src/Application/Packets/Events/DeviceConnected/DeviceConnected.hpp
        src/Application/Packets/Events/DeviceDisconnected/DeviceDisconnected.cpp
        src/Application/Packets/Events/DeviceDisconnected/DeviceDisconnected.hpp
        src/Application/Packets/Events/ClassOfDeviceChanged/ClassOfDeviceChanged.cpp
        src/Application/Packets/Events/ClassOfDeviceChanged/ClassOfDeviceChanged.hpp
        src/Application/Packets/Events/NewSettings/NewSettings.cpp
        src/Application/Packets/Events/NewSettings/NewSettings.hpp
        src/Application/Packets/Events/ControllerAdded/ControllerAdded.cpp
        src/Application/Packets/Events/ControllerAdded/ControllerAdded.hpp
        src/Application/Packets/Events/ControllerRemoved/ControllerRemoved.cpp
        src/Application/Packets/Events/ControllerRemoved/ControllerRemoved.hpp
        src/Application/Packets/Errors/BaBLEError/BaBLEErrorPacket.cpp
        src/Application/Packets/Errors/BaBLEError/BaBLEErrorPacket.hpp
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
        src/Application/PacketContainer/PacketContainer.cpp
        src/Application/PacketContainer/PacketContainer.hpp
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
        src/bootstrap.hpp
        src/Format/HCI/constants.hpp
        src/Format/HCI/HCIFormat.hpp
        src/Format/HCI/HCIFormatExtractor.cpp
        src/Format/HCI/HCIFormatExtractor.hpp
        src/Transport/Socket/HCI/HCISocket.cpp
        src/Transport/Socket/HCI/HCISocket.hpp
        src/Format/HCI/HCIFormatBuilder.cpp
        src/Format/HCI/HCIFormatBuilder.hpp
        src/Application/Packets/Commands/Read/Read.cpp
        src/Application/Packets/Commands/Read/Read.hpp
        )
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
