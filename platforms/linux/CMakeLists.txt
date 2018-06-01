cmake_minimum_required(VERSION 3.10)
project(baBLE_linux)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-funsigned-char")

## Build executable from sources files
set(SOURCES_FILES
        src/main.cpp
        src/Log/Log.cpp
        src/Application/PacketBuilder/PacketBuilder.cpp
        src/Application/Packets/Commands/GetMGMTInfo/GetMGMTInfoRequest.cpp
        src/Application/Packets/Commands/StartScan/StartScanRequest.cpp
        src/Application/Packets/Commands/StopScan/StopScanRequest.cpp
        src/Application/Packets/Commands/AddDevice/AddDeviceRequest.cpp
        src/Application/Packets/Commands/RemoveDevice/RemoveDeviceRequest.cpp
        src/Application/Packets/Commands/Disconnect/DisconnectRequest.cpp
        src/Application/Packets/Commands/SetPowered/SetPoweredRequest.cpp
        src/Application/Packets/Commands/SetDiscoverable/SetDiscoverableRequest.cpp
        src/Application/Packets/Commands/SetConnectable/SetConnectableRequest.cpp
        src/Application/Packets/Commands/Read/ReadRequest.cpp
        src/Application/Packets/Commands/Write/WriteRequest.cpp
        src/Application/Packets/Commands/WriteWithoutResponse/WriteWithoutResponse.cpp
        src/Application/Packets/Commands/NotificationReceived/NotificationReceived.cpp
        src/Application/Packets/Commands/GetControllersIds/GetControllersIdsRequest.cpp
        src/Application/Packets/Commands/GetControllerInfo/GetControllerInfoRequest.cpp
        src/Application/Packets/Commands/GetConnectedDevices/GetConnectedDevicesRequest.cpp
        src/Application/Packets/Events/DeviceFound/DeviceFound.cpp
        src/Application/Packets/Events/Discovering/Discovering.cpp
        src/Application/Packets/Events/DeviceConnected/DeviceConnected.cpp
        src/Application/Packets/Events/DeviceDisconnected/DeviceDisconnected.cpp
        src/Application/Packets/Events/ClassOfDeviceChanged/ClassOfDeviceChanged.cpp
        src/Application/Packets/Events/NewSettings/NewSettings.cpp
        src/Application/Packets/Events/ControllerAdded/ControllerAdded.cpp
        src/Application/Packets/Events/ControllerRemoved/ControllerRemoved.cpp
        src/Application/Packets/Events/DeviceAdded/DeviceAdded.cpp
        src/Application/Packets/Events/DeviceRemoved/DeviceRemoved.cpp
        src/Application/Packets/Events/LEAdvertisingReport/LEAdvertisingReport.cpp
        src/Application/Packets/Events/LEReadRemoteUsedFeaturesComplete/LEReadRemoteUsedFeaturesComplete.cpp
        src/Application/Packets/Errors/BaBLEError/BaBLEError.cpp
        src/Application/Packets/Meta/GetControllersList/GetControllersList.cpp
        src/Application/Packets/Meta/ProbeServices/ProbeServices.cpp
        src/Application/Packets/Meta/ProbeCharacteristics/ProbeCharacteristics.cpp
        src/Application/Packets/Control/Ready/Ready.cpp
        src/Application/Packets/Control/Exit/Exit.cpp
        src/Format/MGMT/MGMTFormatBuilder.cpp
        src/Format/MGMT/MGMTFormatExtractor.cpp
        src/Format/Flatbuffers/FlatbuffersFormatBuilder.cpp
        src/Format/Flatbuffers/FlatbuffersFormatExtractor.cpp
        src/Format/Ascii/AsciiFormatBuilder.cpp
        src/Format/Ascii/AsciiFormatExtractor.cpp
        src/Format/HCI/HCIFormatBuilder.cpp
        src/Format/HCI/HCIFormatExtractor.cpp
        src/Transport/SocketContainer/SocketContainer.cpp
        src/Transport/Socket/MGMT/MGMTSocket.cpp
        src/Transport/Socket/StdIO/StdIOSocket.cpp
        src/Transport/Socket/HCI/HCISocket.cpp
        src/Application/PacketRouter/PacketRouter.cpp
        src/Application/Packets/Errors/ErrorResponse/ErrorResponse.cpp
        src/Application/Packets/Commands/ReadByType/ReadByTypeRequest.cpp
        src/Application/Packets/Commands/ReadByType/ReadByTypeResponse.cpp
        src/Application/Packets/Commands/ReadByGroupType/ReadByGroupTypeRequest.cpp
        src/Application/Packets/Commands/ReadByGroupType/ReadByGroupTypeResponse.cpp
        src/Application/Packets/Commands/AddDevice/AddDeviceResponse.cpp
        src/Application/Packets/Commands/Disconnect/DisconnectResponse.cpp
        src/Application/Packets/Commands/GetConnectedDevices/GetConnectedDevicesResponse.cpp
        src/Application/Packets/Commands/GetControllerInfo/GetControllerInfoResponse.cpp
        src/Application/Packets/Commands/GetControllersIds/GetControllersIdsResponse.cpp
        src/Application/Packets/Commands/GetMGMTInfo/GetMGMTInfoResponse.cpp
        src/Application/Packets/Commands/Read/ReadResponse.cpp
        src/Application/Packets/Commands/RemoveDevice/RemoveDeviceResponse.cpp
        src/Application/Packets/Commands/StartScan/StartScanResponse.cpp
        src/Application/Packets/Commands/StopScan/StopScanResponse.cpp
        src/Application/Packets/Commands/SetConnectable/SetConnectableResponse.cpp
        src/Application/Packets/Commands/SetDiscoverable/SetDiscoverableResponse.cpp
        src/Application/Packets/Commands/SetPowered/SetPoweredResponse.cpp
        src/Application/Packets/Commands/Write/WriteResponse.cpp
        src/Application/AbstractPacket.cpp)
add_executable(baBLE_linux ${SOURCES_FILES})

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
