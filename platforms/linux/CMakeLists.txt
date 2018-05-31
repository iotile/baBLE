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
        src/Application/PacketUuid.hpp
        src/Application/PacketBuilder/PacketBuilder.cpp
        src/Application/PacketBuilder/PacketBuilder.hpp
        src/Application/Packets/Commands/RequestPacket.hpp
        src/Application/Packets/Commands/ResponsePacket.hpp
        src/Application/Packets/Commands/GetMGMTInfo/GetMGMTInfoRequest.cpp
        src/Application/Packets/Commands/GetMGMTInfo/GetMGMTInfoRequest.hpp
        src/Application/Packets/Commands/StartScan/StartScanRequest.cpp
        src/Application/Packets/Commands/StartScan/StartScanRequest.hpp
        src/Application/Packets/Commands/StopScan/StopScanRequest.cpp
        src/Application/Packets/Commands/StopScan/StopScanRequest.hpp
        src/Application/Packets/Commands/AddDevice/AddDeviceRequest.cpp
        src/Application/Packets/Commands/AddDevice/AddDeviceRequest.hpp
        src/Application/Packets/Commands/RemoveDevice/RemoveDeviceRequest.cpp
        src/Application/Packets/Commands/RemoveDevice/RemoveDeviceRequest.hpp
        src/Application/Packets/Commands/Disconnect/DisconnectRequest.cpp
        src/Application/Packets/Commands/Disconnect/DisconnectRequest.hpp
        src/Application/Packets/Commands/SetPowered/SetPoweredRequest.cpp
        src/Application/Packets/Commands/SetPowered/SetPoweredRequest.hpp
        src/Application/Packets/Commands/SetDiscoverable/SetDiscoverableRequest.cpp
        src/Application/Packets/Commands/SetDiscoverable/SetDiscoverableRequest.hpp
        src/Application/Packets/Commands/SetConnectable/SetConnectableRequest.cpp
        src/Application/Packets/Commands/SetConnectable/SetConnectableRequest.hpp
        src/Application/Packets/Commands/Read/ReadRequest.cpp
        src/Application/Packets/Commands/Read/ReadRequest.hpp
        src/Application/Packets/Commands/Write/WriteRequest.cpp
        src/Application/Packets/Commands/Write/WriteRequest.hpp
        src/Application/Packets/Commands/WriteWithoutResponse/WriteWithoutResponse.cpp
        src/Application/Packets/Commands/WriteWithoutResponse/WriteWithoutResponse.hpp
        src/Application/Packets/Commands/NotificationReceived/NotificationReceived.cpp
        src/Application/Packets/Commands/NotificationReceived/NotificationReceived.hpp
        src/Application/Packets/Commands/GetControllersIds/GetControllersIdsRequest.cpp
        src/Application/Packets/Commands/GetControllersIds/GetControllersIdsRequest.hpp
        src/Application/Packets/Commands/GetControllerInfo/GetControllerInfoRequest.cpp
        src/Application/Packets/Commands/GetControllerInfo/GetControllerInfoRequest.hpp
        src/Application/Packets/Commands/GetConnectedDevices/GetConnectedDevicesRequest.cpp
        src/Application/Packets/Commands/GetConnectedDevices/GetConnectedDevicesRequest.hpp
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
        src/Application/Packets/Events/DeviceAdded/DeviceAdded.cpp
        src/Application/Packets/Events/DeviceAdded/DeviceAdded.hpp
        src/Application/Packets/Events/DeviceRemoved/DeviceRemoved.cpp
        src/Application/Packets/Events/DeviceRemoved/DeviceRemoved.hpp
        src/Application/Packets/Events/LEAdvertisingReport/LEAdvertisingReport.cpp
        src/Application/Packets/Events/LEAdvertisingReport/LEAdvertisingReport.hpp
        src/Application/Packets/Events/LEReadRemoteUsedFeaturesComplete/LEReadRemoteUsedFeaturesComplete.cpp
        src/Application/Packets/Events/LEReadRemoteUsedFeaturesComplete/LEReadRemoteUsedFeaturesComplete.hpp
        src/Application/Packets/Errors/BaBLEError/BaBLEError.cpp
        src/Application/Packets/Errors/BaBLEError/BaBLEError.hpp
        src/Application/Packets/Meta/GetControllersList/GetControllersList.cpp
        src/Application/Packets/Meta/GetControllersList/GetControllersList.hpp
        src/Application/Packets/Meta/ProbeServices/ProbeServices.cpp
        src/Application/Packets/Meta/ProbeServices/ProbeServices.hpp
        src/Application/Packets/Meta/ProbeCharacteristics/ProbeCharacteristics.cpp
        src/Application/Packets/Meta/ProbeCharacteristics/ProbeCharacteristics.hpp
        src/Application/Packets/Control/Ready/Ready.cpp
        src/Application/Packets/Control/Ready/Ready.hpp
        src/Application/Packets/Control/Exit/Exit.cpp
        src/Application/Packets/Control/Exit/Exit.hpp
        src/Format/AbstractFormat.hpp
        src/Format/AbstractExtractor.hpp
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
        src/Format/HCI/constants.hpp
        src/Format/HCI/HCIFormat.hpp
        src/Format/HCI/HCIFormatBuilder.cpp
        src/Format/HCI/HCIFormatBuilder.hpp
        src/Format/HCI/HCIFormatExtractor.cpp
        src/Format/HCI/HCIFormatExtractor.hpp
        src/Transport/AbstractSocket.hpp
        src/Transport/SocketContainer/SocketContainer.cpp
        src/Transport/SocketContainer/SocketContainer.hpp
        src/Transport/Socket/MGMT/MGMTSocket.cpp
        src/Transport/Socket/MGMT/MGMTSocket.hpp
        src/Transport/Socket/StdIO/StdIOSocket.cpp
        src/Transport/Socket/StdIO/StdIOSocket.hpp
        src/Transport/Socket/HCI/HCISocket.cpp
        src/Transport/Socket/HCI/HCISocket.hpp
        src/Exceptions/constants.hpp
        src/Exceptions/AbstractException.hpp
        src/Exceptions/NotFound/NotFoundException.hpp
        src/Exceptions/WrongFormat/WrongFormatException.hpp
        src/Exceptions/Socket/SocketException.hpp
        src/Exceptions/InvalidCommand/InvalidCommandException.hpp
        src/Exceptions/RuntimeError/RuntimeErrorException.hpp
        src/bootstrap.hpp
        src/Application/PacketRouter/PacketRouter.cpp
        src/Application/PacketRouter/PacketRouter.hpp
        src/Application/Packets/Errors/ErrorResponse/ErrorResponse.cpp
        src/Application/Packets/Errors/ErrorResponse/ErrorResponse.hpp
        src/Application/Packets/Commands/ReadByType/ReadByTypeRequest.cpp
        src/Application/Packets/Commands/ReadByType/ReadByTypeRequest.hpp
        src/Application/Packets/Commands/ReadByType/ReadByTypeResponse.cpp
        src/Application/Packets/Commands/ReadByType/ReadByTypeResponse.hpp
        src/Application/Packets/Commands/ReadByGroupType/ReadByGroupTypeRequest.cpp
        src/Application/Packets/Commands/ReadByGroupType/ReadByGroupTypeRequest.hpp
        src/Application/Packets/Commands/ReadByGroupType/ReadByGroupTypeResponse.cpp
        src/Application/Packets/Commands/ReadByGroupType/ReadByGroupTypeResponse.hpp
        src/Application/Packets/Commands/AddDevice/AddDeviceResponse.cpp
        src/Application/Packets/Commands/AddDevice/AddDeviceResponse.hpp
        src/Application/Packets/Commands/Disconnect/DisconnectResponse.cpp
        src/Application/Packets/Commands/Disconnect/DisconnectResponse.hpp
        src/Application/Packets/Commands/GetConnectedDevices/GetConnectedDevicesResponse.cpp
        src/Application/Packets/Commands/GetConnectedDevices/GetConnectedDevicesResponse.hpp
        src/Application/Packets/Commands/GetControllerInfo/GetControllerInfoResponse.cpp
        src/Application/Packets/Commands/GetControllerInfo/GetControllerInfoResponse.hpp
        src/Application/Packets/Commands/GetControllersIds/GetControllersIdsResponse.cpp
        src/Application/Packets/Commands/GetControllersIds/GetControllersIdsResponse.hpp
        src/Application/Packets/Commands/GetMGMTInfo/GetMGMTInfoResponse.cpp
        src/Application/Packets/Commands/GetMGMTInfo/GetMGMTInfoResponse.hpp
        src/Application/Packets/Commands/Read/ReadResponse.cpp
        src/Application/Packets/Commands/Read/ReadResponse.hpp
        src/Application/Packets/Commands/RemoveDevice/RemoveDeviceResponse.cpp
        src/Application/Packets/Commands/RemoveDevice/RemoveDeviceResponse.hpp
        src/Application/Packets/Commands/StartScan/StartScanResponse.cpp
        src/Application/Packets/Commands/StartScan/StartScanResponse.hpp
        src/Application/Packets/Commands/StopScan/StopScanResponse.cpp
        src/Application/Packets/Commands/StopScan/StopScanResponse.hpp
        src/Application/Packets/Commands/SetConnectable/SetConnectableResponse.cpp
        src/Application/Packets/Commands/SetConnectable/SetConnectableResponse.hpp
        src/Application/Packets/Commands/SetDiscoverable/SetDiscoverableResponse.cpp
        src/Application/Packets/Commands/SetDiscoverable/SetDiscoverableResponse.hpp
        src/Application/Packets/Commands/SetPowered/SetPoweredResponse.cpp
        src/Application/Packets/Commands/SetPowered/SetPoweredResponse.hpp
        src/Application/Packets/Commands/Write/WriteResponse.cpp
        src/Application/Packets/Commands/Write/WriteResponse.hpp src/Application/AbstractPacket.cpp)
add_executable(baBLE_linux ${SOURCES_FILES})

#find_program(iwyu_path NAMES include-what-you-use iwyu)
#if(NOT iwyu_path)
#    message(FATAL_ERROR "Could not find the program include-what-you-use")
#endif()
#
#set_property(TARGET baBLE_linux PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})

## Include dependencies
# TODO: download dependencies using cmake ExternalProject -> git repo will be lighter but dependent on other repos...
### libuv
include_directories(deps/uv/libuv/include)
add_subdirectory(deps/uv)
target_link_libraries(baBLE_linux uv)
### uvw
add_subdirectory(deps/uvw)
target_link_libraries(baBLE_linux uvw)
### flatbuffers
add_subdirectory(deps/flatbuffers)
target_link_libraries(baBLE_linux flatbuffers)

## Include generated flatbuffers protocol
include_directories(deps/protocol)

## Install
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/baBLE_linux
        DESTINATION bin)
